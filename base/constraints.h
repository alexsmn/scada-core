#pragma once

#include "base/check.h"


template <class Func>
class ScopedInvariant {
 public:
#if defined(NDEBUG)
  explicit ScopedInvariant(Func&& func) {}
  ~ScopedInvariant() {}
#else
  explicit ScopedInvariant(Func&& func) : func_{func} {
    scada::base::Check(func_());
  }
  ~ScopedInvariant() { scada::base::Check(func_()); }
#endif

 private:
#if !defined(NDEBUG)
  Func func_;
#endif
};
