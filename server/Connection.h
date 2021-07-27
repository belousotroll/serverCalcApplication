#ifndef SERVER_SESSION_H
#define SERVER_SESSION_H

#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include "RequestHandler.h"

class ConnectionPool;

struct User {
    std::string _login;
    std::string _password;
};

/// @class Обрабатывает соединение с клиентом (пока такого нет).
class Connection : public std::enable_shared_from_this<Connection> {
    enum class state {
        login  = 0,
        password,
        calc,
        logout = 4,
        incorrect = -1
    };
public:
    /// Параметризированный конструктор класса.
    explicit Connection(boost::asio::io_context& context, ConnectionPool& connectionPool);

    /// Явно запрещаем любое копирование данных.
    Connection(const Connection& other) = delete;
    Connection& operator=(const Connection& other) = delete;
    ~Connection() = default;

    void startHandling();
    void stopHandling();

    /// Возвращает сокет.
    boost::asio::ip::tcp::socket& socket();

    /// Кладет в асинхронную очередь задачи на чтение данных из сокета.
    void read();
    /// Кладет в асинхронную очередь задачи на запись данных в сокет.
    void write();

    /** Обработчики задач (вызываются после основных операций) */

    void handleRead(const boost::system::error_code& code, std::size_t bytes);
    void handleWrite(const boost::system::error_code& code, std::size_t bytes);

private:
    boost::asio::ip::tcp::socket m_socket;          //< Сокет.

    enum { maxLength = 128u };                      //< Максимальная длина сообщения (в байтах).
    std::array<char, maxLength>  m_writeBuffer;     //< Хранит данные для записи в сокет.
    std::array<char, maxLength>  m_readBuffer;      //< Содержит данные чтения из сокета.

    ConnectionPool&              mr_connectionPool; //< Коллекция подключений.
    RequestHandler               m_requestHandler;  //< Обработчик запроса.
};

#endif //SERVER_SESSION_H
