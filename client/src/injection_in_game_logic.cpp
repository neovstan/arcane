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

      ImGui_ImplDX9_Shutdown();
      ImGui_ImplWin32_Shutdown();

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
    auto_cbug.process(actor.settings.auto_reload);
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

  signals_.reset.after += [](const auto& hook) {
    ImGui_ImplDX9_InvalidateDeviceObjects();
  };
}

void injection_in_game_logic::load_actor() {
  signals_.loop([this]() {
    if (!psdk_utils::player()) return;

    actor.process();
    actor.process_control();

    auto new_run_speed = 1.0f;
    actor.process_fast_run(new_run_speed);
    fast_run_patch_.speed = new_run_speed;
  });

  signals_.single_shot([this]() {
    using patch = ::plugin::patch;

    if (patch::GetUShort(0x4D4610) != 0x25FF) return false;

    const auto address = patch::GetUInt(patch::GetInt(0x4D4610 + 2));
    signals_.blend_animation.set_dest(address);
    signals_.blend_animation.install();

    signals_.blend_animation.set_cb(
        [this](const auto& hook, auto clump, auto group, auto id, auto delta) {
          if (clump != psdk_utils::player()->m_pRwClump || !(group == 0 && id == 120))
            return hook.get_trampoline()(clump, group, id, delta);

          const auto order = actor.process_stay_on_feet();

          if (order == decltype(order)::no_fall_anim) {
            id = 2;
            group = 54;
            delta = 50.f;
          }

          return hook.get_trampoline()(clump, group, id, delta);
        });

    return true;
  });

  signals_.weapon_fire.after +=
      [this](const auto& hook, auto& return_value, auto weapon, auto owner, auto&&... args) {
        if (!return_value || owner != psdk_utils::player()) return;
        const auto order_ammo = actor.process_infinite_ammo();

        if (order_ammo == decltype(order_ammo)::not_decrease_ammo) weapon->m_nTotalAmmo++;

        if (weapon->m_nAmmoInClip == weapon->m_nTotalAmmo) return;
        const auto order_clip = actor.process_infinite_clip();

        if (order_clip == decltype(order_clip)::not_decrease_ammo_in_clip) {
          weapon->m_nAmmoInClip++;
        }
      };

  signals_.compute_will_kill_ped.set_cb(
      [this](const auto& hook, auto calculator, auto ped, auto&&... args) {
        if (ped == psdk_utils::player()) {
          const auto order = actor.process_infinite_health();

          if (order == decltype(order)::not_decrease_player_health) {
            return;
          }
        }

        hook.get_trampoline()(calculator, ped, args...);
      });

  signals_.compute_damage_anim.set_cb([this](const auto& hook, auto event, auto ped, auto flag) {
    if (ped == psdk_utils::player()) {
      const auto order = actor.process_anti_stun();

      if (order == decltype(order)::not_execute_compute_damage_anim_for_local_player) {
        return;
      }
    }

    hook.get_trampoline()(event, ped, flag);
  });

  signals_.loop([this]() {
    const auto player = psdk_utils::player();
    if (!player || !player->IsAlive() || !player->GetIsOnScreen()) {
      camera_reset_patch_.restore();
      return;
    }

    const auto order = actor.process_camera_reset();

    switch (order) {
      case actor::disable_camera_reset::order::not_reset_camera_horizontal:
        camera_reset_patch_.horizontal();
        break;
      case actor::disable_camera_reset::order::not_reset_camera_vertical:
        camera_reset_patch_.vertical();
        break;
      case actor::disable_camera_reset::order::not_reset_camera_all:
        camera_reset_patch_.horizontal();
        camera_reset_patch_.vertical();
        break;

      default:
        camera_reset_patch_.restore();
        break;
    }
  });

  signals_.handle_sprint_energy.set_cb([this](const auto& hook, auto ped, auto&&... args) {
    if (ped == psdk_utils::player()) {
      const auto order = actor.process_infinite_run();

      if (order == decltype(order)::not_decrease_time_can_run) {
        return true;
      }
    }

    return hook.get_trampoline()(ped, args...);
  });

  signals_.compute_will_kill_ped.install();
  signals_.compute_damage_anim.install();
  signals_.handle_sprint_energy.install();
}

void injection_in_game_logic::load_vehicle() {
  signals_.loop([this]() {
    if (!psdk_utils::player()) return;
    vehicle.process();
  });

  signals_.nitrous_control.after += [this](const auto& hook, auto automobile, auto set_boosts) {
    if (automobile != psdk_utils::player()->m_pVehicle) return;
    const auto order = vehicle.process_infinite_nitro();
    if (order == decltype(order)::not_decrease_vehicle_nitro_level) {
      automobile->m_fNitroValue = -0.5f;
    }
  };

  signals_.can_vehicle_be_damaged.set_cb([this](const auto& hook, auto veh, auto&&... args) {
    if (veh == psdk_utils::player()->m_pVehicle) {
      const auto order = vehicle.process_infinite_health();
      if (order == decltype(order)::not_decrease_vehicle_health) {
        return false;
      }
    }
    return hook.get_trampoline()(veh, args...);
  });

  signals_.vehicle_damage_automobile.set_cb([this](const auto& hook, auto veh, auto&&... args) {
    if (veh != psdk_utils::player()->m_pVehicle) return hook.get_trampoline()(veh, args...);

    const auto original_value = veh->m_nVehicleFlags.bCanBeDamaged;
    const auto order = vehicle.process_infinite_health();

    if (order == decltype(order)::not_decrease_vehicle_health) {
      veh->m_nVehicleFlags.bCanBeDamaged = false;
    }

    hook.get_trampoline()(veh, args...);
    veh->m_nVehicleFlags.bCanBeDamaged = original_value;
  });

  signals_.vehicle_damage_bike.set_cb(signals_.vehicle_damage_automobile.get_callback());

  signals_.burst_tyre_automobile.set_cb([this](const auto& hook, auto veh, auto&&... args) {
    if (veh != psdk_utils::player()->m_pVehicle) return hook.get_trampoline()(veh, args...);

    const auto original_value = veh->m_nVehicleFlags.bCanBeDamaged;
    const auto order = vehicle.process_infinite_health();

    if (order == decltype(order)::not_decrease_vehicle_health) {
      veh->m_nVehicleFlags.bTyresDontBurst = false;
    }

    const auto result = hook.get_trampoline()(veh, args...);
    veh->m_nVehicleFlags.bTyresDontBurst = original_value;
    return result;
  });

  signals_.burst_tyre_bike.set_cb(signals_.burst_tyre_automobile.get_callback());

  signals_.pre_render.set_cb([this](const auto& hook, auto automobile) {
    if (automobile == psdk_utils::player()->m_pVehicle) {
      const auto order = vehicle.process_drive_on_water();

      if (order == decltype(order)::not_sink_vehicle) {
        drive_on_water_patch_.install();
      }
    }

    const auto result = hook.get_trampoline()(automobile);
    drive_on_water_patch_.restore();
    return result;
  });

  signals_.can_vehicle_be_damaged.install();
  signals_.vehicle_damage_automobile.install();
  signals_.vehicle_damage_bike.install();
  signals_.burst_tyre_automobile.install();
  signals_.burst_tyre_bike.install();
  signals_.pre_render.install();
}
