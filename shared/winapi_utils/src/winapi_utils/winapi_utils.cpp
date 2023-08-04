#include "winapi_utils.h"

#include <VMProtectSDK.h>  // VMProtectGetCurrentHWID
#include <psapi.h>         // GetModuleFileNameEx
#include <tlhelp32.h>      // CreateToolhelp32Snapshot

#include "data_representation/data_representation.h"  // data_representation::sha256_file

HANDLE winapi_utils::find_process_handle_by_sha256_hash(std::string_view hash) {
  PROCESSENTRY32 process_entry{0};
  process_entry.dwSize = sizeof(PROCESSENTRY32);

  HANDLE handle{};

  HANDLE snapshot{CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL)};
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
  PROCESSENTRY32 process_entry{0};
  process_entry.dwSize = sizeof(PROCESSENTRY32);

  HANDLE handle{};

  HANDLE snapshot{CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL)};
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

std::string winapi_utils::hwid() {
  const auto size = VMProtectGetCurrentHWID(nullptr, 0);
  auto buffer = new char[size]{};
  VMProtectGetCurrentHWID(buffer, size);
  std::string hwid{buffer};
  delete[] buffer;
  return hwid;
}
