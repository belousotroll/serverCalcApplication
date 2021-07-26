#include "ConnectionPool.h"
#include "Connection.h"

#include <iostream>

void ConnectionPool::insert(ConnectionPool::ConnectionPtr&& connection)
{
    m_connections.insert(connection);
    connection->startHandling();
}

void ConnectionPool::remove(ConnectionPool::ConnectionPtr connection)
{
    m_connections.erase(connection);
    connection->stopHandling();
}

void ConnectionPool::removeAll()
{
    for (auto &connection : m_connections) {
        connection->stopHandling();
    }

    m_connections.clear();
}
