#include "Packet.h"
#include <WinSock2.h>
#include <stdexcept>

PacketHeader::PacketHeader()
{
    std::array<uint8_t, CLIENT_ID_LENGTH> emptyId = {};
    this->clientId    = emptyId;
    this->code        = CODE_DEFAULT;
    this->version     = 2;
    this->payloadSize = 0;
}
PacketHeader::PacketHeader(const uint16_t opCode, uint32_t size, std::array<uint8_t, CLIENT_ID_LENGTH>& id)
    : version(PROTOCOL_VERSION), code(opCode), payloadSize(size), clientId(id) {};
//
Packet::Packet() 
{ 
    PacketHeader defHeader;
    this->header  = defHeader;
    this->payload = {};
}
//
Packet::Packet(const uint16_t opCode, const std::vector<uint8_t>& data, std::array<uint8_t, CLIENT_ID_LENGTH>& id)
    : header(opCode, data.size(), id), payload(data) {}
//
std::vector<uint8_t> Packet::serialize() const
{
    size_t pos = 0;
    std::vector<uint8_t> buffer(CLIENT_HEADER_SIZE + payload.size());
    //
    std::memcpy(buffer.data(), header.clientId.data(), CLIENT_ID_LENGTH);
    buffer[CLIENT_ID_LENGTH] = header.version;
    pos += CLIENT_ID_LENGTH + VERSION_LENGTH;
    //
    uint16_t netCode = htons(header.code);
    std::memcpy(buffer.data() + pos, &netCode, CODE_LENGTH);
    pos += CODE_LENGTH;
    //
    uint32_t payloadSize = htonl(header.payloadSize);
    std::memcpy(buffer.data() + pos, &payloadSize, PAYLOAD_SIZE_LENGTH);
    //
    std::memcpy(buffer.data() + CLIENT_HEADER_SIZE, payload.data(), payload.size());
    //
    //
    // Debugging Output
    std::cout << "Serialized Packet (Hex): ";
    for (uint8_t byte : buffer)
        printf("%02X ", byte);
    std::cout << std::endl;
    //
    return buffer;
}
//
Packet Packet::deserialize(const std::vector<uint8_t>& buffer)
{
    if (buffer.size() < SERVER_HEADER_SIZE)
        throw std::runtime_error("Invalid packet size\n");
    //
    std::array<uint8_t, CLIENT_ID_LENGTH> id;
    std::memcpy(id.data(), buffer.data(), CLIENT_ID_LENGTH);
    //
    uint8_t version = buffer[CLIENT_ID_LENGTH];
    if (version != PROTOCOL_VERSION)
        throw std::runtime_error("Protocol version mismatch\n");
    //
    size_t pos = CLIENT_ID_LENGTH + VERSION_LENGTH;
    //
    uint16_t code;
    std::memcpy(&code, buffer.data() + pos, CODE_LENGTH);
    code = ntohs(code);
    pos += CODE_LENGTH;
    //
    uint32_t size;
    std::memcpy(&size, buffer.data() + pos, PAYLOAD_SIZE_LENGTH);
    size = ntohl(size);
    //
    std::vector<uint8_t> payload(buffer.begin() + SERVER_HEADER_SIZE, buffer.end());
    return Packet(code, payload, id);
}