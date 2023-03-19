#ifndef SERVER_STRUCTURES_H
#define SERVER_STRUCTURES_H

#include <boost/hana.hpp>

#define  BOOST_HANA_CONFIG_ENABLE_STRING_UDL 1

// Boost.Hana тут для того, чтобы сделать контейнеры ассоциативными.
// @todo Разделить структуру на несколько (шоб пряума как в базе данных, емаё).
struct User {
    BOOST_HANA_DEFINE_STRUCT(User,
    (std::int64_t, id                  ),
    (std::int64_t, account_balance     ),
    (std::string , login               ),
    (std::string , password            ),
    (std::string , expression          ),
    (float       , resultOfExpression ));
};

#endif //SERVER_STRUCTURES_H
