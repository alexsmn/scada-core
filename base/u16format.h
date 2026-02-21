#pragma once

#include <format>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>

// std::format for char16_t strings (not natively supported by std::format).
// On Windows, wchar_t and char16_t are both 16-bit, so we format as wstring
// and reinterpret.

namespace u16format_detail {

// Convert args to wchar_t-compatible types for std::format:
// - u16string/u16string_view -> wstring_view (reinterpret, same size on Windows)
// - char16_t* -> wchar_t*
// - std::string -> std::wstring (widen)
// - Scoped enums/plain enums -> underlying type
// - Everything else -> pass through
template <typename T>
auto to_wide_arg(T&& arg) {
  using D = std::decay_t<T>;
  if constexpr (std::is_same_v<D, std::u16string>) {
    return std::wstring_view{
        reinterpret_cast<const wchar_t*>(arg.data()), arg.size()};
  } else if constexpr (std::is_same_v<D, std::u16string_view>) {
    return std::wstring_view{
        reinterpret_cast<const wchar_t*>(arg.data()), arg.size()};
  } else if constexpr (std::is_same_v<D, const char16_t*> ||
                        std::is_same_v<D, char16_t*>) {
    return reinterpret_cast<const wchar_t*>(arg);
  } else if constexpr (std::is_same_v<D, std::string>) {
    return std::wstring(arg.begin(), arg.end());
  } else if constexpr (std::is_same_v<D, std::string_view>) {
    return std::wstring(arg.begin(), arg.end());
  } else if constexpr (std::is_enum_v<D>) {
    return static_cast<std::underlying_type_t<D>>(arg);
  } else {
    return std::forward<T>(arg);
  }
}

}  // namespace u16format_detail

template <typename... Args>
std::u16string u16format(std::wstring_view fmt, Args&&... args) {
  // Store converted args in a tuple so they are lvalues for make_wformat_args.
  auto converted = std::make_tuple(
      u16format_detail::to_wide_arg(std::forward<Args>(args))...);
  return std::apply(
      [&](auto&... wide_args) {
        std::wstring wide =
            std::vformat(fmt, std::make_wformat_args(wide_args...));
        return std::u16string{wide.begin(), wide.end()};
      },
      converted);
}
