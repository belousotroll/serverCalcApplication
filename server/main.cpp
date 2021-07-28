#define  BOOST_HANA_CONFIG_ENABLE_STRING_UDL 1

#include <iostream>
#include <boost/asio.hpp>

#include "Server.h"

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cerr << "Серверм нэм мрамботает пампромбуйте тамк: <адрес> <порт> \n";
        return EXIT_FAILURE;
    }

    try {
        // Необходимы для создания точки доступа.
        auto address = boost::asio::ip::address::from_string(argv[1]);
        auto port    = std::atoi(argv[2]);

        // Создаем точку доступа по заданным через консоль адресу и порту.
        boost::asio::ip::tcp::endpoint endpoint(address, port);
        // Создаем объект класса <Server>.
        Server localServer(endpoint);
        // Запускаем очередь.
        localServer.run();
    } catch (const std::exception& exc) {
        std::cerr << exc.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}