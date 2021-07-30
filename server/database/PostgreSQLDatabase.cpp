#include "PostgreSQLDatabase.h"
#include "Structures.h"

#include <iostream>
#include <ozo/request.h>
#include <ozo/shortcuts.h>

template <typename ConnectionType>
static auto handleDatabaseConnectionError(ConnectionType& connection, const ozo::error_code& errorCode){
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

PostgreSQLDatabase::PostgreSQLDatabase(boost::asio::io_context& context,
                                       const std::string_view constring)
                                       : mr_context(context)
                                       , m_ozoConnectionPool(makeOzoConnectionPool(constring))
                                       {}

PostgreSQLDatabase::authReturnType PostgreSQLDatabase::auth(
        const User& user,
        boost::asio::yield_context &yield)
{
    // Для удобства ввода используем литералы.
    using namespace ozo::literals;
    using namespace std::chrono_literals;
    // Хранит в себе результат запроса
    // @todo Добавить поддержку кастомных типов.
    ozo::rows_of<std::optional<std::int64_t>,
                 std::optional<std::int32_t>> result;

    // Содержит в себе код ошибки.
    ozo::error_code errorCode;
    // Формируем запрос (я не мастер БД, поэтому можно самому поправить)
    const auto query = ozo::make_query("SELECT id, account_balance FROM users WHERE login = $1 AND password = $2",
                                       user.login, user.password);
    // Делаем запрос в базу данных.
    const auto connection = ozo::request(m_ozoConnectionPool[mr_context],
                                         query, 5s, ozo::into(result), yield[errorCode]);
    // Обрабатываем возможные ошибки в запросе.
    if (errorCode) {
        handleDatabaseConnectionError<decltype(connection)>(connection, errorCode);
    }
    // Если в ответ на запрос пришли непустые данные, считаем это успехом!
    if (const auto& value = result.front(); !result.empty()) {
        return {std::get<0>(value), std::get<1>(value)};
    }
    // ...
    return {};
}

bool PostgreSQLDatabase::sendCalcResult(
        const User& user,
        const boost::asio::yield_context& yield)
{
    // Для удобства ввода используем литералы.
    using namespace ozo::literals;
    using namespace std::chrono_literals;

    // Хранит в себе результат запроса
    // @todo Добавить поддержку кастомных типов.
    ozo::rows_of<std::optional<std::int32_t>> result;
    // Содержит в себе код ошибки.
    ozo::error_code errorCode;

    // Формируем запрос.
    const auto query =ozo::make_query(
            "INSERT INTO sessions(user_id, date, expression, result_of_expression) VALUES($1, NOW(), $2, $3)",
            user.id, user.expression, user.resultOfExpression);
    // Делаем запрос в базу данных.
    const auto connection = ozo::request(m_ozoConnectionPool[mr_context],
                                         query, 5s, ozo::into(result), yield);
    // Обрабатываем ошибки в запросе ...
    if (errorCode) {
        handleDatabaseConnectionError<decltype(connection)>(connection, errorCode);
        return false;
    }

    return true;
}
