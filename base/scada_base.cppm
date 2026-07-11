// scada.base — named C++20 module facade over the core/base headers (pilot).
//
// Design (dual-mode): the headers remain the source of truth. This interface
// unit #includes them in the global module fragment and re-exports the public
// names with `export using` (the technique MSVC's std.ixx uses). Every entity
// therefore stays attached to the global module, so a TU that does
// `import scada.base;` and a TU that does `#include "base/check.h"` — or one
// TU doing both — refer to the same entities; hybrid linking is ODR-safe.
//
// Rules for consumers:
//  - `import scada.base;` provides *names* only. Macros cannot be exported:
//    the LOG_* macros additionally require `#include "base/boost_log.h"`, and
//    SCADA_LIFETIME_BOUND requires `#include "base/lifetime.h"`. Both are
//    safe alongside the import.
//  - Global ADL operators (e.g. operator<< overloads) are deliberately not
//    exported; include the owning header where needed.
//
// Rules for maintainers:
//  - Every header whose names are exported below must be #included in the
//    global module fragment BEFORE the export declarations; a using-
//    declaration exports only the overloads visible at that point.
//  - Names with internal linkage (anonymous namespaces in headers) are
//    ill-formed to export. internal/detail namespaces are not exported.
//  - Built only when SCADA_CXX_MODULES=ON (see base/CMakeLists.txt); this
//    file is intentionally outside scada_module()'s *.cpp/*.h source GLOB.

module;

// ---- Global module fragment: headers stay the source of truth ----
#include "base/any_executor.h"
#include "base/any_executor_dispatch.h"
#include "base/any_executor_timer.h"
#include "base/async_cache.h"
#include "base/async_completion.h"
#include "base/at_exit.h"
#include "base/auto_reset.h"
#include "base/awaitable.h"
#include "base/base64.h"
#include "base/binary_reader.h"
#include "base/binary_writer.h"
#include "base/bit_mask_string.h"
#include "base/boost_json_file.h"
#include "base/boost_log.h"
#include "base/boost_log_init.h"
#include "base/callback_awaitable.h"
#include "base/cancelation.h"
#include "base/cell.h"
#include "base/check.h"
#include "base/common_types.h"
#include "base/console_logger.h"
#include "base/constraints.h"
#include "base/container_dump.h"
#include "base/containers/mru_cache.h"
#include "base/csv_reader.h"
#include "base/csv_writer.h"
#include "base/debug_holder.h"
#include "base/debug_util.h"
#include "base/enum_mapping.h"
#include "base/environment.h"
#include "base/fork_logger.h"
#include "base/format.h"
#include "base/format_time.h"
#include "base/interval.h"
#include "base/interval_util.h"
#include "base/logger.h"
#include "base/map_util.h"
#include "base/md5.h"
#include "base/memory_istream.h"  // self-guarded: contents are _WIN32-only
#include "base/minute_time.h"
#include "base/nested_logger.h"
#include "base/no_destructor.h"
#include "base/observer_list.h"
#include "base/panic.h"
#include "base/path_service.h"
#include "base/pickle.h"
#include "base/process/process_metrics.h"
#include "base/range_util.h"
#include "base/rate_limiter.h"
#include "base/stop_token.h"
#include "base/stream_utf.h"
#include "base/string_util.h"
#include "base/struct_format.h"
#include "base/struct_writer.h"
#include "base/thread_executor.h"
#include "base/threading/thread_checker.h"
#include "base/threading/thread_collision_warner.h"
#include "base/time/clock.h"
#include "base/time/default_clock.h"
#include "base/time/time.h"
#include "base/time_utils.h"
#include "base/timed_cache.h"
#include "base/timer.h"
#include "base/u16format.h"
#include "base/uri.h"
#include "base/utf_convert.h"
#include "base/value_util.h"
#ifdef _WIN32
#include "base/shared_event.h"
#include "base/synchronization/waitable_event.h"
#endif
// Not included: lifetime.h (macro-only), boost_log_adapter.h (adapter impl),
// timer/timer.h (forwarding header for base/timer.h), win/*.h (pilot keeps
// platform headers include-based).

export module scada.base;

// ---- namespace base ----
export namespace base {

// check.h / panic.h
using base::Check;
using base::CheckCondition;
using base::NotReached;
using base::Panic;

// at_exit.h / auto_reset.h / no_destructor.h / observer_list.h
using base::AtExitManager;
using base::AutoReset;
using base::NoDestructor;
using base::ObserverList;

// async_cache.h / async_completion.h
using base::AsyncCache;
using base::AsyncCompletion;

// base64.h / md5.h
using base::Base64Decode;
using base::Base64Encode;
using base::MD5String;

// bit_mask_string.h / container_dump.h
using base::AsDict;
using base::AsList;
using base::AsOpt;
using base::BitMaskToString;
// The dump wrappers' operator<< / std::formatter specializations are found by
// ADL / the std::formatter primary template; ToString / ToString16 and the
// transitional global container operator<< overloads (debug_util.h) and the
// wide-string operators (stream_utf.h) are found by ordinary lookup and are
// deliberately not exported — include the owning header where needed.

// containers/mru_cache.h
using base::HashingMRUCache;
using base::MRUCache;
using base::MRUCacheBase;

// environment.h
using base::Environment;

// path_service.h (incl. the unnamed-enum path keys)
using base::DIR_CURRENT;
using base::DIR_EXE;
using base::DIR_TEMP;
using base::FILE_EXE;
using base::PATH_BASE_END;
using base::PathProviderFunc;
using base::PathService;
#ifdef _WIN32
using base::DIR_APP_DATA;
using base::DIR_COMMON_APP_DATA;
using base::DIR_LOCAL_APP_DATA;
using base::DIR_WINDOWS;
#endif

// pickle.h
using base::Pickle;
using base::PickleIterator;

// process/process_metrics.h
using base::ProcessHandle;
using base::ProcessMetrics;
using base::SystemMemoryInfoKB;

// time/clock.h / time/default_clock.h / time/time.h
using base::Clock;
using base::DefaultClock;
using base::Time;
using base::TimeDelta;
using base::TimeTicks;
// Free operators on Time/TimeDelta (member operators travel with the
// classes; these using-declarations pick up the free overload sets).
using base::operator+;
using base::operator*;
using base::operator<<;

// uri.h
using base::UnescapeURLComponent;
namespace UnescapeRule {
using base::UnescapeRule::NORMAL;
using base::UnescapeRule::PATH_SEPARATORS;
using base::UnescapeRule::SPACES;
using base::UnescapeRule::Type;
}  // namespace UnescapeRule

#ifdef _WIN32
// synchronization/waitable_event.h (Windows-only header)
using base::WaitableEvent;
#endif

}  // namespace base

// ---- global namespace ----
export {
  // format.h
  using ::Format;
  using ::FormatHexBuffer;
  using ::Parse;
  using ::ParseT;
  using ::ParseWithDefault;

  // string_util.h
  using ::IEqualsAscii;
  using ::IsAsciiCaseInsensitiveEqual;
  using ::IsStringASCII;
  using ::JoinStrings;
  using ::ReplaceFirstSubstringAfterOffset;
  using ::ReplaceSubstringsAfterOffset;
  using ::SplitString;
  using ::ToLowerAscii;
  using ::TrimAsciiWhitespace;
  using ::WriteInto;

  // utf_convert.h
  using ::UtfConvert;

  // format_time.h (global-namespace declarations)
  using ::FormatTime;
  using ::TIME_FORMAT_DATE;
  using ::TIME_FORMAT_DEFAULT;
  using ::TIME_FORMAT_MSEC;
  using ::TIME_FORMAT_TIME;
  using ::TIME_FORMAT_UTC;
  using ::TimeFormatFlags;

  // minute_time.h
  using ::MinsToTime;
  using ::TimeToMins;

  // uri.h (global-namespace declarations)
  using ::EscapePath;
  using ::EscapeQueryParamValue;

  // any_executor.h / any_executor_dispatch.h / any_executor_timer.h
  using ::AnyExecutor;
  using ::AnyExecutorAdapter;
  using ::AnyExecutorFactory;
  using ::AnyExecutorTimer;
  using ::BindExecutor;
  using ::Dispatch;
  using ::MakeAnyExecutor;
  using ::MakeSingleExecutorFactory;
  using ::PostDelayedTask;
  using ::StartRepeatableTimer;

  // awaitable.h / callback_awaitable.h / cancelation.h
  using ::Awaitable;
  using ::BindCancelation;
  using ::BindCancelationFunc;
  using ::BindStopToken;
  using ::CallbackToAwaitable;
  using ::Cancelation;
  using ::CancelationRef;
  using ::CoSpawn;
  using ::RunAwaitable;

  // binary_reader.h / binary_writer.h
  using ::BinaryReader;
  using ::BinaryWriter;

  // boost_json_file.h / value_util.h
  using ::FindDict;
  using ::GetBool;
  using ::GetDict;
  using ::GetInt;
  using ::GetKey;
  using ::GetList;
  using ::GetString;
  using ::GetString16;
  using ::ReadBoostJsonFromFile;
  using ::WriteBoostJsonToFile;

  // boost_log.h / boost_log_init.h (types and functions; the LOG_* macros
  // require including base/boost_log.h, and the operator<< overloads are
  // deliberately not exported)
  using ::BoostLogFormatter;
  using ::BoostLogger;
  using ::BoostLogParams;
  using ::BoostLogSeverity;
  using ::InitBoostLogging;
  using ::ParseLogSeverity;
  using ::ShutdownBoostLogging;
  using ::ToStringView;

  // logger.h / console_logger.h / fork_logger.h / nested_logger.h
  using ::ConsoleLogger;
  using ::ForkLogger;
  using ::Logger;
  using ::LogSeverity;
  using ::NestedLogger;
  using ::NullLogger;

  // cell.h / constraints.h / debug_holder.h / enum_mapping.h
  using ::Cell;
  using ::DebugHolder;
  using ::MapEnum;
  using ::ScopedInvariant;

  // common_types.h
  using ::Clock;
  using ::Duration;
  using ::TimePoint;

  // csv_reader.h / csv_writer.h
  using ::CsvReader;
  using ::CsvWriter;

  // interval.h / interval_util.h
  using ::AreValidIntervals;
  using ::CastIntervals;
  using ::Interval;
  using ::IntervalContains;
  using ::IntervalIntersection;
  using ::IntervalsContain;
  using ::IntervalsOverlap;
  using ::IntervalUnion;
  using ::IsEmptyInterval;
  using ::IsValidInterval;
  using ::UnionIntervals;

  // map_util.h / range_util.h
  using ::FindOrNull;
  using ::FindPtr;
  using ::GetKeyVector;
  using ::Join;

  // rate_limiter.h / stop_token.h
  using ::RateLimiter;
  using ::scoped_stop_source;

  // struct_format.h / struct_writer.h
  using ::EmptyFormatSpec;
  using ::StructFormatter;
  using ::StructWriter;

  // thread_executor.h / threading/*.h
  using ::ThreadChecker;
  using ::ThreadExecutor;
#ifndef NDEBUG
  // thread_collision_warner.h defines these classes only in non-NDEBUG builds
  // (release builds get no-op macros instead).
  using ::ScopedCollisionWarner;
  using ::ScopedRecursiveCollisionWarner;
  using ::ThreadCollisionWarner;
#endif

  // time_utils.h / timed_cache.h / timer.h
  using ::AsChrono;
  using ::Deserialize;
  using ::InMilliseconds;
  using ::InSeconds;
  using ::IsTimedCacheExpired;
  using ::SerializeToString;
  using ::TimedCache;
  using ::TimeDeltaFromSecondsF;
  using ::Timer;

  // u16format.h
  using ::u16format;

#ifdef _WIN32
  // memory_istream.h / shared_event.h (Windows-only)
  using ::MemoryIStream;
  using ::SharedEvent;
#endif

}  // export
