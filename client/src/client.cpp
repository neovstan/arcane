#include "client.h"

#include <protected_string/protected_string.h>

#include <VMProtectSDK.h>

#include "socket.h"
#include "query.h"
#include "injection_in_game_logic.h"

using namespace modification::client;

client::client(std::shared_ptr<injection_in_game_logic> injection)
    : injection_{injection}, thread_{&client::process, this} {
}

void client::process() {
  VMProtectBeginUltra("client::process()");

  modification::client::socket socket{scoped_protected_std_string(ARCANE_SERVER_IP),
                                      std::to_string(ARCANE_SERVER_PORT),
                                      scoped_protected_std_string(ARCANE_SERVER_KEY)};

  query::init(injection_->username_, injection_->password_, injection_->hwid_);

  while (!injection_->has_to_break_thread_) {
    try {
      const auto answer = query::send(socket, scoped_protected_std_string("settings"));
      auto document = nlohmann::json::parse(answer);

      packets::user_json user_json;
      from_json(document, user_json);

      if (user_json.patterns.empty()) {
        using weapon_mode = psdk_utils::weapon::mode;
        for (auto i = weapon_mode::pistols; i != weapon_mode::unknown;
             ++reinterpret_cast<int&>(i)) {
          injection_->vector_aimbot.settings[i] = {};
          injection_->silent_aimbot.settings[i] = {};
        }

        injection_->auto_shot.settings = {};
        injection_->auto_cbug.settings = {};
        injection_->visuals.settings = {};
        injection_->actor.settings = {};
        injection_->vehicle.settings = {};
        injection_->miscellaneous.settings = {};
        continue;
      }

      const auto configuration = user_json.patterns.front();

      injection_->vector_aimbot.friendly_nicknames = user_json.friendly_nicknames;
      injection_->vector_aimbot.model_groups = configuration.groups;

      injection_->vector_aimbot.settings[psdk_utils::weapon::mode::pistols] =
          configuration.vector_aimbot_pistols;

      injection_->vector_aimbot.settings[psdk_utils::weapon::mode::shotguns] =
          configuration.vector_aimbot_shotguns;

      injection_->vector_aimbot.settings[psdk_utils::weapon::mode::semi] =
          configuration.vector_aimbot_semi;

      injection_->vector_aimbot.settings[psdk_utils::weapon::mode::assault] =
          configuration.vector_aimbot_assault;

      injection_->vector_aimbot.settings[psdk_utils::weapon::mode::rifles] =
          configuration.vector_aimbot_rifles;

      injection_->silent_aimbot.friendly_nicknames = user_json.friendly_nicknames;
      injection_->silent_aimbot.model_groups = configuration.groups;

      injection_->silent_aimbot.settings[psdk_utils::weapon::mode::pistols] =
          configuration.silent_aimbot_pistols;

      injection_->silent_aimbot.settings[psdk_utils::weapon::mode::shotguns] =
          configuration.silent_aimbot_shotguns;

      injection_->silent_aimbot.settings[psdk_utils::weapon::mode::semi] =
          configuration.silent_aimbot_semi;

      injection_->silent_aimbot.settings[psdk_utils::weapon::mode::assault] =
          configuration.silent_aimbot_assault;

      injection_->silent_aimbot.settings[psdk_utils::weapon::mode::rifles] =
          configuration.silent_aimbot_rifles;

      injection_->auto_cbug.settings = configuration.auto_cbug;
      injection_->auto_shot.settings = configuration.auto_shot;
      injection_->visuals.settings = configuration.visuals;
      injection_->actor.settings = configuration.actor;
      injection_->vehicle.settings = configuration.vehicle;
      injection_->miscellaneous.settings = configuration.miscellaneous;

      using namespace std::chrono_literals;
      std::this_thread::sleep_for(1s);
    } catch (const socket::exception& e) {
    } catch (const std::exception& e) {
    }
  }

  VMProtectEnd();
}

std::thread& client::thread() {
  return thread_;
}
