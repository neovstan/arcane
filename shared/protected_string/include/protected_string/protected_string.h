#ifndef PROTECTED_STRING_H
#define PROTECTED_STRING_H

#include <VMProtectSDK.h>

namespace protected_string {
class scoped {
 public:
  explicit scoped(const char* p);
  scoped(const scoped&) = delete;
  scoped(scoped&&) = delete;
  ~scoped();

  operator const char*() const;

 private:
  const char* p_;
};
}  // namespace protected_string

#define scoped_protected_string(p) \
  protected_string::scoped {       \
    VMProtectDecryptStringA(p)     \
  }

#define scoped_protected_std_string(p) \
  std::string {                        \
    scoped_protected_string(p)         \
  }

#define scoped_protected_qstring(p) \
  QString {                         \
    scoped_protected_string(p)      \
  }

#endif  // PROTECTED_STRING_H
