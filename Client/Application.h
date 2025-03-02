/*
Application.h class
This class will
*/
#pragma once
#include <unordered_map>
#include <functional>
#include "UI.h"
#include "ConfigManager.h"

class Application
{
private:
    UI                                             m_ui;
    bool                                           m_appRunning;
    ConfigManager                                  m_config;
    std::unordered_map<int, std::function<void()>> m_commandMap;
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
    void run();
};

