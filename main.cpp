
#define BOOST_COROUTINES_NO_DEPRECATION_WARNING 0;

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <Server.h>
#include <database/PostgreSQLDatabase.h>
#include <common/config.h>

int main(int argc, char* argv[])
{
    try {
        boost::asio::io_context context;
        auto address = boost::asio::ip::address::from_string(config::net::address);
        auto port    = config::net::port;
        boost::asio::ip::tcp::endpoint endpoint(address, port);
        PostgreSQLDatabase database(context, config::db::constring);

        Server localServer(context, database, endpoint);
        localServer.run();
    } catch (const std::exception& exc) {
        std::cerr << exc.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
