#ifndef SERVER_CONNECTIONPOOL_H
#define SERVER_CONNECTIONPOOL_H

#include <set>
#include <memory>

/// Forward-declaration
class Connection;

/// Занимается управлением соединений.
/// @todo 1. Сделать поиск по ключу (в моем случае это будет пользователь).
/// @todo 2. Сделать из него Singleton.
class ConnectionPool {

    using ConnectionPtr = std::shared_ptr<Connection>;

public:
    ConnectionPool() = default;
    ConnectionPool(const ConnectionPool& other) = delete;
    ConnectionPool& operator=(const ConnectionPool& other) = delete;

    void insert(ConnectionPtr connection);
    void remove(ConnectionPtr connection);
    void removeAll();

private:
    std::set<ConnectionPtr> m_connections; //!< Коллекция активных соединений
};


#endif //SERVER_CONNECTIONPOOL_H