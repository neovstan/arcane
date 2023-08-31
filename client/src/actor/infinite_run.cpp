#include "infinite_run.h"

using namespace modification::client::actor;

infinite_run::order infinite_run::process(bool enabled) {
  return order{enabled};
}
