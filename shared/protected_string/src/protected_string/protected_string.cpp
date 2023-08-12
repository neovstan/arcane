#include "protected_string.h"

protected_string::scoped::scoped(const char* p) : p_{p} {
}

protected_string::scoped::~scoped() {
  VMProtectFreeString(p_);
}

protected_string::scoped::operator const char*() const {
  return p_;
}
