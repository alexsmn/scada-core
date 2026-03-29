# Chromium-Base Dependencies

No remaining chromium-base dependencies in core/. The `ChromiumBase::base` link has been
removed from `core/base/CMakeLists.txt`.

All former dependencies have been replaced with:
- `std::filesystem` — file paths, file operations
- `boost::json` — JSON parsing and value storage (replacing `base::Value`)
- `boost::program_options` — command-line parsing (replacing `base::CommandLine`)
- `std::to_chars` — double-to-string formatting (replacing `dmg_fp::g_fmt`)
- Local `base::PathService` — path resolution (replacing ChromiumBase's PathService)
- Local `base::win::ScopedHandle` — Windows handle RAII (replacing ChromiumBase's)
- Local `base/time/time.h` — time classes (API-compatible re-implementation)
