#include "main.h"

#include <utils/utils.h>

modification::client::main::main() {
#ifdef VMP_DEBUG
  injection_ = std::make_shared<injection_in_game_logic>(
      "neovstan", "5e6ff6c601541e6c7cccac523647b79354f0e745894c3d4b94edc24a396211bc",
      "0262776b8774d72c53a53d12c4146a55291bd60fd27f80b7a5db8e0f1feee896");
#else
  const std::string data{plugin::properties().data()};
  const auto separator = data.find(';');
  if (separator != std::string::npos) {
    const auto username = data.substr(0, separator);
    const auto password = data.substr(separator + 1);
    injection_ = std::make_shared<injection_in_game_logic>(username, password, utils::hwid());
  }
#endif

  client_ = std::make_unique<class client>(injection_);
  unload_ = std::make_unique<class unload>(plugin::properties().module_handle());
}

modification::client::main::~main() {
}
