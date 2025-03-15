/*
    This class handles the use of files in the program - the server info 
    and user info.
*/
#pragma once
#include "Utility.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <optional>
#include <vector>

class ConfigManager
{
private:
    const std::string m_serverConfigFile = "server.info";
    const std::string m_userConfigFile   = "me.info";
    //
    bool validateIPAdder(const std::string& ip) const;
    bool validatePort(const uint16_t port) const;
public:
    std::optional<std::pair < std::string, uint16_t >> getServerInfo() const;
    std::optional<std::tuple<std::string, std::string, std::string>> getUserInfo() const;
};

