#include "Server.h"
#include "Connection.h"

#include <iostream>

Server::Server(boost::asio::ip::tcp::endpoint &endpoint)
        : m_context()
        , m_acceptor(m_context, endpoint)
{
    std::cout << "Статус сервера: работает нармальна! НАР-МАЛЬ-НА! НАРМАЛЬНА РАБОТАЕТ!" << std::endl;
    accept();
}

void Server::accept()
{
    /// @todo: это соединение обособлено от того, что я передаю в boost:bind(), и потому не добавляется в
    ///        коллекцию соединений в методе self::handleAccept. Однако если заменить handle-метод на
    ///        лямбда-функцию с boost::asio::ip::tcp::socket в аргументе и исправить конструктор класса
    ///        <Connection>, проблем не будет. Этот connectionPtr куда-то деётся.
    auto connectionPtr = std::make_shared<Connection>(m_context, m_connectionPool);
    m_acceptor.async_accept(connectionPtr->socket(),
                            boost::bind(&Server::handleAccept,
                                        this,
                                        connectionPtr, boost::asio::placeholders::error));
}

void Server::handleAccept(std::shared_ptr<Connection> connectionPtr, const boost::system::error_code &errorCode) {
    if (errorCode) return;
    m_connectionPool.insert(std::move(connectionPtr));
    accept();
}

unsigned int Server::run() {
    return m_context.run();
}