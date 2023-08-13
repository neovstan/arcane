#ifndef ARCANE_CLIENT_SRC_VISUALS_VISUALS_H
#define ARCANE_CLIENT_SRC_VISUALS_VISUALS_H

#include <chrono>

#include <imgui.h>
#include <psdk_utils/local_vector.h>
#include <arcane_packets/configuration.hpp>

class CPed;

namespace modification::client::visuals {
class visuals {
 public:
  struct bar {
    ImColor progress_color{};
    psdk_utils::local_vector size{50.0f, 5.0f, 0.0f};
    float border_width{2.0f}, rounding{5.0f};
    ImColor background_color{0.1f, 0.1f, 0.1f, 1.0f};
  };

 public:
  visuals();

  void initialize();
  void process();

 public:
  void flat_box(CPed* ped, ImColor color);
  void three_dimensional_box(CPed* ped, ImColor color);
  void line(CPed* ped, ImColor color);
  void name(CPed* ped, ImColor color, bar health_bar = {{0.92f, 0.25f, 0.2f, 1.0f}},
            bar armor_bar = {{1.0f, 1.0f, 1.0f, 1.0f}});
  void bones(CPed* ped, ImColor color);
  void fov(CPed* ped, ImColor vector_aimbot_color, ImColor silent_aimbot_color);
  void framerate(ImColor color = {0.46f, 0.72f, 0.0f, 1.0f});

 public:
  void draw_progress_bar(bar bar, psdk_utils::local_vector position, float progress, float max);

  void draw_flat_box_in_space(const psdk_utils::local_vector& center, float angle, float radius,
                              float height, float line_width, ImColor color);

 public:
  struct packets::configuration::visuals settings;

 private:
  ImDrawList* draw();
  ImVec2 imvec2(const psdk_utils::local_vector& in);

 private:
  const float line_width_;
  const ImColor vector_aimbot_fov_color_;
  const ImColor silent_aimbot_fov_color_;

  struct font {
    ImFont* ptr;
    float size;
  };

  font nametags_font_;
  font framerate_font_;

  float framerate_;
  std::chrono::steady_clock::time_point framerate_update_time_;
};
}  // namespace modification::client::visuals

#endif  // ARCANE_CLIENT_SRC_VISUALS_VISUALS_H
