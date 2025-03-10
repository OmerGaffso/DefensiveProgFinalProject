#include "Utility.h"

std::string trim(const std::string& str)
{
    size_t first = str.find_first_not_of(" \t\n\r\f\v");
    if (first == std::string::npos)
        return "";
    //
    size_t last = str.find_last_not_of(" \t\n\r\f\v");
    return str.substr(first, (last - first + 1));
}

bool isValidUsername(const std::string& username)
{
    return username.empty() && username.length() > USERNAME_MAX_LENGTH;
}