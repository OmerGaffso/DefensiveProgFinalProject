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
    UI m_ui;
    //
public:
    ClientListManager();
    void updateClientList(const std::vector<std::pair<std::string, std::array<uint8_t, CLIENT_ID_LENGTH>>>& clients);
    std::optional<std::array<uint8_t, CLIENT_ID_LENGTH>> getClientId(const std::string& username) const;
    std::optional<std::string> getPublicKey(const std::array<uint8_t, CLIENT_ID_LENGTH>& clientId) const;
    void storePublicKey(const std::array<uint8_t, CLIENT_ID_LENGTH>& clientId, const std::string& publicKey);
    void pritnClientList() const;
};

