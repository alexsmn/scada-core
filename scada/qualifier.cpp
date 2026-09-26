#include "scada/qualifier.h"

#include <string_view>

namespace scada {
namespace {

// Set once at startup by the UI layer and read thereafter; a function-local
// static keeps it out of static-global-init ordering.
QualifierFlagTextProvider& GetQualifierFlagTextProvider() {
  static QualifierFlagTextProvider provider = nullptr;
  return provider;
}

}  // namespace

void SetQualifierFlagTextProvider(QualifierFlagTextProvider provider) {
  GetQualifierFlagTextProvider() = provider;
}

}  // namespace scada

namespace {

// The quality flags, in the order they are rendered. One table so the compact
// letter form and the spelled-out form cannot drift apart.
//
// Core carries no operator-facing word for a flag: `name` is the enum
// spelling, rendered only when no UI has installed a
// `scada::QualifierFlagTextProvider` — the server, and unit tests. The Qt
// client's words are in `client/services/core_ui_text.cpp`.
struct Flag {
  unsigned bit;
  char letter;
  const char* name;
};

const Flag kFlags[] = {
    {scada::Qualifier::BAD, 'B', "BAD"},
    {scada::Qualifier::BACKUP, 'R', "BACKUP"},
    {scada::Qualifier::OFFLINE, 'O', "OFFLINE"},
    {scada::Qualifier::MANUAL, 'M', "MANUAL"},
    {scada::Qualifier::MISCONFIGURED, 'C', "MISCONFIGURED"},
    {scada::Qualifier::SIMULATED, 'E', "SIMULATED"},
    {scada::Qualifier::SPORADIC, 'S', "SPORADIC"},
    {scada::Qualifier::STALE, 'T', "STALE"},
    {scada::Qualifier::FAILED, 'F', "FAILED"},
};

std::u16string FlagText(const Flag& flag) {
  if (const scada::QualifierFlagTextProvider provider =
          scada::GetQualifierFlagTextProvider()) {
    return provider(flag.bit);
  }
  // Always ASCII, so widening is lossless.
  const std::string_view name = flag.name;
  return std::u16string(name.begin(), name.end());
}

}  // namespace

std::string ToString(scada::Qualifier qualifier) {
  std::string text;
  for (const Flag& flag : kFlags) {
    if (qualifier.flag(flag.bit))
      text += flag.letter;
  }
  return text;
}

std::u16string ToString16(scada::Qualifier qualifier) {
  std::u16string text;
  for (const Flag& flag : kFlags) {
    if (qualifier.flag(flag.bit)) {
      text += FlagText(flag);
      text += u' ';
    }
  }
  return text;
}
