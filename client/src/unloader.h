#ifndef UNLOADER_H
#define UNLOADER_H

#include <utils/utils.h>
#include <windows.h>

typedef struct _UNICODE_STRING {
  USHORT Length;
  USHORT MaximumLength;
  PWSTR Buffer;
} UNICODE_STRING;

typedef struct _LDR_DATA_TABLE_ENTRY {
  LIST_ENTRY InLoadOrderLinks;
  LIST_ENTRY InMemoryOrderLinks;
  union {
    LIST_ENTRY InInitializationOrderLinks;
    LIST_ENTRY InProgressLinks;
  };
  PVOID DllBase;
  PVOID Entrypoint;
  ULONG SizeOfImage;
  UNICODE_STRING FullDllName;
  UNICODE_STRING BaseDllName;
} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;

typedef struct _TLS_ENTRY {
  LIST_ENTRY TlsEntryLinks;
  IMAGE_TLS_DIRECTORY TlsDirectory;
  PLDR_DATA_TABLE_ENTRY ModuleEntry;
  SIZE_T Index;
} TLS_ENTRY, *PTLS_ENTRY;

struct node_data {
  MEMORY_BASIC_INFORMATION mbi{};
  node_data* next;
};

struct shellcode_data {
  node_data* first_node;
  PTLS_ENTRY tls_list_addr;
  PTLS_ENTRY tls_node_addr;
  decltype(VirtualFree)* virtual_free;
  decltype(free)* free;
};

class unloader {
  static PTLS_ENTRY get_tls_entry_ptr();
  static unsigned long __stdcall shellcode(shellcode_data* data);

  uintptr_t mod_base_ = 0;
  void* mod_tls_ = 0;

 public:
  static bool initialize(HMODULE mod);
  static void execute();

 private:
  static unloader this_;
};

#endif  // UNLOADER_H