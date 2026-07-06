// Hybrid-mode test for the scada.core module facade: this TU both imports
// the module AND #includes the same headers (textual includes first),
// proving ODR-safe coexistence and that header-only macros keep working
// alongside the import.

#include "scada/localized_text.h"
#include "scada/node_id.h"
#include "scada/status.h"
#include "scada/variant.h"

#include <string_view>

#include <gtest/gtest.h>

// Import after the textual includes (AppleClang 21 libc++ merging bug).
import scada.core;

namespace scada_core_module {
namespace {

TEST(ScadaCoreModuleMixed, ImportAndIncludeCoexist) {
  // Entities defined out-of-line in scada_core, reached via both paths.
  scada::NodeId parsed{scada::String{"abc"}, 3};
  EXPECT_EQ(parsed.namespace_index(), 3);
  EXPECT_FALSE(ToString(scada::StatusCode::Bad).empty());
}

TEST(ScadaCoreModuleMixed, MacroWorksAlongsideImport) {
  // LOCALIZED_TEXT comes from the textual include; macros are never
  // exported by the module.
  std::wstring_view text = LOCALIZED_TEXT("smoke");
  EXPECT_EQ(text.size(), 5u);
}

}  // namespace
}  // namespace scada_core_module
