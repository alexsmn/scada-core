# Chromium-Base Dependencies in Core

## Remaining Chromium Headers

| Header | Files | Notes |
|--------|-------|-------|
| `base/time/time.h` | 5 | `base::Time` |
| **Total** | **5** | **1 unique header** |

### Files by header

**`base/time/time.h`** (5):
`base/format_time.cpp`, `base/minute_time.h`, `base/time_utils.h`,
`base/test/test_time.h`, `scada/date_time.h`

## Removed Headers

| Header | Removed |
|--------|---------|
| ~~`base/strings/utf_string_conversions.h`~~ | Replaced with `boost::locale::conv::utf_to_utf` |
| ~~`base/strings/string_util_win.h`~~ | Replaced with `boost::locale::conv::utf_to_utf` |
| ~~`base/strings/string_util.h`~~ | Replaced with standard library |
| ~~`base/strings/stringprintf.h`~~ | Replaced with `std::format` |
| ~~`base/containers/span.h`~~ | Replaced with `std::span` |
| ~~`base/strings/strcat.h`~~ | Replaced with `std::format` / `u16format` |
| ~~`base/strings/sys_string_conversions.h`~~ | Replaced with `UtfConvert<>` / `boost::locale::conv` |
| ~~`base/strings/string_number_conversions.h`~~ | Replaced with `std::to_string` / `std::from_chars` / `Parse()` / `WideFormat()` |

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
