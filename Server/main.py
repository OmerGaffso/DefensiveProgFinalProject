"""
Main entry point for the messaging server.
Initializes logging, reads server port configuration, and starts the server.
"""
import logging
from server import Server
from config import get_port


def main():
    logging.basicConfig(
        format="%(asctime)s - %(levelname)s - %(message)s",
        level=logging.DEBUG,
    )
    port = get_port()
    logging.info(f"Starting server on port {port}...")

    server = Server(port)
    server.start()


if __name__ == "__main__":
    main()
