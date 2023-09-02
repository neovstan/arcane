#include "main.h"

#include <utils/utils.h>

modification::client::main::main() {
#ifdef VMP_DEBUG
  injection_ = std::make_shared<injection_in_game_logic>(
      "neovstan", "Dofragen84!",
      "04f69ea92742cedbc361a73049716f3301addeb551dd5ed006bebbf4e113a171");
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
