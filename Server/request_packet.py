import struct

from constants import CLIENT_ID_SIZE, CLIENT_HEADER_SIZE, HEADER_FORMAT


class RequestPacket:
    def __init__(self, raw_data: bytes):
        if len(raw_data) < CLIENT_HEADER_SIZE:
            raise ValueError("Invalid packet size")

        # Extract header fields
        self.client_id = raw_data[:CLIENT_ID_SIZE]  # First 16 bytes are clientId
        self.version, self.code, self.payload_size = struct.unpack(HEADER_FORMAT,
                                                                   raw_data[CLIENT_ID_SIZE:CLIENT_HEADER_SIZE])
        self.payload = raw_data[CLIENT_HEADER_SIZE:]  # Remaining bytes

    def get_payload(self) -> bytes:
        return self.payload

    def get_client_id(self) -> bytes:
        return self.client_id

    # """
    #     Deserialize received bytes into a RequestPacket object.
    # """
    # @classmethod
    # def from_bytes(cls, data: bytes):
    #     if len(data) < CLIENT_HEADER_SIZE:
    #         raise ValueError("Incomplete request")
    #
    #     client_id, version, code, payload_size = struct.unpack(HEADER_FORMAT, data[:CLIENT_HEADER_SIZE])
    #     payload = data[CLIENT_HEADER_SIZE:] if len(data) >= CLIENT_HEADER_SIZE + payload_size else b""
    #
    #     return cls(client_id, version, code, payload)
