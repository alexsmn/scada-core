#pragma once
#include <OAIdl.h>  // BSTR, SysAllocString, SysFreeString
#include <string>
#include <string_view>

namespace base {
namespace win {

class ScopedBstr {
 public:
  ScopedBstr() = default;
  explicit ScopedBstr(const wchar_t* str) : bstr_(SysAllocString(str)) {}
  explicit ScopedBstr(const std::wstring& str)
      : bstr_(SysAllocStringLen(str.c_str(), static_cast<UINT>(str.size()))) {}
  explicit ScopedBstr(std::wstring_view str)
      : bstr_(SysAllocStringLen(str.data(), static_cast<UINT>(str.size()))) {}
  ~ScopedBstr() { SysFreeString(bstr_); }

  ScopedBstr(const ScopedBstr&) = delete;
  ScopedBstr& operator=(const ScopedBstr&) = delete;

  ScopedBstr(ScopedBstr&& other) noexcept : bstr_(other.bstr_) {
    other.bstr_ = nullptr;
  }

  void Reset(const wchar_t* str = nullptr) {
    SysFreeString(bstr_);
    bstr_ = str ? SysAllocString(str) : nullptr;
  }

  // For receiving out-parameters.
  BSTR* Receive() {
    Reset();
    return &bstr_;
  }

  operator BSTR() const { return bstr_; }
  BSTR Get() const { return bstr_; }
  size_t Length() const { return bstr_ ? SysStringLen(bstr_) : 0; }

 private:
  BSTR bstr_ = nullptr;
};

}  // namespace win
}  // namespace base
