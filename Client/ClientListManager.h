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
    std::unordered_map<std::string, std::array<uint8_t, CLIENT_ID_LENGTH>> clientMap;
    UI m_ui;
    //
public:
    ClientListManager();
    void updateClientList(const std::vector<std::pair<std::string, std::array<uint8_t, CLIENT_ID_LENGTH>>>& clients);
    std::optional<std::array<uint8_t, CLIENT_ID_LENGTH>> getClientId(const std::string& username) const;
    void pritnClientList() const;
};

