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