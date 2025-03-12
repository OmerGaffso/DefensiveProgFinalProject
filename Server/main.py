import logging
from server import Server
from config import get_port


def main():
    logging.basicConfig(
        format="%(asctime)s - %(levelname)s - %(message)s",
        level=logging.DEBUG,
    )
    port = get_port()
    server = Server(port)
    server.start()


if __name__ == "__main__":
    main()
