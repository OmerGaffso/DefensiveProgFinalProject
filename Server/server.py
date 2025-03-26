"""
server.py
Implements the main server logic using non-blocking sockets with the `selectors` module.
Handles incoming client connections, parses requests, delegates handling, and sends responses.
"""

import socket
import logging
import selectors

from constants import *
from database import Database
from request_handler import RequestHandler
from request_packet import RequestPacket


class Server:
    """
    Main server class responsible for accepting connections and handling requests.
    """
    def __init__(self, port=1357):
        self.host = "0.0.0.0"
        self.port = port
        self.selector = selectors.DefaultSelector()
        self.handler = RequestHandler()

    def start(self):
        """
        Starts the server, binds the socket, and enters the main event loop.
        """
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket:
            server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            server_socket.bind((self.host, self.port))
            server_socket.listen(MAX_CONNECTIONS)
            server_socket.setblocking(False)  # Non-blocking mode
            self.selector.register(server_socket, selectors.EVENT_READ, self.accept_client)
            logging.info(f"Server started on port {self.port}")

            try:
                while True:
                    events = self.selector.select(timeout=None)  # wait for events
                    for key, _ in events:
                        callback = key.data
                        callback(key.fileobj)
            except Exception as e:
                logging.error(f"Server error: {e}")
            finally:
                logging.info("Shutting down server...")
                self.cleanup()

    def accept_client(self, server_socket):
        """
        Accepts a new client connection and registers it for read events.
        """
        try:
            client_socket, addr = server_socket.accept()
            logging.info(f"New connection from {addr}")
            client_socket.setblocking(False)
            self.selector.register(client_socket, selectors.EVENT_READ, self.handle_client)
        except Exception as e:
            logging.error(f"Error accepting client: {e}")

    def handle_client(self, client_socket):
        """
        Handles communication with a connected client.
        """
        try:
            db = Database()
            data = self.recv_full(client_socket)
            if not data:
                logging.info("Client disconnected.")
                self.disconnect_client(client_socket)
                return

            packet = RequestPacket(data)
            logging.info(f"Received data from {packet.client_id}")
            response_packet, message_ids = self.handler.handle_request(packet, db)

            self.send_full(client_socket, response_packet.to_bytes())

            if packet.code == CODE_PENDING_MESSAGES and message_ids:
                db.delete_messages(message_ids)
                logging.info(f"Deleted {len(message_ids)} messages for client {packet.client_id}")

        except Exception as e:
            logging.error(f"Error handling client: {e}")
            self.disconnect_client(client_socket)

    def disconnect_client(self, client_socket):
        """
        Unregisters and closes the specified client socket.
        """
        try:
            self.selector.unregister(client_socket)
            client_socket.close()
            logging.info("Client disconnected.")
        except Exception as e:
            logging.error(f"Error disconnecting client: {e}")

    def recv_full(self, client_socket):
        """
        Reads the full request from the client, including header and full payload.
        Returns the full data as bytes if successful, otherwise None.
        """
        try:
            header = client_socket.recv(CLIENT_HEADER_SIZE)
            if not header or len(header) < CLIENT_HEADER_SIZE:
                return None

            client_id = header[:CLIENT_ID_SIZE]
            client_version = header[CLIENT_ID_SIZE]
            client_code = int.from_bytes(header[CLIENT_ID_SIZE + VERSION_SIZE:CLIENT_ID_SIZE + VERSION_SIZE + CODE_SIZE]
                                         , byteorder="big")
            payload_size = int.from_bytes(header[CLIENT_ID_SIZE + VERSION_SIZE + CODE_SIZE:CLIENT_HEADER_SIZE]
                                          , byteorder='big')

            # Convert header back to little endian format expected by struct
            little_end_header = (client_id +
                                 bytes([client_version]) +
                                 client_code.to_bytes(2, byteorder="little") +
                                 payload_size.to_bytes(4, byteorder="little")
                                 )

            data = bytearray(little_end_header)

            while len(data) < CLIENT_HEADER_SIZE + payload_size:
                chunk = client_socket.recv(CHUNK_SIZE)
                if not chunk:
                    break
                data.extend(chunk)

            return bytes(data) if len(data) == CLIENT_HEADER_SIZE + payload_size else None

        except Exception as e:
            logging.error(f"Error receiving data: {e}")
            return None

    def send_full(self, client_socket, data):
        """
        Sends the entire `data` buffer to the client.
        """
        try:
            total_sent = 0
            while total_sent < len(data):
                sent = client_socket.send(data[total_sent:])
                if sent == 0:
                    raise ConnectionError("Socket connection broken during send.")
                total_sent += sent

        except Exception as e:
            logging.error(f"Error sending data: {e}")

    def cleanup(self):
        """
        Performs cleanup operations when shutting down the server.
        """
        try:
            self.selector.close()
            logging.info("Selector closed.")
        except Exception as e:
            logging.error(f"Error during cleanup: {e}")
