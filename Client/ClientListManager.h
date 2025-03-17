#pragma once
#include "Utility.h"
#include <unordered_map>
#include <array>
#include <vector>
#include <optional>
#include "UI.h"

class ClientListManager
{
private:
    std::unordered_map<std::string, std::array<uint8_t, CLIENT_ID_LENGTH>> clientMap; // maps username to client ID
    std::unordered_map<std::array<uint8_t, CLIENT_ID_LENGTH>, std::string, ArrayHasher> publicKeyMap; // maps client ID to public key
    std::unordered_map<std::array<uint8_t, CLIENT_ID_LENGTH>, std::vector<uint8_t>, ArrayHasher> m_symmetricKeys; // maps client ID to symmetric key
    UI m_ui;
    //
public:
    ClientListManager();
    void updateClientList(const std::vector<std::pair<std::string, std::array<uint8_t, CLIENT_ID_LENGTH>>>& clients);
    std::optional<std::array<uint8_t, CLIENT_ID_LENGTH>> getClientId(const std::string& username) const;
    void pritnClientList() const;
    std::optional<std::string> getUsername(const std::array<uint8_t, CLIENT_ID_LENGTH>& clientId) const;
    //
    // Public key methods:
    std::optional<std::string> getPublicKey(const std::array<uint8_t, CLIENT_ID_LENGTH>& clientId) const;
    void storePublicKey(const std::array<uint8_t, CLIENT_ID_LENGTH>& clientId, const std::string& publicKey);
    //
    // Symmetric key methods:
    void storeSymmetricKey(const std::array<uint8_t, CLIENT_ID_LENGTH>& clientId, const std::vector<uint8_t>& symmetricKey);
    std::optional<std::vector<uint8_t>> getSymmetricKey(const std::array<uint8_t, CLIENT_ID_LENGTH>& clientId) const;
};

