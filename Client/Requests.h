#pragma

#include <vector>
#include <cstdint>
#include <cstring>

#define CLIENT_ID_SIZE 16U

struct Message
{
    uint8_t                 clientID[CLIENT_ID_SIZE]; // unique client identifier
    uint8_t                 clientVersion; 
    uint16_t                reqCode;       // request code
    uint32_t                payloadSize;
    std::vector<uint8_t>    payload;       // request payload


    std::vector<uint8_t> serializeMessage() const
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
};