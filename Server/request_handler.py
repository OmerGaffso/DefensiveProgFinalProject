import logging
import uuid
from typing import Callable, Optional
from request_packet import RequestPacket
from response_packet import ResponsePacket
from database import Database
from constants import *


class RequestHandler:
    def __init__(self):
        self.handlers: dict[int, Callable[[RequestPacket, Database], Optional[ResponsePacket]]] = {
            600: self.handle_register_req,
            601: self.handle_client_list_req,
            602: self.handle_public_key_req,
            603: self.handle_send_msg_req,
            604: self.handle_pending_msgs_req,
        }

    # Determines the appropriate handler based on request code
    def handle_request(self, packet: RequestPacket, db: Database) -> tuple:
        handler: Callable[[RequestPacket], Optional[ResponsePacket]] = self.handlers.get(
            packet.code, self.handle_invalid_requests
        )
        try:
            response = handler(packet, db)

            if packet.code == CODE_PENDING_MESSAGES_RESPONSE:
                if not isinstance(response, tuple) or len(response) != 2:
                    raise ValueError("Pending messages handler must return a tuple (ResponsePacket, list)")
                return response   # Tuple of (ResponsePacket, messages_ids)

            if not isinstance(response, ResponsePacket):
                raise ValueError("Non-pending handlers must return a ResponsePacket")

            return response, []

        except Exception as e:
            logging.error(f"Error in  handle_request: {e}")
            return ResponsePacket(CODE_ERROR, b""), []  # Generic error response

    # Handles user registration request.
    @staticmethod
    def handle_register_req(packet: RequestPacket, db: Database):
        username = packet.payload[:USERNAME_SIZE].decode('ascii').rstrip('\x00')
        public_key = packet.payload[USERNAME_SIZE:]

        if db.user_exists(username):
            logging.error(f"User: {username} already exist")
            db.print_database()
            return ResponsePacket(CODE_ERROR)  # Error

        client_id = uuid.uuid4().bytes
        db.add_user(client_id.hex(), username, public_key)
        logging.info(f"User: {username} added with UID: {client_id.hex()} UID size: {len(client_id)}")
        db.print_database()
        return ResponsePacket(CODE_REGISTER_SUCCESS, client_id)

    # Handles client list request.
    @staticmethod
    def handle_client_list_req(packet: RequestPacket, db: Database):
        client_list = db.get_clients(exclude_id=packet.client_id)

        if not client_list:
            logging.info(f"DEBUG: No clients found in database.")
            return ResponsePacket(CODE_CLIENT_LIST_RESPONSE, b"")

        payload = b"".join([bytes.fromhex(cid) + name.encode().ljust(USERNAME_SIZE, b'\x00') for cid, name in client_list])
        logging.info(f"DEBUG: Sending {len(client_list)} clients. Payload Size: {len(payload)} Bytes.")
        return ResponsePacket(CODE_CLIENT_LIST_RESPONSE, payload)

    # Handles public key request.
    @staticmethod
    def handle_public_key_req(packet: RequestPacket, db: Database):
        target_id = packet.payload[:CLIENT_ID_SIZE].decode()
        public_key = db.get_public_key(target_id)
        if public_key:
            return ResponsePacket(CODE_PUBLIC_KEY_RESPONSE, target_id.encode() + public_key)
        return ResponsePacket(CODE_ERROR)

    # Handles send message request.
    @staticmethod
    def handle_send_msg_req(packet: RequestPacket, db: Database):
        target_id = packet.payload[:CLIENT_ID_SIZE].decode()
        message_type = packet.payload[CLIENT_ID_SIZE]
        content_size = int.from_bytes(packet.payload[17:21], 'big')
        message_content = packet.payload[21:]  # TODO - Change magic numbers!
        message_id = db.save_message(target_id, packet.client_id, message_type, message_content)
        return ResponsePacket(CODE_SEND_MESSAGE_RESPONSE, target_id.encode() + message_id.to_bytes(4, "big"))

    # Handles pending messages request.
    @staticmethod
    def handle_pending_msgs_req(packet: RequestPacket, db: Database):
        messages = db.get_pending_messages(packet.client_id)

        if not messages:
            return ResponsePacket(CODE_PENDING_MESSAGES_RESPONSE, b""), []

        payload = b"".join([
            msg[0].encode() + msg[1].to_bytes(4, "big") + msg[2].to_bytes(1, "big") +
            len(msg[3]).to_bytes(4, "big") + msg[3] for msg in messages
        ])
        db.delete_messages([msg[1] for msg in messages])
        return ResponsePacket(CODE_PENDING_MESSAGES_RESPONSE, payload)

    # Handles invalid requests.
    @staticmethod
    def handle_invalid_requests(packet: RequestPacket):
        return ResponsePacket(CODE_ERROR)

