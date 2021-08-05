
#define BOOST_COROUTINES_NO_DEPRECATION_WARNING 0;

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "Server.h"
#include "PostgreSQLDatabase.h"
#include "config.h"

int main(int argc, char* argv[])
{
    try {
        // Создаем очередь задач.
        boost::asio::io_context context;
        // Необходимы для создания точки доступа.
        auto address = boost::asio::ip::address::from_string(config::net::address);
        auto port    = config::net::port;
        // Создаем точку доступа по заданным через консоль адресу и порту.
        boost::asio::ip::tcp::endpoint endpoint(address, port);
        // Создаем базу данных.
        PostgreSQLDatabase database(context, config::db::constring);
        // Создаем сервер.
        Server localServer(context, database, endpoint);
        // Запускам сервер.
        localServer.run();
    } catch (const std::exception& exc) {
        std::cerr << exc.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
