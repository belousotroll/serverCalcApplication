#include <iostream>

#include <tinyexpr.h>

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
    // Пока что считаем, что все работает корректно.
    m_response = "";
    m_socket.async_read_some(
            boost::asio::buffer(m_request),
            boost::bind(&Connection::handleRead,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
}

void Connection::handleRead(const boost::system::error_code &errorCode, std::size_t bytesTransferred)
{
    if (errorCode) return;
    // Мы помещаем данные из буфера чтения в <string_view> (наблюдатель), но без последнего
    // байта, так как он содержит в себе символ конца строки ('\n'), который нам не нужен.
    const std::string_view request(m_request.data(), bytesTransferred - 1);
    // Проверяем запрос пользователя на валидность.
    if (!isValidRequest(m_currentState, request)) {
        m_response = "Некорректный запрос!\n";
        write();
        return;
    }

    const auto coroutine = [&](boost::asio::yield_context yield) {
        switch (m_currentState) {
            case login:
                m_user.login = shift(request, 6);
                // Меняем состояние.
                m_currentState = password;
                break;
            case password: {
                m_user.password = shift(request, 9);
                // Делаем запрос в базу данных.
                const auto [id, balance] = mr_database.auth(m_user.login, m_user.password, yield);
                // Пустое значение можно интерпретировать как отсутствие пользователя в базе данных.
                // Прерываем операцию, возвращаемся к изначальному состоянию.
                if (!id && !balance) {
                    m_response = "Неверный логин или пароль! Попробуйте ещё раз!\n";
                    m_currentState = login;
                    break;
                }
                // Присваием пользователю идентификатор и баланс счета и переходим в состояние <calc>.
                m_user.id              = *id;
                m_user.account_balance = *balance;
                // Меняем состояние.
                m_currentState = calc;
                break;
            }
            case calc: {
                // Если у пользователя нулевой баланс, прерываем операцию.
                if (m_user.account_balance <= 0) {
                    m_response = "Недостаточно денях, извините ...\n";
                    break;
                }
                // Пытаемся посчитать ...
                int errorCode = 0;
                m_user.expression         = shift(request, 5);
                m_user.resultOfExpression = te_interp(m_user.expression.data(), &errorCode);
                // Если ввели некорректные данные, прерываем операцию.
                if (errorCode != 0) {
                    m_response = "Вы ввели некорректное мат. выражение! Попробуйте ещё раз!\n";
                    break;
                }

                m_user.account_balance--;
                // Отправляем данные в базу данных.
                mr_database.sendCalcResult(m_user.id, m_user.expression, m_user.resultOfExpression, yield);
                mr_database.updateBalance(m_user.id, m_user.account_balance, yield);

                break;
            }
            case logout:
                // Меняем состояние на изначальное, т.е. на <login>.
                m_currentState = login;
                break;
        }

        // Помещаем в очередь задачу на запись и отправку данных пользователю.
        write();
    };

    boost::asio::spawn(mr_context, coroutine);
}

void Connection::write()
{
    m_socket.async_write_some(
            boost::asio::buffer(m_response),
            boost::bind(&Connection::handleWrite,
                        shared_from_this(),
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred)
    );
}

void Connection::handleWrite(const boost::system::error_code &code, std::size_t bytesTransferred)
{
    if (code) {
        mr_connectionPool.remove(shared_from_this());
    }

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