#pragma once

// ADL-safe rendering of standard containers, maps, and std::optional for
// logging and debugging.
//
// Wrapping a value in base::AsList / base::AsDict / base::AsOpt yields a
// lightweight, non-owning wrapper *owned by namespace base*. Because the
// wrapper's associated namespace is `base`, both `stream << base::AsList(v)`
// and `std::format("{}", base::AsList(v))` are found by ADL — with no global
// operator<< overloads on std:: types (which are found only by ordinary
// unqualified lookup, are fragile across static libraries, and never reached
// boost::log::formatting_ostream). The stream operator is templated on the
// stream type and delegates to std::format, so one definition serves both
// std::ostream and boost::log::formatting_ostream.
//
// Elements must themselves be std::formattable — this is the bottom-up
// enablement the migration away from operator<< requires (see struct_format.h).
//
//   LOG_INFO(logger) << "ids=" << base::AsList(node_ids);
//   std::string s = ToString(base::AsDict(name_to_value));

#include "base/lifetime.h"

#include <concepts>
#include <format>
#include <optional>
#include <string>

namespace base {

// Common base so a single constrained operator<< serves every wrapper.
struct DumpTag {};

// Renders a range as `[a, b, c]`.
template <class R>
struct ListDump : DumpTag {
  const R& range;
};

// Renders an associative range as `{k: v, k2: v2}`.
template <class M>
struct DictDump : DumpTag {
  const M& map;
};

// Renders an optional as its value or `nullopt`.
template <class T>
struct OptDump : DumpTag {
  const std::optional<T>& opt;
};

// Factories. The wrapper borrows its argument for the duration of the enclosing
// full-expression; SCADA_LIFETIME_BOUND flags callers that outlive it.
template <class R>
[[nodiscard]] ListDump<R> AsList(const R& range SCADA_LIFETIME_BOUND) {
  return {{}, range};
}

template <class M>
[[nodiscard]] DictDump<M> AsDict(const M& map SCADA_LIFETIME_BOUND) {
  return {{}, map};
}

template <class T>
[[nodiscard]] OptDump<T> AsOpt(const std::optional<T>& opt SCADA_LIFETIME_BOUND) {
  return {{}, opt};
}

namespace internal {

// Shared std::formatter::parse for the dump wrappers: accept only the empty
// "{}" spec, reject any non-empty spec with std::format_error.
struct DumpParse {
  constexpr std::format_parse_context::iterator parse(
      std::format_parse_context& ctx) const {
    const std::format_parse_context::iterator it = ctx.begin();
    if (it != ctx.end() && *it != '}')
      throw std::format_error{"dump wrapper takes no format spec"};
    return it;
  }
};

}  // namespace internal

// Stream operator for any dump wrapper, delegating to its std::formatter.
// Templated on the stream type (constrained to accept a std::string) so it
// serves std::ostream and boost::log::formatting_ostream via ADL. The return
// type is the stream expression's own type — for a derived stream such as
// std::ostringstream that is std::ostream&, so binding it back to StreamT&
// would fail; deduce from `stream << std::string{}` directly.
template <class StreamT, class W>
  requires std::derived_from<W, DumpTag>
auto operator<<(StreamT& stream, const W& wrapper)
    -> decltype(stream << std::string{}) {
  return stream << std::format("{}", wrapper);
}

}  // namespace base

template <class R>
struct std::formatter<base::ListDump<R>> : base::internal::DumpParse {
  template <class FormatContext>
  auto format(const base::ListDump<R>& w, FormatContext& ctx) const {
    typename FormatContext::iterator out = ctx.out();
    *out++ = '[';
    bool first = true;
    for (const auto& element : w.range) {
      if (!first)
        out = std::format_to(out, ", ");
      out = std::format_to(out, "{}", element);
      first = false;
    }
    *out++ = ']';
    return out;
  }
};

template <class M>
struct std::formatter<base::DictDump<M>> : base::internal::DumpParse {
  template <class FormatContext>
  auto format(const base::DictDump<M>& w, FormatContext& ctx) const {
    typename FormatContext::iterator out = ctx.out();
    *out++ = '{';
    bool first = true;
    for (const auto& [key, value] : w.map) {
      if (!first)
        out = std::format_to(out, ", ");
      out = std::format_to(out, "{}: {}", key, value);
      first = false;
    }
    *out++ = '}';
    return out;
  }
};

template <class T>
struct std::formatter<base::OptDump<T>> : base::internal::DumpParse {
  template <class FormatContext>
  auto format(const base::OptDump<T>& w, FormatContext& ctx) const {
    if (w.opt.has_value())
      return std::format_to(ctx.out(), "{}", *w.opt);
    return std::format_to(ctx.out(), "nullopt");
  }
};
