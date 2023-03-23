#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio.hpp>

#include <ConnectionPool.h>

class Connection;
class PostgreSQLDatabase;

class Server {

    using ConnectionPtr = std::shared_ptr<Connection>;

public:
    explicit Server(boost::asio::io_context& context,
                    PostgreSQLDatabase& database,
                    boost::asio::ip::tcp::endpoint& endpoint);
    ~Server() = default;

    Server(const Server& other) = delete;
    Server& operator=(const Server& other) = delete;

    unsigned int run();

private /*methods*/:
    void accept();

private /*members*/:
    boost::asio::io_context&        mr_context;
    boost::asio::ip::tcp::acceptor  m_acceptor;
    ConnectionPool                  m_connectionPool;

    PostgreSQLDatabase&             mr_databaseAccessor;
};


#endif //SERVER_SERVER_H