# Chromium-Base Dependencies in Core

## Dependency Ranking by Module

| Rank | Module   | Includes | Files |
|------|----------|----------|-------|
| 1    | base/    | 118      | 68    |
| 2    | remote/  | 45       | 23    |
| 3    | scada/   | 43       | 29    |
| 4    | metrics/ | 9        | 7     |
| 5    | model/   | 5        | 2     |
| 6    | net/     | 4        | 3     |

## Most Used Chromium-Base Headers

| Header                                  | Count | Purpose             |
|-----------------------------------------|-------|---------------------|
| `base/strings/utf_string_conversions.h` | 12    | UTF encoding        |
| ~~`base/strings/string_util.h`~~        | 0     | Removed             |
| ~~`base/strings/stringprintf.h`~~       | 0     | Removed             |

## CMake Link Chain

```
ChromiumBase::base (vcpkg)
  └─ scada_base
      ├─ scada_metrics
      ├─ scada_net
      └─ scada_core
          ├─ scada_model
          └─ scada_core_remote
```

All 6 core modules transitively depend on chromium-base through `scada_base`.

## Heaviest Individual Files

| File                         | Includes | Module |
|------------------------------|----------|--------|
| remote_session_manager.cpp   | 5        | remote |
| gtest_main.cpp               | 5        | base   |
| variant.cpp                  | 4        | scada  |
| remote_session_manager.h     | 4        | remote |
| remote_services_unittest.cpp | 4        | remote |
| node_management_stub.cpp     | 4        | remote |
| win_util2.cpp                | 4        | base   |
| format.cpp                   | 4        | base   |
