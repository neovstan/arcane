#ifndef ARCANE_APP_QUERY_H
#define ARCANE_APP_QUERY_H

#include <string>

#include <nlohmann/json.hpp>

namespace arcane::app {
class Client;

struct QueryData
{
    std::string query;
    std::string username;
    std::string password;
    std::string hwid;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(QueryData, query, username, password, hwid);

class Query
{
public:
    static void init(const std::string &username, const std::string &password,
                     const std::string &hwid);
    static void send(Client *socket, const std::string &query);

public:
    static std::string username_;
    static std::string password_;
    static std::string hwid_;
};
} // namespace arcane::app

#endif // ARCANE_APP_QUERY_H
