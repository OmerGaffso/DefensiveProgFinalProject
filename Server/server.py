import socket
import logging
import struct

from constants import MAX_CONNECTIONS, CODE_PENDING_MESSAGES, CLIENT_HEADER_SIZE, CLIENT_HEADER_FORMAT, CHUNK_SIZE
from database import Database
from request_handler import RequestHandler
from request_packet import RequestPacket


class Server:
    def __init__(self, port=1357):
        self.host = "127.0.0.1"  # listen on all interfaces
        self.port = port
        self.handler = RequestHandler()

    # Start the server and listen for incoming connections:
    def start(self):
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket:
            server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            server_socket.bind((self.host, self.port))
            server_socket.listen(MAX_CONNECTIONS)
            logging.info(f"Server started on port {self.port}")

            while True:
                client_socket, addr = server_socket.accept()
                logging.info(f"New connection from {addr}")
                self.handle_client(client_socket)

    # Handle the client connections (Now only 1, later more)
    def handle_client(self, client_socket):
        try:
            db = Database()
            data = self.recv_full(client_socket)
            if not data:
                return

            logging.info(f"Received data: {data.hex()}")  # debug received data
            # Parse the request packet
            packet = RequestPacket(data)

            # Process request
            response_packet, message_ids = self.handler.handle_request(packet, db)

            # Send the response packet
            self.send_full(client_socket, response_packet.to_bytes())

            # If the request is pending messages, delete only after successful transmit
            if packet.code == CODE_PENDING_MESSAGES and message_ids:
                db.delete_messages(message_ids)
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

            print(f"Received Raw Header (Hex): {header.hex()}")

            # client_id, client_version, client_code, payload_size = struct.unpack(CLIENT_HEADER_FORMAT, header)  # Unpack version, code and payload_size
            client_id = header[:16]  # first 16 bytes
            client_version = header[16]  # 17th byte

            client_code = int.from_bytes(header[17:19], byteorder="big")
            payload_size = int.from_bytes(header[19:23], byteorder='big')

            print(
                f"Parsed Values - Client ID: {client_id.hex()}, Version: {client_version}, Code: {client_code}, Payload Size: {payload_size}")

            little_end_header = (client_id +
                                 bytes([client_version]) +
                                 client_code.to_bytes(2, byteorder="little") +
                                 payload_size.to_bytes(4, byteorder="little")
                                 )

            # Ensure full payload is received
            data = bytearray(little_end_header)
            while len(data) < CLIENT_HEADER_SIZE + payload_size:
                chunk = client_socket.recv(CHUNK_SIZE)  # Read chunks of data
                if not chunk:
                    break
                data.extend(chunk)

            # Ensure the full packet received
            return bytes(data) if len(data) == CLIENT_HEADER_SIZE + payload_size else None

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
