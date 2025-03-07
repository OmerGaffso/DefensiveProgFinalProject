import sqlite3
import os
import logging

from constants import DB_FILE


class Database:
    # Initialize the database. Creating the file if necessary.
    def __init__(self):
        db_exists = os.path.exists(DB_FILE)  # Check if the file exists.
        self.conn = sqlite3.connect(DB_FILE, check_same_thread=False)

        if not db_exists:
            logging.info(f"{DB_FILE} file not found. Creating the database.")

        self.create_tables()

    # Create necessary tables if they don't exist
    def create_tables(self):
        try:
            cursor = self.conn.cursor()
            cursor.execute("""
                CREATE TABLE IF NOT EXISTS clients 
                (
                    ID TEXT PRIMARY KEY CHECK(LENGTH(ID) == 32), 
                    UserName TEXT UNIQUE NOT NULL CHECK(LENGTH(UserName) <= 255),
                    PublicKey BLOB UNIQUE NOT NULL CHECK(LENGTH(PublicKey) == 160),
                    LastSeen DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP 
                )
            """)

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

            self.conn.commit()
        except sqlite3.Error as e:
            logging.error(f"Error: Database error: {e}")

    """
        Check if a user exists by username.
        Return: True if username exists, False otherwise
    """
    def user_exists(self, username: str) -> bool:
        cursor = self.conn.cursor()
        cursor.execute("SELECT 1 FROM clients WHERE UserName = ?", (username,))
        return cursor.fetchone() is not None

    """
        Add new user to the database.
        Return: True on success, False otherwise (duplicate username)
    """
    def add_user(self, client_id: str, username:str, public_key: bytes) -> bool:
        try:
            cursor = self.conn.cursor()
            cursor.execute(
                "INSERT INTO clients (ID, UserName, PublicKey) VALUES (?,?,?)",
                (client_id, username, public_key)
            )
            self.conn.commit()
            return True
        except sqlite3.IntegrityError:
            logging.error("Error: Failed to add user (ID or Username may already exist)")
            return False

    """
        Retrieve all clients except the requesting client.
        Returns: a list of tuples (client_id, username)
    """
    def get_clients(self, exclude_id: str) -> list[tuple]:
        cursor = self.conn.cursor()
        cursor.execute("SELECT ID, UserName FROM clients WHERE ID != ?", (exclude_id,))
        return cursor.fetchall()

    """
        Retrieve the public key of a given client ID.
        Returns: the public key if found, None otherwise.
    """
    def get_public_key(self, client_id: str) -> bytes | None:
        cursor = self.conn.cursor()
        cursor.execute("SELECT PublicKey FROM clients WHERE ID = ?", (client_id,))
        result = cursor.fetchone()
        return result[0] if result else None

    """
        Save a message in the database.
        Returns: The new message id on success, None otherwise
    """
    def save_message(self, to_client: str,  from_client: str, msg_type: int, content: bytes) -> int | None:
        try:
            cursor = self.conn.cursor()
            cursor.execute(
                "INSERT INTO messages (ToClient, FromClient, Type, Content) VALUES (?,?,?,?)",
                (to_client, from_client, msg_type,  content)
            )
            self.conn.commit()
            return cursor.lastrowid  # Return the message ID
        except sqlite3.Error as e:
            logging.error(f"Error: Failed to save message: {e}")
            return None

    """
        Retrieve all pending messages for a specific client.
        Returns: a list of tuples (msg_id, from_client, msg_type, content)
    """
    def get_pending_messages(self, client_id: str) -> list[tuple]:
        cursor = self.conn.cursor()
        cursor.execute(
            "SELECT ID, FromClient, Type, Content FROM messages WHERE ToClient = ?",
            (client_id,)
        )
        return cursor.fetchall()

    """
        Delete multiple messages after it has been successfully sent.
    """
    def delete_messages(self, msg_ids: list[int]) -> None:
        if not msg_ids:
            return

        try:
            cursor = self.conn.cursor()
            cursor.executemany("DELETE FROM messages WHERE ID = ?", [(msg_id,) for msg_id in msg_ids])
            self.conn.commit()
        except sqlite3.Error as e:
            logging.error(f"Error: Failed to delete messages: {e}")

    # Close the database connection
    def close(self):
        self.conn.close()

