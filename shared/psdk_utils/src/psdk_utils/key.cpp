#include "key.h"

#include <CTimer.h>

static unsigned char current_states[256]{};
static unsigned char previous_states[256]{};
static unsigned int time_delay_pressed[256]{};

void psdk_utils::key::update() {
  memcpy(previous_states, current_states, 256);
  GetKeyboardState(current_states);
}

bool psdk_utils::key::down(unsigned int key) {
  return key < 256 && (current_states[key] & 0x80);
}

bool psdk_utils::key::pressed(unsigned int key) {
  return key < 256 && (current_states[key] & 0x80) && !(previous_states[key] & 0x80);
}

bool psdk_utils::key::released(unsigned int key) {
  return key < 256 && !(current_states[key] & 0x80) && (previous_states[key] & 0x80);
}

bool psdk_utils::key::down_delayed(unsigned int key, unsigned int time) {
  if (key < 256 && (current_states[key] & 0x80)) {
    if (!(previous_states[key] & 0x80) ||
        CTimer::m_snTimeInMilliseconds > (time_delay_pressed[key] + time)) {
      time_delay_pressed[key] = CTimer::m_snTimeInMilliseconds;
      return true;
    }
  }
  return false;
}
