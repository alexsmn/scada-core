#pragma once

// Portable lifetime annotations.
//
// `SCADA_LIFETIME_BOUND` wraps the compiler's "lifetimebound" attribute.
// Placing it on a function parameter — or, for a member function, after the
// parameter list to bind to the implicit object parameter — tells the compiler
// that the function's returned reference/pointer/view must not outlive that
// argument. Callers that bind the result to a longer-lived variable then get a
// dangling diagnostic (Clang -Wdangling, MSVC C26815/C26816). It expands to
// nothing on toolchains that do not support the attribute (e.g. GCC), so it is
// safe to use unconditionally across platforms.
//
// Example:
//   const std::string& name() const SCADA_LIFETIME_BOUND { return name_; }
//   std::string_view trim(std::string_view s SCADA_LIFETIME_BOUND);

#if defined(__has_cpp_attribute)
#if __has_cpp_attribute(clang::lifetimebound)
#define SCADA_LIFETIME_BOUND [[clang::lifetimebound]]
#elif __has_cpp_attribute(msvc::lifetimebound)
#define SCADA_LIFETIME_BOUND [[msvc::lifetimebound]]
#endif
#endif

#ifndef SCADA_LIFETIME_BOUND
#define SCADA_LIFETIME_BOUND
#endif
