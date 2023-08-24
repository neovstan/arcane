#include "drive_on_water.h"

using namespace modification::client::vehicle;

drive_on_water::order drive_on_water::process(bool enabled) {
  return order{enabled};
}
