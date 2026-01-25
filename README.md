# Telecontrol SCADA Core

A C++ library providing a distributed SCADA (Supervisory Control and Data Acquisition) system framework. It implements OPC UA-like semantics for industrial control and data acquisition, with support for remote communication, metrics, and a hierarchical node-based data model.

- **Language:** C++20
- **Build System:** CMake
- **License:** GPLv3
- **Version:** 2.5.5
- **Platforms:** Windows (MSVC), Linux (GCC/Clang)

## Features

- OPC UA-compatible hierarchical node-based data model
- Protocol Buffers-based remote communication with gRPC support
- Distributed tracing and metrics collection
- Promise-based async patterns
- Seven core services: Attribute, MonitoredItem, Method, History, View, NodeManagement, Session
- Cross-platform support

## Requirements

- C++20 compatible compiler
- CMake 3.21+
- Ninja (recommended) or other CMake generator

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

## Building

### Using CMake Presets (Recommended)

```bash
# Configure
cmake --preset ninja-multi

# Build all targets
cmake --build build --config Release

# Build only core library
cmake --build build --config Release --target core

# Run tests
ctest --preset release
```

### Manual Build

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

### Available CMake Presets

| Preset | Description |
|--------|-------------|
| `ninja-multi` | Ninja Multi-Config generator |
| `ninja-multi-vcpkg` | Ninja with vcpkg toolchain |
| `linux-gcc` | Linux with GCC |
| `linux-clang` | Linux with Clang |
| `windows-msvc` | Windows with MSVC |
| `windows-msvc-vcpkg` | Windows with MSVC and vcpkg |

### Build Presets

| Preset | Description |
|--------|-------------|
| `debug` | Debug build |
| `release` | Release build |
| `relwithdebinfo` | Release with debug info |

### Test Presets

| Preset | Description |
|--------|-------------|
| `debug` | Run tests in Debug configuration |
| `release` | Run tests in Release configuration |

## Services Architecture

The library provides seven core services accessible via `scada::services`:

| Service | Description |
|---------|-------------|
| `AttributeService` | Read/Write node attributes |
| `MonitoredItemService` | Monitor value changes |
| `MethodService` | Call remote methods |
| `HistoryService` | Access historical data |
| `ViewService` | Browse node structure |
| `NodeManagementService` | Create/delete nodes |
| `SessionService` | Manage user sessions |

## Usage Example

```cpp
#include "scada/client.h"
#include "scada/node_id.h"

scada::client client(services);
auto node = client.node(NodeId{1, NS_DEVICES});

// Read attribute
auto value = node.read_value();

// Write attribute
node.write_value(Variant{42});

// Monitor changes
auto item = node.create_monitored_item([](const DataValue& dv) {
    // Handle value change
});

// Async operations with promises
client.connect(params)
    .then([&]() { return node.read_value(); })
    .then([](const DataValue& value) { /* use value */ })
    .except([](std::exception_ptr e) { /* handle error */ });
```

## Testing

```bash
# Run all tests
ctest --test-dir build

# Run specific module tests
ctest --test-dir build -R scada_core_unittests

# Verbose output on failure
ctest --test-dir build --output-on-failure
```

## Static Analysis

The build system integrates cppcheck for static analysis. It runs automatically during compilation with:
- Warning, performance, and portability checks enabled
- Inline suppressions supported
- Suppressions file: `cppcheck-suppressions.txt`

## Protocol Buffers

Proto files are located in `remote/`:
- `scada_core.proto` - Core types (NodeId, Variant, DataValue, etc.)
- `scada.proto` - Service definitions and RPC messages

CMake handles protobuf generation automatically during the build.

## Code Style

Uses Chromium style. Run `clang-format` on modified files (see `.clang-format`).

### Naming Conventions

- **Namespaces:** `scada`, `scada::id`
- **Classes:** `PascalCase` (e.g., `NodeId`, `DataValue`)
- **Functions/Methods:** `snake_case` (e.g., `is_null()`, `numeric_id()`)
- **Member variables:** Trailing underscore (e.g., `namespace_index_`)
- **Files:** `snake_case.h` / `snake_case.cpp`

## License

This project is licensed under the GNU General Public License v3.0 - see the [LICENSE](LICENSE) file for details.
