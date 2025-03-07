from server import Server
from config import get_port


def main():
    port = get_port()
    server = Server(port)
    server.start()


if __name__ == "__main__":
    main()
