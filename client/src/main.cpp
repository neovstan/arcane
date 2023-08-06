#include "main.h"

modification::client::main::main() {
  auto& s = injection_.actor.settings.spread_control;
  s.enable = true;
}

modification::client::main::~main() {
}
