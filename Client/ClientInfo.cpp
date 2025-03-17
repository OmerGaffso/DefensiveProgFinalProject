#include "ClientInfo.h"
#include "AESWrapper.h"
#include "RSAWrapper.h"
#include "Base64Wrapper.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <stdexcept>

ClientInfo::ClientInfo() : m_clientId{}, m_symmetricKey{} {};

bool ClientInfo::loadFromFile(const std::string& filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open())
        return false;
    //
    std::string username, clientIdHex, privateKey;
    if (!std::getline(file, username) || !std::getline(file, clientIdHex) || !std::getline(file, privateKey))
        return false;
    //
    // Convert hex clientId to array
    std::array<uint8_t, CLIENT_ID_LENGTH> clientId;
    for (size_t i = 0; i < CLIENT_ID_LENGTH; ++i)
        clientId[i] = static_cast<uint8_t>(std::stoi(clientIdHex.substr(i * 2, 2), nullptr, CLIENT_ID_LENGTH));
    //
    // Store values
    m_username = username;
    m_clientId = clientId;
    m_privateKeyBase64 = privateKey;
    //
    return true;
}

// Save client info to file
void ClientInfo::saveToFile(const std::string& filePath) const
{
    std::ofstream file(filePath, std::ios::trunc);
    if (!file.is_open())
        throw std::runtime_error("Failed to open file: " + filePath);
    //
    // Convert clientId to hex
    std::ostringstream clientIdHex;
    for (auto byte : m_clientId)
    {
        clientIdHex << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }
    //
    file << m_username << "\n"
        << clientIdHex.str() << "\n"
        << m_privateKeyBase64 << "\n";
    //
    file.close();
}

bool ClientInfo::hasSymmetricKey() const
{
    return !m_symmetricKey.empty();
}
//
std::string ClientInfo::decryptMessage(const std::vector<uint8_t>& encryptedMessage)
{
    if (hasSymmetricKey)
    {
        AESWrapper aes(m_symmetricKey.data(), AESWrapper::DEFAULT_KEYLENGTH);
        return aes.decrypt(reinterpret_cast<const char*>(encryptedMessage.data()), encryptedMessage.size());
    }
}

std::string ClientInfo::decryptWithPrivateKey(const std::string& encryptedData)
{
    RSAPrivateWrapper privateKey(Base64Wrapper::decode(m_privateKeyBase64));
    return privateKey.decrypt(encryptedData);
}
