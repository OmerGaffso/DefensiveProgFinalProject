#pragma once
#include "Utility.h"
#include <cstring>
struct ServerPacketHeader
{
    uint8_t version;
    uint16_t code;
    uint32_t payloadsize;
    //
    ServerPacketHeader();
    ServerPacketHeader(uint16_t opCode, uint32_t size);
};

class ServerPacket
{
public:
    ServerPacketHeader   header;
    std::vector<uint8_t> payload;
    //
    ServerPacket();
    ServerPacket(const uint16_t opCode, const std::vector<uint8_t>& data);
    //
    static ServerPacket deserialize(const std::vector<uint8_t>& buffer);
};

