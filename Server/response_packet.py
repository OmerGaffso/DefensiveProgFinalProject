import struct
from constants import SERVER_VERSION, HEADER_FORMAT


class ResponsePacket:
    def __init__(self, code: int, payload: bytes = b""):
        self.version = SERVER_VERSION
        self.code = code
        self.payload = payload
        self.payload_size = len(payload)

    """
        Serialize the response packet into bytes.
    """
    def to_bytes(self) -> bytes:
        header = struct.pack(HEADER_FORMAT, self.version, self.code, self.payload_size)
        return header + self.payload  # Combine header and payload

