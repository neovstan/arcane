#include "infinite_clip.h"

using namespace modification::client::actor;

infinite_clip::order infinite_clip::process(bool enabled) {
  return order{enabled};
}
