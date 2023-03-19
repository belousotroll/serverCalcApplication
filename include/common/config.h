#ifndef SERVERCALCAPPLICATION_CONFIG_H
#define SERVERCALCAPPLICATION_CONFIG_H

namespace config {
    namespace net {
        constexpr auto address        = "127.0.0.1";
        constexpr unsigned short port = 1234;
    }

    namespace db {
        constexpr auto constring = "user=postgres host=localhost password=postgres dbname=CalcDatabase";
    }
}

#endif //SERVERCALCAPPLICATION_CONFIG_H
