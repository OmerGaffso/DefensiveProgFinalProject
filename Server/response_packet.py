"""
Module: response_packet
Description: Defines the ResponsePacket class used to serialize server responses to clients.
"""

import struct
from constants import SERVER_VERSION, HEADER_FORMAT


class ResponsePacket:
    """
    Represents a response packet sent by the server to a client.

    Attributes:
        version (int): Protocol version (defaulted to SERVER_VERSION).
        code (int): Response operation code.
        payload (bytes): Optional data payload.
        payload_size (int): Size of the payload in bytes.
    """
    def __init__(self, code: int, payload: bytes = b""):
        """
        Initializes a ResponsePacket.
        Args:
            code (int): The response code to send.
            payload (bytes, optional): The optional payload to include. Defaults to empty bytes.
        """
        self.version = SERVER_VERSION
        self.code = code
        self.payload = payload
        self.payload_size = len(payload)

    def to_bytes(self) -> bytes:
        """
        Serializes the response packet into a bytes object.
        Returns:
            bytes: Serialized header and payload ready for transmission.
        """
        header = struct.pack(HEADER_FORMAT, self.version, self.code, self.payload_size)
        return header + self.payload

