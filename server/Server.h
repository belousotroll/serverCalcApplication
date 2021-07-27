#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio.hpp>

#include "ConnectionPool.h"

class Connection;

class Server {

    using ConnectionPtr = std::shared_ptr<Connection>;

public:
    /// Параметризированный конструктор класса.
    Server(boost::asio::ip::tcp::endpoint& endpoint);
    ~Server() = default;

    /// Явно запрещает любое копирование данных.
    Server(const Server& other) = delete;
    Server& operator=(const Server& other) = delete;

    /// Запускает сервер.
    unsigned int run();

private /*methods*/:
    void accept();
    void handleAccept(ConnectionPtr connectionPtr, const boost::system::error_code &errorCode);

private /*members*/:
    boost::asio::io_context         m_context;
    boost::asio::ip::tcp::acceptor  m_acceptor;
    ConnectionPool                  m_connectionPool;
};


#endif //SERVER_SERVER_H