#ifndef SAMP_UTILS_SAMP_UTILS_H
#define SAMP_UTILS_SAMP_UTILS_H

#include <sampapi/CChat.h>
#include <sampapi/CDeathWindow.h>
#include <sampapi/CInput.h>
#include <sampapi/CNetGame.h>
#include <sampapi/CScoreboard.h>

#include <unordered_map>

class CPed;

namespace samp_utils {
constexpr sampapi::ID invalid_id() { return 65535; }

enum class version { unknown, r1, r3 };
version get_version();

template <class T>
auto execute(T func) {
  const auto version = get_version();
  if (version == version::r1) {
    return func(sampapi::versions::v037r1);
  } else if (version == version::r3) {
    return func(sampapi::versions::v037r3);
  }
  using return_type_t = decltype(func(sampapi::versions::v037r1));
  if constexpr (!std::is_void_v<return_type_t>) {
    return return_type_t{};
  }
}

void patch_anticheat();

std::unordered_map<CPed*, sampapi::ID>& get_players();

template <class Version>
class invoke {
 public:
  static bool is_initialized() {
    const auto netgame = RefNetGame(Version{});
    if (!netgame) return false;
    const auto chat = RefChat(Version{});
    if (!chat) return false;
    const auto input = RefInputBox(Version{});
    if (!input) return false;
    const auto death_window = RefDeathWindow(Version{});
    if (!death_window) return false;
    return netgame->GetRakClient() && netgame->GetVehiclePool() &&
           netgame->GetActorPool() && netgame->GetObjectPool() &&
           netgame->GetPlayerPool() && netgame->GetMenuPool() &&
           netgame->GetPickupPool();
  }

  static void update_players() {
    if (!is_initialized()) return;

    const auto player_pool = RefNetGame(Version{})->GetPlayerPool();

    for (sampapi::ID i{}; i <= player_pool->m_nLargestId; ++i) {
      const auto remote_player = player_pool->GetPlayer(i);
      if (!remote_player) continue;

      const auto player_ped = remote_player->m_pPed;
      if (!player_ped) continue;

      get_players()[player_ped->m_pGamePed] = i;
    }
  }

  static sampapi::ID get_id_by_ped(CPed* const ped) {
    if (!is_initialized()) return invalid_id();
    return get_players()[ped];
  }

  static bool is_player_playing(const sampapi::ID id) {
    if (id == invalid_id() || !is_initialized()) return false;
    const auto remote_player =
        RefNetGame(Version{})->GetPlayerPool()->GetPlayer(id);
    return remote_player && remote_player->m_pPed &&
           remote_player->m_pPed->m_pGamePed;
  }

  static bool is_player_alive(const sampapi::ID id) {
    if (!is_player_playing(id)) return false;
    return RefNetGame(Version{})
               ->GetPlayerPool()
               ->GetPlayer(id)
               ->m_onfootData.m_nHealth != 0;
  }

  static bool is_player_afk(const sampapi::ID id) {
    if (!is_player_playing(id)) return false;
    return RefNetGame(Version{})->GetPlayerPool()->GetPlayer(id)->m_nStatus ==
           2;
  }

  static float get_nametag_draw_distance() {
    const auto settings = RefNetGame(Version{})->m_pSettings;
    return settings->m_bNameTags ? settings->m_fNameTagsDrawDist : 0.0f;
  }

  static std::string get_name(const sampapi::ID id) {
    if (!is_player_playing(id)) return {};
    const auto player_pool = RefNetGame(Version{})->GetPlayerPool();
    return player_pool->GetName(id);
  }

  static std::uint8_t get_health(const sampapi::ID id) {
    if (!is_player_playing(id)) return {};
    return RefNetGame(Version{})
        ->GetPlayerPool()
        ->GetPlayer(id)
        ->m_onfootData.m_nHealth;
  }

  static std::uint8_t get_armor(const sampapi::ID id) {
    if (!is_player_playing(id)) return {};
    return RefNetGame(Version{})
        ->GetPlayerPool()
        ->GetPlayer(id)
        ->m_onfootData.m_nArmor;
  }

  static std::uint32_t get_color_as_argb(const sampapi::ID id) {
    if (!is_player_playing(id)) return {};
    return RefNetGame(Version{})
        ->GetPlayerPool()
        ->GetPlayer(id)
        ->GetColorAsARGB();
  }

  static void set_nametag_visibility_status(const bool status) {
    if (!is_initialized()) return;
    RefNetGame(Version{})->m_pSettings->m_bNameTags = status;
  }
};
}  // namespace samp_utils

#endif  // SAMP_UTILS_SAMP_UTILS_H
