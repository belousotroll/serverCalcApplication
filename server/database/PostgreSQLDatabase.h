#ifndef SERVER_POSTGRESQLDATABASE_H
#define SERVER_POSTGRESQLDATABASE_H

#define BOOST_HANA_CONFIG_ENABLE_STRING_UDL 1

#include <string_view>

#include <ozo/connection_info.h>
#include <ozo/connection_pool.h>

using namespace std::string_view_literals;

static auto makeOzoConnectionPool(const std::string_view constring) {
    auto connectionInfo = ozo::connection_info(constring.data());
    ozo::connection_pool_config  connectionConfig;

    /**
     * Можно задать конфигурацию соединения через config.<параметр> = <значение> (даже несколько)
     * Или через initializer list: ozo::connection_info {<Params...>};
     * см. документацию: https://github.com/yandex/ozo
     * */

    return ozo::make_connection_pool(connectionInfo, connectionConfig);
}

using OzoConnectionPool_t = std::invoke_result_t<
        decltype(&makeOzoConnectionPool), const std::string_view>;

class User;

class PostgreSQLDatabase {
    using authReturnT = std::pair<std::optional<std::int64_t>, std::optional<std::int32_t>>;
public:
    explicit PostgreSQLDatabase(boost::asio::io_context& context,  const std::string_view constring);
    ~PostgreSQLDatabase() = default;

    /// Проверяет наличие пользователя в базе данных.
    authReturnT auth(const std::string_view login,
                     const std::string_view password,
                     boost::asio::yield_context& yield);
    /// Обновляет баланс пользователя.
    void updateBalance(const std::int64_t userID,
                       const std::int32_t accountBalance,
                       boost::asio::yield_context& yield);
    /// Отправляет результат математического операции на сервер.
    bool sendCalcResult(const std::int64_t userID,
                        const std::string_view expression,
                        const float resultOfExpression,
                        const boost::asio::yield_context& yield);
private:
    OzoConnectionPool_t      m_ozoConnectionPool;
    boost::asio::io_context& mr_context;
};

#endif //SERVER_POSTGRESQLDATABASE_H
