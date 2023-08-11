#ifndef ARCANE_APP_MANUAL_MAP_H
#define ARCANE_APP_MANUAL_MAP_H

#include <stdexcept>

#include <QByteArray>

struct _IMAGE_NT_HEADERS;
struct _IMAGE_DOS_HEADER;

namespace arcane::app {
class ManualMap
{
public:
    class Exception : public std::runtime_error
    {
    public:
        enum class Code {
            Successful,
            ProvidedFileIsNotAnExecutable,
            AllocateMemoryForLibraryFailed,
            AllocateMemoryForShellcodeFailed,
            AllocateMemoryForParamsFailed,
            WriteHeadersFailed,
            WriteSectionsFailed,
            CreateThreadFailed,
            CloseHandleFailed,
            MissingImportedModule,
            MissingImportedFunction,
            MissingEntryPoint
        };

    public:
        explicit Exception(Code code, quint32 systemError = 0)
            : std::runtime_error("manual map exception, check error code with getCode() and "
                                 "getSystemError()"),
              code_(code),
              systemError_(systemError)
        {
        }

        Code getCode() const { return code_; }
        quint32 getSystemError() const { return systemError_; }

    private:
        Code code_;
        unsigned int systemError_;
    };

public:
    ManualMap(void *processHandle, const QByteArray &binary);

    void give() const;

private:
    struct Params
    {
        quint8 *module;

        struct Procedures
        {
            using LoadLibraryA = quint8 *(__stdcall *)(const char *filename);
            using GetProcAddress = quintptr(__stdcall *)(quint8 *module, const char *procname);
            using RtlZeroMemoryA = void(__stdcall *)(quint8 *data, quint32 size);
            using DllMain = quint32(__stdcall *)(quint8 *module, quint32 reason, void *reserved);

            LoadLibraryA loadLibrary;
            GetProcAddress getProcAddress;
            RtlZeroMemoryA rtlZeroMemory;
        } procedures;
    };

private:
    void allocateMemory();

private:
    void writeHeaders();
    void writeSections();
    void writeShellcode();
    void writeParams();

private:
    void runShellcode() const;

private:
    static Exception::Code shellcode(const Params *params);

private:
    static quint32 allocateVirtualMemory(void *processHandle, quint8 *&out, quint32 size);
    static quint32 writeVirtualMemory(void *processHandle, quint8 *dst, quint8 *src, quint32 size);
    static quint32 createThread(void *processHandle, quint8 *proc, quint8 *params, void *&handle);
    static quint32 waitForSingleObject(void *handle);
    static quint32 closeHandle(void *handle);

private:
    static bool isFileExecutable(_IMAGE_DOS_HEADER *dosHeader);

private:
    void *processHandle_;
    quint8 *data_;

private:
    _IMAGE_DOS_HEADER *dosHeader_;
    _IMAGE_NT_HEADERS *ntHeader_;

private:
    quint8 *libraryRegion_;
    quint8 *shellcodeRegion_;
    quint8 *paramsRegion_;
};
} // namespace arcane::app

#endif // ARCANE_APP_MANUAL_MAP_H
