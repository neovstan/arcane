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

#include <utils/utils.h>
#include <psdk_utils/psdk_utils.h>
#include <samp_utils/samp_utils.h>
#include <winapi_utils/winapi_utils.h>
#include <arcane_packets/configuration.hpp>
#include <protected_string/protected_string.h>

// local

#include "backends/imgui_impl_dx9.h"
#include "backends/imgui_impl_win32.h"

#include "main.h"

using namespace modification::client;

injection_in_game_logic::injection_in_game_logic(std::string_view username,
                                                 std::string_view password, std::string_view hwid)
    : is_aiming_at_person_{},
      was_last_compute_mouse_target_caller_local_player_{},
      username_{username},
      password_{password},
      hwid_{hwid} {
#ifdef VMP_DEBUG
  load_debug_console();
#endif

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
  load_vehicle();
  load_actor();
}

injection_in_game_logic::~injection_in_game_logic() {
  ImGui_ImplDX9_Shutdown();
  ImGui_ImplWin32_Shutdown();
  ImGui::DestroyContext();
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
        main::instance().client()->thread().join();
        delete this;

        main::instance().unload()->execute();
      }}.detach();

      return true;
    }

    return false;
  });
}

void injection_in_game_logic::load_vector_aimbot() {
  signals_.loop([this]() {
    vector_aimbot.process();
  });
}

void injection_in_game_logic::load_silent_aimbot() {
  signals_.loop([this]() {
    silent_aimbot.process(is_aiming_at_person_);
  });

  signals_.gun_shot.before += [this](const auto& hook, auto& origin, auto& target) {
    silent_aimbot.bullet_process(*origin, *target);
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
        was_last_compute_mouse_target_caller_local_player_) {
      is_aiming_at_person_ =
          result && psdk_utils::find_bone_making_minimum_angle_with_camera(
                        reinterpret_cast<CPed*>(*entity), true, true, false, 1000.0f, 0.0f)
                        .existing;

      silent_aimbot.aim_look_process(*end);
      return call_trampoline();
    }

    return result;
  });

  signals_.set_heading.set_cb([this](const auto& hook, auto placeable, auto heading) {
    if (hook.get_return_address() == signals_.set_heading_return_address &&
        was_last_compute_mouse_target_caller_local_player_) {
      silent_aimbot.heading_process(heading);
    }

    return hook.get_trampoline()(placeable, heading);
  });

  signals_.aim_point.install();
  signals_.set_heading.install();
}

void injection_in_game_logic::load_auto_shot() {
  signals_.update_pads.after += [this](const auto& hook, const auto& result) {
    if (psdk_utils::camera::is_player_aiming() && is_aiming_at_person_) auto_shot.process();
  };
}

void injection_in_game_logic::load_auto_cbug() {
  signals_.update_pads.after += [this](const auto& hook, const auto& result) {
    auto_cbug.process();
  };
}

void injection_in_game_logic::load_visuals() {
  signals_.single_shot([this]() {
    using patch = ::plugin::patch;

    const auto device = patch::Get<int>(0xC97C28);
    if (!device) return false;

    const auto vmt = patch::Get<void**>(device);
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

  ::plugin::Events::d3dLostEvent += []() {
    ImGui_ImplDX9_InvalidateDeviceObjects();
  };
}

void injection_in_game_logic::load_actor() {
  signals_.loop([this]() {
    actor.process();

    auto new_run_speed = 1.0f;
    actor.process_fast_run(new_run_speed);
    fast_run_patch_.speed = new_run_speed;
  });

  signals_.weapon_fire.after += [this](const auto& hook, auto& return_value,
                                       auto weapon, auto owner, auto&&... args) {
    if (!return_value || owner != psdk_utils::player()) return;
    if (weapon->m_nAmmoInClip == weapon->m_nTotalAmmo) return;

    const auto order = actor.process_infinite_clip();
    if (order == decltype(order)::not_decrease_ammo_in_clip) {
      weapon->m_nAmmoInClip++;
    }
  };

  signals_.compute_damage_anim.set_cb([this](const auto& hook, auto event, auto ped, auto flag) {
    if (ped == psdk_utils::player()) {
      const auto order = actor.process_anti_stun();

      if (order == decltype(order)::not_execute_compute_damage_anim_for_local_player) {
        return;
      }
    }

    hook.get_trampoline()(event, ped, flag);
  });

  signals_.compute_damage_anim.install();
}
void injection_in_game_logic::load_vehicle() {
  signals_.nitrous_control.after += [this](const auto& hook, auto automobile, auto set_boosts) {
    if (automobile != psdk_utils::player()->m_pVehicle) return;
    const auto order = vehicle.process_infinite_nitro();
    if (order == decltype(order)::not_decrease_vehicle_nitro_level) {
      automobile->m_fNitroValue = -0.5f;
    }
  };
}

injection_in_game_logic::fast_run_patch::fast_run_patch() {
  for (const auto address : addresses_) {
    ::plugin::patch::SetPointer(address, &speed);
  }
}

injection_in_game_logic::fast_run_patch::~fast_run_patch() {
  for (const auto address : addresses_) {
    ::plugin::patch::SetUInt(address, default_value_);
  }
}
