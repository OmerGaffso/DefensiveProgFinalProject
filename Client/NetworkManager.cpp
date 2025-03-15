#include "NetworkManager.h"

NetworkManager::NetworkManager()
    : m_socket(m_io_context), m_connected(false) {}
//
NetworkManager::~NetworkManager()
{
    disconnect();
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
bool NetworkManager::sendPacket(const Packet& packet)
{
    if (!m_connected)
        return false;
    //
    try
    {
        std::vector<uint8_t> data = packet.serialize();
        return writeExact(data.data(), data.size());
    }
    catch (const std::exception& e)
    {
        std::cerr << "Send error: " << e.what() << std::endl;
        return false;
    }
}
//
bool NetworkManager::receivePacket(Packet& packet)
{
    if (!m_connected)
        return false;
    //
    try
    {
        std::vector<uint8_t> buffer(SERVER_HEADER_SIZE);
        if (!readExact(buffer.data(), SERVER_HEADER_SIZE))
            return false;
        //
        uint32_t payloadSize;
        std::memcpy(&payloadSize, buffer.data() + PAYLOAD_SIZE_OFFSET, sizeof(payloadSize));
        //
        buffer.resize(SERVER_HEADER_SIZE + payloadSize);
        if (!readExact(buffer.data() + SERVER_HEADER_SIZE, payloadSize))
            return false;
        //
        packet.deserialize(buffer);
        return true;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Recieve error: " << e.what() << std::endl;
        return false;
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
    catch (...)
    {
        disconnect();
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
    catch (...)
    {
        disconnect();
        return false;
    }
}
//