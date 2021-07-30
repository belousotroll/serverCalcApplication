#include <iostream>

#include <boost/bind.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/spawn.hpp>
#include "PostgreSQLDatabase.h"

#include "ConnectionPool.h"
#include "Connection.h"

Connection::Connection(boost::asio::io_context& context,
                       PostgreSQLDatabase& database,
                       ConnectionPool& connectionPool)
                       : mr_context(context)
                       , mr_database(database)
                       , m_socket(context)
                       , mr_connectionPool(connectionPool)
                       , m_currentState(State::login) {}

boost::asio::ip::tcp::socket &Connection::socket()
{
    return m_socket;
}

void Connection::read()
{
    m_socket.async_read_some(
            boost::asio::buffer(m_readBuffer),
            boost::bind(&Connection::handleRead,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
}

void Connection::handleRead(const boost::system::error_code &errorCode, std::size_t bytesTransferred)
{
    if (errorCode) return;

    const auto coroutine = [&](boost::asio::yield_context yield) {
        // Мы помещаем данные из буфера чтения в <string_view> (наблюдатель), но без последнего
        // байта, так как он содержит в себе символ конца строки ('\n'), который нам не нужен.
        const std::string_view request(m_readBuffer.data(), bytesTransferred - 1);
        // Проверяем запрос пользователя на валидность.
        if (isValidRequest(m_currentState, request)) return;

        switch (m_currentState) {
            case login:
                std::clog << "login ...\n";
                m_user.login = shift(request, 6);
                m_currentState = password;
                break;
            case password: {
                std::clog << "password ...\n";
                m_user.password = shift(request, 9);
                // Делаем запрос в базу данных.
                const auto [id, balance] = mr_database.auth(m_user,
                                                            yield);
                // Пустое значение можно интерпретировать как отсутствие пользователя в базе данных.
                // Прерываем операцию, возвращаемся к изначальному состоянию.
                if (!id && !balance) {
                    m_currentState = login;
                    return;
                }

                std::clog << *id << ' ' << *balance << '\n';

                // Присваием пользователю идентификатор и баланс счета и переходим в состояние <calc>.
                m_user.id              = *id;
                m_user.account_balance = *balance;

                m_currentState = calc;
                break;
            }
            case calc: {
                std::clog << "calc ...\n";
                m_user.expression = shift(request, 5);

                /* Манипуляции с данными ...       */
                /* ЧИКИ-ПИКИ МИНИПУЛЭТИОН          */
                /* Конец манипуляции с данными ... */

                // Записываем результат сложных математических операций.
                m_user.resultOfExpression = "2";
                // Если ... (пока не придумал, хочу спать ...)
                if (mr_database.sendCalcResult(m_user, yield)) {
                    // ...
                }

                break;
            }
            case logout:
                // Разрываем соединение с сервером (или чем там ...)
                mr_connectionPool.remove(shared_from_this());
                break;
        }
    };
    // Запускам корутину.
    boost::asio::spawn(mr_context, coroutine);
    // Помещаем в очередь задачу на запись и отправку данных пользователю.
    write();
}

void Connection::write()
{
    m_socket.async_write_some(
            boost::asio::buffer(m_writeBuffer),
            boost::bind(&Connection::handleWrite,
                        shared_from_this(),
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred)
    );
}

void Connection::handleWrite(const boost::system::error_code &code, std::size_t bytesTransferred)
{
    if (code) { m_socket.close(); }
    read();
}

void Connection::startHandling()
{
    read();
}

void Connection::stopHandling()
{
    m_socket.close();
}