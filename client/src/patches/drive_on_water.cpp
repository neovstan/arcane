#include "drive_on_water.h"

#include <Patch.h>

using namespace modification::client::patches;

drive_on_water::drive_on_water() : address_{0x6AAB7D}, default_value_{0x0A74} {
}

drive_on_water::~drive_on_water() {
  restore();
}

void drive_on_water::install() const {
  plugin::patch::SetUShort(address_, 0x9090);
}

void drive_on_water::restore() const {
  plugin::patch::SetUShort(address_, default_value_);
}
