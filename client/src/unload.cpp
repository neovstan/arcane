#include "unload.h"

#include <stdexcept>

#include <winapi_utils/winapi_utils.h>

using namespace modification::client;

unload::unload(void* module_handle) : module_{module_handle} {
}

void unload::execute() {
  const auto allocated = VirtualAlloc(nullptr, 1024, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
  if (!allocated) throw std::runtime_error{"virtual alloc failed"};

  std::memcpy(allocated, reinterpret_cast<void*>(&shellcode), 1024);

  const auto data = reinterpret_cast<shellcode_data*>(std::malloc(sizeof(shellcode_data)));
  if (!data) throw std::runtime_error{"malloc failed"};

  data->first_node = nullptr;
  data->virtual_free = &VirtualFree;
  data->free = &std::free;
  data->tls_entry_addr = module_.tls_entry();
  data->list_entry_addr = module_.list_entry();

  auto node = &data->first_node;
  auto base = reinterpret_cast<std::uintptr_t>(module_.handle());

  MEMORY_BASIC_INFORMATION mbi{};
  while (VirtualQuery(reinterpret_cast<void*>(base), &mbi, sizeof(MEMORY_BASIC_INFORMATION))) {
    if (mbi.Type != MEM_PRIVATE) break;

    *node = reinterpret_cast<node_data*>(std::malloc(sizeof(node_data)));

    if (*node == nullptr) return;

    std::memset(*node, 0, sizeof(node_data));
    (*node)->mbi = mbi;
    node = &(*node)->next;
    std::free(*node);

    base = reinterpret_cast<uintptr_t>(mbi.BaseAddress) + mbi.RegionSize;
  }

  CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(allocated), data, 0, nullptr);
}

unsigned long __stdcall unload::shellcode(shellcode_data* data) {
  auto tls = reinterpret_cast<module_data::tls_entry_t*>(data->tls_entry_addr->TlsEntryLinks.Flink);

  while (true) {
    auto& entry_links = tls->TlsEntryLinks;

    if (entry_links.Flink == reinterpret_cast<PLIST_ENTRY>(data->list_entry_addr)) {
      reinterpret_cast<module_data::tls_entry_t*>(data->list_entry_addr->TlsEntryLinks.Flink)
          ->TlsEntryLinks.Blink = reinterpret_cast<PLIST_ENTRY>(tls);
      entry_links.Flink = entry_links.Flink->Flink;
      break;
    }

    tls = reinterpret_cast<module_data::tls_entry_t*>(tls->TlsEntryLinks.Flink);
  }

  auto node = data->first_node;

  while (node != nullptr) {
    data->virtual_free(node->mbi.BaseAddress, 0, MEM_RELEASE);
    auto next = node->next;
    data->free(node);
    node = next;
  }

  data->free(data);
  return EXIT_SUCCESS;
}

unload::module_data::module_data(void* handle)
    : handle_{handle}, tls_entry_{winapi_utils::get_tls_entry()} {
  if (!tls_entry_) throw std::runtime_error{"tls entry isn't found"};
}

void* modification::client::unload::module_data::handle() const {
  return handle_;
}

unload::module_data::tls_entry_t* unload::module_data::tls_entry() const {
  return tls_entry_;
}

unload::module_data::tls_entry_t* unload::module_data::list_entry() const {
  return reinterpret_cast<tls_entry_t*>(tls_entry_->TlsEntryLinks.Blink);
}
