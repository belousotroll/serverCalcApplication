#ifndef SERVER_POSTGRESQLDATABASE_H
#define SERVER_POSTGRESQLDATABASE_H

#define BOOST_HANA_CONFIG_ENABLE_STRING_UDL 1

#include <string_view>

#include <ozo/connection_info.h>
#include <ozo/connection_pool.h>

#include <database/IDatabase.h>
#include <database/ConnectionInfo.h>

namespace detail {

inline auto makeOzoConnectionPool(const std::string_view constring) {
    auto connectionInfo = ozo::connection_info(constring.data());
    ozo::connection_pool_config  connectionConfig;

    /**
     * Можно задать конфигурацию соединения через config.<параметр> = <значение> (даже несколько)
     * Или через initializer list: ozo::connection_info {<Params...>};
     * см. документацию: https://github.com/yandex/ozo
     * */
    return ozo::make_connection_pool(connectionInfo, connectionConfig);
}

}

class PostgreSQLDatabase : IDatabase {

    using OzoConnectionPoolT = std::invoke_result_t<
        decltype(&detail::makeOzoConnectionPool), const std::string_view>;

public:
    explicit PostgreSQLDatabase(boost::asio::io_context & context,  ConnectionInfoV connectionInfo);
    ~PostgreSQLDatabase() = default;

    AuthResult auth(const std::string_view login,
                     const std::string_view password,
                     boost::asio::yield_context& yield) override;

    void updateBalance(const std::int64_t userID,
                       const std::int32_t accountBalance,
                       boost::asio::yield_context& yield) override;

    bool sendCalcResult(const std::int64_t userID,
                        const std::string_view expression,
                        const float resultOfExpression,
                        const boost::asio::yield_context& yield) override;
private:
    OzoConnectionPoolT m_connectionPool;
};

#endif //SERVER_POSTGRESQLDATABASE_H
