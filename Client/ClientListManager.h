/*
    ClientListManager.h

    This class maintains mappings between usernames and client IDs, public keys,
    and symmetric keys. It is used by the Application to resolve users and securely
    manage cryptographic context.
*/

#pragma once
#include "Utility.h"
#include <unordered_map>
#include <array>
#include <vector>
#include <optional>
#include "UI.h"

class ClientListManager
{
private:
    std::unordered_map<std::string, std::array<uint8_t, CLIENT_ID_LENGTH>> clientMap; // maps username to client ID
    std::unordered_map<std::array<uint8_t, CLIENT_ID_LENGTH>, std::string, ArrayHasher> publicKeyMap; // maps client ID to public key
    std::unordered_map<std::array<uint8_t, CLIENT_ID_LENGTH>, std::vector<uint8_t>, ArrayHasher> m_symmetricKeys; // maps client ID to symmetric key
    UI m_ui;
    //
public:
    ClientListManager(); // CTOR
    /**
     * @brief Replace the current client list with a new one.
     *
     * @param clients Vector of <username, clientId> pairs.
     */
    void updateClientList(const std::vector<std::pair<std::string, std::array<uint8_t, CLIENT_ID_LENGTH>>>& clients);
    /**
     * @brief Get the client ID for a given username.
     *
     * @param username The target username.
     * @return std::optional containing the client ID if found.
     */
    std::optional<std::array<uint8_t, CLIENT_ID_LENGTH>> getClientId(const std::string& username) const;
    /**
     * @brief Print the current list of registered clients.
     */
    void printClientList() const;
    /**
     * @brief Get the username corresponding to a given client ID.
     *
     * @param clientId The client ID to look up.
     * @return std::optional containing the username if found.
     */
    std::optional<std::string> getUsername(const std::array<uint8_t, CLIENT_ID_LENGTH>& clientId) const;
    //
    // === Public key handling === 
    /**
     * @brief Get the stored public key of a client.
     *
     * @param clientId The client ID.
     * @return std::optional containing the public key if found.
     */
    std::optional<std::string> getPublicKey(const std::array<uint8_t, CLIENT_ID_LENGTH>& clientId) const;
    /**
     * @brief Store the public key for a given client.
     *
     * @param clientId The client ID.
     * @param publicKey Raw public key data as string.
     */
    void storePublicKey(const std::array<uint8_t, CLIENT_ID_LENGTH>& clientId, const std::string& publicKey);
    //
    // === Symmetric key handling ===
    /**
     * @brief Store a symmetric key for a specific client.
     *
     * @param clientId The client ID.
     * @param symmetricKey A 128-bit AES key.
     */
    void storeSymmetricKey(const std::array<uint8_t, CLIENT_ID_LENGTH>& clientId, const std::vector<uint8_t>& symmetricKey);
    /**
     * @brief Retrieve the symmetric key for a given client.
     *
     * @param clientId The client ID.
     * @return std::optional containing the symmetric key if found.
     */
    std::optional<std::vector<uint8_t>> getSymmetricKey(const std::array<uint8_t, CLIENT_ID_LENGTH>& clientId) const;
};

