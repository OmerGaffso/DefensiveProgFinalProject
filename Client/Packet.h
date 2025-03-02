/*
    This file create the packet
*/
#pragma once
#include <array>
#include <vector>
#include <cstdint>
#include <cstring>

constexpr size_t  MAX_PACKET_SIZE     = 1024;
constexpr size_t  HEADER_SIZE         = 23;
constexpr size_t  CLIENT_ID_LENGTH    = 16;
constexpr size_t  VERSION_LENGTH      = 1;
constexpr size_t  CODE_LENGTH         = 2;
constexpr size_t  PAYLOAD_SIZE_LENGTH = 4;

constexpr uint8_t PROTOCOL_VERSION = 2;

struct PacketHeader
{
    std::array <uint8_t, CLIENT_ID_LENGTH> clientId;
    uint8_t                                version;
    uint16_t                               code;
    uint32_t                               payloadSize;
    //
    PacketHeader(uint16_t opCode, uint32_t size, const std::array<uint8_t, CLIENT_ID_LENGTH>& id);
};

struct Packet
{
    PacketHeader         header;
    std::vector<uint8_t> payload;
    //
    Packet(uint16_t opCode, const std::vector<uint8_t>& data, const std::array<uint8_t, CLIENT_ID_LENGTH>& id);
    //
    std::vector<uint8_t> serialize() const;
    static Packet deserialize(const std::vector<uint8_t>& buffer);
};

