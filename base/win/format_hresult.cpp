#include "base/win/format_hresult.h"
#include "base/strings/stringprintf.h"
#include "base/strings/sys_string_conversions.h"

#include <cassert>
#include <windows.h>

std::wstring FormatHresultUTF16(HRESULT err) {
  LPWSTR buf = NULL;
  if (!FormatMessageW(
          FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL,
          err, 0, reinterpret_cast<LPWSTR>(&buf), 0, NULL))
    return base::StringPrintf(L"Ошибка Windows %d", err);

  assert(buf);

  // Delete ending line breaks.
  size_t len = wcslen(buf);
  while (len && (buf[len - 1] == L'\r' || buf[len - 1] == L'\n' ||
                 buf[len - 1] == L'.')) {
    len--;
  }

  std::wstring str(buf, len);
  LocalFree(buf);
  return str;
}

std::string FormatHresult(HRESULT err) {
  LPSTR buf = NULL;
  if (!FormatMessageA(
          FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL,
          err, 0, reinterpret_cast<LPSTR>(&buf), 0, NULL))
    return base::StringPrintf("Windows error %d", err);

  assert(buf);

  // Delete ending line breaks.
  size_t len = strlen(buf);
  while (len && (buf[len - 1] == '\r' || buf[len - 1] == '\n' ||
                 buf[len - 1] == '.')) {
    len--;
  }

  std::string str(buf, len);
  LocalFree(buf);
  return str;
}
