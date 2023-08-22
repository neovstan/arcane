#include "camera_reset.h"

#include <Patch.h>

using namespace modification::client::patches;

camera_reset::camera_reset() : addresses_{0x5231A6, 0x52322D, 0x5233BA}, default_value_{0x75} {
}

camera_reset::~camera_reset() {
  restore();
}

void camera_reset::horizontal() const {
  plugin::patch::SetUChar(addresses_.at(0), 0xEB);
}

void camera_reset::vertical() const {
  for (auto i = 1; i != addresses_.size(); i++) {
    plugin::patch::SetUChar(addresses_.at(i), 0xEB);
  }
}

void camera_reset::restore() const {
  for (const auto address : addresses_) {
    plugin::patch::SetUChar(address, default_value_);
  }
}
