#include "NetworkClient.h"

std::vector<uint8_t> RequestHeader::serialize() const
{
    std::vector<uint8_t> buff(HEADER_SIZE);
    size_t offset = 0;
    //
    // Copy client ID
    std::memcpy(buff.data() + offset, clientId, CLIENT_ID_SIZE);
    offset += CLIENT_ID_SIZE;
    //
    // Copy Version
    buff[offset] = version;
    offset += VERSION_SIZE;
    //
    // Copy code (network byte order)
    uint16_t netCode = htons(code);
    std::memcpy(buff.data() + offset, &netCode, CODE_SIZE);
    offset += CODE_SIZE;
    //
    // Copy payload size (network byte order)
    uint32_t netSize = htonl(payloadSize);
    std::memcpy(buff.data() + offset, &netSize, PAYLOAD_SIZE_FIELD);
    //
    return buff;
}

bool RequestHeader::deserialize(const std::vector<uint8_t>& buff)
{
    if (buff.size() < HEADER_SIZE)
        return false;
    //
    size_t offset = 0;
    //
    // Extract client ID
    std::memcpy(clientId, buff.data() + offset, CLIENT_ID_SIZE);
    offset += CLIENT_ID_SIZE;
    //
    // Extract version
    version = buff[offset];
    offset += VERSION_SIZE;
    //
    // Extract code
    uint16_t netCode;
    std::memcpy(&netCode, buff.data() + offset, CODE_SIZE);
    code = ntohs(netCode);
    offset += CODE_SIZE;
    //
    // Extract payload size
    uint32_t netSize;
    std::memcpy(&netSize, buff.data() + offset, PAYLOAD_SIZE_FIELD);
    payloadSize = ntohl(netSize);
    //
    return true;
}

NetworkClient::NetworkClient(const std::string& serverIp, int serverPort, const std::string& username, const std::vector<uint8_t>& publicKey = {})
    : sock(io_context), m_serverIP(serverIp), m_clientVersion(2), m_userName(username),
    m_publicKey(publicKey), m_rxBuff(m_buffSize, 0), m_connected(false), m_registered(false)
{
    
}
//
NetworkClient::~NetworkClient()
{

}
//
bool NetworkClient::connect()
{

}
//
bool NetworkClient::disconnect()
{
}
//
bool NetworkClient::registerUser()
{

}
//
bool NetworkClient::getClientList(std::vector<std::string>& clientList)
{

}
//
bool NetworkClient::getUserPublicKey(const std::vector<uint8_t>& targetUserId)
{

}
//
bool NetworkClient::requestSymmetricKey(const std::vector<uint8_t>& targetId)
{

}
//
bool NetworkClient::sendSymmetricKey(const std::vector<uint8_t>& targetId, const std::vector<uint8_t>& encryptedKey)
{

}
//
bool NetworkClient::sendFile(const std::vector<uint8_t>& targetId, const std::vector<uint8_t>& encryptedFile)
{

}
//
bool NetworkClient::getPendingMessages(std::vector<std::tuple<std::vector<uint8_t>, MessageType, std::vector<uint8_t>>>& messages)
{

}
//
bool NetworkClient::sendMessage(const std::vector<uint8_t>& targetId, MessageType type, const std::vector<uint8_t>& content)
{

}
//
bool NetworkClient::sendMessage(const std::string& targetUserIdStr, MessageType type, const std::vector<uint8_t>& content)
{

}
//
bool NetworkClient::sendTextMessage(const std::vector<uint8_t>& targetId, const std::string& message)
{

}
//
bool NetworkClient::isConnected() const
{

}
//
bool NetworkClient::isRegistered() const
{

}
//
std::string NetworkClient::getLastError() const
{

}
//
std::string NetworkClient::getUsername() const
{

}
//
std::string NetworkClient::getClientIdString() const
{

}
//
//
bool NetworkClient::ensureConnectedAndRegistered()
{
}
//
bool NetworkClient::sendRequest(uint16_t code, const std::vector<uint8_t>& payload)
{
    if (!m_connected)
    {
        m_lastError = "Not connected to server";
        return false;
    }
    //
    RequestHeader header;
    std::memcpy(header.clientId, m_clientId.data(), CLIENT_ID_SIZE);
    header.version = m_clientVersion;
    header.code = code;
    header.payloadSize = static_cast<uint32_t>(payload.size());
    //
    // Serialize header
    std::vector<uint8_t> requestBuff = header.serialize();
    //
    // Append payload if exists
    if (!payload.empty())
        requestBuff.insert(requestBuff.end(), payload.begin(), payload.end());
    //
    // Transmit the request
    boost::system::error_code ec;
    boost::asio::write(sock, boost::asio::buffer(requestBuff), ec);
    if (ec)
    {
        m_lastError = "Failed to send request: " + ec.message();
        return false;
    }
    //
    return true;
}
//
bool NetworkClient::receiveResponse(RequestHeader& header, std::vector<uint8_t>& payload)
{
    if (!m_connected)
    {
        m_lastError = "Not connected to server";
        return false;
    }
    //
    // Read header
    std::vector<uint8_t> respBuff(HEADER_SIZE);
    boost::system::error_code ec;
    boost::asio::read(sock, boost::asio::buffer(respBuff), ec);
    if (ec)
    {
        m_lastError = "Failed to receive header: " + ec.message();
        return false;
    }
    //
    // Deserialize the header
    if (!header.deserialize(respBuff))
    {
        m_lastError = "Failed to parse response header";
        return false;
    }
    //
    // Read the full payload (if exists)
    if (header.payloadSize > 0)
    {
        respBuff.resize(HEADER_SIZE + header.payloadSize);
        boost::asio::read(sock, boost::asio::buffer(respBuff.data() + HEADER_SIZE, header.payloadSize), ec);
        if (ec)
        {
            m_lastError = "Failed to receive payload: " + ec.message();
            return false;
        }
    }
    //
    // Extract payload
    payload.assign(respBuff.begin() + HEADER_SIZE, respBuff.end());
}
