#pragma once

#include <vector>
#include <cstdint>
#include <cstring>

#define CLIENT_ID_SIZE 16U

class Message
{
    uint8_t                 clientID[CLIENT_ID_SIZE]; // unique client identifier
    uint8_t                 clientVersion; 
    uint16_t                reqCode;       // request code
    uint32_t                payloadSize;
    std::vector<uint8_t>    payload;       // request payload
    //



public:
    enum class Type
    {
        EXIT = 0,
        ERROR = 1,
        REGISTER = 110,
        CLIENT_ID_REQ = 120,
        PUBLIC_KEY_REQ = 130,
        PENDING_MSGS_REQ = 140,
        SEND_TEXT_MSG = 150,
        SYMTRIC_KEY_REQ = 151,
        SEND_SYMETRIC_KEY = 152,
        SEND_FILE_REQ = 153,
    };
    //
    Message();

    std::vector<uint8_t> serializeMessage() const;

};