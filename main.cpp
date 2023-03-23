#define BOOST_COROUTINES_NO_DEPRECATION_WARNING 0;

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <Server.h>
#include <database/PostgreSQLDatabase.h>
#include <common/ProgramOptions.h>

int main(int argc, char* argv[])
{
    try
    {
        auto options = getProgramOptions(argc, argv);

        if (!options)
        {
            return EXIT_SUCCESS;
        }

        boost::asio::io_context context;
        auto address = boost::asio::ip::address::from_string("localhost");
        auto port    = 0;
        boost::asio::ip::tcp::endpoint endpoint(address, port);
        PostgreSQLDatabase database(context, options->connectionInfo);

        Server server(context, database, endpoint);
        server.run();
    }
    catch (const std::exception & exc)
    {
        std::cerr << exc.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
