#pragma once
#include "Utility.h"
#include <cstring>

struct ClientPacketHeader
{
    std::array<uint8_t, CLIENT_ID_LENGTH> clientId;
    uint8_t version;
    uint16_t code;
    uint32_t payloadSize;
    //
    ClientPacketHeader();
    ClientPacketHeader(uint16_t opCode, uint32_t size, const std::array<uint8_t, CLIENT_ID_LENGTH>& id);
};

class ClientPacket
{
    ClientPacketHeader header;
    std::vector<uint8_t> payload;
    //
public:
    ClientPacket();
    ClientPacket(uint16_t opCode, const std::vector<uint8_t>& data, const std::array<uint8_t, CLIENT_ID_LENGTH>& id);
    //
    std::vector<uint8_t> serialize() const;
};

