import struct

# Constants for header
HEADER_FORMAT = "!BHI"
HEADER_SIZE = struct.calcsize(HEADER_FORMAT)

SERVER_VERSION = 2

class RequestPacket:
    def __init__(self, raw_data: bytes):
        if len(raw_data) < HEADER_SIZE:
            raise ValueError("Invalid packet size")

        # Extract header fields
        self.version, self.code, self.payload_size = struct.unpack(HEADER_FORMAT, raw_data[:HEADER_SIZE])
        self.payload = raw_data[HEADER_SIZE:HEADER_SIZE + self.payload_size]

    def get_payload(self):
        return self.payload


class ResponsePacket:
    def __init__(self, code: int, payload: bytes = b""):
        self.version = SERVER_VERSION
        self.code = code
        self.payload = payload
        self.payload_size = len(payload)

    def to_bytes(self) -> bytes:
        header = struct.pack(HEADER_FORMAT, self.version, self.code, self.payload_size)
        return header + self.payload






