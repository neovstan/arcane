#include "vehicle.h"

using namespace modification::client::vehicle;

void vehicle::process() {
}

infinite_nitro::order vehicle::process_infinite_nitro() {
  return infinite_nitro.process(settings.infinite_nitro);
}

drive_on_water::order vehicle::process_drive_on_water() {
  return drive_on_water.process(settings.drive_on_water);
}
