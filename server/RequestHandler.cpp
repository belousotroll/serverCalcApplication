#include "RequestHandler.h"
#include "Request.h"
#include <cctype>
#include <iostream>

RequestHandler::requestType RequestHandler::defineRequestType(const std::string_view request) {
    /// @todo Добавить поддержку широких (последовательных) пробелов.
    const auto isLessThanTwoWords = [&request]() {
        unsigned short spaceCounter = 0;
        for (const auto& character : request) {
            if (isspace(character)) { spaceCounter++; }
        }

        return (spaceCounter <= 1);
    };

    // Если больше двух пробелов, то можем считать запрос некорректным.
    if (!isLessThanTwoWords()) return requestType::incorrect;

    if (const auto login_position = request.find("login ");
            login_position != std::string::npos)    { return requestType::login; }
    else if (const auto password_position = request.find("password ");
            password_position != std::string::npos) { return requestType::password; }
    else if (const auto calc_position = request.find("calc ");
            calc_position != std::string::npos)     { return requestType::calc; }
    else if (const auto logout_position = request.find("logout");
            logout_position != std::string::npos)   { return requestType::logout; }

    return requestType::incorrect;
}

std::optional<bool> RequestHandler::handle(const std::string_view request) {
    switch (defineRequestType(request)) {
        case requestType::login:       return LoginRequest::exec(request);
        case requestType::password:    return PasswordRequest::exec(request);
        case requestType::calc:        return CalcRequest::exec(request);
        case requestType::logout:      return LogoutRequest::exec(request);
    }

    return IncorrentRequest::exec(request);
}