#pragma once

#include <windows.h>
#include <malloc.h>
#include <cstdint>

namespace win32 {
std::uintptr_t getProcAddress(::HMODULE hModule, const char* szAPIName);
::HANDLE getCurrentProcess();
::HMODULE getModuleBase(const char* lpModuleName);
}  // namespace win32