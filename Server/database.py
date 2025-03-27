"""
database.py

This module provides a `Database` class that encapsulates all interactions with the SQLite database
used by the secure messaging server. It supports:

- Creating the required schema on first use
- Registering new users and storing their public keys
- Retrieving the list of registered clients
- Managing public keys for clients
- Storing and retrieving encrypted messages between clients
- Deleting messages after successful delivery

All database operations are wrapped in exception handling with logging for debugging and reliability.
"""

import sqlite3
import os
import logging
from constants import DB_FILE


class Database:
    """
    Handles all database operations:
    - User registration and lookup
    - Public key retrieval
    - Storing, retrieving, and deleting messages
    """
    def __init__(self):
        """Initialize database file and tables if needed."""
        db_exists = os.path.exists(DB_FILE)
        if not db_exists:
            logging.info(f"{DB_FILE} file not found. Creating new database.")
            self.create_tables()

    def create_tables(self):
        """Create required tables for users and messages."""
        try:
            with sqlite3.connect(DB_FILE) as conn:
                cursor = conn.cursor()
                cursor.execute("""
                    CREATE TABLE IF NOT EXISTS clients 
                    (
                        ID TEXT UNIQUE PRIMARY KEY CHECK(LENGTH(ID) == 16), 
                        UserName TEXT UNIQUE NOT NULL CHECK(LENGTH(UserName) <= 255),
                        PublicKey BLOB UNIQUE NOT NULL CHECK(LENGTH(PublicKey) == 160), 
                        LastSeen DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP 
                    )
                """)
                cursor.execute("""
                    CREATE TABLE IF NOT EXISTS messages
                    (
                        ID INTEGER PRIMARY KEY AUTOINCREMENT,
                        ToClient TEXT NOT NULL CHECK(LENGTH(ToClient) == 16),
                        FromClient TEXT TEXT NOT NULL CHECK(LENGTH(FromClient) == 16),
                        Type INTEGER NOT NULL CHECK(Type BETWEEN 1 AND 4),
                        Content BLOB NOT NULL
                    )
                """)
                conn.commit()
        except sqlite3.Error as e:
            logging.error(f"Database error while creating tables: {e}")

    def user_exists(self, username: str) -> bool:
        """
        Check if a username exists in the database.
        """
        try:
            with sqlite3.connect(DB_FILE) as conn:
                cursor = conn.cursor()
                cursor.execute("SELECT * FROM clients WHERE UserName = ?", (username,))
                result = cursor.fetchone()
                return result is not None
        except sqlite3.Error as e:
            logging.error(f"Database error in user_exists(): {e}")
            return False

    @staticmethod
    def add_user(client_id: bytes, username: str, public_key: bytes) -> bool:
        """
        Add new user to the database.
        Return: True on success, False on error or duplication.
        """
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
            logging.error("Failed to add user (duplicate ID or username).")
            return False
        except sqlite3.Error as e:
            logging.error(f"Database error in add_user(): {e}")
            return False

    @staticmethod
    def get_clients(exclude_id: bytes) -> list[tuple]:
        """
        Get all users excluding the one with the specified ID.
        Returns a list of (client_id, username) tuples.
        """
        try:
            # exclude_id_hex = exclude_id.hex()
            with sqlite3.connect(DB_FILE) as conn:
                cursor = conn.cursor()
                cursor.execute("SELECT ID, UserName FROM clients WHERE ID != ?", (exclude_id,))
                return cursor.fetchall()
        except sqlite3.Error as e:
            logging.error(f"Database error in get_clients(): {e}")
            return []

    @staticmethod
    def get_public_key(client_id: str) -> bytes | None:
        """
        Get the public key for a given client ID.
        """
        try:
            with sqlite3.connect(DB_FILE) as conn:
                cursor = conn.cursor()
                cursor.execute("SELECT PublicKey FROM clients WHERE ID = ?", (client_id,))
                result = cursor.fetchone()
                return result[0] if result else None
        except sqlite3.Error as e:
            logging.error(f"Database error in get_public_key(): {e}")
            return None

    @staticmethod
    def save_message(to_client: str, from_client: str, msg_type: int, content: bytes) -> int | None:
        """
        Save an outgoing message in the database.
        Returns the inserted message ID, or None on error.
        """
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
            logging.error(f"Database error in save_message(): {e}")
            return None

    @staticmethod
    def get_pending_messages(client_id: str) -> list[tuple]:
        """
        Get all pending messages for a given client ID.
        Returns a list of (msg_id, from_client, type, content) tuples.
        """
        try:
            with sqlite3.connect(DB_FILE) as conn:
                cursor = conn.cursor()
                cursor.execute(
                    "SELECT ID, FromClient, Type, Content FROM messages WHERE ToClient = ?",
                    (client_id,)
                )
                return cursor.fetchall()
        except sqlite3.Error as e:
            logging.error(f"Database error in get_pending_messages(): {e}")
            return []

    @staticmethod
    def delete_messages(msg_ids: list[int]) -> None:
        """
        Delete a list of messages from the database using their message IDs.
        """
        if not msg_ids:
            return

        try:
            with sqlite3.connect(DB_FILE) as conn:
                cursor = conn.cursor()
                cursor.executemany("DELETE FROM messages WHERE ID = ?", [(msg_id,) for msg_id in msg_ids])
                conn.commit()
        except sqlite3.Error as e:
            logging.error(f"Database error in delete_messages(): {e}")

    def update_last_seen(self, client_id: bytes):
        try:
            with sqlite3.connect(DB_FILE) as conn:
                cursor = conn.cursor()
                cursor.execute("UPDATE clients SET LastSeen = CURRENT_TIMESTAMP WHERE ID = ?", (client_id,))
                conn.commit()
        except sqlite3.Error as e:
            logging.error(f"Failed to update last seen for {client_id.hex()}: {e}")

