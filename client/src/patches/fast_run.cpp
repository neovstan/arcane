#include "fast_run.h"

#include <Patch.h>

using namespace modification::client::patches;

fast_run::fast_run() : addresses_{0x60B435 + 2, 0x60B446 + 2}, default_value_{0x858624} {
  for (const auto address : addresses_) {
    plugin::patch::SetPointer(address, &speed);
  }
}

fast_run::~fast_run() {
  for (const auto address : addresses_) {
    plugin::patch::SetUInt(address, default_value_);
  }
}
