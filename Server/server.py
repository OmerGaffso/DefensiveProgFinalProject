import socket
import logging
import struct

from constants import MAX_CONNECTIONS, CODE_PENDING_MESSAGES, CLIENT_HEADER_SIZE, CLIENT_HEADER_FORMAT, CHUNK_SIZE
from database import Database
from request_handler import RequestHandler
from request_packet import RequestPacket


class Server:
    def __init__(self, port=1357):
        self.host = "0.0.0.0"  # listen on all interfaces
        self.port = port
        self.db = Database()
        self.handler = RequestHandler()

    # Start the server and listen for incoming connections:
    def start(self):
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket:
            server_socket.bind((self.host, self.port))
            server_socket.listen(MAX_CONNECTIONS)
            logging.info(f"Server started on port{self.port}")

            while True:
                client_socket, addr = server_socket.accept()
                logging.info(f"New connection from {addr}")
                self.handle_client(client_socket)

    # Handle the client connections (Now only 1, later more)
    def handle_client(self, client_socket):
        try:
            data = self.recv_full(client_socket)
            if not data:
                return

            logging.info(f"Received data: {data.hex()}")  # debug received data
            # Parse the request packet
            packet = RequestPacket(data)

            # Process request
            response_packet, message_ids = self.handler.handle_request(packet)

            # Send the response packet
            self.send_full(client_socket, response_packet.to_bytes())

            # If the request is pending messages, delete only after successful transmit
            if packet.code == CODE_PENDING_MESSAGES and message_ids:
                self.db.delete_messages(message_ids)
                logging.info(f"Deleted {len(message_ids)} messages for client {packet.client_id}")
        except Exception as e:
            logging.error(f"Error: Error handling client: {e}")
        finally:
            client_socket.close()

    """
        Receives the full packet by first reading the header to determine payload size 
    """
    def recv_full(self, client_socket):
        try:
            header = client_socket.recv(CLIENT_HEADER_SIZE)  # client_id(16) + version(1) + code(2) + payload_size (4)
            if not header or len(header) < CLIENT_HEADER_SIZE:
                return None

            _, _, payload_size = struct.unpack(CLIENT_HEADER_FORMAT, header)  # Unpack version, code and payload_size

            # Ensure full payload is received
            data = bytearray(header)
            while len(data) < CLIENT_HEADER_SIZE + payload_size:
                chunk = client_socket.recv(CHUNK_SIZE)  # Read chunks of data
                if not chunk:
                    break
                data.extend(chunk)

            # Ensure the full packet received
            return bytes(data) if len(data) == CLIENT_HEADER_FORMAT + payload_size else None

        except Exception as e:
            logging.error(f"Error: Error receiving data: {e}")
            return None

    """
        Sends the full data, ensuring the bytes are transmitted.
    """
    def send_full(self, client_socket, data):
        try:
            total_sent = 0
            while total_sent < len(data):
                sent = client_socket.send(data[total_sent:])
                if sent == 0:
                    raise ConnectionError("Socket connection broken")
                total_sent += sent

        except Exception as e:
            logging.error(f"Error: Error sending data: {e}")