"""
request_packet module
Defines the RequestPacket class which parses raw bytes received from the client into structured data.
"""

import struct

from constants import CLIENT_ID_SIZE, CLIENT_HEADER_SIZE, HEADER_FORMAT


class RequestPacket:
    """
    Represents a parsed client request packet.

    Attributes:
        client_id (bytes): 16-byte unique identifier of the client.
        version (int): Protocol version.
        code (int): Operation code indicating the request type.
        payload_size (int): Size of the payload (in bytes).
        payload (bytes): The actual data content of the packet.
    """
    def __init__(self, raw_data: bytes):
        """
        Initializes a RequestPacket by parsing raw data from a client.
        params: raw_data (bytes): Raw byte stream from the client.
        raise: ValueError: If the raw data is smaller than the expected header size.
        """
        if len(raw_data) < CLIENT_HEADER_SIZE:
            raise ValueError("Invalid packet size")

        # Extract header fields
        self.client_id = raw_data[:CLIENT_ID_SIZE]  # First 16 bytes are clientId
        self.version, self.code, self.payload_size = struct.unpack(HEADER_FORMAT,
                                                                   raw_data[CLIENT_ID_SIZE:CLIENT_HEADER_SIZE])
        self.payload = raw_data[CLIENT_HEADER_SIZE:]  # Remaining bytes

    def get_payload(self) -> bytes:
        """Returns the payload section of the packet."""
        return self.payload

    def get_client_id(self) -> bytes:
        """Returns the client ID from the packet."""
        return self.client_id
