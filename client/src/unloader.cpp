#include "unloader.h"

PTLS_ENTRY unloader::get_tls_entry_ptr() {
  uintptr_t address = utils::memory::find_pattern(
      "ntdll.dll", "\xC7\x45\x00\x00\x00\x00\x00\xA1\x00\x00\x00\x00\x89", "xx?????x????x");
  // new win10/11

  if (address == 0)
    address = utils::memory::find_pattern("ntdll.dll", "\xC7\x45\xD4\x00\x00\x00\x00\x8B\x1D",
                                          "xxx????xx");  // win7

  return address != 0 ? *reinterpret_cast<PTLS_ENTRY*>(address + 3) : nullptr;
}

unsigned long __stdcall unloader::shellcode(shellcode_data* data) {
  PTLS_ENTRY list_entry = data->tls_list_addr;
  PTLS_ENTRY tls = reinterpret_cast<PTLS_ENTRY>(list_entry->TlsEntryLinks.Flink);

  while (true) {
    if (tls->TlsEntryLinks.Flink == reinterpret_cast<PLIST_ENTRY>(data->tls_node_addr)) {
      reinterpret_cast<PTLS_ENTRY>(data->tls_node_addr->TlsEntryLinks.Flink)->TlsEntryLinks.Blink =
          reinterpret_cast<PLIST_ENTRY>(tls);
      tls->TlsEntryLinks.Flink = tls->TlsEntryLinks.Flink->Flink;
      break;
    }
    tls = reinterpret_cast<PTLS_ENTRY>(tls->TlsEntryLinks.Flink);
  }

  node_data* node = data->first_node;

  while (node != nullptr) {
    data->virtual_free(node->mbi.BaseAddress, 0, MEM_RELEASE);
    node_data* next = node->next;
    data->free(node);
    node = next;
  }

  data->free(data);
  return 777;
}

bool unloader::initialize(HMODULE mod) {
  this_.mod_base_ = reinterpret_cast<uintptr_t>(mod);
  PTLS_ENTRY entry = get_tls_entry_ptr();

  if (entry == nullptr) return false;

  this_.mod_tls_ = entry->TlsEntryLinks.Blink;
  return true;
}

void unloader::execute() {
  if (this_.mod_base_ == 0) return;

  auto allocated = VirtualAlloc(nullptr, 1024, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

  if (allocated == nullptr) return;

  memcpy(allocated, shellcode, 1024);

  shellcode_data* data = reinterpret_cast<shellcode_data*>(malloc(sizeof(shellcode_data)));

  if (data == nullptr) return;

  data->first_node = nullptr;
  data->virtual_free = &VirtualFree;
  data->free = &free;
  data->tls_list_addr = get_tls_entry_ptr();
  data->tls_node_addr = reinterpret_cast<PTLS_ENTRY>(this_.mod_tls_);

  node_data** node = &data->first_node;

  MEMORY_BASIC_INFORMATION mbi{};

  uintptr_t base = this_.mod_base_;

  while (VirtualQuery(reinterpret_cast<void*>(base), &mbi, sizeof(MEMORY_BASIC_INFORMATION)) != 0) {
    if (mbi.Type != MEM_PRIVATE) break;

    *node = reinterpret_cast<node_data*>(malloc(sizeof(node_data)));

    if (*node == nullptr) return;

    memset(*node, 0, sizeof(node_data));
    (*node)->mbi = mbi;
    node = &(*node)->next;
    free(*node);

    base = reinterpret_cast<uintptr_t>(mbi.BaseAddress) + mbi.RegionSize;
  }
  CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(allocated), data, 0, nullptr);
}

unloader unloader::this_;