#pragma once
#include "utils.h"
#include <Windows.h>
#include <TlHelp32.h>
#include <string>

typedef NTSTATUS(WINAPI *pNtReadVirtualMemory)(HANDLE ProcessHandle, PVOID BaseAddress, PVOID Buffer, ULONG NumberOfBytesToRead, PULONG NumberOfBytesRead);
typedef NTSTATUS(WINAPI *pNtWriteVirtualMemory)(HANDLE Processhandle, PVOID BaseAddress, PVOID Buffer, ULONG NumberOfBytesToWrite, PULONG NumberOfBytesWritten);

class Memory {

  HANDLE m_Handle = 0;
  DWORD m_Pid = 0;

  pNtReadVirtualMemory m_ReadVM;
  pNtWriteVirtualMemory m_WriteVM;

public:
  ~Memory() {

    if (m_Handle)
      CloseHandle(m_Handle);
  }

  long get_process_by_name(const char *process_name) {

    LOG("%s", process_name);

    PROCESSENTRY32 pe;
    HANDLE valid_ts = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, m_Pid);
    long pid = 0;

    if (valid_ts != INVALID_HANDLE_VALUE) {
      while (Process32Next(valid_ts, &pe)) {
        if (!strcmp(process_name, pe.szExeFile)) {

          pid = pe.th32ProcessID;
          break;
        }
      }
    }

    if (valid_ts)
      CloseHandle(valid_ts);

    LOG("%d", pid);

    return pid;
  }

  uintptr_t get_module_base(int pid,const char *module_name) {
    MODULEENTRY32 pe;

    LOG("%s", module_name);

    HANDLE valid_ts = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);

    uintptr_t module_base = 0;

    if (valid_ts != INVALID_HANDLE_VALUE) {
      while (Module32Next(valid_ts,&pe)) {
        if (!strcmp(module_name,pe.szModule)) {

          module_base = reinterpret_cast<uintptr_t>(pe.modBaseAddr);
          break;
        }
      }
    }

    if (valid_ts)
      CloseHandle(valid_ts);

    LOG("%p", module_base);

    if (module_base != 0)
      return module_base;


    return false;
  }

};

inline Memory g_Memory;