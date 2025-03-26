/*
    ClientInfo.h

    This class stores and manages information about the current client,
    including username, client ID, public/private key pair, and methods
    to persist/load the client data from file.
*/

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
    std::string m_publicKey;         // Raw 160-byte RSA public key
    std::string m_privateKeyBase64;  // Base64-encoded DER-formatted RSA private key
public:
    ClientInfo(); // CTOR
    //
    /**
     * @brief Loads client data (username, client ID, private key) from file.
     * The private key may span multiple lines (Base64).
     * If the file is corrupted, the user will have to register again with new username.
     *
     * @param filePath Path to the client configuration file.
     * @return true if successfully loaded, false otherwise.
     */
    bool loadFromFile(const std::string& filePath); // Maybe delete the parameter.
    //
    /**
     * @brief Saves client data to file.
     *
     * @param filePath Path to the file for saving.
     */
    void saveToFile(const std::string& filePath) const; // maybe delete param
    //
    /**
     * @brief Decrypts data using the client's private RSA key.
     *
     * @param encryptedData Encrypted input (typically an AES key).
     * @return Decrypted string.
     */
    std::string decryptWithPrivateKey(const std::string& encryptedData);
    //
    /**
     * @brief prints error and resets the file contents
     */
    bool resetCorruptedFile(const std::string& filePath);
    //
    // === Getters ===
    const std::string& getUsername() const { return m_username; };
    const std::array<uint8_t, CLIENT_ID_LENGTH>& getClientId() const { return m_clientId; };
    const std::string& getPublicKey() const { return m_publicKey; };
    const std::string& getPrivateKey() const { return m_privateKeyBase64; };
    //
    // === Setters ===
    void setUsername(const std::string& username) { m_username = username; }
    void setClientId(const std::array<uint8_t, CLIENT_ID_LENGTH>& clientId) { m_clientId = clientId; }
    void setPublicKey(const std::string& publicKey) { m_publicKey = publicKey; }
    void setPrivateKey(const std::string& privateKey) { m_privateKeyBase64 = privateKey; }
};

