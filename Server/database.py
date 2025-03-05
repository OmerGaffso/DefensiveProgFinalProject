import sqlite3
import os
import logging

DB_FILE = "defensive.db"

class Database:
    # Initialize the database. Creating the file if necessary.
    def __init__(self):
        db_exists = os.path.exists(DB_FILE) # Check if the file exists.
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
            logging.error(f"Database error: {e}")

    # Close the database connection
    def close(self):
        self.conn.close()

