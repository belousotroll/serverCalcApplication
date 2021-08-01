#include "Server.h"

#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/spawn.hpp>

#include "PostgreSQLDatabase.h"
#include "Connection.h"

Server::Server(boost::asio::io_context& context,
               PostgreSQLDatabase& database,
               boost::asio::ip::tcp::endpoint& endpoint)
               : mr_context(context)
               , mr_databaseAccessor(database)
               , m_acceptor(mr_context, endpoint)
{
    std::clog << "Статус сервера: работает нармальна! НАР-МАЛЬ-НА! НАРМАЛЬНА РАБОТАЕТ!" << std::endl;
    accept();
}

void Server::accept()
{
    /// @todo: Это соединение обособлено от того, что я передаю в boost:bind(), и потому не добавляется в
    ///        коллекцию соединений в методе self::handleAccept. Однако если заменить handle-метод на
    ///        лямбда-функцию с boost::asio::ip::tcp::socket в аргументе и исправить конструктор класса
    ///        <Connection>, проблем не будет. Этот connectionPtr куда-то деётся и уничтожается
    ///        после обрыва соединения.
    auto connectionPtr = std::make_shared<Connection>(mr_context, mr_databaseAccessor, m_connectionPool);
    // Заставяляем ожидать соединения.
    m_acceptor.async_accept(connectionPtr->socket(),
                            boost::bind(&Server::handleAccept,
                                        this,
                                        connectionPtr, boost::asio::placeholders::error));
}

void Server::handleAccept(ConnectionPtr connectionPtr, const boost::system::error_code &errorCode) {
    if (errorCode) {
        m_connectionPool.removeAll();
        return;
    }

    // Добавляем соединение к пулу соединений.
    m_connectionPool.insert(connectionPtr);
    // Продолжаем ожидать соединения.
    accept();
}

unsigned int Server::run() {
    // Запускаем очередь задач (то есть по сути сервер).
    return mr_context.run();
}
