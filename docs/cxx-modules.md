# C++20 Modules Pilot: the `scada.base` facade

Status: experimental, opt-in via `-DSCADA_CXX_MODULES=ON` (default OFF).
When OFF, the build is unchanged — no module targets exist, nothing scans
for imports.

## Design

`core/base/scada_base.cppm` defines the named module `scada.base` as a
**facade**: its global module fragment `#include`s the public `base/`
headers (the headers remain the source of truth) and the module purview
re-exports the public names with `export using` — the same technique MSVC's
`std.ixx` uses. Every entity stays attached to the global module, so a TU
that does `import scada.base;` and a TU that does `#include "base/check.h"`
(or one TU doing both) refer to the same entities; hybrid linking is
ODR-safe. This is what makes the dual-mode (headers + module) build possible.

The module is built by the dedicated target `scada_base_module`
(`core/base/CMakeLists.txt`), which links `PUBLIC scada_base`. Consumers
link `scada_base_module` and set `CXX_SCAN_FOR_MODULES ON` (the tree's CMake
policy version predates CMP0155, so ordinary sources are not scanned unless
opted in). `core/metrics` is the pilot consumer (`meter.cpp`, gated by the
`SCADA_USE_BASE_MODULE` compile definition).

## Rules for importing TUs

- `import scada.base;` provides *names* only. Macros cannot be exported:
  - `LOG_*` additionally requires `#include "base/boost_log.h"`.
  - `SCADA_LIFETIME_BOUND` requires `#include "base/lifetime.h"`.
  Both are safe alongside the import.
- Put the import **after** the textual `#include`s. The reverse order trips
  an AppleClang 21 declaration-merging bug in libc++ (dependent alias
  template `__promote_t`: "type alias template redefinition with different
  types").
- A TU that *defines* a coroutine needs `#include <coroutine>` textually,
  even when all coroutine-related names come from the import.
- Global ADL `operator<<` overloads (e.g. from `base/boost_log.h`,
  `base/debug_util.h`) are deliberately not exported — include the owning
  header where needed.
- CTAD deduction guides declared in headers (e.g. `StructFormatter`) may not
  be reachable through the import; include the header if you rely on them.

## Rules for CMake targets with importing TUs

- Link `scada_base_module`, set `CXX_SCAN_FOR_MODULES ON`.
- **Clear the compiler launcher** (`CXX_COMPILER_LAUNCHER ""`): ccache does
  not hash the imported BMI, so it can restore stale objects for importing
  TUs (observed: a TU kept an old inlined function body after the module
  was rebuilt). This is a correctness issue, not a cache-miss issue.
- **No PCH** on such targets: clang-scan-deps cannot read the AppleClang
  PCH and silently emits an empty dependency scan, so the import goes
  unnoticed and the build breaks later (see `core/metrics/CMakeLists.txt`).
- Set `CXX_CPPCHECK ""` — cppcheck cannot parse module units/importing TUs.

## Toolchain matrix

| Toolchain | Requirement | Notes |
|---|---|---|
| CMake | >= 3.28 | `FILE_SET CXX_MODULES` with Ninja; configure-time guard |
| MSVC | cl >= 19.34 | supported natively by CMake |
| GCC (Linux) | >= 14 | P1689 scanning; GCC 13 fails configure fast when ON |
| AppleClang | 21 tested | needs two workarounds, both in `core/CMakeLists.txt` / presets: CMake wires no scanner for AppleClang (mirrored `Clang-CXX.cmake` block, using Homebrew LLVM's `clang-scan-deps` + explicit `-isysroot`), and named modules parse only behind `-fcxx-modules` (added PUBLIC on `scada_base_module`) |

## How to build

- macOS: `cmake --preset macos-local-modules && cmake --build --preset
  release-macos-local-modules`; tests via
  `ctest --preset test-release-macos-local-modules`.
- Windows: `ninja-dev-modules` / `release-dev-modules` /
  `test-release-dev-modules` (requires CMake >= 3.28 at
  `C:/Program Files/CMake`).
- Smoke coverage: `core/base/module_test/` — an import-only TU and a mixed
  import+include TU (proves hybrid ODR/linkage).

## Extending the export surface

Add the header to the global module fragment **before** the export blocks
(a using-declaration exports only the overloads visible at that point), then
add `using` declarations for its public names. Do not export names with
internal linkage (anonymous namespaces in headers are ill-formed to export)
or `internal`/`detail` namespaces. Names that exist only in some build
configurations need matching guards (e.g. `ThreadCollisionWarner` is
`#ifndef NDEBUG`; `WaitableEvent`, `SharedEvent`, `MemoryIStream` are
`#ifdef _WIN32`).

## Non-goals of the pilot

`import std` / header units; converting modules beyond `core/base` (plus the
one `scada_metrics` TU); true per-file modularization; removing PCH from the
header build; modules as a required CI check; GCC 13 or the Visual Studio
generator.
