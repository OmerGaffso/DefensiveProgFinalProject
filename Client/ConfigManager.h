/*
    ConfigManager class
    Handles reading configuration files: server information and user credentials.
    - server.info: contains IP:port
    - me.info: contains username, client_id (hex), and private key (base64)
*/
#pragma once
#include "Utility.h"
#include "UI.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <optional>
#include <vector>

class ConfigManager
{
private:
    const std::string   m_serverConfigFile = "server.info"; //< File containing server IP and port 
    const std::string   m_userConfigFile   = "me.info"; //< File containing user credentials
    std::unique_ptr<UI> m_ui;
    //
    /**
     * Validates the IP address (IPv4 format or "localhost").
     */
    bool validateIPAdder(const std::string& ip) const;
    /**
     * Validates port number is in the range [1024, 65535].
     */
    bool validatePort(const uint16_t port) const;
public:
    /**
     * Reads the server info from the file (ip:port).
     * @return std::optional pair of IP and port if successful.
     */
    std::optional<std::pair < std::string, uint16_t >> getServerInfo() const;
    /**
     * Reads the user info (username, client ID, private key).
     * @return std::optional tuple of username, client ID hex, and base64 private key.
     */
    std::optional<std::tuple<std::string, std::string, std::string>> getUserInfo() const;
    //
    /**
     * Gets the path to the user config file.
     */
    std::string getConfigFilePath() { return m_userConfigFile; }
};

