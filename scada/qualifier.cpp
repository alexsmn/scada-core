#include "scada/qualifier.h"

#if defined(SCADA_USE_BASE_MODULE)
// Modules-pilot consumer (SCADA_CXX_MODULES=ON): base names come from the
// scada.base facade.
import scada.base;
#else
#include "base/ui_text.h"
#endif

namespace {

// The quality flags, in the order they are rendered. One table so the compact
// letter form and the spelled-out form cannot drift apart.
//
// `label` is the operator-facing word in English; `ToString16` runs it through
// `TranslateUiText`, so the Russian abbreviations these replaced now live in
// the client's `client_ru.ts` keyed by these strings. They are deliberately
// short: the rendering is a space-separated run inside a narrow grid cell.
//
// "Bad quality" and "No link" are spelled that way because `Translate()` looks
// up by source text with no disambiguation context, and the catalog already
// maps "Bad" to "Недостоверно" and "Offline" to "Нет связи" — the long forms
// used elsewhere. Reusing those sources would have silently widened this strip.
struct Flag {
  bool (scada::Qualifier::*test)() const noexcept;
  char letter;
  const char* label;
};

const Flag kFlags[] = {
    {&scada::Qualifier::bad, 'B', "Bad quality"},
    {&scada::Qualifier::backup, 'R', "Backup"},
    {&scada::Qualifier::offline, 'O', "No link"},
    {&scada::Qualifier::manual, 'M', "Manual"},
    {&scada::Qualifier::misconfigured, 'C', "Misconfigured"},
    {&scada::Qualifier::simulated, 'E', "Simulated"},
    {&scada::Qualifier::sporadic, 'S', "Sporadic"},
    {&scada::Qualifier::stale, 'T', "Stale"},
    {&scada::Qualifier::failed, 'F', "Failed"},
};

}  // namespace

std::string ToString(scada::Qualifier qualifier) {
  std::string text;
  for (const Flag& flag : kFlags) {
    if ((qualifier.*flag.test)())
      text += flag.letter;
  }
  return text;
}

std::u16string ToString16(scada::Qualifier qualifier) {
  std::u16string text;
  for (const Flag& flag : kFlags) {
    if ((qualifier.*flag.test)()) {
      text += scada::TranslateUiText(flag.label);
      text += u' ';
    }
  }
  return text;
}
