/**
 * NetworkManager.h
 * This class handles the TCP network communication between the client and the server.
 * Uses Boost.Asio to manage connections, send and receive packets, and handle connection reliability.
 */

#pragma once
#include "ClientPacket.h"
#include "ServerPacket.h"
#include <boost/asio.hpp>
#include <string>
#include <iostream>

class NetworkManager
{
    boost::asio::io_context      m_io_context; //< Boost I/O context
    boost::asio::ip::tcp::socket m_socket; //< TCP socket
    bool                         m_connected; //< Connection state
    std::string                  m_serverIp; //< Server IP address
    uint16_t                     m_serverPort; //< Server Port
    //
    /**
     * @brief Reads exactly `size` bytes from the socket into the buffer.
     * @param buffer Pointer to buffer to read into.
     * @param size Number of bytes to read.
     * @return True if successful, false otherwise.
     */
    bool readExact(void* buffer, size_t size);
    /**
     * @brief Writes exactly `size` bytes from the buffer to the socket.
     * @param buffer Pointer to buffer to write.
     * @param size Number of bytes to write.
     * @return True if successful, false otherwise.
     */
    bool writeExact(const void* buffer, size_t size);
    /**
     * @brief Prints a standardized connection error message.
     */
    void printConnectionError();

public:
    NetworkManager(); // CTOR, initializes the socket.
    ~NetworkManager(); // DTOR, closes the socket.
    //
    /**
     * @brief Attempts to connect to the given IP and port.
     * @param ip IP address of the server.
     * @param port Port number of the server.
     * @return True if connection was successful, false otherwise.
     */
    bool ConnectToServer(const std::string& ip, uint16_t port);
    /**
     * @brief Sends a serialized packet to the server.
     * @param packet The packet to send.
     * @return True if sent successfully, false otherwise.
     */
    bool sendPacket(const ClientPacket& packet);
    /**
     * @brief Receives a full packet from the server and deserializes it.
     * @param packet Output parameter to store the received packet.
     * @return True if successful, false if connection lost or deserialization failed.
     */
    bool receivePacket(ServerPacket& packet);
    /**
     * @brief Closes the connection.
     */
    void disconnect();
    /**
    * @brief If the server disconnected try to reconnect 5 times.
    */
    bool reconnect(const std::string& ip, uint16_t port);
};

