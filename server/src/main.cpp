#include <plog/Initializers/RollingFileInitializer.h>
#include <plog/Log.h>

#include <nlohmann/json.hpp>

#include "configuration/json_naked.hpp"
#include "data_representation/data_representation.h"
#include "database/database.h"
#include "socket/socket.h"

static void get_configuration(mariadb::result_set_ref result,
                              const modification::configuration::rpc& rpc, std::string& answer) {
  using namespace modification;
  std::string json{result->get_string("json")};

  auto sha = [](const char* const str) {
    return data_representation::sha256_string(str);
  };

  auto form_document = [sha]() {
    nlohmann::json document{};

    document[sha("vector_aimbot_pistols")] = configuration::vector_aimbot{};
    document[sha("vector_aimbot_shotguns")] = configuration::vector_aimbot{};
    document[sha("vector_aimbot_semi")] = configuration::vector_aimbot{};
    document[sha("vector_aimbot_assault")] = configuration::vector_aimbot{};
    document[sha("vector_aimbot_rifles")] = configuration::vector_aimbot{};

    document[sha("silent_aimbot_pistols")] = configuration::silent_aimbot{};
    document[sha("silent_aimbot_shotguns")] = configuration::silent_aimbot{};
    document[sha("silent_aimbot_semi")] = configuration::silent_aimbot{};
    document[sha("silent_aimbot_assault")] = configuration::silent_aimbot{};
    document[sha("silent_aimbot_rifles")] = configuration::silent_aimbot{};

    return document;
  };

  try {
    auto document = nlohmann::json::parse(json);

    try {
      configuration::vector_aimbot vector_aimbot{};
      configuration::silent_aimbot silent_aimbot{};

      from_json(document[sha("vector_aimbot_pistols")], vector_aimbot);
      from_json(document[sha("vector_aimbot_shotguns")], vector_aimbot);
      from_json(document[sha("vector_aimbot_semi")], vector_aimbot);
      from_json(document[sha("vector_aimbot_assault")], vector_aimbot);
      from_json(document[sha("vector_aimbot_rifles")], vector_aimbot);

      from_json(document[sha("silent_aimbot_pistols")], silent_aimbot);
      from_json(document[sha("silent_aimbot_shotguns")], silent_aimbot);
      from_json(document[sha("silent_aimbot_semi")], silent_aimbot);
      from_json(document[sha("silent_aimbot_assault")], silent_aimbot);
      from_json(document[sha("silent_aimbot_rifles")], silent_aimbot);
    } catch (...) {
      nlohmann::json updated_document{form_document()};
      updated_document.update(document, true);
      json = updated_document.dump();

      server::database::connection()->execute(
          std::format("UPDATE `accounts` SET `json` = '{}' WHERE "
                      "`name` = '{}'",
                      json, rpc.name));
    }
  } catch (...) {
    json = form_document().dump();

    server::database::connection()->execute(
        std::format("UPDATE `accounts` SET `json` = '{}' WHERE "
                    "`name` = '{}'",
                    json, rpc.name));
  }

  answer = json;
}

int main() {
  using namespace modification;

  plog::init(plog::debug, "server.log");

  static auto client = data_representation::file_as_hex_string("client.dll");
  static auto hash = data_representation::sha256_file("client.dll");

  // If an exception was caught, the Socket class destructor will trigger, where
  // resources will be released, after which a new socket will be created.
  while (true) {
    try {
      server::socket socket{ARCANE_SERVER_PORT, ARCANE_SERVER_KEY};

      socket.on_receive() += [](const std::string& message, std::string& answer) {
        try {
          PLOG_INFO << "New incoming connection";

          const auto new_hash = data_representation::sha256_file("client.dll");
          if (hash != new_hash) {
            client = data_representation::file_as_hex_string("client.dll");
            hash = new_hash;

            PLOG_INFO << "client.dll has been updated";
          } else {
            PLOG_INFO << "Update is not necessary";
          }

          auto json = nlohmann::json::parse(message);

          configuration::rpc rpc{};
          from_json(json, rpc);

          const auto result = server::database::connection()->query(
              std::format("SELECT * FROM `accounts` WHERE `name` = '{}' "
                          "AND `password` = '{}' AND "
                          "`end_time` > UNIX_TIMESTAMP() LIMIT 1",
                          rpc.name, rpc.password));

          if (!result->next() || !rpc.hwid.length()) {
            answer = "You have no access.";
            PLOG_INFO << "User " << rpc.name << " has been rejected.";
            return;
          }

          auto hwid = result->get_string("hwid");
          if (!hwid.length()) {
            hwid = rpc.hwid;
            server::database::connection()->execute(
                std::format("UPDATE `accounts` SET `hwid` = '{}' WHERE "
                            "`name` = '{}'",
                            hwid, rpc.name));
          } else if (hwid != rpc.hwid) {
            answer = "You have no access.";
            PLOG_INFO << "User " << rpc.name << " has no access.";
            return;
          }

          if (rpc.command == "get_configuration") {
            get_configuration(result, rpc, answer);
            PLOG_INFO << "User " << rpc.name << " has got configuration";
          } else if (rpc.command == "get_client") {
            answer = client;
            PLOG_INFO << "User " << rpc.name << " has got client";
          }
        } catch (const std::exception& e) {
          PLOG_WARNING << "Exception caught: " << e.what();
        } catch (...) {
          PLOG_WARNING << "Unknown exception caught.";
        }
      };

      PLOG_INFO << "Socket has been successfuly created.";

      using namespace std::chrono_literals;
      while (true) {
        socket.process();
      }
    } catch (const server::socket::exception& exception) {
      PLOG_ERROR << exception.what() << " with code " << exception.code();
    } catch (...) {
    }

    PLOG_FATAL << "Re-creating the socket...";
  }
}
