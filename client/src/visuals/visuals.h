#ifndef ARCANE_CLIENT_SRC_VISUALS_VISUALS_H
#define ARCANE_CLIENT_SRC_VISUALS_VISUALS_H

#include <imgui.h>

#include <configuration/configuration.hpp>

#include "CVector.h"
#include "CVector2D.h"

class CPed;

namespace modification::client::visuals {
class visuals {
 public:
  struct bar {
    ImU32 progress_color{};
    CVector2D size{50.0f, 5.0f};
    float border_width{2.0f}, rounding{5.0f};
    ImU32 background_color{ImGui::ColorConvertFloat4ToU32({0.1f, 0.1f, 0.1f, 1.0f})};
  };

  void initialize();
  void process();

  void flat_box(CPed* ped, ImU32 color);
  void three_dimensional_box(CPed* ped, ImU32 color);
  void line(CPed* ped, ImU32 color);
  void name(CPed* ped, ImU32 color,
            bar health_bar = {ImGui::ColorConvertFloat4ToU32({0.92f, 0.25f, 0.2f, 1.0f})},
            bar armor_bar = {ImGui::ColorConvertFloat4ToU32({1.0f, 1.0f, 1.0f, 1.0f})});
  void bones(CPed* ped, ImU32 color);
  void fov(CPed* ped, ImU32 vector_aimbot_color, ImU32 silent_aimbot_color);
  void framerate(ImU32 color = {ImGui::ColorConvertFloat4ToU32({0.46f, 0.72f, 0.0f, 1.0f})});

  void draw_progress_bar(bar bar, CVector2D position, float progress, float max);

  void draw_flat_box_in_space(const CVector& center, float angle, float radius, float height,
                              float line_width, ImU32 color);

  CVector get_point_on_circumference(const CVector& center, float angle, float radius);

  configuration::visuals settings{};

 private:
  ImDrawList* draw() { return ImGui::GetForegroundDrawList(); }

  ImVec2 imvec2(const CVector& in) { return ImVec2{in.x, in.y}; }
  ImVec2 imvec2(const CVector2D& in) { return ImVec2{in.x, in.y}; }

  const float line_width_{1.5f};
  const ImU32 vector_aimbot_fov_color{ImGui::ColorConvertFloat4ToU32({0.09f, 0.92f, 0.59f, 1.0f})},
      silent_aimbot_fov_color{ImGui::ColorConvertFloat4ToU32({0.09f, 0.92f, 0.88f, 1.0f})};

  struct font {
    ImFont* ptr{};
    float size{};
  };

  font nametags_font_{};
  font framerate_font_{};

  float framerate_{};
  std::chrono::steady_clock::time_point framerate_update_time_{};
};
}  // namespace modification::client::visuals

#endif  // ARCANE_CLIENT_SRC_VISUALS_VISUALS_H
