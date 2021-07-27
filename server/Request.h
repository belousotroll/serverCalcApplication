#ifndef SERVER_REQUEST_H
#define SERVER_REQUEST_H

#include <boost/asio/io_context.hpp>
#include <iostream>

// CRTP | SFINAE

static const std::array<std::string_view, 1> passwordCollection = {"password"};
static const std::array<std::string_view, 4> loginCollection = {
        "belousotroll", "sappyk",
          "zakruzhnoy", "gladkikh"
};

/// Находит совпадения в базе данных ...
template<typename ArrayType, unsigned short shiftNBytes>
static auto hasDatabaseMath(const std::string_view request, const ArrayType& databaseArray) {
    // Арифметика указателей: смещаем данные на shiftNBytes байт вправо.
    const auto shiftedData = std::string_view {request.data() + shiftNBytes, request.size() - shiftNBytes};
    const auto iter = std::find(std::begin(databaseArray), std::end(databaseArray), shiftedData);
    return iter != std::end(databaseArray);
}

struct login_executor final {
    using DatabaseType = std::array<std::string_view, 4>;
    static std::optional<bool> exec(const std::string_view unhandled) noexcept {
        return hasDatabaseMath<DatabaseType, 6>(unhandled, loginCollection);
    }
};

struct password_executor final {
    using DatabaseType = std::array<std::string_view, 1>;
    static std::optional<bool> exec(const std::string_view unhandled) noexcept {
        return hasDatabaseMath<DatabaseType, 9>(unhandled, passwordCollection);
    }
};

struct logout_executor final {
    static std::optional<bool> exec(const std::string_view unhandled) noexcept {
        return true;
    }
};

struct calc_executor final {
    static std::optional<bool> exec(const std::string_view unhandled) noexcept {
        // Арифметика указателей: смещаем данные на 5 байт вправо. (размер "calc " составляет 5 байт)
        const auto shifted_data = unhandled.data() + 5u;
        const auto iter = std::find(std::begin(loginCollection), std::end(loginCollection), shifted_data);
        return iter != std::end(loginCollection);    }
};

struct incorrect_executor final {
    static std::optional<bool> exec(const std::string_view unhandled) noexcept {
        return true;
    }
};

template<class RequestExecutor>
struct basic_executor final {
    using executor_t = RequestExecutor;
    constexpr static std::optional<bool> exec(const std::string_view unhandled) noexcept {
        return executor_t::exec(unhandled);
    }
};

/// Я пока не придумал интерфейс класса Request, поэтому добавил эту
/// заглушку чисто как напоминание (могу забыть ...)
template<>
struct basic_executor<nullptr_t> final {
    using executor = nullptr_t;
    constexpr static std::optional<bool> exec(const std::string_view unhandled) noexcept {
        return {};
    }
};

using Request [[maybe_unused]]          = basic_executor<nullptr_t>;
using LoginRequest [[maybe_unused]]     = basic_executor<login_executor>;
using PasswordRequest [[maybe_unused]]  = basic_executor<password_executor>;
using CalcRequest [[maybe_unused]]      = basic_executor<calc_executor>;
using LogoutRequest [[maybe_unused]]    = basic_executor<logout_executor>;
using IncorrentRequest [[maybe_unused]] = basic_executor<incorrect_executor>;

#endif //SERVER_REQUEST_H