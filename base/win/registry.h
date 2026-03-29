#pragma once

#include <windows.h>
#include <string>

namespace base {
namespace win {

class RegKey {
 public:
  RegKey() = default;

  RegKey(HKEY root, const wchar_t* subkey, REGSAM access) {
    LONG result = ::RegOpenKeyExW(root, subkey, 0, access, &key_);
    if (result != ERROR_SUCCESS && (access & KEY_WRITE)) {
      ::RegCreateKeyExW(root, subkey, 0, nullptr, 0, access, nullptr, &key_,
                         nullptr);
    }
  }

  ~RegKey() { Close(); }

  RegKey(const RegKey&) = delete;
  RegKey& operator=(const RegKey&) = delete;

  bool Valid() const { return key_ != nullptr; }

  LONG ReadValue(const wchar_t* name, std::wstring* value) const {
    if (!key_)
      return ERROR_INVALID_HANDLE;
    DWORD type = 0;
    DWORD size = 0;
    LONG result = ::RegQueryValueExW(key_, name, nullptr, &type, nullptr, &size);
    if (result != ERROR_SUCCESS || type != REG_SZ)
      return result != ERROR_SUCCESS ? result : ERROR_INVALID_DATA;
    value->resize(size / sizeof(wchar_t));
    result = ::RegQueryValueExW(key_, name, nullptr, nullptr,
                                 reinterpret_cast<BYTE*>(value->data()), &size);
    if (result == ERROR_SUCCESS && !value->empty() && value->back() == L'\0')
      value->pop_back();
    return result;
  }

  LONG WriteValue(const wchar_t* name, const wchar_t* value) {
    if (!key_)
      return ERROR_INVALID_HANDLE;
    return ::RegSetValueExW(key_, name, 0, REG_SZ,
                             reinterpret_cast<const BYTE*>(value),
                             static_cast<DWORD>((wcslen(value) + 1) * sizeof(wchar_t)));
  }

  LONG WriteValue(const wchar_t* name, DWORD value) {
    if (!key_)
      return ERROR_INVALID_HANDLE;
    return ::RegSetValueExW(key_, name, 0, REG_DWORD,
                             reinterpret_cast<const BYTE*>(&value),
                             sizeof(value));
  }

  LONG ReadValueDW(const wchar_t* name, DWORD* value) const {
    if (!key_)
      return ERROR_INVALID_HANDLE;
    DWORD type = 0;
    DWORD size = sizeof(DWORD);
    return ::RegQueryValueExW(key_, name, nullptr, &type,
                               reinterpret_cast<BYTE*>(value), &size);
  }

 private:
  void Close() {
    if (key_) {
      ::RegCloseKey(key_);
      key_ = nullptr;
    }
  }

  HKEY key_ = nullptr;
};

}  // namespace win
}  // namespace base
