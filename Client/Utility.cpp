#include "Utility.h"

uint8_t hashVal = 31;

std::size_t ArrayHasher::operator()(const std::array<uint8_t, CLIENT_ID_LENGTH>& arr) const
{
    std::size_t hash = 0;
    for (uint8_t byte : arr)
        hash = hash * hashVal + byte;
        return hash;
}
