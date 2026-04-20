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

// TODO:
// https://github.com/sumatrapdfreader/sumatrapdf/blob/9a2183db3c3db5cbf242ac9d8f8576750f581096/src/regress/Regress.cpp#L150
bool DumpException(const wchar_t* path, const EXCEPTION_POINTERS& exc) {
  HANDLE file = CreateFileW(path, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
                            FILE_ATTRIBUTE_NORMAL, nullptr);
  if (file == INVALID_HANDLE_VALUE)
    return FALSE;

  MINIDUMP_EXCEPTION_INFORMATION info;
  info.ThreadId = GetCurrentThreadId();
  info.ClientPointers = FALSE;
  info.ExceptionPointers = const_cast<EXCEPTION_POINTERS*>(&exc);

  BOOL res = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), file,
                               MiniDumpWithDataSegs /*MiniDumpWithFullMemory*/,
                               &info, nullptr, nullptr);

  CloseHandle(file);
  return res != 0;
}
