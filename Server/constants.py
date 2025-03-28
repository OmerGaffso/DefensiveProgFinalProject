"""
constants.py

Contains all protocol-level constants used by the server for:
- Port configuration
- Request/response codes
- Protocol headers
- Payload sizes
- Database settings
"""

# === Server Configurations
DEFAULT_PORT    = 1357
MIN_PORT_VAL    = 1024
MAX_PORT_VAL    = 65535
MAX_CONNECTIONS = 10
SERVER_VERSION  = 2
CHUNK_SIZE      = 4096

# === Request Codes ===
CODE_REGISTER_USER    = 600
CODE_CLIENT_LIST      = 601
CODE_PUBLIC_KEY       = 602
CODE_SEND_MESSAGE     = 603
CODE_PENDING_MESSAGES = 604

# === Response Codes ===
CODE_REGISTER_SUCCESS          = 2100
CODE_CLIENT_LIST_RESPONSE      = 2101
CODE_PUBLIC_KEY_RESPONSE       = 2102
CODE_SEND_MESSAGE_RESPONSE     = 2103
CODE_PENDING_MESSAGES_RESPONSE = 2104
CODE_ERROR                     = 9000

# === Protocol Sizes ===
CLIENT_ID_SIZE     = 16
VERSION_SIZE       = 1
CODE_SIZE          = 2
PAYLOAD_SIZE       = 4
USERNAME_SIZE      = 255
PUBLIC_KEY_SIZE    = 160
MESSAGE_ID_SIZE    = 4
MESSAGE_TYPE_SIZE  = 1
MESSAGE_SIZE_FIELD = 4

# === Packet Header Formats (struct) ===
CLIENT_HEADER_FORMAT = "<16sBHI"
HEADER_FORMAT = "<BHI"

CLIENT_HEADER_SIZE = 23  # Client ID (16) + Version (1) + Code (2) + Payload Size (4)
SERVER_HEADER_SIZE = 7  # Version (1) + Code (2) + Payload Size (4)

# === Message Tuple Indexes ===
INDEX_MSG_ID         = 0
INDEX_FROM_CLIENT    = 1
INDEX_MSG_TYPE       = 2
INDEX_MSG_CONTENT    = 3

# === Database ===
DB_FILE = "defensive.db"

