#include <common/ProgramOptions.h>

#include <boost/program_options.hpp>

#include <iostream>

namespace po = boost::program_options;

ProgramOptions getProgramOptions(int argc, char ** argv)
{
    auto desc = po::options_description("Allowed options");

    auto options = ProgramOptionsV();

    desc.add_options()
        ("help,h",
         R"(Produce help message)")
        ("user",
         po::value<std::string>(&options.connectionInfo.user)->default_value("postgres"),
         R"(The username to use for authentication)")
        ("host",
         po::value<std::string>(&options.connectionInfo.host)->default_value("localhost"),
         R"(The hostname or IP address of the database server)")
        ("password",
         po::value<std::string>(&options.connectionInfo.password)->default_value("postgres"),
         R"(The password to use for authentication)")
        ("database",
         po::value<std::string>(&options.connectionInfo.database)->default_value("CalcDatabase"),
         R"(The name of the database to connect to)")
        ("port",
         po::value<std::uint16_t>(&options.connectionInfo.port)->default_value(9696),
         R"(The port number to use when connecting to the database server)");

    auto vm = boost::program_options::variables_map();
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        std::cout << desc << std::endl;
        return std::nullopt;
    }

    return options;
}