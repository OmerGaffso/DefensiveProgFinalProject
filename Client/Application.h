/**
 * @file Application.h
 * @brief Core application logic for the secure messaging client.
 *
 * The Application class orchestrates user interactions, network communication,
 * and cryptographic operations. It handles client registration, sending/receiving
 * messages and files, and interacting with the configuration, UI, and networking components.
 */

#pragma once
#include <unordered_map>
#include <functional>
#include "UI.h"
#include "ConfigManager.h"
#include "NetworkManager.h"
#include "ClientInfo.h"
#include "ClientListManager.h"
//
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
    //
    /**
     * @brief Handles client registration with the server.
     */
    void registerUser();
    /**
     * @brief Requests and displays the list of all registered clients from the server.
     */
    void requestClientList();
    /**
     * @brief Requests the public key of another client by username.
     */
    void requestPublicKey();
    /**
     * @brief Requests any pending messages from the server.
     */
    void requestPendingMessages();
    /**
     * @brief Sends a request for a symmetric key to another client.
     */
    void requestSymmetricKey();
    /**
     * @brief Sends an encrypted text message to a selected recipient.
     */
    void sendTextMessage();
    /**
     * @brief Generates and sends a symmetric key to a recipient using their public key.
     */
    void sendSymmetricKey();
    /**
     * @brief Sends an encrypted file to a recipient.
     */
    void sendFile();
    /**
     * @brief Ends the application loop and exits.
     */
    void exitProgram();
    //
    // === Requests and Responses
    //
    /**
     * @brief Receives and verifies a server response, then passes the payload to a handler.
     * @param expectedCode The expected response code from the server.
     * @param handler Function to process the response payload if valid.
     */
    void receiveAndHandleResponse(uint16_t expectedCode, std::function<void(std::vector<uint8_t>)> handler);
    /**
     * @brief Constructs and sends a packet to the server.
     * @param code The request code.
     * @param payload The request payload data.
     * @param senderId The client ID of the sender.
     * @return True if the packet was sent successfully, false otherwise.
     */
    bool sendClientPacket(uint16_t code, const std::vector<uint8_t>& payload, const std::array<uint8_t, CLIENT_ID_LENGTH>& senderId);
    /**
     * @brief Handles user input by executing the corresponding command.
     * @param choice The user-selected menu option.
     */
    void processUserInput(int choice);
    /**
     * @brief Processes an incoming message based on its type.
     * @param senderId The ID of the message sender.
     * @param messageType The type of the message.
     * @param messageContent The encrypted content of the message.
     * @return Decrypted or interpreted message as a string.
     */
    std::string processMessage(
        const std::array<uint8_t, CLIENT_ID_LENGTH>& senderId,
        uint8_t messageType,
        const std::vector<uint8_t>& messageContent);
    //
    /**
     * @brief Handles incoming encrypted symmetric key and stores it.
     * @param senderId ID of the sender.
     * @param encryptedKey The encrypted symmetric key data.
     * @return Status message of the result.
     */
    std::string handleSymmetricKeyResponse(const std::array<uint8_t, CLIENT_ID_LENGTH>& senderId,
        const std::vector<uint8_t>& encryptedKey);
    /**
     * @brief Decrypts and saves a received file.
     * @param senderId ID of the sender.
     * @param encryptedFile The encrypted file content.
     * @return Path to the saved file or error string.
     */
    std::string handleIncomingFile(const std::array<uint8_t, CLIENT_ID_LENGTH>& senderId,
        const std::vector<uint8_t>& encryptedFile);
    /**
     * @brief Decrypts and returns a received text message.
     * @param senderId ID of the sender.
     * @param encryptedMessage Encrypted message data.
     * @return Decrypted text or error string.
     */
    std::string handleTextMessage(
        const std::array<uint8_t, CLIENT_ID_LENGTH>& senderId,
        const std::vector<uint8_t>& encryptedMessage);
    /**
     * @brief Builds the payload for a message packet.
     * @param recipientId ID of the message recipient.
     * @param messageType Type of the message being sent.
     * @param messageContent Encrypted content of the message.
     * @return Assembled payload as a byte vector.
     */
    std::vector<uint8_t> constructMessagePayload(
        const std::array<uint8_t, CLIENT_ID_LENGTH>& recipientId,
        uint8_t messageType,
        const std::vector<uint8_t>& messageContent);
    //
    // === Helpers ===
    /**
     * @brief Returns the platform-specific temporary directory path.
     * @return Temporary directory path as string.
     */
    static std::string getTempDirectory();
    //
public:
    Application(); // CTOR
    ~Application(); // DTOR
    //
    /**
     * @brief Starts and runs the main loop of the application.
     */
    void run();
};

