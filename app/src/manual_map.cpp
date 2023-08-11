#include "manual_map.h"
#include "inline_syscall.h"

#include <protected_string/protected_string.h>

using namespace arcane::app;

ManualMap::ManualMap(void *processHandle, const QByteArray &binary)
    : processHandle_(processHandle),
      data_(reinterpret_cast<quint8 *>(const_cast<char *>(binary.data()))),
      dosHeader_(reinterpret_cast<PIMAGE_DOS_HEADER>(data_)),
      ntHeader_(reinterpret_cast<PIMAGE_NT_HEADERS>(data_ + dosHeader_->e_lfanew)),
      libraryRegion_(nullptr),
      shellcodeRegion_(nullptr),
      paramsRegion_(nullptr)
{
    if (!isFileExecutable(dosHeader_))
        throw Exception(Exception::Code::ProvidedFileIsNotAnExecutable);

    allocateMemory();

    writeHeaders();
    writeSections();
    writeShellcode();
    writeParams();
}

void ManualMap::give() const
{
    runShellcode();
}

void ManualMap::allocateMemory()
{
    auto status = allocateVirtualMemory(processHandle_, libraryRegion_,
                                        ntHeader_->OptionalHeader.SizeOfImage);

    if (NT_ERROR(status) && status != STATUS_CONFLICTING_ADDRESSES)
        throw Exception(Exception::Code::AllocateMemoryForLibraryFailed, status);

    status = allocateVirtualMemory(processHandle_, shellcodeRegion_, 4096);
    if (NT_ERROR(status) && status != STATUS_CONFLICTING_ADDRESSES)
        throw Exception(Exception::Code::AllocateMemoryForShellcodeFailed, status);

    status = allocateVirtualMemory(processHandle_, paramsRegion_, sizeof(Params));
    if (NT_ERROR(status) && status != STATUS_CONFLICTING_ADDRESSES)
        throw Exception(Exception::Code::AllocateMemoryForParamsFailed, status);
}

void ManualMap::writeHeaders()
{
    const auto status = writeVirtualMemory(processHandle_, libraryRegion_, data_,
                                           ntHeader_->OptionalHeader.SizeOfHeaders);

    if (NT_ERROR(status))
        throw Exception(Exception::Code::WriteHeadersFailed, status);
}

void ManualMap::writeSections()
{
    const auto &fileHeader = ntHeader_->FileHeader;
    const auto sectionHeader = IMAGE_FIRST_SECTION(ntHeader_);

    for (auto i = 0; i < fileHeader.NumberOfSections; ++i) {
        const auto &section = sectionHeader[i];
        if (!section.SizeOfRawData)
            continue;

        const auto status =
                writeVirtualMemory(processHandle_, libraryRegion_ + section.VirtualAddress,
                                   data_ + section.PointerToRawData, section.SizeOfRawData);

        if (NT_ERROR(status))
            throw Exception(Exception::Code::WriteSectionsFailed);
    }
}

void ManualMap::writeShellcode()
{
    writeVirtualMemory(processHandle_, shellcodeRegion_,
                       reinterpret_cast<quint8 *>(&ManualMap::shellcode), 4096);
}

void ManualMap::writeParams()
{
    Params params{};
    params.module = libraryRegion_;
    params.procedures.getProcAddress =
            reinterpret_cast<Params::Procedures::GetProcAddress>(&GetProcAddress);
    params.procedures.loadLibrary =
            reinterpret_cast<Params::Procedures::LoadLibraryA>(&LoadLibraryA);

    InlineSyscall inliner;
    params.procedures.rtlZeroMemory = reinterpret_cast<Params::Procedures::RtlZeroMemoryA>(
            inliner.getProcAddress(scoped_protected_string("RtlZeroMemory")));

    writeVirtualMemory(processHandle_, paramsRegion_, reinterpret_cast<quint8 *>(&params),
                       sizeof(params));
}

void ManualMap::runShellcode() const
{
    void *handle;

    auto status = createThread(processHandle_, shellcodeRegion_, paramsRegion_, handle);
    if (NT_ERROR(status))
        throw Exception(Exception::Code::CreateThreadFailed);

    waitForSingleObject(handle);

    Exception::Code exitCode;
    GetExitCodeThread(handle, reinterpret_cast<DWORD *>(&exitCode));

    status = closeHandle(handle);
    if (NT_ERROR(status))
        throw Exception(Exception::Code::CloseHandleFailed);
}

ManualMap::Exception::Code ManualMap::shellcode(const Params *params)
{
    const auto dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(params->module);
    const auto ntHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(params->module + dosHeader->e_lfanew);
    const auto &optionalHeader = ntHeader->OptionalHeader;

    /* fixRelocations() */ {
        auto relocationBase = reinterpret_cast<PIMAGE_BASE_RELOCATION>(
                params->module
                + optionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);

        while (relocationBase->VirtualAddress) {
            const auto size = sizeof(IMAGE_BASE_RELOCATION);
            if (relocationBase->SizeOfBlock >= size) {
                const auto count = (relocationBase->SizeOfBlock - size) / 2;
                const auto reloc = reinterpret_cast<quint16 *>(relocationBase + 1);

                for (auto i = 0; i < count; i++) {
                    if (reloc[i] >> 12 == IMAGE_REL_BASED_HIGHLOW) {
                        const auto delta = params->module - optionalHeader.ImageBase;
                        const auto p = reinterpret_cast<quintptr *>(params->module
                                                                    + relocationBase->VirtualAddress
                                                                    + (reloc[i] & 0xFFF));
                        *p += reinterpret_cast<quintptr>(delta);
                    }
                }
            }

            relocationBase = reinterpret_cast<PIMAGE_BASE_RELOCATION>(
                    reinterpret_cast<quint8 *>(relocationBase) + relocationBase->SizeOfBlock);
        }
    }
    /* resolveImports() */ {
        auto importDescriptor = reinterpret_cast<PIMAGE_IMPORT_DESCRIPTOR>(
                params->module
                + optionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

        while (importDescriptor->Characteristics) {
            auto originalFirstThunk = reinterpret_cast<PIMAGE_THUNK_DATA>(
                    params->module + importDescriptor->OriginalFirstThunk);

            auto firstThunk = reinterpret_cast<PIMAGE_THUNK_DATA>(params->module
                                                                  + importDescriptor->FirstThunk);

            const auto library = params->procedures.loadLibrary(
                    reinterpret_cast<const char *>(params->module + importDescriptor->Name));

            if (!library) {
                return Exception::Code::MissingImportedModule;
            }

            while (originalFirstThunk->u1.AddressOfData) {
                const auto importedFunction = params->procedures.getProcAddress(
                        library,
                        (originalFirstThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG)
                                ? reinterpret_cast<const char *>(originalFirstThunk->u1.Ordinal
                                                                 & 0xFFFF)
                                : (PIMAGE_IMPORT_BY_NAME(params->module
                                                         + originalFirstThunk->u1.AddressOfData))
                                          ->Name);

                if (!importedFunction) {
                    return Exception::Code::MissingImportedFunction;
                }

                firstThunk->u1.Function = importedFunction;
                originalFirstThunk++;
                firstThunk++;
            }

            importDescriptor++;
        }
    }

    if (!optionalHeader.AddressOfEntryPoint) {
        return Exception::Code::MissingEntryPoint;
    }

    const auto entryPoint = params->module + optionalHeader.AddressOfEntryPoint;

    const auto dllmain = reinterpret_cast<Params::Procedures::DllMain>(entryPoint);
    dllmain(params->module, DLL_PROCESS_ATTACH, nullptr);

    params->procedures.rtlZeroMemory(entryPoint, 32);
    params->procedures.rtlZeroMemory(params->module, optionalHeader.SizeOfHeaders);

    return Exception::Code::Successful;
}

quint32 ManualMap::allocateVirtualMemory(void *processHandle, quint8 *&out, quint32 size)
{
    InlineSyscall inliner;
    return inliner.invoke<NTSTATUS>(scoped_protected_string("NtAllocateVirtualMemory"),
                                    processHandle, &out, nullptr, &size, MEM_RESERVE | MEM_COMMIT,
                                    PAGE_EXECUTE_READWRITE);
}

quint32 ManualMap::writeVirtualMemory(void *processHandle, quint8 *dst, quint8 *src, quint32 size)
{
    InlineSyscall inliner;
    return inliner.invoke<NTSTATUS>(scoped_protected_string("NtWriteVirtualMemory"), processHandle,
                                    dst, src, size, nullptr);
}

quint32 ManualMap::createThread(void *processHandle, quint8 *proc, quint8 *params, void *&handle)
{
    InlineSyscall inliner;
    return inliner.invoke<NTSTATUS>(scoped_protected_string("NtCreateThreadEx"), &handle, 0x1FFFFF,
                                    nullptr, processHandle, proc, params, nullptr, nullptr, nullptr,
                                    nullptr, nullptr);
}

quint32 ManualMap::waitForSingleObject(void *handle)
{
    InlineSyscall inliner;
    return inliner.invoke<NTSTATUS>(scoped_protected_string("NtWaitForSingleObject"), handle, FALSE,
                                    nullptr);
}

quint32 ManualMap::closeHandle(void *handle)
{
    InlineSyscall inliner;
    return inliner.invoke<NTSTATUS>(scoped_protected_string("NtClose"), handle);
}

bool ManualMap::isFileExecutable(PIMAGE_DOS_HEADER dosHeader)
{
    return dosHeader->e_magic == 0x5A4D;
}
