#ifndef UTILS_RANDOMIZER_HPP
#define UTILS_RANDOMIZER_HPP

#include <chrono>
#include <random>

namespace utils {
template <class T, class Distribution, class Engine>
class randomizer_engine {
 public:
  randomizer_engine() {}
  randomizer_engine(T min, T max) : distribution_{min, max} {}

  T operator()() {
    engine_.seed(std::random_device{}());
    return distribution_(engine_);
  }

 private:
  Engine engine_;
  Distribution distribution_;
};

template <class T>
class randomizer;

template <>
class randomizer<float>
    : public randomizer_engine<float, std::uniform_real_distribution<float>,
                               std::default_random_engine> {
 public:
  randomizer() {}
  randomizer(float min, float max) : randomizer_engine{min, max} {}
};

template <>
class randomizer<int>
    : public randomizer_engine<int, std::uniform_int_distribution<int>,
                               std::default_random_engine> {
 public:
  randomizer() {}
  randomizer(int min, int max) : randomizer_engine{min, max} {}
};
}  // namespace utils

#endif  // UTILS_RANDOMIZER_HPP
