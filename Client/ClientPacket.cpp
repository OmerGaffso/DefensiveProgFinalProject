#include "ClientPacket.h"
#include <WinSock2.h>
#include <stdexcept>

ClientPacketHeader::ClientPacketHeader()
{
    clientId.fill(0);
    code    = CODE_DEFAULT;
    version = PROTOCOL_VERSION;
    payloadSize = 0;
}

ClientPacketHeader::ClientPacketHeader(uint16_t opCode, uint32_t size, const std::array<uint8_t, CLIENT_ID_LENGTH>& id)
    : version(PROTOCOL_VERSION), code(opCode), payloadSize(size), clientId(id) {}

ClientPacket::ClientPacket() : header(), payload() {}

ClientPacket::ClientPacket(uint16_t opCode, const std::vector<uint8_t>& data, const std::array<uint8_t, CLIENT_ID_LENGTH>& id)
    : header(opCode, data.size(), id), payload(data) {}
    //
std::vector<uint8_t> ClientPacket::serialize() const
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