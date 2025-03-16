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
    ServerPacketHeader   header;
    std::vector<uint8_t> payload;
public:
    //
    ServerPacket();
    ServerPacket(const uint16_t opCode, const std::vector<uint8_t>& data);
    //
    static ServerPacket deserialize(const std::vector<uint8_t>& buffer);
    // Getters:
    uint8_t  getVersion() { return header.version; }
    uint16_t getCode() { return header.code; }
    std::vector<uint8_t> getPayload() { return payload; }
    //
    // Setters:
    void setCode(uint16_t opCode) { header.code = opCode; }
};

