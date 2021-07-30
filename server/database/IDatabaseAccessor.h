#ifndef SERVER_IDATABASEACCESSOR_H
#define SERVER_IDATABASEACCESSOR_H

#include <optional>

class User;

class IDatabaseAccessor {
protected:
    using authReturnType = std::pair<std::optional<std::int64_t>, std::optional<std::int32_t>>;
    virtual ~IDatabaseAccessor() = default;
    /// Проверяет наличие пользователя в базе данных.
    virtual authReturnType auth(const User& user, boost::asio::yield_context& yield) = 0;
    /// Отправляет результат математического операции на сервер.
    virtual bool sendCalcResult(const User& user, const boost::asio::yield_context& yield) = 0;
};

#endif //SERVER_IDATABASEACCESSOR_H
