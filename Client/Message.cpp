#include "Message.h"

std::vector<uint8_t>  Message::serializeMessage() const
{
    std::vector<uint8_t> buff;
    buff.insert(buff.end(), clientID, clientID + CLIENT_ID_SIZE);
    buff.push_back(clientVersion);
    buff.push_back(reqCode >> 8);
    buff.push_back(reqCode & 0xFF);
    buff.push_back((payloadSize >> 24) & 0xFF);
    buff.push_back((payloadSize >> 16) & 0xFF);
    buff.push_back((payloadSize >> 8) & 0xFF);
    buff.push_back(payloadSize & 0xFF);
    buff.insert(buff.end(), payload.begin(), payload.end());
    return buff;
}