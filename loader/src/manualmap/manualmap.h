#pragma once

#ifndef WIN32
static_assert(0, "This manual mapping injector only works with Windows x86");
#endif

#include <windows.h>

#include <string>

#include "winapi.h"

enum class MANUALMAP_ERROR_CODE {
  EVERYTHING_IS_OK,

  MODULE_DOES_NOT_EXIST,
  MODULE_UNKNOWN_ARCHITECTURE,
  MODULE_VIRTUAL_MEMORY_ALLOCATION,
  MODULE_VIRTUAL_MEMORY_INITIALIZATION,
  MODULE_MAPPING_SECTION_INVALID,
  MODULE_MISSING_IMPORTED_MODULE,
  MODULE_MISSING_IMPORTED_FUNCTION,
  MODULE_HAS_NO_ENTRYPOINT,

  SHELLCODE_VIRTUAL_MEMORY_ALLOCATION,
  SHELLCODE_VIRTUAL_MEMORY_INITIALIZATION,

  PARAMS_VIRTUAL_MEMORY_ALLOCATION,
  PARAMS_VIRTUAL_MEMORY_INITIALIZATION,

  REMOTE_THREAD_CREATION,
  WAIT_FOR_SINGLE_OBJECT,
  GET_EXIT_CODE_THREAD,
  NTMODULE_DOES_NOT_EXIST,
  NTMODULE_HAS_NO_FUNCTIONS
};

namespace manualmap {
using MANUALMAP_ERROR_HANDLER = void (*)(MANUALMAP_ERROR_CODE errorCode,
                                         ::NTSTATUS ntError);
bool inject(::HANDLE targetProcess, const std::uint8_t* libraryBytecode,
            MANUALMAP_ERROR_HANDLER errorHandler, std::string_view data);
}  // namespace manualmap