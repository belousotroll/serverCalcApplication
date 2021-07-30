#ifndef SERVER_POSTGRESQLDATABASE_H
#define SERVER_POSTGRESQLDATABASE_H

#define BOOST_HANA_CONFIG_ENABLE_STRING_UDL 1

#include <string_view>

#include <ozo/connection_info.h>
#include <ozo/connection_pool.h>

#include "IDatabaseAccessor.h"

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

class PostgreSQLDatabase final : public IDatabaseAccessor {
public:
    explicit PostgreSQLDatabase(boost::asio::io_context& context,
                                const std::string_view constring
                                = "user=postgres host=localhost password=postgres dbname=CalcDatabase");
    ~PostgreSQLDatabase() override = default;

    /// Проверяет наличие пользователя в базе данных.
    authReturnType auth(const User& user, boost::asio::yield_context& yield) override;
    /// Отправляет результат математического операции на сервер.
    bool sendCalcResult(const User& user, const boost::asio::yield_context& yield) override;
private:
    OzoConnectionPool_t      m_ozoConnectionPool;
    boost::asio::io_context& mr_context;
};

#endif //SERVER_POSTGRESQLDATABASE_H
