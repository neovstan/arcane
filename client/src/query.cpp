#include "query.h"

#include "socket.h"

using namespace modification::client;

std::string query::username_;
std::string query::password_;
std::string query::hwid_;

void query::init(const std::string& username, const std::string& password,
                 const std::string& hwid) {
  username_ = username;
  password_ = password;
  hwid_ = hwid;
}

std::string query::send(socket& socket, const std::string& query) {
  query_data packet;
  packet.query = query;
  packet.username = username_;
  packet.password = password_;
  packet.hwid = hwid_;

  const auto data = nlohmann::json(packet).dump();
  return socket.send(data);
}
