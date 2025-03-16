#include "NetworkManager.h"

NetworkManager::NetworkManager()
    : m_socket(m_io_context), m_connected(false) {}
//
NetworkManager::~NetworkManager()
{
    if (m_connected)
    {
        std::cout << "Closing connection on exit.\n";
        disconnect();
    }
}
//
bool NetworkManager::ConnectToServer(const std::string& ip, uint16_t port)
{
    try
    {
        boost::asio::ip::tcp::resolver resolver(m_io_context);
        boost::asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(ip, std::to_string(port));
        boost::asio::connect(m_socket, endpoints);
        m_connected = true;
        return m_connected;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Connection failed: " << e.what() << std::endl;
        return m_connected;
    }
}
//
bool NetworkManager::sendPacket(const ClientPacket& packet)
{
    bool res = true;
    if (!m_connected)
    {
        printConnectionError();
        res = false;
    }
    //
    try
    {
        std::vector<uint8_t> data = packet.serialize();
        if (!writeExact(data.data(), data.size()))
        {
            std::cerr << "Error: Failed to send packet. Connection may be close.\n";
            res = false;
        }
        return res;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Send error: " << e.what() << std::endl;
        res = false;
        return res;
    }
}
//
bool NetworkManager::receivePacket(ServerPacket& packet)
{
    bool res = true;
    if (!m_connected)
    {
        printConnectionError();
        res = false;
    }
    //
    try
    {
        std::vector<uint8_t> buffer(SERVER_HEADER_SIZE);
        if (!readExact(buffer.data(), SERVER_HEADER_SIZE))
        {
            std::cerr << "Warning: Failed to receive packet header.\n";
            res = false;
        }
        //
        uint32_t payloadSize;
        std::memcpy(&payloadSize, buffer.data() + SERVER_PAYLOAD_SIZE_OFFSET, sizeof(payloadSize));
        //
        buffer.resize(SERVER_HEADER_SIZE + payloadSize);
        if (!readExact(buffer.data() + SERVER_HEADER_SIZE, payloadSize))
        {
            std::cerr << "Warning: Incomplete packet received. Skipping...\n";
            res = false;
        }
        //
        packet = ServerPacket::deserialize(buffer);
        return res;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: Packet processing failed: " << e.what() << std::endl;
        res = false;
        return res;
    }
}
//
void NetworkManager::disconnect()
{
    if (m_connected)
    {
        std::cout << "Closing socket\n";
        m_socket.close();
        m_connected = false;
    }
}
//
bool NetworkManager::readExact(void* buffer, size_t size)
{
    try
    {
        boost::asio::read(m_socket, boost::asio::buffer(buffer, size));
        return true;
    }
    catch (const boost::system::system_error& e)
    {
        std::cerr << "Warning: Read failed: " << e.what() << "\n";
        if (e.code() == boost::asio::error::eof || e.code() == boost::asio::error::connection_reset)
        {
            std::cerr << "Connection lost. Disconnecting...\n";
            disconnect();
        }
        return false;
    }
}
//
bool NetworkManager::writeExact(const void* buffer, size_t size)
{
    try
    {
        boost::asio::write(m_socket, boost::asio::buffer(buffer, size));
        return true;
    }
    catch (const boost::system::system_error& e)
    {
        std::cerr << "Warning: Write failed: " << e.what() << "\n";
        if (e.code() == boost::asio::error::eof || e.code() == boost::asio::error::connection_reset)
        {
            std::cerr << "Connection lost. Disconnecting...\n";
            disconnect();
        }
        return false;
    }
}
//
void NetworkManager::printConnectionError()
{
    std::cerr << "Error: Not connected to server.\n";
}