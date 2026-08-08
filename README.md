# Telecontrol SCADA Core

A C++ library providing a distributed SCADA (Supervisory Control and Data Acquisition) system framework. It implements OPC UA-like semantics for industrial control and data acquisition, with support for remote communication, metrics, and a hierarchical node-based data model.

- **Language:** C++20
- **Build System:** CMake
- **License:** GPLv3
- **Version:** 2.6.0
- **Platforms:** Windows (MSVC), Linux (GCC/Clang)

## Features

- OPC UA-compatible hierarchical node-based data model
- Protocol Buffers-based remote communication with gRPC support
- Distributed tracing and metrics collection
- Coroutine-based async patterns
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
- Net (networking abstraction)

**Optional:**
- gRPC (for gRPC service generation)
- GTest/GMock (for testing)

**Windows-specific:**
- OpenSSL (libssl, libcrypto)
- OPC UA libraries

## Building

### Using CMake Presets

core builds standalone. It consumes one other product, `net`, which must be
checked out beside it as `../net`. Set `VCPKG_ROOT` in the environment;
anything else machine-specific goes in `.scada-local.cmake` next to
`build-support/`. See [build-support/README.md](build-support/README.md).

```bash
# Configure
cmake --preset ninja

# Build all targets
cmake --build --preset release

# Build only the core library
cmake --build --preset release --target scada_core

# Run tests
ctest --preset test-release
```

Output lands in `build/ninja/bin/<config>/`.

If `net` lives somewhere else, name it:

```bash
cmake --preset ninja -DSCADA_PRODUCT_ROOT_NET=/path/to/net
```

### Available presets

Every product in the SCADA tree carries the same set, so the commands do not
change from one to the next.

| Preset | Kind | Description |
|--------|------|-------------|
| `ninja` | configure | Ninja Multi-Config with the vcpkg toolchain |
| `debug` | build | Debug build |
| `release` | build | Release build |
| `relwithdebinfo` | build | Release with debug info |
| `test-debug` | test | Run tests in Debug |
| `test-release` | test | Run tests in Release |

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

Additional documentation:

- `docs/services.md` for service API and adapter reference

## Usage Example

```cpp
#include "scada/client.h"
#include "scada/node_id.h"

scada::client client(services);
auto node = client.node(NodeId{1, NS_DEVICES});

// Read attribute
auto value = co_await node.read_value();

// Write attribute
co_await node.write_value(Variant{42});

// Monitor changes
auto item = node.create_monitored_item([](const DataValue& dv) {
    // Handle value change
});

// Async operations with coroutines
co_await client.connect(params);
auto connected_value = co_await node.read_value();
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
