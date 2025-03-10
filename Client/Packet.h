/*
    This file create the packet
*/
#pragma once
#include <array>
#include <vector>
#include <cstdint>
#include <cstring>

constexpr size_t  MAX_PACKET_SIZE     = 1024;
constexpr size_t  CLIENT_ID_LENGTH    = 16;
constexpr size_t  VERSION_LENGTH      = 1;
constexpr size_t  CODE_LENGTH         = 2;
constexpr size_t  PAYLOAD_SIZE_OFFSET = CLIENT_ID_LENGTH + VERSION_LENGTH + CODE_LENGTH;
constexpr size_t  PAYLOAD_SIZE_LENGTH = 4;
constexpr size_t  HEADER_SIZE         = CLIENT_ID_LENGTH + VERSION_LENGTH + CODE_LENGTH + PAYLOAD_SIZE_LENGTH;
//
constexpr uint8_t PROTOCOL_VERSION = 2;
//
constexpr uint16_t CODE_DEFAULT               = 0;
constexpr uint16_t CODE_REGISTER_USER         = 600;
constexpr uint16_t CODE_REQ_USER_LIST         = 601;
constexpr uint16_t CODE_REQ_USER_PUBLIC_KEY   = 602;
constexpr uint16_t CODE_SEND_MESSAGE_TO_USER  = 603;
constexpr uint16_t CODE_REQ_PENDING_MESSAGES  = 604;
// payload lengths:
// register message
constexpr size_t REGISTER_USERNAME_LEN   = 255;
constexpr size_t REGISTER_PUBLIC_KEY_LEN = 160;
constexpr size_t REGISTER_PAYLOAD_LEN    = REGISTER_USERNAME_LEN + REGISTER_PUBLIC_KEY_LEN;
// get public key
constexpr size_t CLIENT_ID_LEN           = 16;
// send message
constexpr size_t MESSAGE_TYPE_LEN    = 1;
constexpr size_t MESSAGE_CONTENT_LEN = 4;
//
struct PacketHeader
{
    std::array <uint8_t, CLIENT_ID_LENGTH> clientId;
    uint8_t                                version;
    uint16_t                               code;
    uint32_t                               payloadSize;
    //
    PacketHeader();
    PacketHeader(const uint16_t opCode, uint32_t size, std::array<uint8_t, CLIENT_ID_LENGTH>& id);
};

struct Packet
{
    PacketHeader         header;
    std::vector<uint8_t> payload;
    //
    Packet();
    Packet(const uint16_t opCode, const std::vector<uint8_t>& data, std::array<uint8_t, CLIENT_ID_LENGTH>& id);
    //
    std::vector<uint8_t> serialize() const;
    static Packet deserialize(const std::vector<uint8_t>& buffer);
};

