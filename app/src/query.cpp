#include "query.h"

#include <QDataStream>

#include "client.h"

using namespace arcane::app;

std::string Query::username_;
std::string Query::password_;
std::string Query::hwid_;

void Query::init(const std::string &username, const std::string &password, const std::string &hwid)
{
    username_ = username;
    password_ = password;
    hwid_ = hwid;
}

void Query::send(Client *socket, const std::string &query)
{
    QueryData packet{ query, username_, password_, hwid_ };
    const auto data = nlohmann::json(packet).dump();
    socket->send(data.c_str());
}
