# CLAUDE.md - AI Assistant Guide for scada-core

This document provides comprehensive guidance for AI assistants working with the scada-core codebase.

## Project Overview

**scada-core** is a C++ library providing a distributed SCADA (Supervisory Control and Data Acquisition) system framework. It implements OPC UA-like semantics for industrial control and data acquisition, with support for remote communication, metrics, and a hierarchical node-based data model.

- **Language:** C++23
- **Build System:** CMake
- **License:** GPLv3
- **Version:** 2.6.0
- **Platforms:** Windows (MSVC), Linux (GCC/Clang)

## Quick Reference Commands

### Building with CMake Presets (Preferred)

```bash
# Configure
cmake --preset ninja-multi

# Build (all targets)
cmake --build --preset relwithdebinfo

# Build only core library
cmake --build build --config RelWithDebInfo --target core

# Run tests
ctest --preset release
```

### Building (Manual)

```bash
# Configure (Linux)
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Configure (Windows)
cmake -B build

# Build
cmake --build build --config Release

# Run tests
ctest --test-dir build --build-config Release
```

### Common Development Tasks

```bash
# Generate protocol buffers (handled by CMake automatically)
# Proto files: remote/scada.proto, remote/scada_core.proto

# Run specific test suite
ctest --test-dir build -R <module>_unittests
```

## Directory Structure

```
scada-core/
├── base/           # Core utilities & infrastructure (~5,700 LOC)
├── scada/          # Main SCADA API & services (~5,800 LOC)
├── metrics/        # Telemetry & monitoring (~700 LOC)
├── model/          # Data model & node definitions (~850 LOC)
├── remote/         # Remote communication protocol (~5,400 LOC)
├── .github/        # CI/CD workflows
├── CMakeLists.txt  # Root CMake configuration
├── scada_module.cmake   # Module build macros
└── boost.cmake     # Boost dependency configuration
```

## Module Architecture

### base/ - Core Infrastructure
Foundation utilities used throughout the codebase:
- **Async:** `awaitable.h` - Coroutine-based async patterns
- **Logging:** `boost_log.h`, `console_logger.h`, `nested_logger.h`
- **Executors:** `executor.h`, `thread_executor.h`, `asio_executor.h`
- **Data:** `json.h`, `csv_reader.h`, `bytemsg.h`, `bytebuf.h`
- **Utilities:** `string_util.h`, `uri.h`, `time_utils.h`, `cancelation.h`

### scada/ - Main API Layer
Core SCADA abstractions and services:
- **`node.h`** - Node abstraction for hierarchical data model
- **`node_id.h`** - Namespace-based node identification
- **`services.h`** - Service locator with 7 core services
- **`client.h`** - High-level client API
- **`variant.h`** - Multi-type value container
- **`data_value.h`** - Values with timestamps and status
- **`status.h` / `status_or.h`** - Error handling patterns

### metrics/ - Observability
- **`metric_service.h`** - Main metrics interface
- **`tracer.h` / `tracing.h`** - Distributed tracing support
- Provider/sink pattern for metric collection

### model/ - Data Model (MOVED)
The model library moved out of core into the `common` submodule at
`common/model/`. The nodeset XML there is the single source of truth and the
`namespaces.*` / `*_node_ids.h` headers are generated from it at build time. See
`common/model/` and `common/CLAUDE.md`. `core` no longer depends on the model;
low-level code that needs a friendly NodeId log string uses
`scada::NodeIdToLogString` (`core/scada/node_id_log.h`), whose namespace-name
resolver the model installs at startup.

### remote/ - Protocol Layer
Protocol Buffers-based remote communication:
- **`scada.proto`** - Service definitions, request/response types
- **`scada_core.proto`** - Core data structures
- **`protocol_utils.cpp`** - Type conversions (~29KB)
- Proxy classes (client-side) and stub classes (server-side)

## Services Architecture

The `services` struct in `scada/services.h` provides access to seven core services:

```cpp
struct services {
  AttributeService* attribute_service;      // Read/Write node attributes
  MonitoredItemService* monitored_item_service;  // Monitor value changes
  MethodService* method_service;            // Call remote methods
  HistoryService* history_service;          // Access historical data
  ViewService* view_service;                // Browse node structure
  NodeManagementService* node_management_service;  // Create/delete nodes
  SessionService* session_service;          // Manage user sessions
};
```

## Coding Conventions

### Naming
- **Namespaces:** `scada`, `scada::id` (for node IDs)
- **Classes:** `PascalCase` (e.g., `NodeId`, `DataValue`)
- **Functions/Methods:** `snake_case` (e.g., `is_null()`, `numeric_id()`)
- **Constants:** `PascalCase` in namespace `id` (e.g., `id::Server`)
- **Member variables:** Trailing underscore (e.g., `namespace_index_`)
- **Files:** `snake_case.h` / `snake_case.cpp`

### Header Guards
Use `#pragma once` (not traditional include guards)

### Includes
Order: Standard library, external deps (Boost, etc.), project headers

### Async Patterns
- Use `Awaitable<T>` for async operations
- Return `Awaitable<void>` for operations without results
- Use `StatusOr<T>` for operations that may fail with status codes

### Error Handling
- 40+ status codes defined (Good, Uncertain_*, Bad_*)
- Use `Status` for error-only returns
- Use `StatusOr<T>` for value-or-error returns
- `StatusOr<T>` derives from `std::expected<T, Status>`: implicit
  construction from `Status`/`StatusCode`, fail-stop (panicking) accessors
  instead of throwing/UB ones, plus the standard monadic operations
  (`and_then`/`transform`/`or_else`)
- Coroutines propagate unexpected failures via exceptions
- Internal invariants: `base::Check(cond, "message")` / `base::NotReached()`
  from `base/check.h` — always-on in every build type, fail-stop via
  `base::Panic`. Never bare `assert()`; no debug-only checks. External input
  (wire data, config) never panics — validate, log, return `Status`/`StatusOr`

### Modern C++ Features (C++23)
- `constexpr` and `noexcept` where appropriate
- `std::variant` for type-safe unions
- Spaceship operator (`<=>`) for comparisons
- `std::string_view` for non-owning string references

## Build System Details

### CMake Module System

The `scada_module.cmake` provides macros for defining modules:

```cmake
# Create a library module (auto-discovers sources)
scada_module(module_name)

# Add sources from subdirectory
scada_module_sources(module_name PRIVATE "subdir")

# Unit tests (auto-registered with CTest)
scada_module_unittests(module_name source_files)
```

### Source Discovery
- Auto-discovers `*.cpp` and `*.h` files
- Excludes `*_mock.*` files from library
- Excludes `*_unittest.*` files (added to test target)
- Platform-specific code in `win/` subdirectories

### Dependencies

**Required:**
- Boost (atomic, context, date_time, filesystem, json, log, program_options, thread)
- Protobuf 3
- Net (networking abstraction)

**Optional:**
- gRPC (for gRPC service generation)
- GTest/GMock (for testing)

**Windows-specific:**
- OpenSSL (libssl, libcrypto)
- OPC UA libraries

## Testing

### Test Organization
- Each module has a `test/` subdirectory
- Unit tests: `*_unittest.cpp`
- Test infrastructure: `base/test/`
- Mocks: `*_mock.h`

### Running Tests
```bash
# All tests
ctest --test-dir build

# Specific module
ctest --test-dir build -R scada_core_unittests

# Verbose output
ctest --test-dir build --output-on-failure
```

### Test Utilities
- `test_executor.h` - Executor for async tests
- `data_value_awaiter.h` - Wait for data updates
- `event_awaiter.h` - Wait for events
- `test_monitored_item.h` - Mock monitored items

## Protocol Buffers

### Proto Files
- `remote/scada_core.proto` - Core types (NodeId, Variant, DataValue, etc.)
- `remote/scada.proto` - Service definitions and RPC messages

### Regenerating
CMake handles protobuf generation automatically. To manually regenerate:
```bash
# See remote/README.md for JavaScript generation
```

## Key Patterns

### Node Operations
```cpp
scada::client client(services);
auto node = client.node(NodeId{1, NS_DEVICES});

// Read attribute
auto value = node.read_value();

// Write attribute
node.write_value(Variant{42});

// Monitor changes
auto item = node.create_monitored_item([](const DataValue& dv) {
    // Handle change
});
```

### Coroutine Usage
```cpp
co_await client.connect(params);
auto value = co_await node.read_value();
```

## CI/CD

GitHub Actions workflow (`.github/workflows/cmake-multi-platform.yml`):
- Triggers on push/PR to `release/2.5`
- Matrix build: Windows (MSVC), Linux (GCC, Clang)
- Steps: Configure, Build, Test

## Chromium-Base Dependencies

None. The former `ChromiumBase::base` dependency was fully removed; the
chromium-heritage `base/...` headers under `base/` (`at_exit.h`,
`threading/`, `win/scoped_*.h`, etc.) are scada-owned, std-based
reimplementations in `namespace scada::base`. The replacements were:
`std::filesystem` (paths/files), `boost::json` (for `base::Value`),
`boost::program_options` (for `base::CommandLine`), `std::to_chars`
(for `dmg_fp::g_fmt`), and local API-compatible `base::PathService`
and `base::win::ScopedHandle`.

The former chromium `base/time/time.h` (`base::Time` / `base::TimeDelta` /
`base::TimeTicks`) is now thin `std::chrono` aliases: `base::Time =
std::chrono::sys_time<std::chrono::microseconds>` (also `scada::DateTime`),
`base::TimeDelta = std::chrono::microseconds` (also `scada::Duration`), and
monotonic timing uses `std::chrono::steady_clock` directly. The "null"
timestamp (0 ticks since the 1601 Windows epoch) is `base::kNullTime` —
distinct from the default-constructed value (the Unix epoch) — tested with
`base::IsNull()`; range sentinels are `base::kMaxTime`/`kMinTime`. The
µs-since-1601 wire/on-disk encoding is preserved behind
`base/time/time_wire_codec.h`, and calendar/string conversions are the free
functions in `base/time/calendar.h` (UTC via `std::chrono`, local time via the
OS since libc++ ships no tzdb).

## Important Files Reference

| Purpose | File |
|---------|------|
| Main client API | `scada/client.h` |
| Node abstraction | `scada/node.h` |
| Node identification | `scada/node_id.h` |
| Service interfaces | `scada/services.h` |
| Status codes | `scada/status.h` |
| Variant type | `scada/variant.h` |
| Protocol definition | `remote/scada.proto` |
| Async utilities | `base/awaitable.h` |
| Metrics API | `metrics/otel_metrics.h` |
| Namespace definitions | `common/model/` (generated; see common/CLAUDE.md) |

## Common Tasks for AI Assistants

### Adding a New Service Method
1. Define the method signature in the appropriate service interface (`scada/*_service.h`)
2. Add protocol message types to `remote/scada.proto`
3. Implement stub handler in `remote/*_stub.cpp`
4. Implement proxy in `remote/*_proxy.cpp`
5. Add protocol conversions in `remote/protocol_utils.cpp`
6. Write unit tests

### Adding a New Node Type
The model now lives in `common/model/`. Add the node to the appropriate
`common/model/nodesets/*.xml` with a `symbolicName` (and `codeNs` if it belongs
to a different domain than the file default); the `scada::<domain>::id::` C++
constant is generated on the next build. For an id with no static node, add a
row to `common/model/nodesets/extra_node_ids.csv`. Namespaces come from
`common/model/nodesets/namespaces.csv`. Never change an existing numeric id —
they are persisted in user configuration databases (see the
`ModelFrozenIds` regression test).

### Adding Unit Tests
1. Create `*_unittest.cpp` in the module directory
2. Tests are auto-discovered by CMake
3. Use GTest/GMock patterns
4. Link against `base_unittest` for test infrastructure

## Notes for Code Modifications

- Always read existing code before making changes
- Follow existing naming and formatting conventions
- Use `Awaitable<T>` for new async APIs
- Add appropriate `constexpr` and `noexcept` specifiers
- Prefer `StatusOr<T>` over exceptions for expected failures
- Keep backward compatibility with Protocol Buffers (don't reuse field numbers)
- Run tests after changes: `ctest --test-dir build`
