# Chromium-Base Dependencies in Core

## Remaining Chromium Headers

| Header | Files | Notes |
|--------|-------|-------|
| `base/strings/sys_string_conversions.h` | 5 | `SysNativeMBToWide`/`SysWideToNativeMB` |
| `base/strings/string_number_conversions.h` | 4 | `StringToInt`, `NumberToString`, etc. |
| `base/time/time.h` | 1 | `base::Time` |
| **Total** | **10** | **3 unique headers** |

### Files by header

**`base/strings/sys_string_conversions.h`** (5):
`base/win/format_hresult.cpp`, `remote/node_management_proxy.cpp`,
`remote/node_management_stub.cpp`, `remote/session_proxy.cpp`,
`scada/localized_text.cpp`

**`base/strings/string_number_conversions.h`** (4):
`model/namespaces.cpp`, `model/node_id_util.cpp`,
`scada/attribute_ids.cpp`, `scada/node_id.cpp`

**`base/time/time.h`** (1): `scada/date_time.h`

## Removed Headers

| Header | Removed |
|--------|---------|
| ~~`base/strings/utf_string_conversions.h`~~ | Replaced with `boost::locale::conv::utf_to_utf` |
| ~~`base/strings/string_util_win.h`~~ | Replaced with `boost::locale::conv::utf_to_utf` |
| ~~`base/strings/string_util.h`~~ | Replaced with standard library |
| ~~`base/strings/stringprintf.h`~~ | Replaced with `std::format` |
| ~~`base/containers/span.h`~~ | Replaced with `std::span` |
| ~~`base/strings/strcat.h`~~ | Replaced with `std::format` / `u16format` |

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
