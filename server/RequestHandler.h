#ifndef SERVER_REQUESTHANDLER_H
#define SERVER_REQUESTHANDLER_H

#include <string_view>
#include <optional>

#include "Request.h"

class RequestHandler {
    enum class requestType : short {
        login = 0, password, calc, logout = 4,
        incorrect = -1
    };

public:
    explicit RequestHandler() noexcept = default;
    /// Возвращает тип запроса (login, password ...).
    RequestHandler::requestType defineRequestType(const std::string_view request);

    /// @todo Перенести содержимое запроса внутрь класса <request>. Передавать
    ///       тоже нужно будет экземпляр класса.
    std::optional<bool> handle(const std::string_view request);
};

#endif //SERVER_REQUESTHANDLER_H