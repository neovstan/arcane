#include "manualmap.h"

#include <VMProtectSDK.h>

#ifdef VMP_API
#define MMAP_STRING(string) VMProtectDecryptStringA(string)
#else
#define MMAP_STRING(string) string
#endif

struct stLoaderParams {
  ::DWORD dwGetProcAddress;
  ::DWORD dwLoadLibrary;
  ::DWORD dwModuleBase;
  ::DWORD dwRtlZeroMemory;
  ::PIMAGE_NT_HEADERS imageNtHeader;
  ::PIMAGE_BASE_RELOCATION imageBaseRelocation;
  ::PIMAGE_IMPORT_DESCRIPTOR imageImportDescriptor;
  char data[2048]{};
};

MANUALMAP_ERROR_CODE WINAPI LibraryInitializationRemoteThread(::LPVOID lpThreadParameter) {
  auto loaderParams{reinterpret_cast<stLoaderParams *>(lpThreadParameter)};

  auto dwGetProcAddress{loaderParams->dwGetProcAddress};
  auto dwLoadLibrary{loaderParams->dwLoadLibrary};
  auto moduleBase{loaderParams->dwModuleBase};

  auto getProcAddress{reinterpret_cast<decltype(&::GetProcAddress)>(dwGetProcAddress)};
  auto loadLibrary{reinterpret_cast<decltype(&::LoadLibraryA)>(dwLoadLibrary)};

  const ::DWORD dwDelta{moduleBase - loaderParams->imageNtHeader->OptionalHeader.ImageBase};
  auto imageBaseRelocation{loaderParams->imageBaseRelocation};

  while (imageBaseRelocation->VirtualAddress) {
    if (imageBaseRelocation->SizeOfBlock >= sizeof(::IMAGE_BASE_RELOCATION)) {
      ::DWORD dwCount{(imageBaseRelocation->SizeOfBlock - sizeof(::IMAGE_BASE_RELOCATION)) /
                      sizeof(::WORD)};

      auto relocationInfo{::PWORD(imageBaseRelocation + 1)};

      for (::DWORD i{0}; i < dwCount; i++) {
        if (relocationInfo[i] >> 12 == IMAGE_REL_BASED_HIGHLOW) {
          *reinterpret_cast<::PDWORD>(moduleBase + (imageBaseRelocation->VirtualAddress +
                                                    (relocationInfo[i] & 0xFFF))) += dwDelta;
        }
      }
    }

    imageBaseRelocation = reinterpret_cast<::PIMAGE_BASE_RELOCATION>(
        ::LPBYTE(imageBaseRelocation) + imageBaseRelocation->SizeOfBlock);
  }

  auto imageImportDescriptor{loaderParams->imageImportDescriptor};

  while (imageImportDescriptor->Characteristics) {
    auto originalFirstThunk{reinterpret_cast<::PIMAGE_THUNK_DATA>(
        moduleBase + imageImportDescriptor->OriginalFirstThunk)};

    auto firstThunk{
        reinterpret_cast<::PIMAGE_THUNK_DATA>(moduleBase + imageImportDescriptor->FirstThunk)};

    ::HMODULE libraryModule{
        loadLibrary(reinterpret_cast<::LPCSTR>(moduleBase + imageImportDescriptor->Name))};

    if (!libraryModule) {
      return MANUALMAP_ERROR_CODE::MODULE_MISSING_IMPORTED_MODULE;
    }

    while (originalFirstThunk->u1.AddressOfData) {
      auto importedFunction{::DWORD(getProcAddress(
          libraryModule,
          originalFirstThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG
              ? ::LPCSTR(originalFirstThunk->u1.Ordinal & 0xFFFF)
              : (::PIMAGE_IMPORT_BY_NAME(moduleBase + originalFirstThunk->u1.AddressOfData))
                    ->Name))};

      if (!importedFunction) {
        return MANUALMAP_ERROR_CODE::MODULE_MISSING_IMPORTED_FUNCTION;
      }

      firstThunk->u1.Function = importedFunction;
      originalFirstThunk++;
      firstThunk++;
    }

    imageImportDescriptor++;
  }

  if (!loaderParams->imageNtHeader->OptionalHeader.AddressOfEntryPoint) {
    return MANUALMAP_ERROR_CODE::MODULE_HAS_NO_ENTRYPOINT;
  }

  using DllMain_t = ::BOOL(WINAPI *)(::HMODULE, ::DWORD, const char *);
  using RtlZeroMemory_t = void(WINAPI *)(::LPVOID, ::SIZE_T);

  auto dllMain{reinterpret_cast<DllMain_t>(
      moduleBase + loaderParams->imageNtHeader->OptionalHeader.AddressOfEntryPoint)};

  dllMain(::HMODULE(moduleBase), DLL_PROCESS_ATTACH, loaderParams->data);

  auto rtlZeroMemory{reinterpret_cast<RtlZeroMemory_t>(loaderParams->dwRtlZeroMemory)};

  rtlZeroMemory(
      ::LPVOID(moduleBase + loaderParams->imageNtHeader->OptionalHeader.AddressOfEntryPoint), 32);
  rtlZeroMemory(::LPVOID(moduleBase), loaderParams->imageNtHeader->OptionalHeader.SizeOfHeaders);

  return MANUALMAP_ERROR_CODE::EVERYTHING_IS_OK;
}

bool manualmap::inject(::HANDLE targetProcess, const std::uint8_t *staticBytecode,
                       MANUALMAP_ERROR_HANDLER errorHandler, std::string_view data) {
#ifdef VMP_API
  VMProtectBeginUltra("manualmap::inject");
#endif

  ::HMODULE ntdllLibrary{win32::getModuleBase(MMAP_STRING("ntdll.dll"))};
  if (!ntdllLibrary) {
    errorHandler(MANUALMAP_ERROR_CODE::NTMODULE_DOES_NOT_EXIST, NULL);
    return false;
  }

  using ZwAllocateVirtualMemory_t =
      ::NTSTATUS(NTAPI *)(::HANDLE, ::LPVOID *, ::ULONG_PTR, ::PSIZE_T, ::ULONG, ::ULONG);

  using ZwFreeVirtualMemory_t = ::NTSTATUS(NTAPI *)(::HANDLE, ::LPVOID *, ::PSIZE_T, ::ULONG);
  using NtCreateThreadEx_t =
      ::NTSTATUS(NTAPI *)(::PHANDLE, ::ACCESS_MASK, ::PVOID, ::HANDLE, ::PVOID, ::PVOID, ::ULONG,
                          ::SIZE_T, ::SIZE_T, ::SIZE_T, ::PVOID);

  using ZwWaitForSingleObject_t = ::NTSTATUS(NTAPI *)(::HANDLE, ::BOOLEAN, ::PLARGE_INTEGER);

  auto zwAllocateVirtualMemory{ZwAllocateVirtualMemory_t(
      win32::getProcAddress(ntdllLibrary, MMAP_STRING("ZwAllocateVirtualMemory")))};

  auto zwFreeVirtualMemory{ZwFreeVirtualMemory_t(
      win32::getProcAddress(ntdllLibrary, MMAP_STRING("ZwFreeVirtualMemory")))};

  auto ntCreateThreadEx{
      NtCreateThreadEx_t(win32::getProcAddress(ntdllLibrary, MMAP_STRING("NtCreateThreadEx")))};

  auto zwWriteVirtualMemory{decltype(&::WriteProcessMemory)(
      win32::getProcAddress(ntdllLibrary, MMAP_STRING("ZwWriteVirtualMemory")))};

  auto zwCloseHandle{
      decltype(&::CloseHandle)(win32::getProcAddress(ntdllLibrary, MMAP_STRING("ZwClose")))};

  auto zwWaitForSingleObject{ZwWaitForSingleObject_t(
      win32::getProcAddress(ntdllLibrary, MMAP_STRING("ZwWaitForSingleObject")))};

  if (nullptr == zwAllocateVirtualMemory || nullptr == zwFreeVirtualMemory ||
      nullptr == zwWriteVirtualMemory || nullptr == ntCreateThreadEx || nullptr == zwCloseHandle ||
      nullptr == zwWaitForSingleObject) {
    errorHandler(MANUALMAP_ERROR_CODE::NTMODULE_HAS_NO_FUNCTIONS, NULL);
    return false;
  }

  auto libraryBytecode = const_cast<std::uint8_t *>(staticBytecode);

  if (!libraryBytecode) {
    errorHandler(MANUALMAP_ERROR_CODE::MODULE_DOES_NOT_EXIST, NULL);
    return false;
  }

  auto imageDosHeader{reinterpret_cast<::PIMAGE_DOS_HEADER>(libraryBytecode)};
  if (imageDosHeader->e_magic != 0x5A4D) {
    errorHandler(MANUALMAP_ERROR_CODE::MODULE_UNKNOWN_ARCHITECTURE, NULL);
    return false;
  }

  auto imageNtHeader{reinterpret_cast<::PIMAGE_NT_HEADERS>(
      reinterpret_cast<::DWORD>(libraryBytecode) + imageDosHeader->e_lfanew)};

  ::PIMAGE_OPTIONAL_HEADER imageOptionalHeader{&imageNtHeader->OptionalHeader};
  ::PIMAGE_FILE_HEADER imageFileHeader{&imageNtHeader->FileHeader};

  ::LPVOID imageVirtualMemory{nullptr};
  ::NTSTATUS ntError{zwAllocateVirtualMemory(targetProcess, &imageVirtualMemory, NULL,
                                             &imageOptionalHeader->SizeOfImage,
                                             MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE)};

  if (!imageVirtualMemory || ntError) {
    errorHandler(MANUALMAP_ERROR_CODE::MODULE_VIRTUAL_MEMORY_ALLOCATION, ntError);
    return false;
  }

  ntError = zwWriteVirtualMemory(targetProcess, imageVirtualMemory, libraryBytecode,
                                 imageOptionalHeader->SizeOfHeaders, NULL);

  if (ntError) {
    errorHandler(MANUALMAP_ERROR_CODE::MODULE_VIRTUAL_MEMORY_INITIALIZATION, ntError);
    zwFreeVirtualMemory(targetProcess, &imageVirtualMemory, 0, MEM_RELEASE);

    return false;
  }

  ::PIMAGE_SECTION_HEADER imageSectionHeader{IMAGE_FIRST_SECTION(imageNtHeader)};
  for (::DWORD i{0}; i < imageFileHeader->NumberOfSections; ++i) {
    if (imageSectionHeader[i].SizeOfRawData) {
      const ::NTSTATUS sectionWriteResult{zwWriteVirtualMemory(
          targetProcess,
          reinterpret_cast<::LPVOID>(reinterpret_cast<::DWORD>(imageVirtualMemory) +
                                     imageSectionHeader[i].VirtualAddress),
          reinterpret_cast<::LPCVOID>(reinterpret_cast<::DWORD>(libraryBytecode) +
                                      imageSectionHeader[i].PointerToRawData),
          imageSectionHeader[i].SizeOfRawData, nullptr)};

      if (sectionWriteResult) {
        errorHandler(MANUALMAP_ERROR_CODE::MODULE_MAPPING_SECTION_INVALID, sectionWriteResult);
        zwFreeVirtualMemory(targetProcess, &imageVirtualMemory, 0, MEM_RELEASE);

        return false;
      }
    }
  }

  ::SIZE_T oneVirtualPageSize{4096};
  ::LPVOID shellcodeVirtualMemory{nullptr};
  ntError =
      zwAllocateVirtualMemory(targetProcess, &shellcodeVirtualMemory, NULL, &oneVirtualPageSize,
                              MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);

  if (!shellcodeVirtualMemory || ntError) {
    errorHandler(MANUALMAP_ERROR_CODE::SHELLCODE_VIRTUAL_MEMORY_ALLOCATION, ntError);
    zwFreeVirtualMemory(targetProcess, &imageVirtualMemory, 0, MEM_RELEASE);

    return false;
  }

  auto pairFree = [&]() {
    zwFreeVirtualMemory(targetProcess, &imageVirtualMemory, 0, MEM_RELEASE);
    zwFreeVirtualMemory(targetProcess, &shellcodeVirtualMemory, 0, MEM_RELEASE);
  };

  ntError = zwWriteVirtualMemory(targetProcess, shellcodeVirtualMemory,
                                 reinterpret_cast<::LPCVOID>(&LibraryInitializationRemoteThread),
                                 oneVirtualPageSize, nullptr);

  if (ntError) {
    errorHandler(MANUALMAP_ERROR_CODE::SHELLCODE_VIRTUAL_MEMORY_INITIALIZATION, ntError);
    pairFree();

    return false;
  }

  ::SIZE_T loaderSize{sizeof(stLoaderParams)};
  ::LPVOID paramsVirtualMemory{nullptr};
  ntError = zwAllocateVirtualMemory(targetProcess, &paramsVirtualMemory, NULL, &loaderSize,
                                    MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);

  if (!paramsVirtualMemory || ntError) {
    errorHandler(MANUALMAP_ERROR_CODE::PARAMS_VIRTUAL_MEMORY_ALLOCATION, ntError);
    pairFree();

    return false;
  }

  auto fullyFree = [&]() {
    zwFreeVirtualMemory(targetProcess, &imageVirtualMemory, 0, MEM_RELEASE);
    zwFreeVirtualMemory(targetProcess, &shellcodeVirtualMemory, 0, MEM_RELEASE);
    zwFreeVirtualMemory(targetProcess, &paramsVirtualMemory, 0, MEM_RELEASE);
  };

  stLoaderParams loaderParams;

  loaderParams.dwGetProcAddress = reinterpret_cast<::DWORD>(&::GetProcAddress);
  loaderParams.dwLoadLibrary = reinterpret_cast<::DWORD>(&::LoadLibraryA);
  loaderParams.dwModuleBase = reinterpret_cast<::DWORD>(imageVirtualMemory);

  loaderParams.dwRtlZeroMemory =
      ::DWORD(win32::getProcAddress(ntdllLibrary, MMAP_STRING("RtlZeroMemory")));

  loaderParams.imageBaseRelocation = reinterpret_cast<::PIMAGE_BASE_RELOCATION>(
      reinterpret_cast<::DWORD>(imageVirtualMemory) +
      imageOptionalHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);

  loaderParams.imageImportDescriptor = reinterpret_cast<::PIMAGE_IMPORT_DESCRIPTOR>(
      reinterpret_cast<::DWORD>(imageVirtualMemory) +
      imageOptionalHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

  loaderParams.imageNtHeader = reinterpret_cast<::PIMAGE_NT_HEADERS>(
      reinterpret_cast<::DWORD>(imageVirtualMemory) + imageDosHeader->e_lfanew);

  std::strcpy(loaderParams.data, data.data());

  ntError = zwWriteVirtualMemory(targetProcess, paramsVirtualMemory,
                                 reinterpret_cast<::LPCVOID>(&loaderParams), sizeof(stLoaderParams),
                                 nullptr);

  if (ntError) {
    errorHandler(MANUALMAP_ERROR_CODE::PARAMS_VIRTUAL_MEMORY_INITIALIZATION, ntError);
    fullyFree();

    return false;
  }

  ::HANDLE threadHandle{NULL};
  ntError =
      ntCreateThreadEx(&threadHandle, 0x1FFFFF, nullptr, targetProcess, shellcodeVirtualMemory,
                       paramsVirtualMemory, NULL, NULL, NULL, NULL, nullptr);

  if (INVALID_HANDLE_VALUE == threadHandle || !threadHandle || ntError) {
    errorHandler(MANUALMAP_ERROR_CODE::REMOTE_THREAD_CREATION, ntError);
    fullyFree();

    return false;
  }

  ntError = zwWaitForSingleObject(threadHandle, FALSE, nullptr);
  if (ntError) {
    errorHandler(MANUALMAP_ERROR_CODE::WAIT_FOR_SINGLE_OBJECT, ntError);
    fullyFree();

    zwCloseHandle(threadHandle);
    return false;
  }

  ::DWORD threadExitCode{EXIT_SUCCESS};
  if (!::GetExitCodeThread(threadHandle, &threadExitCode)) {
    errorHandler(MANUALMAP_ERROR_CODE::GET_EXIT_CODE_THREAD, NULL);
    fullyFree();

    zwCloseHandle(threadHandle);
    return false;
  }

  if (EXIT_SUCCESS != threadExitCode) {
    errorHandler(MANUALMAP_ERROR_CODE(threadExitCode), NULL);
    fullyFree();

    zwCloseHandle(threadHandle);
    return false;
  }

  zwFreeVirtualMemory(targetProcess, &shellcodeVirtualMemory, 0, MEM_RELEASE);
  zwFreeVirtualMemory(targetProcess, &paramsVirtualMemory, 0, MEM_RELEASE);

  zwCloseHandle(threadHandle);

  return true;

#ifdef VMP_API
  VMProtectEnd();
#endif
}