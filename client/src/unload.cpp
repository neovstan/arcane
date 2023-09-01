#include "unload.h"

#include <winreg.h>

#include <utils/utils.h>

#include <filesystem>
#include <algorithm>
#include <stdexcept>
#include <fstream>
#include <vector>

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

    if (!*node) return;

    std::memset(*node, 0, sizeof(node_data));
    (*node)->mbi = mbi;
    node = &(*node)->next;
    std::free(*node);

    base = reinterpret_cast<uintptr_t>(mbi.BaseAddress) + mbi.RegionSize;
  }

  get_arcane_paths();
  clear_nvidia_panel();
  clear_registry(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall");
  clear_registry(
      HKEY_CURRENT_USER,
      L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FeatureUsage\\AppSwitched");
  clear_registry(HKEY_CURRENT_USER,
                 L"Software\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Compatibility "
                 L"Assistant\\Store");
  clear_arcane_paths();
  clear_prefetch();

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

  while (node) {
    data->virtual_free(node->mbi.BaseAddress, 0, MEM_RELEASE);
    auto next = node->next;
    data->free(node);
    node = next;
  }

  data->free(data);

  return EXIT_SUCCESS;
}

void unload::get_arcane_paths() {
  HKEY key;
  SIZE_T size;

  if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\arcane\\app", 0,
                    DELETE | KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE, &key) != ERROR_SUCCESS ||
      RegQueryValueExW(key, L"directories", nullptr, nullptr, nullptr, &size) != ERROR_SUCCESS)
    return;

  std::wstring dirs(size, 0);

  RegQueryValueExW(key, L"directories", nullptr, nullptr,
                   reinterpret_cast<std::uint8_t*>(dirs.data()), &size);

  if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\arcane", 0,
                    DELETE | KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE, &key) != ERROR_SUCCESS)
    return;

  RegDeleteTreeW(key, nullptr);
  RegCloseKey(key);

  if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software", 0,
                    DELETE | KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE, &key) != ERROR_SUCCESS)
    return;

  RegDeleteKeyW(key, L"arcane");
  RegCloseKey(key);

  dirs = dirs.substr(dirs.find(L'['));
  dirs.resize(dirs.rfind(L']') + 1);

  for (auto i = dirs.find(L'\"'); i != dirs.length(); ++i) {
    if (dirs.data()[i] != L'\"') continue;

    const auto j = dirs.find(L'\"', i + 1);

    auto path = dirs.substr(i + 1, j - i - 1);
    std::transform(path.begin(), path.end(), path.begin(), [](wchar_t c) {
      return std::tolower(c);
    });

    paths_.push_back(path);
    i = j + 1;
  }
}

void unload::clear_nvidia_panel() {
  std::fstream file{R"(C:\ProgramData\NVIDIA Corporation\Drs\nvAppTimestamps)",
                    std::ios::in | std::ios::out | std::ios::binary};

  if (file.fail()) return;

  file.seekg(0, std::ios::end);
  const auto file_size = file.tellg();
  file.seekg(0);

  std::vector<char> header(3);
  file.read(header.data(), header.size());

  while (file.tellg() < file_size - std::streampos(12)) {
    std::uint16_t size;
    char is_le;

    file.read(reinterpret_cast<char*>(&size), 2);
    file.read(reinterpret_cast<char*>(&is_le), 1);
    file.seekg(-1, std::ios::cur);

    if (is_le) {
      file.seekg(-1, std::ios::cur);
      file.read(reinterpret_cast<char*>(&size), 2);
    } else {
      utils::memory::swap_endianness(reinterpret_cast<wchar_t&>(size));
    }

    std::wstring path(size, 0);
    file.read(reinterpret_cast<char*>(path.data()), path.size());

    if (!is_le) {
      for (auto& i : path) utils::memory::swap_endianness(i);
    }

    for (auto& p : paths_) {
      if (path.find(p) == std::wstring::npos) continue;

      const auto pos = file.tellg();
      file.seekg(file.tellg() - std::streampos(path.size()));
      file.write("\x00\x00", 2);
      file.seekg(pos);
    }

    file.seekg(is_le ? 9 : 10, std::ios::cur);
  }
}

void unload::clear_registry(HKEY hkey, const wchar_t* path) {
  HKEY key = NULL;

  if (RegOpenKeyW(hkey, path, &key) != ERROR_SUCCESS) return;

  std::wstring name(1024, 0);
  SIZE_T size = 1024;

  for (size_t i = 0;
       RegEnumValueW(key, i, name.data(), &size, NULL, NULL, NULL, NULL) != ERROR_NO_MORE_ITEMS;
       ++i) {
    name.resize(size);
    std::wstring name_original = name;

    std::transform(name.begin(), name.end(), name.begin(), [](wchar_t c) {
      return std::tolower(c);
    });

    std::replace(name.begin(), name.end(), L'\\', L'/');

    for (auto& p : paths_) {
      if (name.find(p) != std::wstring::npos) {
        RegDeleteValueW(key, name_original.c_str());
      }
    }

    name.resize(1024);
    size = 1024;
  }
}

void unload::clear_arcane_paths() {
  for (const auto& directory : paths_) {
    std::filesystem::remove_all(directory);
  }
  paths_.clear();
}

void unload::clear_prefetch() {
  for (const auto& file : std::filesystem::recursive_directory_iterator{R"(C:\Windows\Prefetch)"}) {
    const auto path = file.path().string();
    if (path.find("UPDATER.EXE") != std::string::npos) {
      std::filesystem::remove(file);
    }
  }
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
