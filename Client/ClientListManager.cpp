#include "ClientListManager.h"
#include <iostream>

ClientListManager::ClientListManager()
{
    clientMap.clear();
}

void ClientListManager::updateClientList(const std::vector<std::pair<std::string, std::array<uint8_t, CLIENT_ID_LENGTH>>>& clients)
{
    clientMap.clear();
    for (const auto& client : clients)
        clientMap[client.first] = client.second;
}
//
std::optional<std::array<uint8_t, CLIENT_ID_LENGTH>> ClientListManager::getClientId(const std::string& username) const
{
    auto it = clientMap.find(username);
    if (it != clientMap.end())
        return it->second;
    //
    return std::nullopt;
}
//

std::optional<std::string> ClientListManager::getPublicKey(const std::array<uint8_t, CLIENT_ID_LENGTH>& clientId) const
{
    auto it = publicKeyMap.find(clientId);
    if (it != publicKeyMap.end())
        return it->second;
    return std::nullopt;
}
//
void ClientListManager::storePublicKey(const std::array<uint8_t, CLIENT_ID_LENGTH>& clientId, const std::string& publicKey)
{
    publicKeyMap[clientId] = publicKey;
}
//
void ClientListManager::pritnClientList() const
{
    if (clientMap.empty())
    {
        m_ui.displayMessage("No users registered.");
        return;
    }
    //
    m_ui.displayMessage("Registered users:");
    for (const auto& client : clientMap)
        m_ui.displayMessage("- " + client.first);
}
//
std::optional<std::string> ClientListManager::getUsername(const std::array<uint8_t, CLIENT_ID_LENGTH>& clientId) const
{
    for (const auto& [username, id] : clientMap)
    {
        if (std::equal(id.begin(), id.end(), clientId.begin()))
            return username;
    }
    return std::nullopt;
}
//
// Store symmetric key for specific client ID
void ClientListManager::storeSymmetricKey(const std::array<uint8_t, CLIENT_ID_LENGTH>& clientId, const std::vector<uint8_t>& symmetricKey)
{
    m_symmetricKeys[clientId] = symmetricKey;
}
//
// Retrieve symmetric key for specific client ID
std::optional<std::vector<uint8_t>> ClientListManager::getSymmetricKey(const std::array<uint8_t, CLIENT_ID_LENGTH>& clientId) const
{
    auto it = m_symmetricKeys.find(clientId);
    if (it != m_symmetricKeys.end())
        return it->second;
    //
    return std::nullopt; // No key found
}
