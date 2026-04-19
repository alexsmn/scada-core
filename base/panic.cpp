#include "base/panic.h"

#include <cstdio>
#include <cstdlib>

#if defined(WIN32)
#include <windows.h>
#include <intrin.h>
#endif

namespace base {

namespace {

// Borrowed from Chromium's immediate_crash.h approach: emit diagnostics first,
// then terminate with a non-continuable trap instead of relying on assert-only
// behavior in debug builds.
[[noreturn]] void ImmediateCrash() {
#if defined(WIN32)
  __fastfail(FAST_FAIL_FATAL_APP_EXIT);
#elif defined(__clang__) || defined(__GNUC__)
  __builtin_trap();
  __builtin_unreachable();
#else
  std::abort();
#endif
}

}  // namespace

[[noreturn]] void Panic(std::string_view message) {
  // Keep panic reporting on stderr. Boost.Log is too heavyweight for a panic
  // path because it may allocate, lock, or depend on partially torn down
  // logging infrastructure.
  std::fwrite(message.data(), 1, message.size(), stderr);
  std::fputc('\n', stderr);
  std::fflush(stderr);
  ImmediateCrash();
}

}  // namespace base
