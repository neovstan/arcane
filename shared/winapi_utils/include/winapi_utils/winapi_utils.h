#ifndef WINAPI_UTILS_WINAPI_UTILS_H
#define WINAPI_UTILS_WINAPI_UTILS_H

#include <string>
// let the standard libraries be included first than windows.h

#include <windows.h>

#include "details.hpp"

namespace winapi_utils {
HANDLE find_process_handle_by_sha256_hash(std::string_view hash);
HANDLE find_process_handle_by_pattern(std::uintptr_t address, std::string_view pattern);
details::PTLS_ENTRY get_tls_entry();
bool is_process_elevated();
}  // namespace winapi_utils

#endif  // WINAPI_UTILS_WINAPI_UTILS_H
