#ifndef SERVERCALCAPPLICATION_PROGRAMOPTIONS_H
#define SERVERCALCAPPLICATION_PROGRAMOPTIONS_H

#include <string>
#include <optional>

#include <database/ConnectionInfo.h>

struct ProgramOptionsV
{
    ConnectionInfoV connectionInfo;
};

using ProgramOptions = std::optional<ProgramOptionsV>;

ProgramOptions getProgramOptions(int argc, char ** argv);

#endif //SERVERCALCAPPLICATION_PROGRAMOPTIONS_H
