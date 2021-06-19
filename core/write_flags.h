#pragma once

#include "base/debug_util.h"

#include <ostream>

namespace scada {

class WriteFlags {
 public:
  enum Flag { SELECT = 0x0001, PARAM = 0x0002 };

  WriteFlags() = default;
  explicit WriteFlags(unsigned flags) : flags_{flags} {}

  unsigned raw() const { return flags_; }
  bool get(Flag flag) const { return (flags_ & flag) != 0; }
  bool select() const { return get(SELECT); }
  bool param() const { return get(PARAM); }

  WriteFlags& set(Flag flag) {
    flags_ |= flag;
    return *this;
  }
  WriteFlags& set_select() { return set(SELECT); }
  WriteFlags& set_param() { return set(PARAM); }

  bool operator==(WriteFlags other) const { return flags_ == other.flags_; }

 private:
  unsigned flags_ = 0;
};

inline std::ostream& operator<<(std::ostream& stream, WriteFlags flags) {
  constexpr std::string_view kBitStrings[] = {
      "Select",
      "Parameter",
  };
  return stream << BitMaskToString(flags.raw(), kBitStrings);
}

}  // namespace scada
