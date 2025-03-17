#pragma once

#include <iostream>
#include <string>
#include <array>
#include <vector>
#include <cstdint>
#include <sstream>
#include <iomanip>

#define MIN_PORT 1024
#define MAX_PORT 65535


// === Protocol === 
// Sizes
constexpr size_t  CLIENT_ID_LENGTH = 16;
constexpr size_t  VERSION_LENGTH = 1;
constexpr size_t  CODE_LENGTH = 2;
constexpr size_t  PAYLOAD_SIZE_OFFSET = CLIENT_ID_LENGTH + VERSION_LENGTH + CODE_LENGTH;
constexpr size_t  PAYLOAD_SIZE_LENGTH = 4;
constexpr size_t  CLIENT_HEADER_SIZE = CLIENT_ID_LENGTH + VERSION_LENGTH + CODE_LENGTH + PAYLOAD_SIZE_LENGTH;
constexpr size_t  SERVER_HEADER_SIZE = VERSION_LENGTH + CODE_LENGTH + PAYLOAD_SIZE_LENGTH;
constexpr size_t  SERVER_PAYLOAD_SIZE_OFFSET = VERSION_LENGTH + CODE_LENGTH;
//
// Constants
constexpr uint8_t PROTOCOL_VERSION = 2;
//
// Menu Options:
constexpr uint16_t OPT_EXIT              = 0;
constexpr uint16_t OPT_REGISTER          = 110;
constexpr uint16_t OPT_REQ_CLIENT_LIST   = 120;
constexpr uint16_t OPT_REQ_PUBLIC_KEY    = 130;
constexpr uint16_t OPT_REQ_PENDING_MSGS  = 140;
constexpr uint16_t OPT_SEND_TEXT_MSG     = 150;
constexpr uint16_t OPT_REQ_SYMETRIC_KEY  = 151;
constexpr uint16_t OPT_SEND_SYMETRIC_KEY = 152;
constexpr uint16_t OPT_SEND_FILE         = 153;
// === Request Codes ===
constexpr uint16_t CODE_DEFAULT = 0;
constexpr uint16_t CODE_REGISTER_USER        = 600; 
constexpr uint16_t CODE_REQ_USER_LIST        = 601;
constexpr uint16_t CODE_REQ_USER_PUBLIC_KEY  = 602;
constexpr uint16_t CODE_SEND_MESSAGE_TO_USER = 603;
constexpr uint16_t CODE_REQ_PENDING_MESSAGES = 604;
//
// === Response Codes === 
constexpr uint16_t RESP_CODE_REGISTER_SUCCCESS = 2100;
constexpr uint16_t RESP_CODE_GET_CLIENT_LIST   = 2101;
constexpr uint16_t RESP_CODE_GET_PUBLIC_KEY    = 2102;
constexpr uint16_t RESP_CODE_SEND_MSG_SUCCESS  = 2103;
constexpr uint16_t RESP_CODE_GET_PENDING_MSGS  = 2104;
constexpr uint16_t RESP_CODE_ERROR             = 9000;
//
// === Message Types ===
constexpr uint8_t MSG_TYPE_SYMM_KEY_REQ      = 1;
constexpr uint8_t MSG_TYPE_SYMM_KEY_RESP     = 2;
constexpr uint8_t MSG_TYPE_SEND_TEXT_MSG     = 3;
constexpr uint8_t MSG_TYPE_SEND_FILE         = 4;
// payload lengths:
// register message:
constexpr size_t REGISTER_USERNAME_LEN   = 255;
constexpr size_t REGISTER_PUBLIC_KEY_LEN = 160;
constexpr size_t REGISTER_PAYLOAD_LEN    = REGISTER_USERNAME_LEN + REGISTER_PUBLIC_KEY_LEN;
// send message
constexpr size_t MESSAGE_TYPE_LEN    = 1;
constexpr size_t MESSAGE_CONTENT_LEN = 4;
constexpr uint8_t MSG_ID_LEN         = 4;

constexpr uint8_t USERNAME_MAX_LENGTH = 254; // leaving place for null termination. 

inline bool isValidUsername(const std::string& username)
{
    return username.empty() || username.length() > USERNAME_MAX_LENGTH;
}

struct ArrayHasher
{
    std::size_t operator()(const std::array<uint8_t, CLIENT_ID_LENGTH>& arr) const;
};

std::string toHex(const std::array<uint8_t, CLIENT_ID_LENGTH>& data);
