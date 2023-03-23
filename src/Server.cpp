#include "Server.h"

#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio/spawn.hpp>

#include <database/PostgreSQLDatabase.h>
#include <Connection.h>

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
    auto connectionPtr = std::make_shared<Connection>(mr_context, mr_databaseAccessor, m_connectionPool);
    m_acceptor.async_accept(
        connectionPtr->socket(),
        [this,
         connection = connectionPtr](const boost::system::error_code error) {
            if (error) {
                m_connectionPool.removeAll();
                return;
            }

            m_connectionPool.insert(std::move(connection));

            accept();
         }
     );
}


unsigned int Server::run() {
    return mr_context.run();
}
