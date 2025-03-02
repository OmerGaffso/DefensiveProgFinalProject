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

#define MIN_PORT 1024
#define MAX_PORT 65535

class ConfigManager
{
private:
    const std::string m_serverConfigFile = "server.info";
    const std::string m_userConfigFile   = "my.info";

    //std::string m_serverIP;
    //int         m_serverPort;
    //bool        m_isValid;
    //std::string m_errMsg;

    // private methods:
    //bool parseConfigFile(const std::string& filename);
    bool validateIPAdder(const std::string& ip);
    bool validatePort(const int port);

public:
    std::optional<std::pair < std::string, uint16_t >> getServerInfo() const;
    std::optional<std::tuple<std::string, std::string, std::string>> getUserInfo() const;
    void createUserInfo(const std::string& username);

    //bool isValid() const;
    //std::string getErrorMsg() const;

    //std::string getServerIp() const;
    //int getServerPort() const;

};

