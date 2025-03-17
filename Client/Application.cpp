#include "Application.h"
#include "Utility.h"
#include "ServerPacket.h"
#include "ClientPacket.h"
#include "RSAWrapper.h"
#include "Base64Wrapper.h"

static bool                 isRegistered = false;
static std::vector<uint8_t> emptyPayload;


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
        {OPT_REGISTER,          [this]() { registerUser(); }},
        {OPT_REQ_CLIENT_LIST,   [this]() { requestClientList(); }},
        {OPT_REQ_PUBLIC_KEY,    [this]() { requestPublicKey(); }},
        {OPT_REQ_PENDING_MSGS,  [this]() { requestPendingMessages(); }},
        {OPT_SEND_TEXT_MSG,     [this]() { sendTextMessage(); }},
        {OPT_REQ_SYMETRIC_KEY,  [this]() { requestSymmetricKey(); }},
        {OPT_SEND_SYMETRIC_KEY, [this]() { sendSymmetricKey(); }},
        {OPT_SEND_FILE,         [this]() { sendFile(); }},
        {OPT_EXIT,              [this]() { exitProgram(); }},

    };
}
//
Application::~Application()
{ }
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
    // Load client info and check if registered
    isRegistered = m_client.loadFromFile(m_config->getConfigFilePath());
    if (isRegistered)
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
            isRegistered = true;
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
        ClientPacket request(CODE_REQ_USER_LIST, emptyPayload, m_client.getClientId());
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
            size_t numOfClients = payload.size() / (CLIENT_ID_LENGTH + USERNAME_MAX_LENGTH);
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
        m_ui->displayMessage("Requesting public key for: " + targetUsername);
        //
        // Look up the client ID for the target username
        auto targetClientIdOpt = m_clientList.getClientId(targetUsername);
        if (!targetClientIdOpt)
        {
            m_ui->displayError("User " + targetUsername + " not found in the client list");
            return;
        }
        std::array<uint8_t, CLIENT_ID_LENGTH> targetClientId = targetClientIdOpt.value();
        //
        std::vector<uint8_t> payload(targetClientId.begin(), targetClientId.end());
        ClientPacket packet(CODE_REQ_USER_PUBLIC_KEY, payload, m_client.getClientId());
        //
        if (!m_network->sendPacket(packet))
        {
            m_ui->displayError("Failed to send public key request.");
            return;
        }
        //
        ServerPacket resp;
        if (!m_network->receivePacket(resp))
        {
            m_ui->displayError("No reponse from server.");
            return;
        }
        if (resp.getCode() != RESP_CODE_GET_PUBLIC_KEY)
        {
            m_ui->displayError("Failed to retrieve public key.");
            return;
        }
        //
        payload = resp.getPayload();
        if (payload.size() < CLIENT_ID_LENGTH)
            throw std::runtime_error("Error: Response payload too short to extract public key.");
        //
        std::string publicKeyBase64(payload.begin() + CLIENT_ID_LENGTH, payload.end());
        //
        m_clientList.storePublicKey(targetClientId, publicKeyBase64);
        //
        // Debug
        m_ui->displayMessage("Public key for " + targetUsername + ":\n" + publicKeyBase64);
    }
    catch (const std::runtime_error& e)
    {
        m_ui->displayError(e.what());
    }
}
//
void Application::requestPendingMessages()
{
    try
    {
        m_ui->displayMessage("Requesting pending messages...\n");
        //
        ClientPacket packet(CODE_REQ_PENDING_MESSAGES, emptyPayload, m_client.getClientId());
        //
        // Send the request:
        if (!m_network->sendPacket(packet))
        {
            m_ui->displayError("Failed to request pending messages.");
            return;
        }
        //
        // Receive server response
        ServerPacket resp;
        if (!m_network->receivePacket(resp) || resp.getCode() != RESP_CODE_GET_PENDING_MSGS)
        {
            m_ui->displayError("Server error: Failed to retrieve pending messages.");
            return;
        }
        //
        // Extract messages from payload
        std::vector<uint8_t> payload = resp.getPayload();
        size_t pos = 0;
        //
        while (pos < payload.size())
        {
            if (pos + CLIENT_ID_LENGTH + VERSION_LENGTH + CODE_LENGTH + PAYLOAD_SIZE_LENGTH > payload.size())
                break; // Avoid accessing beyond payload
            //
            // Extract sender client id
            std::array<uint8_t, CLIENT_ID_LENGTH> senderId;
            std::memcpy(senderId.data(), payload.data() + pos, CLIENT_ID_LENGTH);
            pos += CLIENT_ID_LENGTH;
            //
            // Extract message ID
            uint32_t messageId;
            std::memcpy(&messageId, payload.data() + pos, sizeof(MSG_ID_LEN));
            messageId = ntohl(messageId);
            pos += MSG_ID_LEN;
            //
            // Extract message type
            uint8_t messageType = payload[pos++];
            //
            // Extract message content size
            uint32_t messageSize;
            std::memcpy(&messageSize, payload.data() + pos, sizeof(MESSAGE_CONTENT_LEN));
            messageSize = ntohl(messageSize);
            pos += MESSAGE_CONTENT_LEN;
            //
            // Extract message content
            std::vector<uint8_t> messageContent(payload.begin() + pos, payload.begin() + pos + messageSize);
            pos += messageSize;
            //
            // Lookup sender username
            std::optional<std::string> senderUsername = m_clientList.getClientId(senderId);
            std::string sender = senderUsername ? *senderUsername : Utility::toHex(senderId);
            //
            // Process message types
            std::string content;
            if (messageType == MSG_TYPE_SYMM_KEY_REQ)
                content = "Request for symmetric key";
            else if (messageType == MSG_TYPE_SYMM_KEY_RESP)
                content = "Symmetric key received";
            else if (messageType == MSG_TYPE_SEND_TEXT_MSG)
            {
                if (m_client.hasSymmetricKey())
                {
                    try
                    {
                        std::string decryptedMessage = m_client.decryptMessage(messageContent);
                        content decryptedMessage;
                    }
                    catch (...)
                    {
                        content = "Can't decrypt message";
                    }
                }
                else
                    content = "Can't decrypt message";
            }
            else if (messageType == MSG_TYPE_SEND_FILE)
            {
                /* TODO - ADD IMPLEMENTATION HERE */
            }
            else
                content = "Unknown message type";
            //
            // Print message
            m_ui->displayMessage("From " + sender);
            m_ui->displayMessage("Content:\n" + content);
            m_ui->displayMessage("---<EOM>---\n");
        }
    }
    catch (const std::runtime_error& e)
    {
        m_ui->displayError(e.what());
        return;
    }
}
//
void Application::requestSymmetricKey()
{
    try 
    {
        if (!isRegistered)
        {
            m_ui->displayError("You must be registered first.");
            return;
        }
        //
        // Get the recipient username
        std::string recipientUsername = m_ui->getTargetUsername();
        //
        // Lookup recipient ID from the client list
        auto recipientIdOpt = m_clientList.getClientId(recipientUsername);
        if (!recipientIdOpt)
        {
            m_ui->displayError("User not found in client list.");
            return;
        }
        //
        std::array<uint8_t, CLIENT_ID_LENGTH> recipientId = recipientIdOpt.value();
        //
        // Create and send the request packet
        std::vector<uint8_t> payload;
        payload.insert(payload.end(), recipientId.begin(), recipientId.end()); // target client id
        payload.push_back(MSG_TYPE_SYMM_KEY_REQ);
        payload.insert(payload.end(), MESSAGE_CONTENT_LEN, 0); 
        ClientPacket packet(CODE_SEND_MESSAGE_TO_USER, payload, m_client.getClientId());
        //
        if (!m_network->sendPacket(packet))
        {
            m_ui->displayError("Failed to send symmetric key request.");
            return;
        }
        //
        // DEBUG
        m_ui->displayMessage("Symmetric key request sent to " + recipientUsername);
        //
        // Receive response from server
        ServerPacket resp;
        if (!m_network->receivePacket(resp))
        {
            m_ui->displayError("No response from server.");
            return;
        }
        //
        // Verify
        if (resp.getCode() == RESP_CODE_SEND_MSG_SUCCESS)
        {
            if (resp.getPayload().size() < MSG_ID_LEN)
            {
                m_ui->displayError("Invalid response: missing message ID.");
                return;
            }
            //
            // Debug
            uint32_t messageId;
            std::memcpy(&messageId, resp.getPayload().data() + CLIENT_ID_LENGTH, sizeof(messageId));
            messageId = ntohl(messageId);  // Convert from network byte order

            m_ui->displayMessage("Symmetric key response received. Message ID: " + std::to_string(messageId));
        }
        else
            m_ui->displayError("Server failed to process symmetric key request.");
    }
    catch (const std::runtime_error& e)
    {
        m_ui->displayError(e.what());
    }
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
    // If user is not registered, allow only Register or Exit options
    if (!isRegistered && choice != OPT_REGISTER && choice != OPT_EXIT)
    {
        m_ui->displayError("You must register first.");
        return;
    }
    //
    auto it = m_commandMap.find(choice);
    if (it != m_commandMap.end())
        it->second();
    else
        m_ui->displayError("Invalid option. Try again.\n");
}
