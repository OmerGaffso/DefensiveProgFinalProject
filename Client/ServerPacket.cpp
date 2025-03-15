#include "ServerPacket.h"
#include <WinSock2.h>
#include <stdexcept>

ServerPacketHeader::ServerPacketHeader()
{
    this->code = CODE_DEFAULT;
    this->version = PROTOCOL_VERSION;
    this->payloadsize = 0;
}

ServerPacketHeader::ServerPacketHeader(uint16_t opCode, uint32_t size)
    : version(PROTOCOL_VERSION), code(opCode), payloadsize(size) {}


ServerPacket::ServerPacket()
{
    ServerPacketHeader defHeader;
    this->header = defHeader;
    this->payload = {};
}

ServerPacket::ServerPacket(const uint16_t opCode, const std::vector<uint8_t>& data)
    : header(opCode, data.size()), payload(data) {}

ServerPacket ServerPacket::deserialize(const std::vector<uint8_t>& buffer)
{
    if (buffer.size() < SERVER_HEADER_SIZE)
        throw std::runtime_error("Invalid packet size\n");
    //
    size_t pos = 0;
    uint8_t version = buffer[pos];
    pos += VERSION_LENGTH;
    //
    uint16_t code;
    std::memcpy(&code, buffer.data() + pos, CODE_LENGTH);
    pos += CODE_LENGTH;
    //
    uint32_t payloadSize;
    std::memcpy(&payloadSize, buffer.data() + pos, PAYLOAD_SIZE_LENGTH);
    pos += PAYLOAD_SIZE_LENGTH;
    //
    std::vector<uint8_t> payload(buffer.begin() + SERVER_HEADER_SIZE, buffer.end());
    return ServerPacket(code, payload);
}

