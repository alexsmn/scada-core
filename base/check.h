#pragma once

#include <concepts>
#include <source_location>
#include <string_view>

#include "base/panic.h"

namespace base {

// Hardened invariant checks - active in ALL build types, including
// NDEBUG/release. On failure they terminate via base::Panic, reporting the
// caller's source location (file:line:function) and the optional message.
//
// Use these for internal invariants only. Expected failures - malformed
// network, device, or configuration input - must NOT panic: validate the
// input, log, and return scada::Status / scada::StatusOr instead.
//
// Unlike assert(), the condition text is not captured automatically; the
// source location pinpoints the failing call site, and call sites whose
// condition is not self-explanatory should pass a message.

// Constraint for Check conditions: anything usable in a boolean context,
// including types with an explicit operator bool (smart pointers,
// std::optional), matching the semantics of the assert() macro.
template <typename C>
concept CheckCondition =
    requires(C&& condition) { static_cast<bool>(std::forward<C>(condition)); };

// Panics if `condition` is false. `message` should state the violated
// invariant, e.g. base::Check(!items_.empty(), "aggregate has no items").
template <CheckCondition C>
constexpr void Check(
    C&& condition,
    std::string_view message = "Check failed",
    const std::source_location& location = std::source_location::current()) {
  if (!static_cast<bool>(condition)) [[unlikely]]
    Panic(message, location);
}

// Check overload whose message is built only on failure, for call sites
// where formatting the message is not free, e.g.
//   base::Check(size == expected, [&] { return std::format("size={}", size);
//   });
template <CheckCondition C, std::invocable F>
constexpr void Check(
    C&& condition,
    F&& make_message,
    const std::source_location& location = std::source_location::current()) {
  if (!static_cast<bool>(condition)) [[unlikely]]
    Panic(std::forward<F>(make_message)(), location);
}

// Marks a code path that must be impossible to reach (exhaustive switch
// defaults, dead branches). Always panics. Being [[noreturn]], it satisfies
// missing-return and switch-exhaustiveness diagnostics without invoking
// undefined behavior the way C++23 std::unreachable would.
[[noreturn]] inline void NotReached(
    std::string_view message = "NotReached",
    const std::source_location& location = std::source_location::current()) {
  Panic(message, location);
}

}  // namespace base
