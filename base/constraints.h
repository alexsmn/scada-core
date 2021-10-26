#pragma once

#include <cassert>

template <class Func>
class ScopedInvariant {
 public:
#if defined(NDEBUG)
  explicit ScopedInvariant(Func&& func) {}
  ~ScopedInvariant() {}
#else
  explicit ScopedInvariant(Func&& func) : func_{func} { assert(func_()); }
  ~ScopedInvariant() { assert(func_()); }
#endif

 private:
#if !defined(NDEBUG)
  Func func_;
#endif
};
