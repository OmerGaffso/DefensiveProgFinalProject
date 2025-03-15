#include "Application.h"
#include "Utility.h"
#include "RSAWrapper.h"
#include "Base64Wrapper.h"

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
    if (!m_network->ConnectToServer(serverIP, serverPort))
    {
        m_ui->displayError("Failed to connect to the server.\n");
        return;
    }
    //
    if (m_client.loadFromFile(m_config->getConfigFilePath()))
        m_ui->displayMessage("Client info loaded: " + m_client.getUsername());
    //
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
    try 
    {
        if (!m_client.getUsername().empty())
        {
            m_ui->displayError("Registration not allowed. User already exists.\n");
            return;
        }
        //
        std ::string username = m_ui->getUsername();
        std::vector<uint8_t> usernameData(username.begin(), username.end());
        //
        // Generate keys
        RSAPrivateWrapper privateKey;
        std::string privateKeyBase64 = Base64Wrapper::encode(privateKey.getPrivateKey());
        std::string publicKeyBase64 = Base64Wrapper::encode(privateKey.getPublicKey());
        //
        if (publicKeyBase64.size() > REGISTER_PUBLIC_KEY_LEN)
            publicKeyBase64 = publicKeyBase64.substr(0, REGISTER_PUBLIC_KEY_LEN);
        else
            publicKeyBase64.append(REGISTER_PUBLIC_KEY_LEN - publicKeyBase64.size(), '\0');
        //
        std::vector<uint8_t> payload;
        payload.insert(payload.end(), username.begin(), username.end());
        payload.resize(REGISTER_USERNAME_LEN, '\0'); // ensure username size 255
        payload.insert(payload.end(), publicKeyBase64.begin(), publicKeyBase64.end());
        //
        std::array<uint8_t, CLIENT_ID_LENGTH> emptyClientId = {};
        emptyClientId.fill(0);
        Packet packet(CODE_REGISTER_USER, payload, emptyClientId);
        //
        if (!m_network->sendPacket(packet))
        {
            m_ui->displayError("Failed to send registration request.");
            return;
        }
        //
        Packet resp;
        if (!m_network->receivePacket(resp))
        {
            m_ui->displayError("No reponse from server.");
            return;
        }
        //
        if (resp.header.code == RESP_CODE_REGISTER_SUCCCESS)         {
            std::array<uint8_t, CLIENT_ID_LENGTH> clientId;
            std::memcpy(clientId.data(), resp.payload.data(), CLIENT_ID_LENGTH);
            //
            m_client.setUsername(username);
            m_client.setClientId(clientId);
            m_client.setPrivateKey(privateKeyBase64);
            m_client.setPublicKey(publicKeyBase64);
            //
            m_client.saveToFile(m_config->getConfigFilePath());
            //
            m_ui->displayMessage("Registration successful.");
        }
        else
            m_ui->displayError("Registration failed.");
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
