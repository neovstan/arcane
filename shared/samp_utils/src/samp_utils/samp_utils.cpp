#include "samp_utils.h"

#include <sampapi/sampapi.h>
#include <windows.h>

samp_utils::version samp_utils::get_version() {
  const auto base = sampapi::GetBase();
  if (!base) return {};

  const auto header = reinterpret_cast<::IMAGE_NT_HEADERS*>(
      base + reinterpret_cast<::IMAGE_DOS_HEADER*>(base)->e_lfanew);

  switch (header->OptionalHeader.AddressOfEntryPoint) {
    case 0x31DF13:
      return version::r1;
    case 0xCC4D0:
      return version::r3;
    default:
      return {};
  }
}

void samp_utils::patch_anticheat() {
  const auto version = samp_utils::get_version();
  const auto base = sampapi::GetBase();
  if (version == version::r1) {
    const auto first_instruction = reinterpret_cast<void*>(base + 0x7165E),
               second_instruction = reinterpret_cast<void*>(base + 0x71680);

    DWORD old_prot;
    VirtualProtect(first_instruction, 5, PAGE_READWRITE, &old_prot);
    VirtualProtect(second_instruction, 5, PAGE_READWRITE, &old_prot);

    std::memset(first_instruction, 0x90, 5);
    std::memset(second_instruction, 0x90, 5);
  } else if (version == version::r3) {
    DWORD old_prot;
    VirtualProtect(reinterpret_cast<void*>(base + 0x60EEB), 1, PAGE_READWRITE, &old_prot);
    VirtualProtect(reinterpret_cast<void*>(base + 0xC4DC7), 1, PAGE_READWRITE, &old_prot);

    *reinterpret_cast<std::uint8_t*>(base + 0x60EEB) = 0xEB;
    *reinterpret_cast<std::uint8_t*>(base + 0xC4DC7) = 0xEB;
  }
}

std::unordered_map<CPed*, sampapi::ID>& samp_utils::get_players() {
  static std::unordered_map<CPed*, sampapi::ID> instance{};
  return instance;
}
