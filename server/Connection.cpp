#include <iostream>

#include "Connection.h"
#include "ConnectionPool.h"

//Connection::Connection(boost::asio::ip::tcp::socket&& socket,
//                       ConnectionPool& connectionPool)
//    : m_socket(std::move(socket))
//    , mr_connectionPool(connectionPool)
//{
//    std::cout << "ALIVE\n";
//}

Connection::Connection(boost::asio::io_context& context,
                       ConnectionPool& connectionPool)
        : m_socket(context)
        , mr_connectionPool(connectionPool) {}

boost::asio::ip::tcp::socket &Connection::socket()
{
    return m_socket;
}

void Connection::read()
{
    m_socket.async_read_some(
            boost::asio::buffer(m_readBuffer),
            boost::bind(&Connection::handleRead,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
}

void Connection::handleRead(const boost::system::error_code &errorCode, std::size_t bytes_transferred)
{
    if (errorCode) return;
    write();
}

void Connection::write()
{
    m_socket.async_write_some(
            boost::asio::buffer(m_writeBuffer),
            boost::bind(&Connection::handleWrite,
                        shared_from_this(),
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred)
    );
}

void Connection::handleWrite(const boost::system::error_code &code, std::size_t bytes_transferred)
{
    if (code) {
        std::cerr << "Error (writing): " << code.message() << std::endl;
        m_socket.close();
    }

    mr_connectionPool.remove(shared_from_this());
}

void Connection::startHandling()
{
    read();
}

void Connection::stopHandling()
{
    m_socket.close();
}
