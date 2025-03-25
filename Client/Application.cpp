#include "Application.h"
#include "Utility.h"
#include "ServerPacket.h"
#include "ClientPacket.h"
#include "RSAWrapper.h"
#include "Base64Wrapper.h"
#include "AESWrapper.h"
#include <ctime>
//
static bool                 isRegistered = false;
static std::vector<uint8_t> emptyPayload;
//
//
Application::Application() : m_appRunning(true)
{
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
    try
    {
        auto serverInfo = m_config->getServerInfo();
        if (!serverInfo)
        {
            m_ui->displayError("Failed to load server info.\n");
            return;
        }
        //
        if (!m_network->ConnectToServer(serverInfo->first, serverInfo->second))
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
    catch (const std::exception& e)
    {
        m_ui->displayError(std::string("Error: ") + e.what());
        std::cout << "Press Enter to exit...";
        std::cin.get();
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
        //
        // Generate keys
        RSAPrivateWrapper privateKey;
        std::string privateKeyStr = privateKey.getPrivateKey(); // Get the raw private key
        std::string publicKeyStr = privateKey.getPublicKey();   // Get the corresponding public key 
        //
        // Encode keys 
        std::string privateKeyBase64 = Base64Wrapper::encode(privateKeyStr);
        //
        // Ensure public key is exactly 160 bytes
        if (publicKeyStr.size() != REGISTER_PUBLIC_KEY_LEN)
        {
            m_ui->displayError("Unexpected public key size: " + std::to_string(publicKeyStr.size()));
            return;
        }        
        //
        // Prepare payload: username (255 bytes) and public key (160 bytes).
        std::vector<uint8_t> payload;
        payload.insert(payload.end(), username.begin(), username.end());
        payload.resize(REGISTER_USERNAME_LEN, '\0'); // ensure username size 255
        payload.insert(payload.end(), publicKeyStr.begin(), publicKeyStr.end());
        //
        // Create default client id
        std::array<uint8_t, CLIENT_ID_LENGTH> emptyClientId = {};
        emptyClientId.fill(0);
        //
        if(!sendClientPacket(CODE_REGISTER_USER, payload, emptyClientId))
            return;
        //
        receiveAndHandleResponse(RESP_CODE_REGISTER_SUCCCESS, [this, username, privateKeyBase64, publicKeyStr](std::vector<uint8_t> payload) 
        {
            if (payload.size() < CLIENT_ID_LENGTH)
            {
                m_ui->displayError("Invalid response payload.");
                return;
            }
            //
            // Extract client ID from response payload
            std::array<uint8_t, CLIENT_ID_LENGTH> clientId;
            std::memcpy(clientId.data(), payload.data(), CLIENT_ID_LENGTH);
            //
            // Store user info in memory and save to file
            m_client.setUsername(username);
            m_client.setClientId(clientId);
            m_client.setPrivateKey(privateKeyBase64);
            m_client.setPublicKey(publicKeyStr);
            //
            m_client.saveToFile(m_config->getConfigFilePath());
            //
            m_ui->displayMessage("Registration successful.");
            isRegistered = true;
        });
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
        if (!sendClientPacket(CODE_REQ_USER_LIST, emptyPayload, m_client.getClientId()))
            return;
        //
        receiveAndHandleResponse(RESP_CODE_GET_CLIENT_LIST, [this](const std::vector<uint8_t>& payload) 
        {
            if (payload.empty())
            {
                m_ui->displayMessage("No other clients found.");
                return;
            }
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
                // Extract username (trimming trailing null characters)
                std::string username(reinterpret_cast<const char*>(&payload[offset]), REGISTER_USERNAME_LEN);
                username.erase(std::find(username.begin(), username.end(), '\0'), username.end());
                offset += REGISTER_USERNAME_LEN;
                //
                clients.emplace_back(username, clientId);
            }
            //
            // Update local client list and print results
            m_clientList.updateClientList(clients);
            m_clientList.pritnClientList();
        });
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
        if (!sendClientPacket(CODE_REQ_USER_PUBLIC_KEY, payload, m_client.getClientId()))
            return;
        //
        receiveAndHandleResponse(RESP_CODE_GET_PUBLIC_KEY, [this, targetClientId, targetUsername](const std::vector<uint8_t>& payload) 
        {
            if (payload.size() < CLIENT_ID_LENGTH)
            {
                m_ui->displayError("Error: Response payload too short to extract public key.");
                return;
            }
            //
            // Extract public key from payload
            std::string publicKeyStr(payload.begin() + CLIENT_ID_LENGTH, payload.end());
            //
            // Store the retrieved public key
            m_clientList.storePublicKey(targetClientId, publicKeyStr);
        });
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
        if (!sendClientPacket(CODE_REQ_PENDING_MESSAGES, emptyPayload, m_client.getClientId()))
            return;
        //
        receiveAndHandleResponse(RESP_CODE_GET_PENDING_MSGS, [this](const std::vector<uint8_t>& payload) 
        {
            if (payload.empty())
            {
                m_ui->displayMessage("No pending messages.");
                return;
            }
            //
            size_t pos = 0;
            while (pos < payload.size())
            {
                if (pos + CLIENT_ID_LENGTH + MSG_ID_LEN + MESSAGE_TYPE_LEN + MESSAGE_CONTENT_LEN > payload.size())
                    break; // Prevent out-of-bounds read
                //
                // Extract sender client ID
                std::array<uint8_t, CLIENT_ID_LENGTH> senderId;
                std::memcpy(senderId.data(), payload.data() + pos, CLIENT_ID_LENGTH);
                pos += CLIENT_ID_LENGTH;
                //
                // Extract message ID
                uint32_t messageId;
                std::memcpy(&messageId, payload.data() + pos, MSG_ID_LEN);
                messageId = ntohl(messageId);
                pos += MSG_ID_LEN;
                //
                // Extract message type
                uint8_t messageType = payload[pos++];
                //
                // Extract message content size
                uint32_t messageSize;
                std::memcpy(&messageSize, payload.data() + pos, MESSAGE_CONTENT_LEN);
                messageSize = ntohl(messageSize);
                pos += MESSAGE_CONTENT_LEN;
                //
                // Extract message content
                std::vector<uint8_t> messageContent(payload.begin() + pos, payload.begin() + pos + messageSize);
                pos += messageSize;
                //
                // Lookup sender username
                std::optional<std::string> senderUsername = m_clientList.getUsername(senderId);
                std::string sender = senderUsername ? *senderUsername : toHex(senderId);
                //
                // Process message content
                std::string content = processMessage(senderId, messageType, messageContent);
                //
                // Display message
                m_ui->displayMessage("From " + sender);
                m_ui->displayMessage("Content:\n" + content);
                m_ui->displayMessage("---<EOM>---\n");
            }
        });
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
        // Get the recipient username and validate client ID
        std::string recipientUsername = m_ui->getTargetUsername();
        auto recipientIdOpt = m_clientList.getClientId(recipientUsername);
        if (!recipientIdOpt)
        {
            m_ui->displayError("User not found in client list.");
            return;
        }
        //
        // Create and send the request packet
        std::array<uint8_t, CLIENT_ID_LENGTH> recipientId = recipientIdOpt.value();
        std::vector<uint8_t> payload;
        payload.insert(payload.end(), recipientId.begin(), recipientId.end()); // target client id
        payload.push_back(MSG_TYPE_SYMM_KEY_REQ);
        payload.insert(payload.end(), MESSAGE_CONTENT_LEN, 0); 
        //
        if (!sendClientPacket(CODE_SEND_MESSAGE_TO_USER, payload, m_client.getClientId()))
            return;
        //
        receiveAndHandleResponse(RESP_CODE_SEND_MSG_SUCCESS, [this](std::vector<uint8_t> payload)
        {
            if (payload.size() < MSG_ID_LEN)
            {
                m_ui->displayError("Invalid response: missing message ID.");
                return;
            }
            //
            // Extract and print message ID
            uint32_t messageId;
            std::memcpy(&messageId, payload.data() + CLIENT_ID_LENGTH, sizeof(messageId));
            messageId = ntohl(messageId);  // Convert from network byte order
            //
            m_ui->displayMessage("Symmetric key response received. Message ID: " + std::to_string(messageId));
        });
    }
    catch (const std::runtime_error& e)
    {
        m_ui->displayError(e.what());
    }
}
//
void Application::sendTextMessage()
{
    try
    {
        std::string recipientUsername = m_ui->getTargetUsername();
        //
        // Get recipient client ID
        std::optional<std::array<uint8_t, CLIENT_ID_LENGTH>> recipientIdOpt = m_clientList.getClientId(recipientUsername);
        if (!recipientIdOpt)
        {
            m_ui->displayError("Recipient not found in client list.");
            return;
        }
        std::array<uint8_t, CLIENT_ID_LENGTH> recipientId = recipientIdOpt.value();
        //
        // Get symmetric key for recipient
        std::optional<std::vector<uint8_t>> symmetricKeyOpt = m_clientList.getSymmetricKey(recipientId);
        if (!symmetricKeyOpt)
        {
            m_ui->displayError("No symmetric key found for this recipient.");
            return;
        }
        std::vector<uint8_t> symmetricKey = symmetricKeyOpt.value();
        //
        // Get message from user
        std::string msg = m_ui->getMesssage();
        if (msg.empty())
        {
            m_ui->displayError("Message cannot be empty.");
            return;
        }
        //
        // Encrypt message
        AESWrapper aes(symmetricKey.data(), AESWrapper::DEFAULT_KEYLENGTH);
        std::string encryptedMsg = aes.encrypt(msg.c_str(), msg.size());
        //
        // Prepare payload 
        std::vector<uint8_t> payload = constructMessagePayload(recipientId, MSG_TYPE_TEXT_MSG,
            std::vector<uint8_t>(encryptedMsg.begin(), encryptedMsg.end()));
        //
        if (!sendClientPacket(CODE_SEND_MESSAGE_TO_USER, payload, m_client.getClientId()))
            return;
        //
        receiveAndHandleResponse(RESP_CODE_SEND_MSG_SUCCESS, [this](std::vector<uint8_t> payload)
        {
            if (payload.size() < MSG_ID_LEN)
            {
                m_ui->displayError("Invalid response: missing message ID.");
                return;
            }
            //
            // Extract message ID from response
            uint32_t messageId;
            std::memcpy(&messageId, payload.data(), sizeof(messageId));
            messageId = ntohl(messageId);
            //
            m_ui->displayMessage("Message sent successfully. Message ID: " + std::to_string(messageId));
        });
    }
    catch (const std::runtime_error& e)
    {
        m_ui->displayError(e.what());
    }
}
//
void Application::sendSymmetricKey()
{
    try
    {
        std::string recipientUsername = m_ui->getTargetUsername();
        std::optional<std::array<uint8_t, CLIENT_ID_LENGTH>> recipientIdOpt = m_clientList.getClientId(recipientUsername);
        if (!recipientIdOpt)
        {
            m_ui->displayError("Recipient not found in client list.");
            return;
        }
        std::array<uint8_t, CLIENT_ID_LENGTH> recipientId = recipientIdOpt.value();
        //
        // Get recipient public key
        std::optional<std::string>recipientPublicKey = m_clientList.getPublicKey(recipientId);
        if (!recipientPublicKey || recipientPublicKey->empty())
        {
            m_ui->displayError("Recipient's public key is not stored. Please request it from the server.");
            return;
        }
        try
        {
            RSAPublicWrapper recipientRSAPublicKey(reinterpret_cast<const char*>(recipientPublicKey->data()), recipientPublicKey->size());
            //
            // Generate AES symmetric key
            AESWrapper aes;
            std::string symmetricKey = std::string(reinterpret_cast<const char*>(aes.getKey()), AESWrapper::DEFAULT_KEYLENGTH);
            //
            // Encrypt symmetric key using recipient's public key
            std::string encryptedSymmetricKey = recipientRSAPublicKey.encrypt(symmetricKey);
            //
            // Create payload (target client ID + message type + encrypted key)
            std::vector<uint8_t> payload = constructMessagePayload(recipientId, MSG_TYPE_SYMM_KEY_RESP,
                std::vector<uint8_t>(encryptedSymmetricKey.begin(), encryptedSymmetricKey.end()));
            //
            if (!sendClientPacket(CODE_SEND_MESSAGE_TO_USER, payload, m_client.getClientId()))
                return;
            //
            // Receive response from server
            receiveAndHandleResponse(RESP_CODE_SEND_MSG_SUCCESS, [this, recipientId, symmetricKey](std::vector<uint8_t> payload)
            {
                // Convert key to vector and store in the client list
                std::vector<uint8_t> symmetricKeyVector(symmetricKey.begin(), symmetricKey.end());
                m_clientList.storeSymmetricKey(recipientId, symmetricKeyVector);
                //
                m_ui->displayMessage("Symmetric key sent successfully.");
            });
        }
        catch (const std::exception& e)
        {
            m_ui->displayError("RSA public key creation failed: " + std::string(e.what()));
            return;
        }
    }
    catch (const std::runtime_error& e)
    {
        m_ui->displayError(e.what());
    }
}
//
void Application::sendFile()
{
    try 
    {
        std::string recipientUsername = m_ui->getTargetUsername();
        //
        std::optional<std::array<uint8_t, CLIENT_ID_LENGTH>> recipientIdOpt = m_clientList.getClientId(recipientUsername);
        if (!recipientIdOpt)
        {
            m_ui->displayError("Recipient not found in client list.");
            return;
        }
        std::array<uint8_t, CLIENT_ID_LENGTH> recipientId = recipientIdOpt.value();
        //
        // verify symmetric key exists
        std::optional<std::vector<uint8_t>> symmetricKeyOpt = m_clientList.getSymmetricKey(recipientId);
        if (!symmetricKeyOpt)
        {
            m_ui->displayError("No symmetric key available for recipient. Request or exchange one first.");
            return;
        }
        std::vector<uint8_t> symmetricKey = symmetricKeyOpt.value();
        //
        // Get file path
        std::string filePath = m_ui->getFilePath();
        std::ifstream file(filePath, std::ios::binary | std::ios::ate);
        if (!file)
        {
            m_ui->displayError("File not found.");
            return;
        }
        //
        // Read file into buffer
        std::streamsize fileSize = file.tellg();
        if (fileSize <= 0)
        {
            m_ui->displayError("Error reading file.");
            return;
        }
        file.seekg(0, std::ios::beg);
        //
        std::vector<uint8_t> fileData(fileSize);
        if (!file.read(reinterpret_cast<char*>(fileData.data()), fileSize))
        {
            m_ui->displayError("Error reading file.");
            return;
        }
        file.close();
        //
        // Encrypt file with symmetric key
        AESWrapper aes(symmetricKey.data(), AESWrapper::DEFAULT_KEYLENGTH);
        std::string encryptedFile = aes.encrypt(reinterpret_cast<const char*>(fileData.data()), fileData.size());
        //
        // Construct payload
        std::vector<uint8_t> payload = constructMessagePayload(recipientId, MSG_TYPE_SEND_FILE,
            std::vector<uint8_t>(encryptedFile.begin(), encryptedFile.end()));
        //
        if (!sendClientPacket(CODE_SEND_MESSAGE_TO_USER, payload, m_client.getClientId()))
            return;
        //
        receiveAndHandleResponse(RESP_CODE_SEND_MSG_SUCCESS, [&](std::vector<uint8_t> payload) 
        {
            m_ui->displayMessage("File sent successfully.");
        });
    }
    catch (const std::runtime_error& e)
    {
        m_ui->displayError(e.what());
    }
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

std::string Application::handleSymmetricKeyResponse(
    const std::array<uint8_t, CLIENT_ID_LENGTH>& senderId,
    const std::vector<uint8_t>& encryptedKey)
{
    try
    {
        if (encryptedKey.empty())
            return "Received empty symmetric key.";
        //
        std::string encryptedSymmetricKey(encryptedKey.begin(), encryptedKey.end());
        //
        std::string decryptedKey = m_client.decryptWithPrivateKey(encryptedSymmetricKey);
        //
        // Ensure the key is of valid length
        if (decryptedKey.size() != AESWrapper::DEFAULT_KEYLENGTH)
            return "Decryption succeeded but key length is incorrect.";
        //
        // Convert key to vector and store it
        std::vector<uint8_t> symmetricKeyVector(decryptedKey.begin(), decryptedKey.end());
        m_clientList.storeSymmetricKey(senderId, symmetricKeyVector);
        //
        return "Symmetric key received.";
    }
    catch (const std::exception& e)
    {
        return "Failed to decrypt symmetric key: " + std::string(e.what());
    }
}

std::string Application::handleIncomingFile(const std::array<uint8_t, CLIENT_ID_LENGTH>& senderId,
    const std::vector<uint8_t>& encryptedFile)
{
    try
    {
        std::optional<std::vector<uint8_t>> symmetricKeyOpt = m_clientList.getSymmetricKey(senderId);
        if (!symmetricKeyOpt)
            return "No symmetric key available for this sender.";
        //
        std::vector<uint8_t> symmetricKey = symmetricKeyOpt.value();
        //
        AESWrapper aes(symmetricKey.data(), AESWrapper::DEFAULT_KEYLENGTH);
        std::string decryptedFile = aes.decrypt(reinterpret_cast<const char*>(encryptedFile.data()), encryptedFile.size());
        //
        // Generate unique filename - recieved_<senderId>_<timestamp>
        std::stringstream filenameStream;
        filenameStream << "received_" << toHex(senderId) << "_";
        // Get current timestamp
        std::time_t now = std::time(nullptr);
        std::tm localTime;
        localtime_s(&localTime, &now);
        filenameStream << std::put_time(&localTime, "%Y%m%d_%H%M%S");
        //
        std::string filePath = getTempDirectory() + filenameStream.str();
        //
        // Save the decrypted content to the file
        std::ofstream outFile(filePath, std::ios::binary);
        if (!outFile)
            return "Failed to save decrypted file.";
        //
        outFile.write(decryptedFile.data(), decryptedFile.size());
        outFile.close();
        //
        return filePath;
    }
    catch (const std::exception& e)
    {
        return "Error processing file: " + std::string(e.what());
    }
}
//
std::string Application::handleTextMessage(
    const std::array<uint8_t, CLIENT_ID_LENGTH>& senderId,
    const std::vector<uint8_t>& encryptedMessage)
{
    // Retrieve the symmetric key for the sender
    std::optional<std::vector<uint8_t>> symmetricKeyOpt = m_clientList.getSymmetricKey(senderId);
    if (!symmetricKeyOpt)
        return "Can't decrypt message (No symmetric key)";
    //
    try
    {
        AESWrapper aes(symmetricKeyOpt->data(), AESWrapper::DEFAULT_KEYLENGTH);
        //
        return aes.decrypt(reinterpret_cast<const char*>(encryptedMessage.data()), encryptedMessage.size());
    }
    catch (...)
    {
        return "Can't decrypt message (Decryption failed)";
    }
}
//
void Application::receiveAndHandleResponse(uint16_t expectedCode, std::function<void(std::vector<uint8_t>)> handler)
{
    ServerPacket resp;
    if (!m_network->receivePacket(resp))
    {
        m_ui->displayError("No response from server.");
        return;
    }
    //
    uint16_t receivedCode = resp.getCode();
    //
    if (receivedCode == RESP_CODE_ERROR)
    {
        m_ui->displayMessage("Server responded with an error.");
        return;
    }
    //
    if (receivedCode != expectedCode)
    {
        m_ui->displayError("Unexpected response code from server.");
        return;
    }
    //
    handler(resp.getPayload());  // Call handler with the response payload
}
//
bool Application::sendClientPacket(uint16_t code, const std::vector<uint8_t>& payload, const std::array<uint8_t, CLIENT_ID_LENGTH>& senderId)
{
    ClientPacket packet(code, payload, senderId);
    //
    if (!m_network->sendPacket(packet))
    {
        m_ui->displayError("Failed to send packet with code: " + std::to_string(code));
        return false;
    }
    return true;
}
//
std::string Application::processMessage(
    const std::array<uint8_t, CLIENT_ID_LENGTH>& senderId,
    uint8_t messageType,
    const std::vector<uint8_t>& messageContent)
    {
    switch (messageType)
    {
    case MSG_TYPE_SYMM_KEY_REQ:
        return "Request for symmetric key";
    //
    case MSG_TYPE_SYMM_KEY_RESP:
        return handleSymmetricKeyResponse(senderId, messageContent);
    //
    case MSG_TYPE_TEXT_MSG:
        return handleTextMessage(senderId, messageContent);
        //
    case MSG_TYPE_SEND_FILE:
        return handleIncomingFile(senderId, messageContent);
    //
    default:
        return "Unknown message type";
    }
}
// 
// Helper function that returns the temporary file path
std::string Application::getTempDirectory()
{
#ifdef _WIN32
    char* tempPath = nullptr; 
    size_t len = 0;
    errno_t err = _dupenv_s(&tempPath, &len, "TMP");
    std::string tempDirectory;
    //
    if (err != 0 || tempPath == nullptr)
        tempDirectory = "C:\\Temp"; // Default tmp path
    else
    {
        tempDirectory = std::string(tempPath);
        free(tempPath); // free memory
    }
    //s
    return tempDirectory + "\\";
#else
    return "/tmp";
#endif // _WIN32
}
//
std::vector<uint8_t> Application::constructMessagePayload(
    const std::array<uint8_t, CLIENT_ID_LENGTH>& recipientId,
    uint8_t messageType,
    const std::vector<uint8_t>& messageContent)
{
    std::vector<uint8_t> payload;
    payload.insert(payload.end(), recipientId.begin(), recipientId.end());
    payload.push_back(messageType);
    //
    uint32_t contentSize = htonl(messageContent.size());
    payload.insert(payload.end(), reinterpret_cast<uint8_t*>(&contentSize),
        reinterpret_cast<uint8_t*>(&contentSize) + sizeof(contentSize));
    //
    payload.insert(payload.end(), messageContent.begin(), messageContent.end());
    //
    return payload;
}