#pragma once
#include "ClientPacket.h"
#include "ServerPacket.h"
#include <boost/asio.hpp>
#include <string>
#include <iostream>

class NetworkManager
{
    boost::asio::io_context      m_io_context;
    boost::asio::ip::tcp::socket m_socket;
    bool                         m_connected;
    //
    bool readExact(void* buffer, size_t size);
    bool writeExact(const void* buffer, size_t size);
    void printConnectionError();

public:
    NetworkManager();
    ~NetworkManager();
    //
    bool ConnectToServer(const std::string& ip, uint16_t port);
    bool sendPacket(const ClientPacket& packet);
    bool receivePacket(ServerPacket& packet);
    void disconnect();
};

