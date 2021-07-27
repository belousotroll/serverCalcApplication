#include <iostream>

#include "Connection.h"
#include "ConnectionPool.h"

Connection::Connection(boost::asio::io_context& context,
                       ConnectionPool& connectionPool)
        : m_socket(context)
        , mr_connectionPool(connectionPool) {}

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
    // Мы помещаем данные из буфера чтения в <string_view> (наблюдатель), но без последнего
    // байта, так как он содержит в себе символ конца строки ('\n'), который нам не нужен.
    const std::string_view unhandledRequestData(m_readBuffer.data(), bytesTransferred - 1);
    // Определяем, какой тип запроса прислал клиент.
    const auto requestType = m_requestHandler.defineRequestType(unhandledRequestData);

    /*сложная или не очень реализация конечного автомата (хотя вряд ли... заебно)*/

    // Обрабатывает запрос.
    // @todo Поменять интерфейс обработчика запросов, пока что он сосет (мы два раза
    //       используем одни и те же данные ...
    m_requestHandler.handle(unhandledRequestData);
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