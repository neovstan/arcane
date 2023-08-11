#include "inline_syscall.h"

#include <Windows.h>

HINSTANCE InlineSyscall::hSubsystemInstances[2];

InlineSyscall::InlineSyscall()
    : lastError(IS_INIT_NOT_APPLIED), initialized(0), syscallStub(nullptr)
{
    //	Fill hSubsystemInstances
    hSubsystemInstances[0] = LoadLibraryA("ntdll.dll");
    hSubsystemInstances[1] = LoadLibraryA("win32u.dll");

    //	Could not load the modules??
    for (const auto &instance : hSubsystemInstances)
        if (!instance) {
            lastError = IS_MODULE_NOT_FOUND;
            return;
        }

    //	Set up the system call stub
    //	as in NTDLL.DLL services
    syscallStub =
            (UCHAR *)VirtualAlloc(nullptr, 21, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!syscallStub) {
        lastError = IS_CALLBACK_KILL_FAILURE;
        return;
    }

    //	Try killing callbacks
    callback();
    if (lastError != IS_SUCCESS)
        return;

    lastError = IS_SUCCESS;
    initialized = 1;
}

void InlineSyscall::callback()
{
    //	Kill any system call callback
    NTSTATUS status;
    pNtSetInformationProcess *ntSetInformationProcess;
    PROCESS_INSTRUMENTATION_CALLBACK_INFORMATION syscallCallback{};

    ntSetInformationProcess = (pNtSetInformationProcess *)GetProcAddress(hSubsystemInstances[0],
                                                                         "NtSetInformationProcess");
    if (ntSetInformationProcess == nullptr) {
        setError(IS_ADDRESS_NOT_FOUND);
        return;
    }

    //	Disable any callbacks caused by the syscall instruction
    //	( Prevents monitoring of the syscall )
    syscallCallback.reserved = 0;
    syscallCallback.version = 0;
    syscallCallback.callbackPtr = nullptr;

    status = ntSetInformationProcess(GetCurrentProcess(), (PROCESS_INFORMATION_CLASS)40,
                                     &syscallCallback, sizeof(syscallCallback));

    if (!NT_SUCCESS(status)) {
        setError(IS_CALLBACK_KILL_FAILURE);
        return;
    }

    setError(IS_SUCCESS);
}

void InlineSyscall::unload()
{
    if (!syscallStub)
        return;

    memset(syscallStub, 0, 21);
    VirtualFree(syscallStub, 0, MEM_RELEASE);
}

FARPROC InlineSyscall::getProcAddress(LPCSTR ServiceName) const
{
    for (const auto &instance : hSubsystemInstances) {
        const auto functionAddress = GetProcAddress(instance, ServiceName);
        if (functionAddress)
            return functionAddress;
    }

    return nullptr;
}
