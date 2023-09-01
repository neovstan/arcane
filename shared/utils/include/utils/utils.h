#ifndef UTILS_UTILS_H
#define UTILS_UTILS_H

#include <string>

namespace utils {
std::string hwid();
namespace memory {
void swap_endianness(wchar_t& a);
bool compare_data(uint8_t* data, const char* bytes, const char* mask);
uintptr_t find_pattern(const char* name, const char* bytes, const char* mask);
}
}  // namespace utils

#endif  // UTILS_UTILS_H
