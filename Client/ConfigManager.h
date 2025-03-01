/*
    This class handles the use of files in the program - the server info 
    and user info.
*/
#pragma once
#include "Utility.h"
#include <fstream>

#define MIN_PORT 1024
#define MAX_PORT 65535

class ConfigManager
{
private:
    std::string m_serverIP;
    int         m_serverPort;
    bool        m_isValid;
    std::string m_errMsg;

    // private methods:
    bool parseConfigFile(const std::string& filename);
    bool validateIPAdder(const std::string& ip);
    bool validatePort(const int port);

public:
    ConfigManager(const std::string& configFile = "server.info");

    bool isValid() const;
    std::string getErrorMsg() const;

    std::string getServerIp() const;
    int getServerPort() const;

};

