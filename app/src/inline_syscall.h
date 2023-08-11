#ifndef ARCANE_APP_INLINE_SYSCALL_H
#define ARCANE_APP_INLINE_SYSCALL_H

#include <Windows.h>

#define IS_ADDRESS_NOT_FOUND -1
#define IS_CALLBACK_KILL_FAILURE -2
#define IS_INTEGRITY_STUB_FAILURE -3
#define IS_MODULE_NOT_FOUND -4
#define IS_INIT_NOT_APPLIED -6
#define IS_SUCCESS 0

#ifndef NT_SUCCESS
#  define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)
#endif

#ifndef NT_ERROR
#  define NT_ERROR(Status) ((ULONG)(Status) >> 30 == 3)
#endif

#define STATUS_CONFLICTING_ADDRESSES 0xC0000018

class InlineSyscall
{
public:
    InlineSyscall();
    ~InlineSyscall() { unload(); }

public:
    void unload();
    void callback();

    void setError(int error_code) { lastError = error_code; }

public:
    int getError() const { return lastError; }
    bool isInit() const { return initialized; }
    UCHAR *getStub() const { return syscallStub; }

public:
    template<typename returnType, typename... args>
    returnType invoke(LPCSTR ServiceName, args... arguments);

    FARPROC getProcAddress(LPCSTR ServiceName) const;

private:
    int lastError;
    bool initialized;
    UCHAR *syscallStub;

    typedef NTSTATUS __stdcall pNtSetInformationProcess(
            HANDLE ProcessHandle, PROCESS_INFORMATION_CLASS ProcessInformationClass,
            PVOID ProcessInformation, ULONG ProcessInformationLength);

    struct PROCESS_INSTRUMENTATION_CALLBACK_INFORMATION
    {
        ULONG version;
        ULONG reserved;
        PVOID callbackPtr;
    };

private:
    static HINSTANCE hSubsystemInstances[2];
};

template<typename returnType, typename... args>
returnType InlineSyscall::invoke(LPCSTR ServiceName, args... arguments)
{
    //	Kill monitoring callback
    callback();

    typedef returnType __stdcall NtFunction(args...);
    auto function = (NtFunction *)InlineSyscall::syscallStub;

    for (const auto &instance : hSubsystemInstances) {
        // Get the address
        const auto functionAddress =
                reinterpret_cast<UCHAR *>(GetProcAddress(instance, ServiceName));

        // Prepare to execute
        if (functionAddress) {
            //	Small check against modified stubs
            //	I'd call it an integrity check because we copy the whole stub
            if (functionAddress[0] != 0xB8
                && functionAddress[5]
                        != 0xBA) // mov eax, index \x??\x??\x??\x?? mov edx, KiFastSystemCall
                return (returnType)IS_INTEGRITY_STUB_FAILURE;

            memcpy(InlineSyscall::getStub(), functionAddress, 15);

            InlineSyscall::setError(IS_SUCCESS);
            return function(arguments...);
        }
    }

    InlineSyscall::setError(IS_MODULE_NOT_FOUND);
    return IS_MODULE_NOT_FOUND;
}

#endif // ARCANE_APP_INLINE_SYSCALL_H
