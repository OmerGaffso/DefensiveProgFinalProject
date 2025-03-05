import socket
import logging
from database import Database

MAX_CONNECTIONS   = 10
PACKET_MAX_LENGTH = 1024

class Server:
    def __init__(self, port=1357):
        self.host = "0.0.0.0"  # listen on all interfaces
        self.port = port
        self.db = Database()

    # Start the server and listen for connections:
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
            data = client_socket.recv(PACKET_MAX_LENGTH)
            if not data:
                return
            logging.info(f"Received data: {data}")
            # TODO: handle requests here
        except Exception as e:
            logging.error(f"Error handling client: {e}")
        finally:
            client_socket.close()