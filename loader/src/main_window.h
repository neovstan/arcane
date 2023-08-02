#ifndef ARCANE_LOADER_SRC_MAIN_WINDOW_H
#define ARCANE_LOADER_SRC_MAIN_WINDOW_H

#include <mutex>
#include <string>

namespace modification::loader {
class main_window {
 public:
  void update();
  void inject();
  void load();
  void save();

  int width{250}, height{150};
  bool state{true}, show_password{false}, save_data{true};
  char login[128]{}, password[128]{};
  std::mutex mtx{};
  std::string status{"Not injected"};
  bool is_injecting{};
};
}  // namespace modification::loader

#endif  // ARCANE_LOADER_SRC_MAIN_WINDOW_H
