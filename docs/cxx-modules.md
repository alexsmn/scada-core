# C++20 Modules: the core/ facade modules

Status: experimental, opt-in via `-DSCADA_CXX_MODULES=ON` (default OFF).
When OFF, the build is unchanged — no module targets exist, nothing scans
for imports, PCH stays enabled.

## Module map

| Module | Facade target | Interface unit | `export import`s |
|---|---|---|---|
| `scada.base` | `scada_base_module` | `base/scada_base.cppm` | — |
| `scada.net` | `scada_net_module` | `net/scada_net.cppm` | `scada.base` |
| `scada.metrics` | `scada_metrics_module` | `metrics/scada_metrics.cppm` | `scada.base` |
| `scada.core` | `scada_core_module` | `scada/scada_core.cppm` | `scada.base`, `scada.metrics` |
| `scada.model` | `scada_model_module` | `model/scada_model.cppm` | `scada.core` |
| `scada.remote` | `scada_core_remote_module` | `remote/scada_remote.cppm` | `scada.core` |

In the common repo (same helpers; the AppleClang scandep variables are CACHE
INTERNAL so sibling repos see them):

| Module | Facade target | Interface unit | `export import`s |
|---|---|---|---|
| `scada.common` | `scada_common_module` | `common/scada_common.cppm` | `scada.core`, `scada.metrics` |
| `scada.events` | `scada_common_events_module` | `events/scada_events.cppm` | `scada.common` |
| `scada.timed_data` | `timed_data_module` | `timed_data/scada_timed_data.cppm` | `scada.common`, `scada.events` |
| `scada.address_space` | `address_space_module` | `address_space/scada_address_space.cppm` | `scada.core` |
| `scada.node_service` | `node_service_module` | `node_service/scada_node_service.cppm` | `scada.common` |
| `scada.opcua_bridge` | `scada_opcua_bridge_module` | `opcua_bridge/scada_opcua_bridge.cppm` | `scada.core` |

Not facaded in common: `node_service_v1/v2/v3/proxy` (implementation
targets), `scada_common_opc` / `scada_common_vidicon*` (Windows-only —
unbuildable on the macOS iteration platform). Additional common-specific
exclusions: `vds_runtime_api.h` (extern-C plugin ABI) and
`session_proxy_notifier.h` (its global SessionProxyNotifier template
collides by name with core/remote's class — exporting both would make TUs
importing scada.common and scada.remote together ill-formed).

Each facade `export import`s the facades of its PUBLIC-linked dependencies —
the module analogue of PUBLIC link transitivity — so `import scada.core;`
provides the full transitive surface (base + metrics + core names).

## Design

Each `.cppm` is a **facade**: its global module fragment `#include`s the
library's public headers (the headers remain the source of truth) and the
module purview re-exports the public names with `export using` (the
technique MSVC's `std.ixx` uses). Every entity stays attached to the global
module, so a TU that imports and a TU that includes — or one TU doing both —
refer to the same entities; hybrid linking is ODR-safe. This is what makes
the dual-mode (headers + modules) build possible.

CMake plumbing lives in three helpers at the end of
`core/scada_module.cmake` (all no-ops when the option is OFF):
`scada_add_module_facade`, `scada_module_import_pilot`,
`scada_add_module_smoke_test`.

## What is never exported (include the header textually, before the import)

- **Macros**: `LOG_*` (`base/boost_log.h`), `SCADA_LIFETIME_BOUND`
  (`base/lifetime.h`), `LOCALIZED_TEXT` (`scada/localized_text.h`),
  `REGISTER_DATA_SERVICES` (`scada/data_services_factory.h`).
- **Internal-linkage constants**: namespace-scope non-inline `const` /
  `constexpr` objects — all node-id constants (`model/*_node_ids.h`,
  `scada/standard_node_ids.h`), `NamespaceIndexes::*` (`model/namespaces.h`),
  `protocol::kHeaderSize`. Exporting them is ill-formed
  ([module.interface]); the constant headers are also deliberately excluded
  from the model facade's GMF (pure BMI bloat). `inline constexpr` /
  `inline const` objects have external linkage and ARE exported (e.g.
  `scada::access_level::*`, `cfg::NUM_CHANNELS`).
- **Global-namespace ADL `operator<<` overloads** (`base/boost_log.h`,
  `base/debug_util.h`, `scada/status_or.h`): exporting `::operator<<` would
  drag the entire global overload set. The `scada::` and `base::`
  namespace-scope operator sets ARE exported.
- **Third-party names** (`boost::*`, `transport::*`, `opentelemetry::*`,
  protobuf-generated types): include their headers textually.
- **Protobuf surface of remote**: `remote/protocol.h` and
  `remote/protocol_utils.h` are excluded from the `scada.remote` GMF (they
  pull the generated `scada.pb.h`). `Convert()` overloads,
  `PROTOCOL_VERSION_*`, and pb message types are include-only.
- **`internal`/`detail` namespaces** everywhere.

## Rules for importing TUs

- Put the import **after** the textual `#include`s. The reverse order trips
  an AppleClang 21 declaration-merging bug in libc++ (dependent alias
  template `__promote_t`: "type alias template redefinition with different
  types").
- A TU that *defines* a coroutine needs `#include <coroutine>` textually.
- A library's own TUs import only its *dependencies'* facades, never its
  own (self-import would be a target cycle).
- `std::hash<scada::NodeId>` / `std::formatter<scada::NodeId>` /
  `std::formatter<scada::Event...>` are kept decl-reachable by
  `static_assert`s in `scada_core.cppm` and covered by its smoke test — if
  a new std specialization is added to a header, add a keep-alive.
- CTAD deduction guides declared in headers may not be reachable through an
  import; include the header if you rely on them.

## Rules for CMake targets with importing TUs

Use `scada_module_import_pilot(<lib> IMPORTS <facades> DEFINES <macros>)`.
It sets `CXX_SCAN_FOR_MODULES ON` (the tree's policy version predates
CMP0155) and **clears the compiler launcher**: ccache does not hash the
imported BMI and can restore stale objects — a correctness issue, not a
cache miss. Additionally wrap the lib's
`target_precompile_headers(... REUSE_FROM scada_base)` in
`if(NOT SCADA_CXX_MODULES)`: clang-scan-deps cannot read the AppleClang PCH
and silently emits an EMPTY dependency scan, so the import goes unnoticed.
Watch for TUs that relied on the PCH for std headers (e.g. `<algorithm>`)
— they surface as errors once the PCH is off; add the missing includes.
Set `CXX_CPPCHECK ""` where cppcheck would see module units.

## Toolchain matrix

| Toolchain | Requirement | Notes |
|---|---|---|
| CMake | >= 3.28 | `FILE_SET CXX_MODULES` with Ninja; configure-time guard |
| MSVC | cl >= 19.34 | supported natively by CMake |
| GCC (Linux) | >= 14 | P1689 scanning; GCC 13 fails configure fast when ON |
| AppleClang | 21 tested | two workarounds in `core/CMakeLists.txt` / presets: CMake wires no scanner for AppleClang (mirrored `Clang-CXX.cmake` block, Homebrew LLVM `clang-scan-deps` + explicit `-isysroot`), and named modules parse only behind `-fcxx-modules` |

## How to build

- macOS: `cmake --preset macos-local-modules && cmake --build --preset
  release-macos-local-modules`; tests via
  `ctest --preset test-release-macos-local-modules`.
- Windows: `ninja-dev-modules` / `release-dev-modules` /
  `test-release-dev-modules` (requires CMake >= 3.28 at
  `C:/Program Files/CMake`).
- Smoke coverage: per-library `module_test/` dirs — import-only TUs, mixed
  import+include TUs (base, scada, remote/pb), and a multi-import interop
  TU (model: scada.base + scada.core + scada.model in one TU).

## Cross-toolchain validation checklist (MSVC / GCC 14)

The divergence-prone items to check when running `ninja-dev-modules` or a
GCC 14 build with `-DSCADA_CXX_MODULES=ON`:
1. `ScadaCoreModuleSmoke.StdSpecializationsReachable` — GMF discarding of
   `std::hash`/`std::formatter` specializations differs by frontend; if it
   fails, the fallback is a tiny exported helper that odr-uses them.
2. The mixed import+include smoke tests — GMF/textual declaration merging
   is the buggiest frontend area (AppleClang needed include-before-import).
3. Boost-heavy GMFs (`scada.base` Tier 3, `session_service.h` signals2) —
   GCC 14 ICE territory; if a header ICEs, drop it from that GMF and
   document it as include-only.
4. `/WX` on Windows — module-mixing warnings (C5201/C5244 family) may need
   local `/wd` on the facade targets.
5. `-fcxx-modules` is AppleClang-only (guarded); MSVC/GCC need no extra
   flags.

## Non-goals

`import std` / header units; converting `common/`/`server/`/`client/`;
true per-file modularization; removing PCH from the header build; modules
as a required CI check; GCC 13 or the Visual Studio generator.
