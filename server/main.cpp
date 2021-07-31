
#define BOOST_COROUTINES_NO_DEPRECATION_WARNING 0;

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "Server.h"
#include "PostgreSQLDatabase.h"

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cerr << "Серверм нэм мрамботает пампромбуйте тамк: <адрес> <порт> \n";
        return EXIT_FAILURE;
    }

    try {
        using namespace std::string_view_literals;
        // ...
        boost::asio::io_context context;

        // Необходимы для создания точки доступа.
        auto address = boost::asio::ip::address::from_string(argv[1]);
        auto port    = std::atoi(argv[2]);
        // Создаем точку доступа по заданным через консоль адресу и порту.
        boost::asio::ip::tcp::endpoint endpoint(address, port);

        // Создаем базу данных.
        constexpr auto constring = "user=postgres host=localhost password=postgres dbname=CalcDatabase"sv;
        PostgreSQLDatabase database(context, constring);
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
