#include "main.h"

#include <utils/utils.h>

modification::client::main::main() {
#ifdef VMP_DEBUG
  injection_ = std::make_shared<injection_in_game_logic>(
      "neovstan", "Dofragen84!",
      "ba9f02cc366af8a77ac19c607a6eef85a18e355566d18d4ed49573cae8df6e4c");
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
