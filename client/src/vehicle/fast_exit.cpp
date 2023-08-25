#include "fast_exit.h"

using namespace modification::client::vehicle;

fast_exit::order fast_exit::process(bool enabled) {
  return order{enabled};
}
