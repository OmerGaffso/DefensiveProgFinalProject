#include "Application.h"
#include "Utility.h"

Application::Application() : m_appRunning(true)
{
    m_commandMap[110] = [this]() {registerUser(); };
    m_commandMap[120] = [this]() {requestClientList(); };
    m_commandMap[130] = [this]() {requestPublicKey(); };
    m_commandMap[140] = [this]() {requestPendingMessages(); };
    m_commandMap[150] = [this]() {sendTextMessage(); };
    m_commandMap[151] = [this]() {requestSymmetricKey(); };
    m_commandMap[152] = [this]() {sendSymmetricKey(); };
    m_commandMap[153] = [this]() {sendFile(); };
    m_commandMap[0]   = [this]() {exitProgram(); };

}
//
void Application::run()
{
    auto serverInfo = m_config.getServerInfo();
    if (!serverInfo)
    {
        m_ui.displayError("Failed to load server info.\n");
        return;
    }
    std::string serverIP   = serverInfo->first;
    uint16_t    serverPort = serverInfo->second;
    //
    // Doesn't matter if the user exist (my.info) display the menu.
    
    while (m_appRunning)
    {
        m_ui.displayMenu();
        int choice;
        std::cin >> choice;
        processUserInput(choice);
    }
}
//
void Application::registerUser()
{
    if (m_config.getUserInfo())
    {
        m_ui.displayError("Registration not allowed. User already exists.\n");
        return;
    }
    //
    std::string username;
    std::cout << "Enter username: ";
    std::getline(std::cin >> std::ws, username);
    //
    if (isValidUsername(username))
    {
        m_ui.displayError("Invalid username.\n");
        return;
    }
    //
    // Send request to server TODO - add implementation
    m_ui.displayMessage("Registering user: " + username);
}
//
void Application::requestClientList()
{
    m_ui.displayMessage("Requesting client list...\n");
    // TODO- add logic here
}
//
void Application::requestPublicKey()
{
    std::string targetUsername;
    std::cout << "Enter target username: ";
    std::getline(std::cin >> std::ws, targetUsername);
    //
    if (isValidUsername(targetUsername))
    {
        m_ui.displayError("Invalid target username.\n");
        return;
    }
    //
    // Send request to server
    m_ui.displayMessage("Requesting public key for: " + targetUsername);
    // TODO - add logic
}
//
void Application::requestPendingMessages()
{

}
//
void Application::requestSymmetricKey()
{

}
//
void Application::sendTextMessage()
{

}
//
void Application::sendSymmetricKey()
{

}
//
void Application::sendFile()
{

}
//
void Application::exitProgram()
{
    m_ui.displayMessage("Exiting application...\n");
    m_appRunning = false;
}
//
void Application::processUserInput(int choice)
{
    auto it = m_commandMap.find(choice);
    if (it != m_commandMap.end())
        it->second();
    else
        m_ui.displayError("Invalid option. Try again.\n");
}
