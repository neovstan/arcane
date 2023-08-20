#include "plugin.h"

#include "main.h"

class modification::client::plugin::initialization {
 public:
  explicit initialization(const information::initialization_data& data) {
    properties().data_ = data;
  }
};

BOOL WINAPI DllMain(HMODULE module_handle, DWORD reason_for_calling, LPVOID reserved) {
  if (reason_for_calling == DLL_PROCESS_ATTACH) {
    const auto nt_header = reinterpret_cast<PIMAGE_NT_HEADERS>(
        reinterpret_cast<std::uintptr_t>(module_handle) +
        reinterpret_cast<PIMAGE_DOS_HEADER>(module_handle)->e_lfanew);

    modification::client::plugin::initialization{{module_handle,
                                                  nt_header->OptionalHeader.SizeOfImage,
                                                  reinterpret_cast<const char*>(reserved)}};
    modification::client::main::instance();
    unloader::initialize(module_handle);
  }

  return TRUE;
}
