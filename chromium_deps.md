# Chromium-Base Dependencies in Core

## Remaining Chromium Headers

All Chromium-base headers have been removed from core/.

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
| ~~`base/time/time.h`~~ | Re-implemented in `core/base/time/` with identical API; uses NSPR from vcpkg for string parsing |
| ~~`base/containers/contains.h`~~ | Replaced with `.contains()` member (C++20) or `std::ranges::find` |
| ~~`base/containers/cxx20_erase.h`~~ | Replaced with `std::erase` / `std::erase_if` (C++20) |
| ~~`base/memory/scoped_ptr.h`~~ | Replaced with `std::unique_ptr` |
| ~~`base/memory/singleton.h`~~ | Removed (unused include) |
| ~~`base/strings/utf_ostream_operators.h`~~ | Removed direct include; operators still provided by chromebase via ADL |

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
