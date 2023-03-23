#ifndef SERVERCALCAPPLICATION_IDATABASE_H
#define SERVERCALCAPPLICATION_IDATABASE_H

#include <boost/asio/io_context.hpp>
#include <boost/asio/spawn.hpp>

class IDatabase {

protected:

    using AuthResult = std::pair<
        std::optional<std::int64_t>,
        std::optional<std::int32_t>
    >;

    IDatabase() = delete;
    explicit IDatabase(boost::asio::io_context & context);
    virtual ~IDatabase() = default;

    virtual AuthResult auth(const std::string_view login,
                            const std::string_view password,
                            boost::asio::yield_context & yield) = 0;

    virtual void updateBalance(const std::int64_t userID,
                               const std::int32_t newBalance,
                               boost::asio::yield_context& yield) = 0;

    virtual bool sendCalcResult(const std::int64_t userID,
                                const std::string_view expression,
                                const float resultOfExpression,
                                const boost::asio::yield_context& yield) = 0;

protected:
    boost::asio::io_context& mr_context;
};


#endif //SERVERCALCAPPLICATION_IDATABASE_H
