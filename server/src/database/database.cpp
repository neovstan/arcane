#include "database.h"

namespace modification::server::database {
class Loader {
 public:
  Loader() {
    using namespace mariadb;

    // set up the account
    account() = account::create("127.0.0.1", "root", "tugole63", "arcane");

    // create connection
    connection() = connection::create(account());
  }

  Loader(const Loader&&) = delete;
  Loader(Loader&&) = delete;
  void operator=(const Loader&) = delete;
};

mariadb::account_ref& account() {
  static mariadb::account_ref instance{};
  return instance;
}

mariadb::connection_ref& connection() {
  static mariadb::connection_ref instance{};
  return instance;
}
}  // namespace modification::server::database

static modification::server::database::Loader instance{};
