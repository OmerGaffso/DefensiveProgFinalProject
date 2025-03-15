/*
    This file create the packet
*/
#pragma once
#include <array>
#include <vector>
#include <cstdint>
#include <cstring>
#include "Utility.h"

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

