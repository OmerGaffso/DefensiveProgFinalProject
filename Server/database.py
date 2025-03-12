import sqlite3
import os
import logging

from constants import DB_FILE


class Database:
    # Initialize the database. Creating the file if necessary.
    def __init__(self):
        db_exists = os.path.exists(DB_FILE)  # Check if the file exists.
        # self.conn = sqlite3.connect(DB_FILE, check_same_thread=False)
        print(f"Using database file: {os.path.abspath(DB_FILE)}")
        if not db_exists:
            logging.info(f"{DB_FILE} file not found. Creating the database.")
            self.create_tables()

    # Create necessary tables if they don't exist
    def create_tables(self):
        try:
            with sqlite3.connect(DB_FILE) as conn:
                cursor = conn.cursor()
                cursor.execute("""
                    CREATE TABLE IF NOT EXISTS clients 
                    (
                        ID TEXT PRIMARY KEY CHECK(LENGTH(ID) == 16), 
                        UserName TEXT UNIQUE NOT NULL CHECK(LENGTH(UserName) <= 255),
                        PublicKey BLOB UNIQUE NOT NULL CHECK(LENGTH(PublicKey) == 160), 
                        LastSeen DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP 
                    )
                """)
                #

                cursor.execute("""
                    CREATE TABLE IF NOT EXISTS messages
                    (
                        ID INTEGER PRIMARY KEY AUTOINCREMENT,
                        ToClient TEXT NOT NULL CHECK(LENGTH(ToClient) == 32),
                        FromClient TEXT TEXT NOT NULL CHECK(LENGTH(FromClient) == 32),
                        Type INTEGER NOT NULL CHECK(Type BETWEEN 1 AND 4),
                        Content BLOB NOT NULL
                    )
                """)

                conn.commit()
        except sqlite3.Error as e:
            logging.error(f"Error: Database error: {e}")

    """
        Check if a user exists by username.
        Return: True if username exists, False otherwise
    """
    def user_exists(self, username: str) -> bool:
        try:
            with sqlite3.connect(DB_FILE) as conn:
                cursor = conn.cursor()
                cursor.execute("SELECT * FROM clients WHERE UserName = ?", (username,))
                result = cursor.fetchone()
                print(f"Checking if user {username} exist: {result}")
                return result is not None
        except sqlite3.Error as e:
            logging.error(f"Database error:{e}")
            return False

    """
        Add new user to the database.
        Return: True on success, False otherwise (duplicate username)
    """
    @staticmethod
    def add_user(client_id: str, username:str, public_key: bytes) -> bool:
        try:
            with sqlite3.connect(DB_FILE) as conn:
                cursor = conn.cursor()
                cursor.execute(
                    "INSERT INTO clients (ID, UserName, PublicKey) VALUES (?,?,?)",
                    (client_id, username, public_key)
                )
                conn.commit()
                return True
        except sqlite3.IntegrityError:
            logging.error("Failed to add user (ID or Username may already exist)")
            return False
        except sqlite3.Error as e:
            logging.error(f"Database error: {e}")
            return False

    """
        Retrieve all clients except the requesting client.
        Returns: a list of tuples (client_id, username)
    """
    @staticmethod
    def get_clients(exclude_id: str) -> list[tuple]:
        try:
            with sqlite3.connect(DB_FILE) as conn:
                cursor = conn.cursor()
                cursor.execute("SELECT ID, UserName FROM clients WHERE ID != ?", (exclude_id,))
                return cursor.fetchall()
        except sqlite3.Error as e:
            logging.error(f"Database error: {e}")
            return []

    """
        Retrieve the public key of a given client ID.
        Returns: the public key if found, None otherwise.
    """
    @staticmethod
    def get_public_key(client_id: str) -> bytes | None:
        try:
            with sqlite3.connect(DB_FILE) as conn:
                cursor = conn.cursor()
                cursor.execute("SELECT PublicKey FROM clients WHERE ID = ?", (client_id,))
                result = cursor.fetchone()
                return result[0] if result else None
        except sqlite3.Error as e:
            logging.error(f"Database error: {e}")
            return None

    """
        Save a message in the database.
        Returns: The new message id on success, None otherwise
    """
    @staticmethod
    def save_message(to_client: str, from_client: str, msg_type: int, content: bytes) -> int | None:
        try:
            with sqlite3.connect(DB_FILE) as conn:
                cursor = conn.cursor()
                cursor.execute(
                    "INSERT INTO messages (ToClient, FromClient, Type, Content) VALUES (?,?,?,?)",
                    (to_client, from_client, msg_type,  content)
                )
                conn.commit()
                return cursor.lastrowid  # Return the message ID
        except sqlite3.Error as e:
            logging.error(f"Error: Failed to save message: {e}")
            return None

    """
        Retrieve all pending messages for a specific client.
        Returns: a list of tuples (msg_id, from_client, msg_type, content)
    """
    @staticmethod
    def get_pending_messages(client_id: str) -> list[tuple]:
        try:
            with sqlite3.connect(DB_FILE) as conn:
                cursor = conn.cursor()
                cursor.execute(
                    "SELECT ID, FromClient, Type, Content FROM messages WHERE ToClient = ?",
                    (client_id,)
                )
                return cursor.fetchall()
        except sqlite3.Error as e:
            logging.error(f"Database error: {e}")
            return []

    """
        Delete multiple messages after it has been successfully sent.
    """
    @staticmethod
    def delete_messages(msg_ids: list[int]) -> None:
        if not msg_ids:
            return

        try:
            with sqlite3.connect(DB_FILE) as conn:
                cursor = conn.cursor()
                cursor.executemany("DELETE FROM messages WHERE ID = ?", [(msg_id,) for msg_id in msg_ids])
                conn.commit()
        except sqlite3.Error as e:
            logging.error(f"Error: Failed to delete messages: {e}")

    # Print all tables. Used for debug
    @staticmethod
    def print_database():
        try:
            with sqlite3.connect(DB_FILE) as conn:
                cursor = conn.cursor()

                # Get all table names
                cursor.execute("SELECT name FROM sqlite_master WHERE type='table';")
                tables = cursor.fetchall()

                if not tables:
                    print("No tables found in the database.")
                    return

                for table in tables:
                    table_name = table[0]
                    print(f"\n=== Table: {table_name} ===")
                    cursor.execute(f"PRAGMA table_info({table_name});")
                    columns = cursor.fetchall()
                    column_names = [col[1] for col in columns]

                    print(f"Columns: {', '.join(column_names)}")

                    # Get all data from the table
                    cursor.execute(f"SELECT * FROM {table_name};")
                    rows = cursor.fetchall()

                    if rows:
                        for row in rows:
                            print(row)
                    else:
                        print("No data in this table.")
        except sqlite3.Error as e:
            logging.error(f"Database error: {e}")
