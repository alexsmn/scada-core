#pragma once
#include <OAIdl.h>  // VARIANT
#include <cstdint>

namespace base {
namespace win {

class ScopedVariant {
 public:
  ScopedVariant() { VariantInit(&var_); }

  explicit ScopedVariant(const wchar_t* str) {
    VariantInit(&var_);
    var_.vt = VT_BSTR;
    var_.bstrVal = SysAllocString(str);
  }

  ScopedVariant(const wchar_t* str, UINT length) {
    VariantInit(&var_);
    var_.vt = VT_BSTR;
    var_.bstrVal = SysAllocStringLen(str, length);
  }

  explicit ScopedVariant(const VARIANT& source) {
    VariantInit(&var_);
    VariantCopy(&var_, &source);
  }

  explicit ScopedVariant(long value) {
    VariantInit(&var_);
    var_.vt = VT_I4;
    var_.lVal = value;
  }

  explicit ScopedVariant(int value) {
    VariantInit(&var_);
    var_.vt = VT_I4;
    var_.lVal = value;
  }

  ScopedVariant(long value, VARTYPE vt) {
    VariantInit(&var_);
    var_.vt = vt;
    var_.lVal = value;
  }

  ScopedVariant(double value, VARTYPE vt) {
    VariantInit(&var_);
    var_.vt = vt;
    var_.dblVal = value;
  }

  ~ScopedVariant() { VariantClear(&var_); }

  ScopedVariant(const ScopedVariant&) = delete;
  ScopedVariant& operator=(const ScopedVariant&) = delete;

  ScopedVariant(ScopedVariant&& other) noexcept : var_(other.var_) {
    VariantInit(&other.var_);
  }

  // For receiving out-parameters. Clears current value first.
  VARIANT* Receive() {
    VariantClear(&var_);
    VariantInit(&var_);
    return &var_;
  }

  void Reset() {
    VariantClear(&var_);
    VariantInit(&var_);
  }

  const VARIANT* ptr() const { return &var_; }
  VARIANT* ptr() { return &var_; }

  // Allow passing as VARIANT parameter.
  operator const VARIANT&() const { return var_; }

  // Raw pointer access (for COM calls).
  const VARIANT* AsInput() const { return &var_; }
  VARIANT* AsInput() { return &var_; }

  // Member access.
  VARTYPE type() const { return var_.vt; }

  // Set typed values.
  void Set(bool value) {
    Reset();
    var_.vt = VT_BOOL;
    var_.boolVal = value ? VARIANT_TRUE : VARIANT_FALSE;
  }
  void Set(int8_t value) { Reset(); var_.vt = VT_I1; var_.cVal = value; }
  void Set(uint8_t value) { Reset(); var_.vt = VT_UI1; var_.bVal = value; }
  void Set(int16_t value) { Reset(); var_.vt = VT_I2; var_.iVal = value; }
  void Set(uint16_t value) { Reset(); var_.vt = VT_UI2; var_.uiVal = value; }
  void Set(int32_t value) { Reset(); var_.vt = VT_I4; var_.lVal = value; }
  void Set(uint32_t value) { Reset(); var_.vt = VT_UI4; var_.ulVal = value; }
  void Set(int64_t value) { Reset(); var_.vt = VT_I8; var_.llVal = value; }
  void Set(uint64_t value) { Reset(); var_.vt = VT_UI8; var_.ullVal = value; }
  void Set(float value) { Reset(); var_.vt = VT_R4; var_.fltVal = value; }
  void Set(double value) { Reset(); var_.vt = VT_R8; var_.dblVal = value; }
  void Set(const wchar_t* value) {
    Reset();
    var_.vt = VT_BSTR;
    var_.bstrVal = SysAllocString(value);
  }

  // Release ownership without clearing.
  VARIANT Release() {
    VARIANT tmp = var_;
    VariantInit(&var_);
    return tmp;
  }

  // Attach a VARIANT (take ownership).
  void Attach(VARIANT* source) {
    Reset();
    var_ = *source;
    VariantInit(source);
  }

  // Copy from another VARIANT.
  HRESULT Copy(const VARIANT* source) {
    Reset();
    return VariantCopy(&var_, source);
  }

 private:
  VARIANT var_;
};

}  // namespace win
}  // namespace base
