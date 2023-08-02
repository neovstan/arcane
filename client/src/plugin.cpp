#include "plugin.h"

#include "main.h"

class modification::client::plugin::initialization {
 public:
  explicit initialization(const information::initialization_data& data) {
    properties().data_ = data;
  }
};

BOOL WINAPI DllMain(const HMODULE module_handle, const DWORD reason_for_calling,
                    const LPVOID reserved) {
  if (reason_for_calling == DLL_PROCESS_ATTACH) {
    const auto ntheader = reinterpret_cast<IMAGE_NT_HEADERS*>(
        reinterpret_cast<std::uintptr_t>(module_handle) +
        reinterpret_cast<IMAGE_DOS_HEADER*>(module_handle)->e_lfanew);

    modification::client::plugin::initialization{{module_handle,
                                                  ntheader->OptionalHeader.SizeOfImage,
                                                  reinterpret_cast<const char*>(reserved)}};
    modification::client::main::instance();
  }

  return TRUE;
}
