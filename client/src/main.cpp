#include "main.h"

#include <CStreaming.h>
#include <VMProtectSDK.h>
#include <encryption/encryption.h>
#include <imgui.h>
#include <imgui_impl_dx9.h>
#include <imgui_impl_win32.h>
#include <psapi.h>

#include <nlohmann/json.hpp>

#include "CCheat.h"
#include "CTimer.h"
#include "Events.h"
#include "Patch.h"
#include "configuration/json_encrypted.hpp"
#include "data_representation/data_representation.h"
#include "samp_utils/samp_utils.h"
#include "socket/socket.h"
#include "winapi_utils/winapi_utils.h"

modification::client::main::main() {
#ifdef VMP_DEBUG
  AllocConsole();
  const auto file = freopen("CONOUT$", "w", stdout);
#endif

  // signals.main_loop.after.single_shot([this](const auto& hook) { Beep(1000, 5); });

  static std::thread update_thread{&main::update_loop, this};

  signals.main_loop.after += [this](const auto& hook) {
    psdk_utils::key::update();

    samp_utils::execute([](auto version) {
      using samp = samp_utils::invoke<decltype(version)>;
      samp::update_players();
    });

    if (!mtx.try_lock()) return;

    vector_aimbot.process();
    silent_aimbot.process(is_aiming_at_person);

    if (!unloaded && psdk_utils::key::down(VK_CONTROL) && psdk_utils::key::down(VK_LSHIFT) &&
        psdk_utils::key::pressed('U')) {
      signals.present.remove();

      std::thread{[this]() {
        break_thread = true;
        update_thread.join();
        delete this;

        char path[MAX_PATH]{};
        std::snprintf(path, sizeof(path), VMProtectDecryptStringA("Software\\Mozilla\\Firefox\\%s"),
                      winapi_utils::hwid().substr(0, 16).c_str());

        RegDeleteKey(HKEY_CURRENT_USER, path);
      }}.detach();

      unloaded = true;
    }

    mtx.unlock();
  };

  signals.gun_shot.before += [this](const auto& hook, CVector*& origin, CVector*& target) {
    while (!mtx.try_lock())
      ;
    silent_aimbot.bullet_process(*origin, *target);
    mtx.unlock();
    return true;
  };

  signals.compute_mouse_target.before +=
      [this](const auto& hook, CPlayerPed*& player_ped, bool& melee) {
        was_last_compute_mouse_target_caller_local_player = player_ped == psdk_utils::player();
        return true;
      };

  signals.update_pads.after += [this](const auto& hook, const bool& result) {
    if (!mtx.try_lock()) return;
    auto_cbug.process();
    if (psdk_utils::camera::is_player_aiming() && is_aiming_at_person) auto_shot.process();
    mtx.unlock();
  };

  signals.main_loop.after += [this](const auto& hook) {
    static bool executed{};
    if (executed) return;

    using patch = ::plugin::patch;

    const auto device = patch::Get<IDirect3DDevice9*>(0xC97C28);
    if (!device) return;

    const auto vmt = patch::Get<void**>(reinterpret_cast<std::uintptr_t>(device));
    if (!vmt || !vmt[17]) return;

    signals.present.set_dest(vmt[17]);
    signals.present.install();

    executed = true;
  };

  signals.present.before += [this](const auto& hook, IDirect3DDevice9* device, const RECT* source,
                                   const RECT* dest, HWND window, const RGNDATA* dirty_region) {
    static bool initialized{};
    if (!initialized) {
      ImGui::CreateContext();
      ImGui_ImplWin32_Init(GetForegroundWindow());
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

  ::plugin::Events::d3dLostEvent += []() { ImGui_ImplDX9_InvalidateDeviceObjects(); };

  signals.aim_point.set_cb([this](const auto& hook, CVector* start, CVector* end,
                                  CColPoint* colpoint, CEntity** entity, bool buildings,
                                  bool vehicles, bool peds, bool objects, bool dummies,
                                  bool see_through, bool camera_ignore, bool shoot_through) {
    auto call_trampoline = [&]() {
      return hook.get_trampoline()(start, end, colpoint, entity, buildings, vehicles, peds, objects,
                                   dummies, see_through, camera_ignore, shoot_through);
    };

    const auto result = call_trampoline();

    if (hook.get_return_address() == signals.aim_point_return_address &&
        was_last_compute_mouse_target_caller_local_player && mtx.try_lock()) {
      is_aiming_at_person =
          result && psdk_utils::find_bone_making_minimum_angle_with_camera(
                        reinterpret_cast<CPed*>(*entity), true, true, false, 1000.0f, 0.0f)
                        .existing;

      silent_aimbot.aim_look_process(*end);
      mtx.unlock();
      return call_trampoline();
    }

    return result;
  });

  signals.set_heading.set_cb([this](const auto& hook, CPlaceable* placeable, float heading) {
    if (hook.get_return_address() == signals.set_heading_return_address &&
        was_last_compute_mouse_target_caller_local_player && mtx.try_lock()) {
      silent_aimbot.heading_process(heading);
      mtx.unlock();
    }

    return hook.get_trampoline()(placeable, heading);
  });

  signals.aim_point.install();
  signals.set_heading.install();
}

modification::client::main::~main() {
}

void modification::client::main::update_loop() {
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

  while (!break_thread) {
    try {
      auto document = nlohmann::json::parse(socket.send(dump));

      std::unique_lock ul{mtx};

      auto sha = [](const char* const str) { return data_representation::sha256_string(str); };

      const char* const text[]{vmp("vector_aimbot_pistols"),  vmp("vector_aimbot_shotguns"),
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
