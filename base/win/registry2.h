#pragma once

#include <string>

#include <atlbase.h>

class Registry {
 public:
  Registry(HKEY par, const wchar_t* key, BOOL read = FALSE) {
    REGSAM acc = read ? KEY_QUERY_VALUE : KEY_SET_VALUE | KEY_QUERY_VALUE;
    if (reg.Open(par, key, acc) != ERROR_SUCCESS) {
      if (!read)
        reg.Create(par, key, NULL, 0, acc);
    }
  }

  DWORD GetDWORD(const wchar_t* name, DWORD def = 0) {
    DWORD val;
    if (reg.m_hKey && reg.QueryDWORDValue(name, val) == ERROR_SUCCESS)
      return val;
    else
      return def;
  }

  std::wstring GetString(const wchar_t* name, const wchar_t* def = L"") {
    wchar_t buf[1024] = L"";
    ULONG count = _countof(buf);
    if (reg.m_hKey && reg.QueryStringValue(name, buf, &count) == ERROR_SUCCESS)
      return buf;
    else
      return def;
  }

  LONG SetDWORD(const wchar_t* name, DWORD value) {
    if (reg.m_hKey)
      return reg.SetDWORDValue(name, value);
    else
      return ERROR_INVALID_HANDLE;
  }

  LONG SetString(const wchar_t* name, const wchar_t* val) {
    if (reg.m_hKey)
      return reg.SetStringValue(name, val);
    else
      return ERROR_INVALID_HANDLE;
  }

  ATL::CRegKey reg;
};
