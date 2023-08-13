#ifndef ARCANE_CLIENT_SRC_CLIENT_H
#define ARCANE_CLIENT_SRC_CLIENT_H

#include <memory>
#include <thread>

namespace modification::client {
class injection_in_game_logic;

class client {
 public:
  explicit client(std::shared_ptr<injection_in_game_logic> injection);

  std::thread& thread();

 private:
  void process();

 private:
  std::shared_ptr<injection_in_game_logic> injection_;

 private:
  std::thread thread_;
};
}  // namespace modification::client

#endif  // ARCANE_CLIENT_SRC_CLIENT_H
