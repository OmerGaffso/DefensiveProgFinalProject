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
    std::vector<uint8_t> data = packet.serialize();
    return writeExact(data.data(), data.size());
}
//
bool NetworkManager::receivePacket(Packet& packet)
{
    if (!m_connected)
        return false;
    //
    std::vector<uint8_t> headerBuffer(HEADER_SIZE);
    if (!readExact(headerBuffer.data(), HEADER_SIZE))
        return false;
    //
    PacketHeader header = Packet::deserialize(headerBuffer).header;
    std::vector<uint8_t> payload(header.payloadSize);
    if (!readExact(payload.data(), header.payloadSize))
        return false;
    //
    packet = Packet(header.code, payload, header.clientId);
    return true;
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