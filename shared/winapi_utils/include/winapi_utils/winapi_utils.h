#ifndef WINAPI_UTILS_WINAPI_UTILS_H
#define WINAPI_UTILS_WINAPI_UTILS_H

#include <string>
// let the standard libraries be included first than windows.h

#include <windows.h>

namespace winapi_utils {
HANDLE find_process_handle_by_sha256_hash(std::string_view hash);
HANDLE find_process_handle_by_pattern(std::uintptr_t address,
                                      std::string_view pattern);
std::string hwid();
}  // namespace winapi_utils

#endif  // WINAPI_UTILS_WINAPI_UTILS_H
