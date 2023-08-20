#include "winapi_utils.h"

#include <VMProtectSDK.h>  // VMProtectGetCurrentHWID
#include <psapi.h>         // GetModuleFileNameEx
#include <tlhelp32.h>      // CreateToolhelp32Snapshot

#include <utils/utils.h>
#include <data_representation/data_representation.h>

HANDLE winapi_utils::find_process_handle_by_sha256_hash(std::string_view hash) {
  PROCESSENTRY32 process_entry{};
  process_entry.dwSize = sizeof(PROCESSENTRY32);

  HANDLE handle{};

  auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
  if (Process32First(snapshot, &process_entry)) {
    while (Process32Next(snapshot, &process_entry)) {
      handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_entry.th32ProcessID);

      if (!handle) continue;

      char path[MAX_PATH]{};
      if (GetModuleFileNameEx(handle, NULL, path, MAX_PATH) == 0) continue;

      if (data_representation::sha256_file(path) == hash) {
        break;
      } else {
        handle = nullptr;
      }
    }
  }

  CloseHandle(snapshot);
  return handle;
}

HANDLE winapi_utils::find_process_handle_by_pattern(std::uintptr_t address,
                                                    std::string_view pattern) {
  PROCESSENTRY32 process_entry{};
  process_entry.dwSize = sizeof(PROCESSENTRY32);

  HANDLE handle{};

  auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
  if (Process32First(snapshot, &process_entry)) {
    while (Process32Next(snapshot, &process_entry)) {
      handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_entry.th32ProcessID);

      if (!handle) continue;

      std::vector<std::uint8_t> bytes(pattern.size() - 1);

      if (!ReadProcessMemory(handle, reinterpret_cast<LPCVOID>(address), bytes.data(), bytes.size(),
                             nullptr)) {
        handle = nullptr;
        continue;
      }

      if (std::memcmp(bytes.data(), pattern.data(), bytes.size()) == 0) {
        break;
      } else {
        handle = nullptr;
      }
    }
  }

  CloseHandle(snapshot);
  return handle;
}

winapi_utils::details::PTLS_ENTRY winapi_utils::get_tls_entry() {
  auto address = utils::memory::find_pattern(
      "ntdll.dll", "\xC7\x45\x00\x00\x00\x00\x00\xA1\x00\x00\x00\x00\x89", "xx?????x????x");

  if (!address)
    address = utils::memory::find_pattern("ntdll.dll", "\xC7\x45\xD4\x00\x00\x00\x00\x8B\x1D",
                                          "xxx????xx");

  return address ? *reinterpret_cast<details::PTLS_ENTRY*>(address + 3) : nullptr;
}
