/*
Application.h class
This class will handle the main operations of the application.
*/
#pragma once
#include <unordered_map>
#include <functional>
#include "UI.h"
#include "ConfigManager.h"
#include "NetworkManager.h"
#include "ClientInfo.h"
#include "ClientListManager.h"

class Application
{
private:
    std::unique_ptr<UI>                            m_ui;
    std::unique_ptr<ConfigManager>                 m_config;
    std::unique_ptr<NetworkManager>                m_network;
    //
    bool                                           m_appRunning;
    ClientInfo                                     m_client;
    ClientListManager                              m_clientList;
    //
    std::unordered_map<uint16_t, std::function<void()>> m_commandMap;
    //
    // === Command handling functions ===
    void registerUser();
    void requestClientList();
    void requestPublicKey();
    void requestPendingMessages();
    void requestSymmetricKey();
    void sendTextMessage();
    void sendSymmetricKey();
    void sendFile();
    void exitProgram();
    //
    void receiveAndHandleResponse(uint16_t expectedCode, std::function<void(std::vector<uint8_t>)> handler);
    bool sendClientPacket(uint16_t code, const std::vector<uint8_t>& payload, const std::array<uint8_t, CLIENT_ID_LENGTH>& senderId);
    void processUserInput(int choice);
    std::string processMessage(
        const std::array<uint8_t, CLIENT_ID_LENGTH>& senderId,
        uint8_t messageType,
        const std::vector<uint8_t>& messageContent);
    //
    std::string handleSymmetricKeyResponse(const std::array<uint8_t, CLIENT_ID_LENGTH>& senderId,
        const std::vector<uint8_t>& encryptedKey);
    std::string handleIncomingFile(const std::array<uint8_t, CLIENT_ID_LENGTH>& senderId,
        const std::vector<uint8_t>& encryptedFile);
    std::string handleTextMessage(
        const std::array<uint8_t, CLIENT_ID_LENGTH>& senderId,
        const std::vector<uint8_t>& encryptedMessage);
    //
    //
    static std::string getTempDirectory();
    std::vector<uint8_t> constructMessagePayload(
        const std::array<uint8_t, CLIENT_ID_LENGTH>& recipientId,
        uint8_t messageType,
        const std::vector<uint8_t>& messageContent);
    //
public:
    Application();
    ~Application();
    void run();
};

