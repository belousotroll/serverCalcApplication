#ifndef SERVER_SESSION_H
#define SERVER_SESSION_H

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "PostgreSQLDatabase.h"
#include "models/Structures.h"

class PostgreSQLDatabase;
class ConnectionPool;

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
    boost::asio::ip::tcp::socket m_socket;   //!< Сокет.
    boost::asio::io_context&     mr_context; //!< Ссылка на обработчик.
    std::string           m_response; //!< Хранит данные для записи в сокет.
    std::array<char, 512> m_request;  //!< Содержит данные чтения из сокета.

    PostgreSQLDatabase& mr_database; //!< База данных.
    ConnectionPool&     mr_connectionPool; //!< Ссылка на коллекция подключений.
    User  m_user;         //!< Пользователь.
    State m_currentState; //!< Текущее состояние.
};

static std::string shift(const std::string_view unhandled, uint8_t n)
{
    return {unhandled.data() + n, unhandled.size() - n};
};

static auto isValidRequest(Connection::State& currectState, const std::string_view request)
{
    const auto isLessThanTwoWords = [&request]() {
        unsigned short spaceCount = 0;
        for (const auto& character : request) {
            if (isspace(character)) { spaceCount++; }
        }

        return (spaceCount <= 1);
    };

    // Если больше двух пробелов, то можем считать запрос некорректным.
    if (!isLessThanTwoWords()) return false;

    Connection::State requestType;

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

    if (requestType == Connection::State::logout && currectState == Connection::State::calc) {
        currectState = Connection::State::logout;
        return true;
    }

    return currectState == requestType;
}

#endif //SERVER_SESSION_H
