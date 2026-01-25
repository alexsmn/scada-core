# CLAUDE.md - AI Assistant Guide for scada-core

This document provides comprehensive guidance for AI assistants working with the scada-core codebase.

## Project Overview

**scada-core** is a C++ library providing a distributed SCADA (Supervisory Control and Data Acquisition) system framework. It implements OPC UA-like semantics for industrial control and data acquisition, with support for remote communication, metrics, and a hierarchical node-based data model.

- **Language:** C++20
- **Build System:** CMake
- **License:** GPLv3
- **Version:** 2.5.5
- **Platforms:** Windows (MSVC), Linux (GCC/Clang)

## Quick Reference Commands

### Building with CMake Presets (Preferred)

```bash
# Configure
cmake --preset ninja

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
- **Async:** `promise.h` - Promise-based async patterns (uses `promise-hpp`)
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

### model/ - Data Model
OPC UA-compatible node definitions:
- **`namespaces.h`** - Namespace indices (NS0-NS29)
- **`*_node_ids.h`** - Generated node ID headers from `node_ids.csv`

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
- Use `promise<T>` for async operations (from `promise-hpp`)
- Return `promise<void>` for operations without results
- Use `StatusOr<T>` for operations that may fail with status codes

### Error Handling
- 40+ status codes defined (Good, Uncertain_*, Bad_*)
- Use `Status` for error-only returns
- Use `StatusOr<T>` for value-or-error returns
- Promises propagate errors via exceptions

### Modern C++ Features (C++20)
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
- ChromiumBase
- promise-hpp
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

### Promise Usage
```cpp
// Chain operations
client.connect(params)
    .then([&]() { return node.read_value(); })
    .then([](const DataValue& value) { /* use value */ })
    .except([](std::exception_ptr e) { /* handle error */ });
```

## CI/CD

GitHub Actions workflow (`.github/workflows/cmake-multi-platform.yml`):
- Triggers on push/PR to `release/2.5`
- Matrix build: Windows (MSVC), Linux (GCC, Clang)
- Steps: Configure, Build, Test

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
| Async utilities | `base/promise.h` |
| Metrics API | `metrics/metric_service.h` |
| Namespace definitions | `model/namespaces.h` |

## Common Tasks for AI Assistants

### Adding a New Service Method
1. Define the method signature in the appropriate service interface (`scada/*_service.h`)
2. Add protocol message types to `remote/scada.proto`
3. Implement stub handler in `remote/*_stub.cpp`
4. Implement proxy in `remote/*_proxy.cpp`
5. Add protocol conversions in `remote/protocol_utils.cpp`
6. Write unit tests

### Adding a New Node Type
1. Add node ID to `model/node_ids.csv`
2. Regenerate headers (CMake does this automatically)
3. Update namespace if needed in `model/namespaces.h`

### Adding Unit Tests
1. Create `*_unittest.cpp` in the module directory
2. Tests are auto-discovered by CMake
3. Use GTest/GMock patterns
4. Link against `base_unittest` for test infrastructure

## Notes for Code Modifications

- Always read existing code before making changes
- Follow existing naming and formatting conventions
- Use `promise<T>` for new async APIs
- Add appropriate `constexpr` and `noexcept` specifiers
- Prefer `StatusOr<T>` over exceptions for expected failures
- Keep backward compatibility with Protocol Buffers (don't reuse field numbers)
- Run tests after changes: `ctest --test-dir build`
