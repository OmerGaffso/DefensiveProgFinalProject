#include "Application.h"
#include "Utility.h"

Application::Application() : m_appRunning(true)
{
    std::cout << "Starting application:\n";
    //
    m_ui      = std::make_unique<UI>();
    m_config  = std::make_unique<ConfigManager>();
    m_network = std::make_unique<NetworkManager>();
    //
    m_commandMap =
    {
        {110, [this]() { registerUser(); }},
        {120, [this]() { requestClientList(); }},
        {130, [this]() { requestPublicKey(); }},
        {140, [this]() { requestPendingMessages(); }},
        {150, [this]() { sendTextMessage(); }},
        {151, [this]() { requestSymmetricKey(); }},
        {152, [this]() { sendSymmetricKey(); }},
        {153, [this]() { sendFile(); }},
        {0  , [this]() { exitProgram(); }},

    };
}
//
Application::~Application()
{
    m_network->disconnect();
}

//
void Application::run()
{
    auto serverInfo = m_config->getServerInfo();
    if (!serverInfo)
    {
        m_ui->displayError("Failed to load server info.\n");
        return;
    }
    std::string serverIP   = serverInfo->first;
    uint16_t    serverPort = serverInfo->second;
    //
    // Doesn't matter if the user exist (my.info) display the menu.
    
    while (m_appRunning)
    {
        m_ui->displayMenu();
        int choice = m_ui->getUserInput();
        processUserInput(choice);
    }
}
//
void Application::registerUser()
{
    if (m_config->getUserInfo())
    {
        m_ui->displayError("Registration not allowed. User already exists.\n");
        return;
    }
    //
    try 
    {
        std ::string username = m_ui->getUsername();
        // Send request to server TODO - add implementation
        m_ui->displayMessage("Registering user: " + username);
    }
    catch (const std::runtime_error& e)
    {
        m_ui->displayError(e.what());
        return;
    }
}
//
void Application::requestClientList()
{
    m_ui->displayMessage("Requesting client list...\n");
    // TODO- add logic here
}
//
void Application::requestPublicKey()
{
    try
    {
        std::string targetUsername = m_ui->getTargetUsername();
        // TODO - add logic
        m_ui->displayMessage("Requesting public key for: " + targetUsername);
    }
    catch (const std::runtime_error& e)
    {
        m_ui->displayError(e.what());
    }
}
//
void Application::requestPendingMessages()
{
    m_ui->displayMessage("Requesting pending messages...\n");
    // TODO- add logic here
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
    m_ui->displayMessage("Exiting application...\n");
    m_appRunning = false;
}
//
void Application::processUserInput(int choice)
{
    auto it = m_commandMap.find(choice);
    if (it != m_commandMap.end())
        it->second();
    else
        m_ui->displayError("Invalid option. Try again.\n");
}
