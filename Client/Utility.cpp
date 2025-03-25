#include "Utility.h"

constexpr uint8_t hashVal = 31;

std::size_t ArrayHasher::operator()(const std::array<uint8_t, CLIENT_ID_LENGTH>& arr) const
{
    std::size_t hash = 0;
    for (uint8_t byte : arr)
        hash = hash * hashVal + byte;
    return hash;
}
//
std::string toHex(const std::array<uint8_t, CLIENT_ID_LENGTH>& data)
{
    std::ostringstream oss;
    for (uint8_t byte : data)
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    return oss.str();
}

std::string toHex(const std::vector<uint8_t>& data)
{
    std::ostringstream oss;
    for (uint8_t byte : data)
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    return oss.str();
}

std::string toHex(const std::string& data)
{
    return toHex(std::vector<uint8_t>(data.begin(), data.end()));
}

