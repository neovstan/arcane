#ifndef ARCANE_CLIENT_SRC_UNLOAD_H
#define ARCANE_CLIENT_SRC_UNLOAD_H

#include <winapi_utils/winapi_utils.h>
#include <vector>

namespace modification::client {
class unload {
 public:
  explicit unload(void* module_handle);
  unload(const unload&) = delete;
  unload(unload&&) = delete;

  void get_arcane_paths();
  void clear_nvidia_panel();
  void clear_registry(HKEY hkey, const wchar_t* path);
  void clear_arcane_paths();

 public:
  void execute();

 private:
  class module_data {
   public:
    using tls_entry_t = winapi_utils::details::TLS_ENTRY;

   public:
    explicit module_data(void* handle);

    [[nodiscard]] void* handle() const;
    [[nodiscard]] tls_entry_t* tls_entry() const;
    [[nodiscard]] tls_entry_t* list_entry() const;

   private:
    void* handle_;
    tls_entry_t* tls_entry_;
  };

  struct node_data {
    MEMORY_BASIC_INFORMATION mbi;
    node_data* next;
  };

  struct shellcode_data {
    node_data* first_node;
    module_data::tls_entry_t* tls_entry_addr;
    module_data::tls_entry_t* list_entry_addr;
    decltype(&VirtualFree) virtual_free;
    decltype(&free) free;
  };

 private:
  static unsigned long __stdcall shellcode(shellcode_data* data);

 private:
  module_data module_;
  std::vector<std::wstring> paths_;
};
}  // namespace modification::client

#endif  // ARCANE_CLIENT_SRC_UNLOAD_H