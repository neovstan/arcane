#include "vehicle.h"

using namespace modification::client::vehicle;

void vehicle::process() {
  acceleration.process(settings.acceleration);
  repair_car.process(settings.repair_car);
  fast_exit.process(settings.fast_exit);
  flip_car.process(settings.flip_car);
  slap_car.process(settings.slap_car);
}

infinite_health::order vehicle::process_infinite_health() {
  return infinite_health.process(settings.infinite_health);
}

infinite_nitro::order vehicle::process_infinite_nitro() {
  return infinite_nitro.process(settings.infinite_nitro);
}

drive_on_water::order vehicle::process_drive_on_water() {
  return drive_on_water.process(settings.drive_on_water);
}
