#include "stay_on_feet.h"

using namespace modification::client::actor;

stay_on_feet::order stay_on_feet::process(bool enabled) {
  return order{enabled};
}
