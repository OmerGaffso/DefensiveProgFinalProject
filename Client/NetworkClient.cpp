#include "NetworkClient.h"

NetworkClient::NetworkClient(const std::string& ip, int port)
{
    m_serverIP = ip;
    m_serverPort = port;
}

NetworkClient::~NetworkClient()
{

}

bool NetworkClient::connect()
{

    return true;
}
bool NetworkClient::disconnect()
{
    return true;
}

bool NetworkClient::sendRequest(const Message& req, Message& resp)
{
    return true;
}

std::string NetworkClient::getLastError() const
{

}
