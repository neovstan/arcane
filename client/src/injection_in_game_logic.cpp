#include "injection_in_game_logic.h"

#include <d3d9.h>

// third-party

#include <imgui.h>

#include <nlohmann/json.hpp>

#include <Patch.h>
#include <CTimer.h>
#include <Events.h>

#include <VMProtectSDK.h>

// shared

#include <psdk_utils/psdk_utils.h>
#include <samp_utils/samp_utils.h>
#include <winapi_utils/winapi_utils.h>
#include <configuration/configuration.hpp>
#include <configuration/json_encrypted.hpp>
#include <data_representation/data_representation.h>

// local

#include "socket.h"
#include "backends/imgui_impl_dx9.h"
#include "backends/imgui_impl_win32.h"

using namespace modification::client;

static std::thread thread_updating_settings_instance;

injection_in_game_logic::injection_in_game_logic()
    : is_aiming_at_person_{}, was_last_compute_mouse_target_caller_local_player_{} {
#ifdef VMP_DEBUG
  load_debug_console();
#endif

  thread_updating_settings_instance =
      std::thread{&injection_in_game_logic::thread_updating_settings, this};

  load_imgui_context();
  load_anticheat_patch();
  load_keys();
  load_samp();
  load_unload();
  load_vector_aimbot();
  load_silent_aimbot();
  load_auto_shot();
  load_auto_cbug();
  load_visuals();
  load_actor();
}

void injection_in_game_logic::load_debug_console() {
  AllocConsole();
  const auto file = std::freopen("CONOUT$", "w", stdout);
}

void injection_in_game_logic::load_imgui_context() {
  ImGui::CreateContext();
}

void injection_in_game_logic::load_anticheat_patch() {
  signals_.single_shot([]() {
    samp_utils::patch_anticheat();
  });
}

void injection_in_game_logic::load_keys() {
  signals_.loop([]() {
    psdk_utils::key::update();
  });
}

void injection_in_game_logic::load_samp() {
  signals_.loop([]() {
    samp_utils::execute([](auto version) {
      using samp = samp_utils::invoke<decltype(version)>;
      samp::update_players();
    });
  });
}

void injection_in_game_logic::load_unload() {
  signals_.single_shot([this]() {
    if (psdk_utils::key::down(VK_CONTROL) && psdk_utils::key::down(VK_LSHIFT) &&
        psdk_utils::key::pressed('U')) {
      signals_.present.remove();

      std::thread{[this]() {
        has_to_break_thread_ = true;
        thread_updating_settings_instance.join();
        delete this;

        char path[MAX_PATH];
        std::snprintf(path, sizeof(path), VMProtectDecryptStringA("Software\\Mozilla\\Firefox\\%s"),
                      winapi_utils::hwid().substr(0, 16).c_str());

        RegDeleteKey(HKEY_CURRENT_USER, path);
      }}.detach();

      return true;
    }

    return false;
  });
}

void injection_in_game_logic::load_vector_aimbot() {
  signals_.loop([this]() {
    if (!mutex_.try_lock()) return;
    vector_aimbot.process();
    mutex_.unlock();
  });
}

void injection_in_game_logic::load_silent_aimbot() {
  signals_.loop([this]() {
    if (!mutex_.try_lock()) return;
    silent_aimbot.process(is_aiming_at_person_);
    mutex_.unlock();
  });

  signals_.gun_shot.before += [this](const auto& hook, auto& origin, auto& target) {
    while (!mutex_.try_lock()) continue;
    silent_aimbot.bullet_process(*origin, *target);
    mutex_.unlock();
    return true;
  };

  signals_.compute_mouse_target.before += [this](const auto& hook, auto player_ped, auto& melee) {
    was_last_compute_mouse_target_caller_local_player_ = player_ped == psdk_utils::player();
    return true;
  };

  signals_.aim_point.set_cb([this](const auto& hook, auto start, auto end, auto colpoint,
                                   auto entity, auto buildings, auto vehicles, auto peds,
                                   auto objects, auto dummies, auto see_through, auto camera_ignore,
                                   auto shoot_through) {
    auto call_trampoline = [&]() {
      return hook.get_trampoline()(start, end, colpoint, entity, buildings, vehicles, peds, objects,
                                   dummies, see_through, camera_ignore, shoot_through);
    };

    const auto result = call_trampoline();

    if (hook.get_return_address() == signals_.aim_point_return_address &&
        was_last_compute_mouse_target_caller_local_player_ && mutex_.try_lock()) {
      is_aiming_at_person_ =
          result && psdk_utils::find_bone_making_minimum_angle_with_camera(
                        reinterpret_cast<CPed*>(*entity), true, true, false, 1000.0f, 0.0f)
                        .existing;

      silent_aimbot.aim_look_process(*end);
      mutex_.unlock();
      return call_trampoline();
    }

    return result;
  });

  signals_.set_heading.set_cb([this](const auto& hook, auto placeable, auto heading) {
    if (hook.get_return_address() == signals_.set_heading_return_address &&
        was_last_compute_mouse_target_caller_local_player_ && mutex_.try_lock()) {
      silent_aimbot.heading_process(heading);
      mutex_.unlock();
    }

    return hook.get_trampoline()(placeable, heading);
  });

  signals_.aim_point.install();
  signals_.set_heading.install();
}

void injection_in_game_logic::load_auto_shot() {
  signals_.update_pads.after += [this](const auto& hook, const auto& result) {
    if (!mutex_.try_lock()) return;
    if (psdk_utils::camera::is_player_aiming() && is_aiming_at_person_) auto_shot.process();
    mutex_.unlock();
  };
}

void injection_in_game_logic::load_auto_cbug() {
  signals_.update_pads.after += [this](const auto& hook, const auto& result) {
    if (!mutex_.try_lock()) return;
    auto_cbug.process();
    mutex_.unlock();
  };
}

void injection_in_game_logic::load_visuals() {
  signals_.single_shot([this]() {
    using patch = plugin::patch;

    const auto device = patch::Get<IDirect3DDevice9*>(0xC97C28);
    if (!device) return false;

    const auto vmt = patch::Get<void**>(reinterpret_cast<std::uintptr_t>(device));
    if (!vmt || !vmt[17]) return false;

    signals_.present.set_dest(vmt[17]);
    signals_.present.install();

    return true;
  });

  signals_.present.before += [this](const auto& hook, auto device, auto source, auto dest,
                                    auto window, auto dirty_region) {
    static auto initialized = false;
    if (!initialized) {
      ImGui_ImplWin32_Init(psdk_utils::hwnd());
      ImGui_ImplDX9_Init(device);
      visuals.initialize();
      initialized = true;
    }

    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();

    ImGui::NewFrame();

    if (!CTimer::m_UserPause) {
      visuals.process();
    }

    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

    return std::nullopt;
  };

  plugin::Events::d3dLostEvent += []() {
    ImGui_ImplDX9_InvalidateDeviceObjects();
  };
}

void injection_in_game_logic::load_actor() {
  signals_.loop([this]() {
    if (!mutex_.try_lock()) return;
    actor.process();
    mutex_.unlock();
  });

  signals_.get_button_sprint_results.after +=
      [this](const auto& hook, auto& return_value, auto player, auto sprint_type) {
        if (sprint_type == 0 && mutex_.try_lock()) {
          actor.process_fast_run(return_value);
          mutex_.unlock();
        }
      };

  signals_.compute_damage_anim.set_cb([this](const auto& hook, auto event, auto ped, auto flag) {
    if (ped == psdk_utils::player()) {
      while (!mutex_.try_lock()) {
      }

      const auto order = actor.process_anti_stun();
      mutex_.unlock();

      if (order == decltype(order)::not_execute_compute_damage_anim_for_local_player) {
        return;
      }
    }

    hook.get_trampoline()(event, ped, flag);
  });

  signals_.compute_damage_anim.install();
}

void injection_in_game_logic::thread_updating_settings() {
  VMProtectBeginUltra("modification::client::main::update_loop()");

  auto vmp = &VMProtectDecryptStringA;

  client::socket socket{vmp(ARCANE_SERVER_IP), vmp(ARCANE_SERVER_PORT), vmp(ARCANE_SERVER_KEY)};

#ifndef VMP_DEBUG
  const auto data = std::string{plugin::properties().data()};
  const auto separator = data.find(";");

  const auto username = data.substr(0, separator);
  const auto password = data.substr(separator + 1);

  configuration::rpc rpc{username, password,
                         data_representation::sha256_string(winapi_utils::hwid()),
                         vmp("get_configuration")};
#else
  configuration::rpc rpc{vmp("carrentine"),
                         vmp("96cae35ce8a9b0244178bf28e4966c2ce1b8385723a96a6b838858cdd6ca0a1e"),
                         vmp("f1485cc5c11dbe6ec65b09a0a91eb1a133012c22284b11f8b8dbfb711169667e"),
                         vmp("get_configuration")};
#endif

  const auto dump = nlohmann::json(rpc).dump();

  while (!has_to_break_thread_) {
    try {
      auto document = nlohmann::json::parse(socket.send(dump));

      std::unique_lock ul{mutex_};

      auto sha = [](const char* str) {
        return data_representation::sha256_string(str);
      };

      const char* text[]{vmp("vector_aimbot_pistols"),  vmp("vector_aimbot_shotguns"),
                         vmp("vector_aimbot_semi"),     vmp("vector_aimbot_assault"),
                         vmp("vector_aimbot_rifles"),   vmp("silent_aimbot_pistols"),
                         vmp("silent_aimbot_shotguns"), vmp("silent_aimbot_semi"),
                         vmp("silent_aimbot_assault"),  vmp("silent_aimbot_rifles")};

      from_json(document[sha(text[0])], vector_aimbot.settings[psdk_utils::weapon::mode::pistols]);
      from_json(document[sha(text[1])], vector_aimbot.settings[psdk_utils::weapon::mode::shotguns]);
      from_json(document[sha(text[2])], vector_aimbot.settings[psdk_utils::weapon::mode::semi]);
      from_json(document[sha(text[3])], vector_aimbot.settings[psdk_utils::weapon::mode::assault]);
      from_json(document[sha(text[4])], vector_aimbot.settings[psdk_utils::weapon::mode::rifles]);

      from_json(document[sha(text[5])], silent_aimbot.settings[psdk_utils::weapon::mode::pistols]);
      from_json(document[sha(text[6])], silent_aimbot.settings[psdk_utils::weapon::mode::shotguns]);
      from_json(document[sha(text[7])], silent_aimbot.settings[psdk_utils::weapon::mode::semi]);
      from_json(document[sha(text[8])], silent_aimbot.settings[psdk_utils::weapon::mode::assault]);
      from_json(document[sha(text[9])], silent_aimbot.settings[psdk_utils::weapon::mode::rifles]);

      for (const auto& i : text) {
        VMProtectFreeString(i);
      }
    } catch (socket::exception&) {
    } catch (...) {
    }
  }

  VMProtectEnd();
}
