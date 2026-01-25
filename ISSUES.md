# Known Issues

## CMAKE_CXX_CPPCHECK Applied to All Presets

**Status:** Open
**Date:** 2026-01-25

### Problem

When configuring with `cmake --preset ninja`, the `CMAKE_CXX_CPPCHECK` variable from the hidden `static-analysis` preset is unexpectedly applied, even though the `ninja` preset does not inherit from it.

This causes build failures when compiling the chromebase dependency because the cppcheck co-compile wrapper (`cmake -E __run_co_compile`) does not properly inherit the MSVC `INCLUDE` environment variable, resulting in:

```
fatal error C1083: Cannot open include file: 'climits': No such file or directory
```

### Expected Behavior

- `ninja` preset: No static analysis tools enabled
- `ninja-analysis` preset: cppcheck and clang-tidy enabled

### Actual Behavior

Both presets have `CMAKE_CXX_CPPCHECK` set in the CMake cache with type `UNINITIALIZED`.

### Inheritance Chain

```
CMakePresets.json:
  base (hidden)           <- No cppcheck
  static-analysis (hidden) <- Has CMAKE_CXX_CPPCHECK
  ninja                   <- inherits: base (NOT static-analysis)
  ninja-analysis          <- inherits: [ninja, static-analysis]
```

### Cache Evidence

```
CMAKE_CXX_CPPCHECK:UNINITIALIZED=C:/Program Files/Cppcheck/cppcheck.exe;--enable=warning,performance,portability;--inline-suppr;--suppressions-list=C:/tc/scada/core/cppcheck-suppressions.txt;--error-exitcode=1
```

### Workarounds

1. **Run from VS Developer Command Prompt** - Ensures `INCLUDE` environment variable is set
2. **Explicitly clear cppcheck:** `cmake --preset ninja -DCMAKE_CXX_CPPCHECK=""`
3. **Remove static-analysis preset** from CMakePresets.json and run cppcheck via CI only

### Investigation Notes

- The issue persists even after deleting the build directory and reconfiguring
- The value matches CMakePresets.json line 25 exactly (static-analysis preset)
- No other CMakeLists.txt or .cmake files set CMAKE_CXX_CPPCHECK
- May be a CMake bug where hidden preset cacheVariables are loaded regardless of inheritance

### Environment

- CMake 4.2
- Visual Studio 18 (2026) Community
- Windows 10
- Ninja Multi-Config generator
