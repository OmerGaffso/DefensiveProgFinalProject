#include "Application.h"
#include "Utility.h"
#include "ServerPacket.h"
#include "ClientPacket.h"
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
        //std::vector<uint8_t> usernameData(username.begin(), username.end());
        //
        // Generate keys
        RSAPrivateWrapper privateKey;
        std::string privateKeyStr = privateKey.getPrivateKey(); // Get the raw private key
        std::string publicKeyStr = privateKey.getPublicKey();   // Get the corresponding public key 
        //
        // Encode keys 
        std::string privateKeyBase64 = Base64Wrapper::encode(privateKeyStr);
        std::string publicKeyBase64 = Base64Wrapper::encode(publicKeyStr);
        //
        // Ensure the public key is exactly 160 bytes
        if (publicKeyBase64.size() > REGISTER_PUBLIC_KEY_LEN)
            publicKeyBase64 = publicKeyBase64.substr(0, REGISTER_PUBLIC_KEY_LEN);
        else
            publicKeyBase64.append(REGISTER_PUBLIC_KEY_LEN - publicKeyBase64.size(), '\0');
        //
        // Prepare payload: username (255 bytes) and public key (160 bytes).
        std::vector<uint8_t> payload;
        payload.insert(payload.end(), username.begin(), username.end());
        payload.resize(REGISTER_USERNAME_LEN, '\0'); // ensure username size 255
        payload.insert(payload.end(), publicKeyBase64.begin(), publicKeyBase64.end());
        //
        // Create default client id
        std::array<uint8_t, CLIENT_ID_LENGTH> emptyClientId = {};
        emptyClientId.fill(0);
        //
        ClientPacket packet(CODE_REGISTER_USER, payload, emptyClientId);
        //
        if (!m_network->sendPacket(packet))
        {
            m_ui->displayError("Failed to send registration request.");
            return;
        }
        //
        ServerPacket resp;
        if (!m_network->receivePacket(resp))
        {
            m_ui->displayError("No reponse from server.");
            return;
        }
        //
        if (resp.getCode() == RESP_CODE_REGISTER_SUCCCESS) 
        {
            // Extract the client ID from the payload
            std::array<uint8_t, CLIENT_ID_LENGTH> clientId;
            std::memcpy(clientId.data(), resp.getPayload().data(), CLIENT_ID_LENGTH);
            //
            // Store user info in memory and save to file
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
    try
    {
        m_ui->displayMessage("Requesting client list...\n");
        //
        // Create and send the client list request
        ClientPacket request(CODE_REQ_USER_LIST, m_client.getClientId());
        //
        if (!m_network->sendPacket(request))
        {
            m_ui->displayError("Failed to send client list request.");
            return;
        }
        //
        ServerPacket response;
        if (!m_network->receivePacket(response))
        {
            m_ui->displayError("No response from server.");
            return;
        }
        //
        // Check response code 
        // TODO - Create a function in utility that get the expected code and the code, returns bool if match.
        if (response.getCode() == RESP_CODE_GET_CLIENT_LIST)
        {
            std::vector<uint8_t> payload = response.getPayload();
            if (payload.empty())
            {
                m_ui->displayMessage("No other clients found.");
                return;
            }
            // DEBUG
            uint32_t numOfClients = payload.size() / (CLIENT_ID_LENGTH + USERNAME_MAX_LENGTH);
            m_ui->displayMessage("Number of clients recieved: " + numOfClients );
            //
            // Extract client IDs and usernames
            std::vector<std::pair<std::string, std::array<uint8_t, CLIENT_ID_LENGTH>>> clients;
            size_t offset = 0;
            //
            while (offset + CLIENT_ID_LENGTH + REGISTER_USERNAME_LEN <= payload.size())
            {
                // Extract client ID
                std::array<uint8_t, CLIENT_ID_LENGTH> clientId;
                std::memcpy(clientId.data(), &payload[offset], CLIENT_ID_LENGTH);
                offset += CLIENT_ID_LENGTH;
                //
                // Extract username (trimming trailing null)
                std::string username(reinterpret_cast<char*>(&payload[offset]), REGISTER_USERNAME_LEN);
                username.erase(std::find(username.begin(), username.end(), '\0'), username.end());
                offset += REGISTER_USERNAME_LEN;
                //
                clients.emplace_back(username, clientId);
            }
            //
            // Update local client list
            m_clientList.updateClientList(clients);
            //
            m_clientList.pritnClientList();
        }
        else
            m_ui->displayError("Failed to retrieve client list.");
    }
    catch (const std::runtime_error& e)
    {
        m_ui->displayError(e.what());
    }
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
