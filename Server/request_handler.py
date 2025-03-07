from typing import (Callable, Optional)
from request_packet import RequestPacket
from response_packet import ResponsePacket
from database import Database
from constants import USERNAME_SIZE, CLIENT_ID_SIZE, CODE_ERROR, CODE_PENDING_MESSAGES_RESPONSE, \
    CODE_SEND_MESSAGE_RESPONSE, CODE_PUBLIC_KEY_RESPONSE, CODE_CLIENT_LIST_RESPONSE, CODE_REGISTER_SUCCESS
import uuid


class RequestHandler:
    def __init__(self):
        self.db = Database()
        self.handlers: dict[int, Callable[[RequestPacket], Optional[ResponsePacket]]] = {
            600: self.handle_register_req,
            601: self.handle_client_list_req,
            602: self.handle_public_key_req,
            603: self.handle_send_msg_req,
            604: self.handle_pending_msgs_req,
        }

    # Determines the appropriate handler based on request code
    def handle_request(self, packet: RequestPacket) -> Optional[ResponsePacket]:
        handler: Callable[[RequestPacket], Optional[ResponsePacket]] = self.handlers.get(
            packet.code, self.handle_invalid_requests
        )
        return handler(packet)

    # Handles user registration request.
    def handle_register_req(self, packet: RequestPacket):
        username = packet.payload[:USERNAME_SIZE].decode('ascii').rstrip('\x00')
        public_key = packet.payload[USERNAME_SIZE:]

        if self.db.user_exists(username):
            return ResponsePacket(CODE_ERROR)  # Error

        client_id = uuid.uuid4().hex
        self.db.add_user(client_id, username, public_key)
        return ResponsePacket(CODE_REGISTER_SUCCESS)

    # Handles client list request.
    def handle_client_list_req(self, packet: RequestPacket):
        client_list = self.db.get_clients(exclude_id=packet.client_id)
        payload = b"".join([cid.encode() + name.encode().ljust(USERNAME_SIZE, b'\x00') for cid, name in client_list])
        return ResponsePacket(CODE_CLIENT_LIST_RESPONSE, payload)

    # Handles public key request.
    def handle_public_key_req(self, packet: RequestPacket):
        target_id = packet.payload[:CLIENT_ID_SIZE].decode()
        public_key = self.db.get_public_key(target_id)
        if public_key:
            return ResponsePacket(CODE_PUBLIC_KEY_RESPONSE, target_id.encode() + public_key)
        return ResponsePacket(CODE_ERROR)

    # Handles send message request.
    def handle_send_msg_req(self, packet: RequestPacket):
        target_id = packet.payload[:CLIENT_ID_SIZE].decode()
        message_type = packet.payload[CLIENT_ID_SIZE]
        content_size = int.from_bytes(packet.payload[17:21], 'big')
        message_content = packet.payload[21:]  # TODO - Change magic numbers!
        message_id = self.db.save_message(target_id, packet.client_id, message_type, message_content)
        return ResponsePacket(CODE_SEND_MESSAGE_RESPONSE, target_id.encode() + message_id.to_bytes(4, "big"))

    # Handles pending messages request.
    def handle_pending_msgs_req(self, packet: RequestPacket):
        messages = self.db.get_pending_messages(packet.client_id)

        if not messages:
            return ResponsePacket(CODE_PENDING_MESSAGES_RESPONSE, b""), []

        payload = b"".join([
            msg[0].encode() + msg[1].to_bytes(4, "big") + msg[2].to_bytes(1, "big") +
            len(msg[3]).to_bytes(4, "big") + msg[3] for msg in messages
        ])
        self.db.delete_messages([msg[1] for msg in messages])
        return ResponsePacket(CODE_PENDING_MESSAGES_RESPONSE, payload)

    # Handles invalid requests.
    @staticmethod
    def handle_invalid_requests(packet: RequestPacket):
        return ResponsePacket(CODE_ERROR)