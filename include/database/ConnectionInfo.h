#ifndef SERVERCALCAPPLICATION_CONNECTIONINFO_H
#define SERVERCALCAPPLICATION_CONNECTIONINFO_H

#include <string>

struct ConnectionInfoV
{
    std::string     user;
    std::string     host;
    std::string     password;
    std::string     database;
    std::uint16_t   port;
};

#endif //SERVERCALCAPPLICATION_CONNECTIONINFO_H
