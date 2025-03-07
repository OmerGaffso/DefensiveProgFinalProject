import logging

from constants import MIN_PORT_VAL, MAX_PORT_VAL, DEFAULT_PORT

PORT_FILE    = "myport.info"


# Reads port from myport.info (if exists). Defaults to 1357 if missing.
def get_port():
    try:
        with open(PORT_FILE, "r") as f:
            port = int(f.read().strip())
            if MIN_PORT_VAL <= port <= MAX_PORT_VAL:
                return port
            else:
                logging.warning("Invalid port in file. Using default port.")
    except FileNotFoundError:
        logging.warning(f"{PORT_FILE} not found. Using default port.")
    except ValueError:
        logging.warning(f"Invalid format in {PORT_FILE}. Using default port.")
    return DEFAULT_PORT
