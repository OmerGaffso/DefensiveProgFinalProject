"""
config.py

Handles server configuration such as port retrieval.
Reads the port number from 'myport.info' file if it exists,
validates it, and returns it. Falls back to default if missing or invalid.
"""

import logging
from constants import MIN_PORT_VAL, MAX_PORT_VAL, DEFAULT_PORT

PORT_FILE    = "myport.info"


def get_port():
    """
    Reads the server port from the 'myport.info' file.
    If the file does not exist or contains an invalid port number,
    it returns the DEFAULT_PORT.

    Returns:
        int: The port number to use.
    """
    try:
        with open(PORT_FILE, "r") as f:
            port = int(f.read().strip())
            if MIN_PORT_VAL <= port <= MAX_PORT_VAL:
                return port
            else:
                logging.warning(f"Port {port} in {PORT_FILE} is out of valid range. Using default port.")
    except FileNotFoundError:
        logging.warning(f"{PORT_FILE} not found. Using default port.")
    except ValueError:
        logging.warning(f"Could not parse a valid port from {PORT_FILE}. Using default port.")
    return DEFAULT_PORT
