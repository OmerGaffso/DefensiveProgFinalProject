#include "ConfigManager.h"
#include <regex>

bool ConfigManager::validateIPAdder(const std::string& ip) const
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

    return std::regex_match(ip, ipv4_pattern);
}

bool ConfigManager::validatePort(const uint16_t port) const
{
    // Validate that the port is between 1024 and 65535
    return (port >= MIN_PORT && port <= MAX_PORT);
}

std::optional<std::pair < std::string, uint16_t >> ConfigManager::getServerInfo() const
{
    std::ifstream file(m_serverConfigFile);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open file: " + m_serverConfigFile);
        return std::nullopt;
    }
    // debug- print all file contents todo - delete later!
    std::cout << file.rdbuf(); 
    file.clear();
    file.seekg(0, std::ios::beg);
    //
    std::string line;
    if (!std::getline(file, line))
    {
        throw std::runtime_error("Error: " + m_serverConfigFile + " empty or invalid\n");
        return std::nullopt;
    }
    //
    size_t colonPos = line.find(':');
    if (colonPos == std::string::npos)
    {
        throw std::runtime_error("Error: Invalid server info format\n");
        return std::nullopt;
    }
    //
    std::string ip = line.substr(0, colonPos);
    uint16_t port = static_cast<uint16_t>(std::stoi(line.substr(colonPos + 1)));
    //
    if (!validateIPAdder(ip))
    {
        throw std::runtime_error("Error: Invalid ip address format\n");
        return std::nullopt;
    }
    if (!validatePort(port))
    {
        throw std::runtime_error("Error: Invalid port\n");
        return std::nullopt;
    }
    //
    // TODO- DEBUG, DELETE PRINT
    std::cout << "\nIP: " << ip << " PORT: " << port << std::endl;
    //
    return std::make_pair(ip, port);
}

std::optional<std::tuple<std::string, std::string, std::string>> ConfigManager::getUserInfo() const
{
    std::ifstream file(m_userConfigFile);
    if (!file.is_open())
    {
        std::cerr << ("Warning: " + m_userConfigFile + " not found. Assuming first-time registration.\n");
        return std::nullopt;
    }
    std::cout << file.rdbuf(); // debug- print all file contents todo - delete later!
    //
    std::string username, uniqueId, privateKey;
    if (!std::getline(file, username) || !std::getline(file, uniqueId) || !std::getline(file, privateKey))
    {
        std::cerr << ("Error: Invalid user info file format\n");
        return std::nullopt;
    }
    //
    return std::make_tuple(username, uniqueId, privateKey);
}

