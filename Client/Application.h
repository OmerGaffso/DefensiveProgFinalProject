/*
Application.h class
This class will
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
    // Command handling functions
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
    void processUserInput(int choice);
    //
public:
    Application();
    ~Application();
    void run();
};

