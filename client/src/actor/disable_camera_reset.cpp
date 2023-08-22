#include "disable_camera_reset.h"

using namespace modification::client::actor;

disable_camera_reset::order disable_camera_reset::process(
    const disable_camera_reset::data& settings) {
  if (!settings.enable || (!settings.horizontal && !settings.vertical))
    return order::no;
  else if (settings.horizontal && settings.vertical)
    return order::not_restore_camera_all;

  return settings.horizontal ? order::not_restore_camera_horizontal
                             : order::not_restore_camera_vertical;
}
