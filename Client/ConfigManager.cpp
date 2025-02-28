#include "ConfigManager.h"
#include <regex>
#include <stdexcept>

bool ConfigManager::parseConfigFile(const std::string& filename)
{
    std::ifstream file(filename);

    if (!file.is_open())
    {
        m_errMsg = "Could not open config file: " + filename;
        return false;
    }

    std::string line;
    if (!std::getline(file, line))
    {
        m_errMsg = "Config file is empty";
        return false;
    }

    line = trim(line); // trim whitespaces.
    //
    // Expected format: ip_addr:port
    size_t separator_pos = line.find(':');
    if (separator_pos == std::string::npos)
    {
        m_errMsg = "Invalid format in config file. Expected: 'ip_addr:port'";
        return false;
    }


    return true;
}

bool ConfigManager::validateIPAdder(const std::string& ip)
{

    return true;
}

bool ConfigManager::validatePort(const int port)
{

    return true;
}

ConfigManager::ConfigManager(const std::string& configFile = "server.info")
    : m_serverPort(0), m_isValid(false)
{
    m_isValid = parseConfigFile(configFile);
}

bool ConfigManager::isValid() const
{
    return m_isValid;
}

std::string ConfigManager::getErrorMsg() const
{
    return m_errMsg;
}

std::string ConfigManager::getServerIp() const
{
    if (!m_isValid)
        throw std::runtime_error("Configuration isn't valid\n");
    return m_serverIP;
}

int ConfigManager::getServerPort() const
{
    if (!m_isValid)
        throw std::runtime_error("Configuration isn't valid\n");
    return m_serverPort;
}