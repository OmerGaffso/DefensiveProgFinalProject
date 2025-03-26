"""
RequestHandler module
Handles dispatching client requests to appropriate logic based on request codes.
This includes user registration, client list retrieval, message handling, and key exchange.
"""

import logging
import uuid
from typing import Callable, Optional
from request_packet import RequestPacket
from response_packet import ResponsePacket
from database import Database
from constants import *


class RequestHandler:
    """
    Main request dispatcher. Maps incoming request codes to handler methods.
    """
    def __init__(self):
        self.handlers: dict[int, Callable[[RequestPacket, Database], Optional[ResponsePacket]]] = {
            CODE_REGISTER_USER: self.handle_register_req,
            CODE_CLIENT_LIST: self.handle_client_list_req,
            CODE_PUBLIC_KEY: self.handle_public_key_req,
            CODE_SEND_MESSAGE: self.handle_send_msg_req,
            CODE_PENDING_MESSAGES: self.handle_pending_msgs_req,
        }

    def handle_request(self, packet: RequestPacket, db: Database) -> tuple:
        """
        Determines and calls the appropriate handler.
        Returns a tuple: (ResponsePacket, list of message IDs to delete if needed)
        """
        handler = self.handlers.get(packet.code, self.handle_invalid_requests)
        try:
            response = handler(packet, db)

            if packet.code == CODE_PENDING_MESSAGES:
                if not isinstance(response, tuple) or len(response) != 2:
                    raise ValueError("Pending messages handler must return a tuple (ResponsePacket, list)")
                return response   # Tuple of (ResponsePacket, messages_ids)

            if not isinstance(response, ResponsePacket):
                raise ValueError("Handler must return ResponsePacket")

            return response, []

        except Exception as e:
            logging.error(f"Error in  handle_request: {e}")
            return ResponsePacket(CODE_ERROR, b""), []

    @staticmethod
    def handle_register_req(packet: RequestPacket, db: Database):
        """
        Handles user registration.
        Payload: username (255 bytes) + public key (160 bytes)
        Returns: Response with assigned UUID if successful.
        """
        username = packet.payload[:USERNAME_SIZE].decode('ascii').rstrip('\x00')
        public_key = packet.payload[USERNAME_SIZE:]

        if len(public_key) != PUBLIC_KEY_SIZE:
            logging.error(f"Invalid public key length: {len(public_key)} (Expected: {PUBLIC_KEY_SIZE})")
            return ResponsePacket(CODE_ERROR)

        if db.user_exists(username):
            logging.error(f"User: {username} already exist")
            return ResponsePacket(CODE_ERROR)

        client_id = uuid.uuid4().bytes
        if not db.add_user(client_id, username, public_key):
            return ResponsePacket(CODE_ERROR)
        logging.info(f"User: {username} added with ID: {client_id.hex()}")
        return ResponsePacket(CODE_REGISTER_SUCCESS, client_id)

    @staticmethod
    def handle_client_list_req(packet: RequestPacket, db: Database):
        """
        Handles request for the list of all registered clients (excluding self).
        """
        client_list = db.get_clients(exclude_id=packet.client_id)

        if not client_list:
            return ResponsePacket(CODE_CLIENT_LIST_RESPONSE, b"")

        payload = b"".join([
            cid + name.encode().ljust(USERNAME_SIZE, b'\x00')
            for cid, name in client_list
        ])
        return ResponsePacket(CODE_CLIENT_LIST_RESPONSE, payload)

    @staticmethod
    def handle_public_key_req(packet: RequestPacket, db: Database):
        """
        Returns the public key of the requested client.
        Payload: 16-byte target client ID
        """
        target_id = packet.payload[:CLIENT_ID_SIZE]
        public_key = db.get_public_key(target_id)
        if public_key:
            return ResponsePacket(CODE_PUBLIC_KEY_RESPONSE, target_id + public_key)
        return ResponsePacket(CODE_ERROR)

    @staticmethod
    def handle_send_msg_req(packet: RequestPacket, db: Database):
        """
        Saves an incoming message in the database.
        Payload: target client ID + message type + 4-byte size + message content
        """
        target_id = packet.payload[:CLIENT_ID_SIZE]
        message_type = packet.payload[CLIENT_ID_SIZE]
        content_size = int.from_bytes(packet.payload[CLIENT_ID_SIZE + MESSAGE_TYPE_SIZE:
                                                     CLIENT_ID_SIZE + MESSAGE_TYPE_SIZE + MESSAGE_SIZE_FIELD], 'big')
        message_content = packet.payload[CLIENT_ID_SIZE + MESSAGE_TYPE_SIZE + MESSAGE_SIZE_FIELD:]
        message_id = db.save_message(target_id, packet.client_id, message_type, message_content)
        if message_id is None:
            return ResponsePacket(CODE_ERROR)
        return ResponsePacket(CODE_SEND_MESSAGE_RESPONSE, target_id + message_id.to_bytes(4, "big"))

    @staticmethod
    def handle_pending_msgs_req(packet: RequestPacket, db: Database):
        """
        Retrieves and deletes all pending messages for the client.
        Returns: (ResponsePacket, [message_ids])
        """
        messages = db.get_pending_messages(packet.client_id)

        if not messages:
            return ResponsePacket(CODE_PENDING_MESSAGES_RESPONSE, b""), []

        payload = b"".join([
            msg[INDEX_FROM_CLIENT] +  # from_client
            msg[INDEX_MSG_ID].to_bytes(4, "big") +  # message_id
            msg[INDEX_MSG_TYPE].to_bytes(1, "big") +  # message_type
            len(msg[INDEX_MSG_CONTENT]).to_bytes(4, "big") +  # content size
            msg[INDEX_MSG_CONTENT]  # message content
            for msg in messages
        ])
        db.delete_messages([msg[0] for msg in messages])
        return ResponsePacket(CODE_PENDING_MESSAGES_RESPONSE, payload), [msg[0] for msg in messages]

    @staticmethod
    def handle_invalid_requests(packet: RequestPacket, db: Database):
        """
        Handles unrecognized request codes.
        """
        logging.warning(f"Received invalid request code: {packet.code}")
        return ResponsePacket(CODE_ERROR)
