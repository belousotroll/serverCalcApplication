#ifndef SERVER_SESSION_H
#define SERVER_SESSION_H

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "PostgreSQLDatabase.h"
#include "Structures.h"

class PostgreSQLDatabase;
class ConnectionPool;

/// @class Обрабатывает соединение с клиентом.
class Connection : public std::enable_shared_from_this<Connection> {
public:
    /// Параметризированный конструктор класса.
    explicit Connection(boost::asio::io_context& context,
                        PostgreSQLDatabase& database,
                        ConnectionPool& connectionPool);

    /// Явно запрещаем любое копирование данных.
    Connection(const Connection& other) = delete;
    Connection& operator=(const Connection& other) = delete;
    ~Connection() = default;

    /// Запускает обработку соединений.
    void startHandling();
    /// Прекращает обработку соединений.
    void stopHandling();

    /// Возвращает сокет.
    boost::asio::ip:: tcp::socket& socket();

    /// Кладет в асинхронную очередь задачи на чтение данных из сокета.
    void read();
    /// Кладет в асинхронную очередь задачи на запись данных в сокет.
    void write();

    /** Задачи, которые закидываются в очередь. */

    void handleRead(const boost::system::error_code& code, std::size_t bytes);
    void handleWrite(const boost::system::error_code& code, std::size_t bytes);

    enum State : uint8_t  { login = 0, password, calc, logout = 4};

private:
    boost::asio::ip::tcp::socket m_socket;          //!< Сокет.
    PostgreSQLDatabase&          mr_database;        //!< База данных.

    enum { maxLength = 128u };                      //!< Максимальная длина сообщения (в байтах).
    std::array<char, maxLength>  m_writeBuffer;     //!< Хранит данные для записи в сокет.
    std::array<char, maxLength>  m_readBuffer;      //!< Содержит данные чтения из сокета.

    boost::asio::io_context&     mr_context;        //!< Ссылка на обработчик.
    ConnectionPool&              mr_connectionPool; //!< Ссылка на коллекция подключений.

    // @todo Сделать std::map<User, Connection>, сейчас у нас два эти класса связаны.
    User                         m_user;            //!< Пользователь.
    State                        m_currentState;    //!< Текущее состояние.
};

static auto shift(const std::string_view unhandled, uint8_t n) {
    return std::string_view {unhandled.data() + n, unhandled.size() - n};
};

static auto isValidRequest(Connection::State currectState, const std::string_view request) {
    // @todo Можно попробовать сделать constexpr map на шаблонах.
    static const std::map<Connection::State, Connection::State> transitionTable = {
            {Connection::State::login,     Connection::State::password}, // login    -> password
            {Connection::State::password,  Connection::State::calc},     // password -> calc
            {Connection::State::calc,    Connection::State::calc}        // calc     -> calc ...
    };

    /// @todo Добавить поддержку широких (последовательных) пробелов.
    const auto isLessThanTwoWords = [&request]() {
        unsigned short spaceCounter = 0;
        for (const auto& character : request) {
            if (isspace(character)) { spaceCounter++; }
        }

        return (spaceCounter <= 1);
    };

    // Если больше двух пробелов, то можем считать запрос некорректным.
    if (!isLessThanTwoWords()) return false;

    Connection::State requestType = Connection::State::login;

    if (const auto login_position = request.find("login "); login_position == 0) {
        requestType = Connection::State::login;
    } else if (const auto password_position = request.find("password "); password_position == 0) {
        requestType = Connection::State::password;
    } else if (const auto calc_position = request.find("calc "); calc_position == 0) {
        requestType = Connection::State::calc;
    } else if (const auto logout_position = request.find("logout"); logout_position == 0) {
        requestType = Connection::State::logout;
    } else {
        return false;
    }

    return requestType == transitionTable.at(currectState) || requestType == Connection::logout;
}

#endif //SERVER_SESSION_H
