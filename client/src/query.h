#ifndef ARCANE_CLIENT_SRC_QUERY_H
#define ARCANE_CLIENT_SRC_QUERY_H

#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>

namespace modification::client {
class socket;

struct query_data {
  std::string query;
  std::string username;
  std::string password;
  std::string hwid;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(query_data, query, username, password, hwid);
};

class query {
 public:
  static void init(std::string_view username, std::string_view password, std::string_view hwid);
  static std::string send(socket& socket, std::string_view query);

 private:
  static std::string username_;
  static std::string password_;
  static std::string hwid_;
};
}  // namespace modification::client

#endif  // ARCANE_CLIENT_SRC_QUERY_H
