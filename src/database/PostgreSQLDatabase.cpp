
#include <database/PostgreSQLDatabase.h>
#include <models/Structures.h>

#include <iostream>
#include <ozo/request.h>
#include <ozo/shortcuts.h>

#include <fmt/format.h>

template <typename ConnectionType>
auto handleDatabaseConnectionError(ConnectionType& connection, const ozo::error_code& errorCode){
    std::cerr << "Request failed with error: " << errorCode.message() << '\n';
    if (!ozo::is_null_recursive(connection)) {
        if (auto msg = ozo::error_message(connection); !msg.empty()) {
            std::cerr << ", error message: " << msg << '\n';
        }
        if (auto ctx = ozo::get_error_context(connection); !ctx.empty()) {
            std::cerr << ", error context: " << ctx << '\n';
        }
    }
}

static std::string createConnectionString(const ConnectionInfoV & info)
{
    return fmt::format("user={} host={} password={} dbname={} port={}",
                       info.user, info.host, info.password, info.database, info.port);
}

static auto makeOzoConnectionPool(const std::string & constring) {
    auto connectionInfo = ozo::connection_info(constring.data());
    ozo::connection_pool_config  connectionConfig;
    return ozo::make_connection_pool(connectionInfo, connectionConfig);
}

PostgreSQLDatabase::PostgreSQLDatabase(boost::asio::io_context& context,
                                       ConnectionInfoV connectionInfo)
    : IDatabase(context)
    , m_connectionPool(makeOzoConnectionPool(createConnectionString(connectionInfo)))
{

}

PostgreSQLDatabase::AuthResult PostgreSQLDatabase::auth(const std::string_view login,
                                                        const std::string_view password,
                                                        boost::asio::yield_context& yield)
{
    using namespace ozo::literals;
    using namespace std::chrono_literals;

    ozo::rows_of<
        std::optional<std::int64_t>,
        std::optional<std::int32_t>
    > result;

    const auto query = ozo::make_query("SELECT id, account_balance FROM users WHERE login = $1 AND password = $2",
                                       login, password);

    ozo::error_code errorCode;
    const auto connection = ozo::request(m_connectionPool[mr_context],
                                         query, 5s, ozo::into(result), yield[errorCode]);
    if (errorCode) {
        handleDatabaseConnectionError<decltype(connection)>(connection, errorCode);
    }

    if (!result.empty()) {
        return { std::get<0>(result.front()), std::get<1>(result.front()) };
    }

    return {};
}

bool PostgreSQLDatabase::sendCalcResult(const std::int64_t userID,
                                        const std::string_view expression,
                                        const float resultOfExpression,
                                        const boost::asio::yield_context& yield)
{
    using namespace ozo::literals;
    using namespace std::chrono_literals;

    ozo::rows_of<std::optional<std::int32_t>> result;
    ozo::error_code errorCode;

    const auto query =ozo::make_query(
            "INSERT INTO sessions(user_id, date, expression, result_of_expression) VALUES($1, NOW(), $2, $3)",
            userID, expression, resultOfExpression);

    const auto connection = ozo::request(m_connectionPool[mr_context],
                                         query, 2s, ozo::into(result), yield);

    if (errorCode) {
        handleDatabaseConnectionError<decltype(connection)>(connection, errorCode);
        return false;
    }

    return true;
}

void PostgreSQLDatabase::updateBalance(const std::int64_t userID,
                                       const std::int32_t accountBalance,
                                       boost::asio::yield_context& yield)
{
    using namespace ozo::literals;
    using namespace std::chrono_literals;

    ozo::rows_of<std::optional<std::int32_t>> result;
    ozo::error_code errorCode;

    const auto updateQuery = ozo::make_query(
            "UPDATE users SET account_balance = $1 WHERE id = $2",
            accountBalance, userID);

    const auto connection = ozo::request(m_connectionPool[mr_context],
                                               updateQuery, 2s, ozo::into(result), yield);

    if (errorCode) {
        handleDatabaseConnectionError<decltype(connection)>(connection, errorCode);
    }
}
