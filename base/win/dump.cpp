#include "base/win/dump.h"

#include <dbghelp.h>
#include <format>
#include <windows.h>

#pragma comment(lib, "dbghelp.lib")

std::string GetDumpFileName(std::string_view prefix) {
  SYSTEMTIME time;
  GetLocalTime(&time);

  return std::format("{}_{:04}{:02}{:02}_{:02}{:02}{:02}.dmp", prefix,
                     time.wYear, time.wMonth, time.wDay, time.wHour,
                     time.wMinute, time.wSecond);
}

bool DumpException(const wchar_t* path, const EXCEPTION_POINTERS& exc) {
  HANDLE file = CreateFileW(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                            FILE_ATTRIBUTE_NORMAL, NULL);
  if (file == INVALID_HANDLE_VALUE)
    return FALSE;

  MINIDUMP_EXCEPTION_INFORMATION info;
  info.ThreadId = GetCurrentThreadId();
  info.ClientPointers = FALSE;
  info.ExceptionPointers = (EXCEPTION_POINTERS*)&exc;

  BOOL res = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), file,
                               MiniDumpWithDataSegs /*MiniDumpWithFullMemory*/,
                               &info, NULL, NULL);

  CloseHandle(file);
  return res != 0;
}
