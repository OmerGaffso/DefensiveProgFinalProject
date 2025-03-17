#pragma once

#include "Utility.h"
#include <array>
#include <vector>
#include <string>
#include <optional>

class ClientInfo
{
private:
    std::string m_username;
    std::array<uint8_t, CLIENT_ID_LENGTH> m_clientId;
    std::string m_publicKeyBase64;
    std::string m_privateKeyBase64;
public:
    ClientInfo();
    //
    // Load client info from file
    bool loadFromFile(const std::string& filePath); // Maybe delete the parameter.
    //
    // Save client info to file
    void saveToFile(const std::string& filePath) const; // maybe delete param
    //
    std::string decryptMessage(const std::vector<uint8_t>& encryptedMessage);
    std::string decryptWithPrivateKey(const std::string& encryptedData);
    //
    // Getters:
    const std::string& getUsername() const { return m_username; };
    const std::array<uint8_t, CLIENT_ID_LENGTH>& getClientId() const { return m_clientId; };
    const std::string& getPublicKey() const { return m_publicKeyBase64; };
    const std::string& getPrivateKey() const { return m_privateKeyBase64; };
    //
    // Setters:
    void setUsername(const std::string& username) { m_username = username; }
    void setClientId(const std::array<uint8_t, CLIENT_ID_LENGTH>& clientId) { m_clientId = clientId; }
    void setPublicKey(const std::string& publicKey) { m_publicKeyBase64 = publicKey; }
    void setPrivateKey(const std::string& privateKey) { m_privateKeyBase64 = privateKey; }
};

