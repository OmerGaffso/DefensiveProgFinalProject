/*
    This Class will handle the socket communication with the server
*/

#pragma once
#include <boost/asio.hpp>
#include <vector>
#include <cstdint>
#include "Requests.h"

class NetworkClient
{
private:
    boost::asio::io_context io_context;
    boost::asio::ip::tcp::socket sock;
    std::string                  m_serverIP;
    int                          m_serverPort;

    // Buffer management
    std::vector<uint8_t> rx_buff;

public:
    NetworkClient(const std::string& ip, int port);
    ~NetworkClient();

    bool connect();
    bool disconnect();

    bool sendRequest(const Message& req, Message& resp);

    std::string getLastError() const;
};

