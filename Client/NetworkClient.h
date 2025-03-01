/*
    This Class will handle the socket communication with the server
*/

#pragma once
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <cstdint>
#include <cstring>
#include <vector>
#include <stdexcept>
#include <iomanip>
#include <sstream>
#include "Utility.h"

// Header fields
constexpr uint8_t CLIENT_ID_SIZE     = 16;
constexpr uint8_t VERSION_SIZE       = 1;
constexpr uint8_t CODE_SIZE          = 2;
constexpr uint8_t PAYLOAD_SIZE_FIELD = 4;
constexpr uint8_t HEADER_SIZE  = CLIENT_ID_SIZE + VERSION_SIZE + CODE_SIZE + PAYLOAD_SIZE_FIELD;

// Register field sizes
#define PUBLIC_KEY_SIZE    160U;

enum class Commands : uint8_t
{
    CMD_REGISTER         = 600,
    CMD_CLIENT_LIST      = 601,
    CMD_PUBLIC_KEY_REQ   = 602,
    CMD_SEND_MSG         = 603,
    CMD_GET_PENDING_MSGS = 604,
};

enum class MessageType : uint8_t
{
    REQ_SYMMETRIC_KEY  = 1,
    SEND_SYMMETRIC_KEY = 2,
    SENT_TEXT_MSG      = 3,
    SEND_FILE          = 4,
};

struct RequestHeader
{
    uint8_t clientId[CLIENT_ID_SIZE];
    uint8_t version;
    uint16_t code;
    uint32_t payloadSize;

    RequestHeader() : clientId{}, version(2), code(0), payloadSize(0) {}

    std::vector<uint8_t> serialize() const;
    bool deserialize(const std::vector<uint8_t>& buff);
};


class NetworkClient
{
private:
    boost::asio::io_context      io_context;
    boost::asio::ip::tcp::socket sock;
    std::string                  m_serverIP;
    int                          m_serverPort;

    // Buffer management
    std::vector<uint8_t> m_clientId;
    uint8_t m_clientVersion;
    std::string m_userName;
    std::vector<uint8_t> m_publicKey;
    //
    std::vector<uint8_t> m_rxBuff;
    const size_t m_buffSize = 8192; // TODO - CHANGE LATER
    //
    std::string m_lastError;
    bool m_connected;
    bool m_registered;

public:
    
    NetworkClient(const std::string& serverIp, int serverPort, const std::string& username, const std::vector<uint8_t>& publicKey = {});
    ~NetworkClient();
    //
    bool connect();
    bool disconnect();
    //
    bool registerUser();
    bool getClientList(std::vector<std::string>& clientList);
    bool getUserPublicKey(const std::vector<uint8_t>& targetUserId);
    bool requestSymmetricKey(const std::vector<uint8_t>& targetId);
    bool sendSymmetricKey(const std::vector<uint8_t>& targetId, const std::vector<uint8_t>& encryptedKey);
    bool sendFile(const std::vector<uint8_t>& targetId, const std::vector<uint8_t>& encryptedFile);
    bool getPendingMessages(std::vector<std::tuple<std::vector<uint8_t>, MessageType, std::vector<uint8_t>>>& messages);
    bool sendMessage(const std::vector<uint8_t> targetId, MessageType type, const std::vector<uint8_t>& content);
    bool sendMessage(const std::string& targetUserIdStr, MessageType type, const std::vector<uint8_t>& content);
    // Text message convenience method 
    bool sendTextMessage(const std::vector<uint8_t>& targetId, const std::string& message);
    //
    // Getters:
    bool isConnected() const;
    bool isRegistered() const;
    std::string getLastError() const;
    std::string getUsername() const;
    std::string getClientIdString() const;
    //
private:
    bool ensureConnectedAndRegistered();
    bool sendRequest(uint16_t code, const std::vector<uint8_t>& payload);
    bool receiveResponse(RequestHeader& header, std::vector<uint8_t>& payload);
};

