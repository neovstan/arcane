//
// Created by Developer on 20.08.2023.
//

#ifndef ARCANE_INFINITE_NITRO_H
#define ARCANE_INFINITE_NITRO_H

namespace modification::client::vehicle {
class infinite_nitro {
 public:
  enum class order { no, not_decrease_vehicle_nitro_level };

 public:
  order process(bool enabled);
};
}

#endif //ARCANE_INFINITE_NITRO_H
