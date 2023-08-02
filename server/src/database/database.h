#ifndef ARCANE_SERVER_SRC_DATABASE_DATABASE_H
#define ARCANE_SERVER_SRC_DATABASE_DATABASE_H

#include <mariadb++/account.hpp>
#include <mariadb++/connection.hpp>
#include <mariadb++/result_set.hpp>

namespace modification::server::database {
mariadb::account_ref& account();
mariadb::connection_ref& connection();
}  // namespace modification::server::database

#endif  // ARCANE_SERVER_SRC_DATABASE_DATABASE_H
