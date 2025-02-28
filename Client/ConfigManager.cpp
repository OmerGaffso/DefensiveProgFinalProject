#include "ConfigManager.h"
#include <regex>
#include <stdexcept>

bool ConfigManager::parseConfigFile(const std::string& filename)
{
    std::ifstream file(filename);
    //
    if (!file.is_open())
    {
        m_errMsg = "Could not open config file: " + filename;
        return false;
    }
    //
    std::string line;
    if (!std::getline(file, line))
    {
        m_errMsg = "Config file is empty";
        return false;
    }
    //
    line = trim(line); // trim whitespaces.
    //
    // Expected format: ip_addr:port
    size_t separator_pos = line.find(':');
    if (separator_pos == std::string::npos)
    {
        m_errMsg = "Invalid format in config file. Expected: 'ip_addr:port'";
        return false;
    }
    //
    std::string ip = line.substr(0, separator_pos);
    std::string portStr = line.substr(separator_pos + 1);
    //
    // trim to handle cases like "ip : port"
    ip = trim(ip);
    portStr = trim(portStr);
    //
    // Validate IP address
    if (!validateIPAdder(ip))
    {
        m_errMsg = "Invalid IP address: " + ip;
        return false;
    }
    //
    m_serverIP = ip; // IP addr OK
    //
    // Parse and validate port
    try
    {
        int port = std::stoi(portStr);
        if (!validatePort(port))
        {
            m_errMsg = "Invalid port number: " + portStr;
            return false;
        }
        m_serverPort = port; // port OK
    }
    catch (const std::exception& e)
    {
        m_errMsg = "Invalid port number: " + portStr;
        return false;
    }
    //
    return true;
}

bool ConfigManager::validateIPAdder(const std::string& ip)
{
    std::regex ipv4_pattern(
        "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
        "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
        "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
        "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)"
    );
    //
    if (ip == "localhost")
        return true;
    
    return std::regex_match(ip,  ipv4_pattern);
}

bool ConfigManager::validatePort(const int port)
{
    // Validate that the port is between 1024 and 65535
    return (port >= MIN_PORT && port <= MAX_PORT);
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