#pragma once

#include "core/status.h"

#include <ostream>
#include <string>

namespace scada {

class Qualifier {
 public:
  enum Limit {
    LIMIT_NORMAL = 0,
    LIMIT_LO = 1,
    LIMIT_HI = 2,
    LIMIT_LOLO = 3,
    LIMIT_HIHI = 4,
  };

  enum {
    MANUAL = 0x0002,
    DELETED_LOCKED = 0x0004,
    BACKUP = 0x0008,

    SIMULATED = 0x0100,
    SPORADIC = 0x0400,

    BAD = 0x0001,            // Data with invalid flag
    FAILED = 0x0040,         // Can't connect object on server
    MISCONFIGURED = 0x0200,  // Configuration is incorrect or device is disabled
    OFFLINE = 0x0020,        // Device connection is broken
    STALE = 0x0080,          // Device value is too old
    GENERAL_BAD = BAD | FAILED | MISCONFIGURED | OFFLINE | STALE,

    // Flags only for current time.
    REALTIME_FLAGS = FAILED | MISCONFIGURED | OFFLINE | STALE | BACKUP,

    LIMIT_MASK = 0x7000,  // 0x1000 | 0x2000 | 0x4000
    LIMIT_SHIFT = 12,
  };

  constexpr Qualifier() noexcept = default;
  constexpr Qualifier(unsigned qualifier) noexcept : qualifier_{qualifier} {}

  void Update(unsigned remove, unsigned add) {
    qualifier_ = (qualifier_ & ~remove) | add;
  }

  void set_flag(unsigned flag, bool set) {
    if (set)
      qualifier_ |= flag;
    else
      qualifier_ &= ~flag;
  }

  void set_bad(bool set) { set_flag(BAD, set); }
  void set_manual(bool set) { set_flag(MANUAL, set); }
  void set_backup(bool set) { set_flag(BACKUP, set); }
  void set_online(bool set) { set_flag(OFFLINE, !set); }
  void set_failed(bool set) { set_flag(FAILED, set); }
  void set_stale(bool set) { set_flag(STALE, set); }
  void set_simulated(bool set) { set_flag(SIMULATED, set); }
  void set_misconfigured(bool set) { set_flag(MISCONFIGURED, set); }
  void set_sporadic(bool set) { set_flag(SPORADIC, set); }

  void set_limit(Limit limit) {
    qualifier_ =
        (qualifier_ & ~LIMIT_MASK) | ((limit << LIMIT_SHIFT) & LIMIT_MASK);
  }

  constexpr bool flag(unsigned flag) const { return (qualifier_ & flag) != 0; }

  // Return raised bad flags.
  constexpr unsigned general_bad() const { return qualifier_ & GENERAL_BAD; }

  constexpr bool bad() const noexcept { return flag(BAD); }
  constexpr bool good() const noexcept { return !flag(BAD); }
  constexpr bool manual() const noexcept { return flag(MANUAL); }
  constexpr bool backup() const noexcept { return flag(BACKUP); }
  constexpr bool offline() const noexcept { return flag(OFFLINE); }
  constexpr bool online() const noexcept { return !flag(OFFLINE); }
  constexpr bool failed() const noexcept { return flag(FAILED); }
  constexpr bool stale() const noexcept { return flag(STALE); }
  constexpr bool simulated() const noexcept { return flag(SIMULATED); }
  constexpr bool misconfigured() const noexcept { return flag(MISCONFIGURED); }
  constexpr bool sporadic() const noexcept { return flag(SPORADIC); }

  constexpr Limit limit() const noexcept {
    return static_cast<Limit>((qualifier_ & LIMIT_MASK) >> LIMIT_SHIFT);
  }

  constexpr unsigned raw() const noexcept { return qualifier_; }

  constexpr bool operator==(Qualifier other) const noexcept {
    return qualifier_ == other.qualifier_;
  }
  constexpr bool operator!=(Qualifier other) const noexcept {
    return qualifier_ != other.qualifier_;
  }

  Qualifier operator|(Qualifier other) const {
    return Qualifier(qualifier_ | other.qualifier_);
  }
  Qualifier& operator|=(Qualifier other) {
    qualifier_ |= other.qualifier_;
    return *this;
  }

  Status ToStatus() const;

 private:
  unsigned qualifier_ = 0;
};

inline Status Qualifier::ToStatus() const {
  Status status(StatusCode::Good);

  if (bad())
    status = Status(StatusCode::Uncertain_DeviceFlag);
  else if (offline())
    status = Status(StatusCode::Uncertain_Disconnected);
  else if (stale())
    status = Status(StatusCode::Uncertain_NotUpdated);
  else if (misconfigured())
    status = Status(StatusCode::Uncertain_Misconfigured);
  else if (manual())
    status = Status(StatusCode::Good_Manual);
  else if (backup())
    status = Status(StatusCode::Good_Backup);
  else if (simulated())
    status = Status(StatusCode::Good_Simulated);
  else if (sporadic())
    status = Status(StatusCode::Good_Sporadic);

  if (limit() == LIMIT_LOLO)
    status.set_limit(StatusLimit::Low);
  else if (limit() == LIMIT_HIHI)
    status.set_limit(StatusLimit::High);

  return status;
}

}  // namespace scada

std::string ToString(scada::Qualifier qualifier);
std::wstring ToString16(scada::Qualifier qualifier);

namespace scada {

inline std::ostream& operator<<(std::ostream& stream, Qualifier qualifier) {
  return stream << ToString(qualifier);
}

}  // namespace scada
