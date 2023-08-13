#ifndef ARCANE_CLIENT_SRC_ACTOR_AIR_WALKING_H
#define ARCANE_CLIENT_SRC_ACTOR_AIR_WALKING_H

#include <chrono>

#include <psdk_utils/local_vector.h>

#include <arcane_packets/configuration.hpp>

namespace modification::client::actor {
class air_walking {
  using data = packets::configuration::actor::air_walking_data;

 public:
  void process(const data& settings);

 private:
  void maintain_validity();

 private:
  float get_camera_heading();
  float get_delta_time();
  float get_current_speed(float delta_time, float speed, float acceleration_time);
  psdk_utils::local_vector compute_movement_vector(const data::movement_keys& keys, float heading,
                                                   float movement_delta_magnitude);

 private:
  void move(const psdk_utils::local_vector& vec);
  bool is_player_ready_to_move();

 private:
  using clock = std::chrono::steady_clock;

 private:
  clock::time_point time_;
  float speed_;
};
}  // namespace modification::client::actor

#endif  // ARCANE_CLIENT_SRC_ACTOR_AIR_WALKING_H
