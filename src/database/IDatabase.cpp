#include <database/IDatabase.h>

IDatabase::IDatabase(boost::asio::io_context & context)
    : mr_context(context)
{
}
