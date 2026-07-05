// Hybrid-mode test for the scada.base module facade: this TU both imports
// the module AND #includes the same headers, proving the facade's entities
// stay attached to the global module (ODR-safe mixing of import and include,
// the invariant the dual-mode design rests on). It also calls out-of-line
// functions (Format, SplitString) so the linker resolves them across the
// module/header boundary.

#include "base/check.h"
#include "base/format.h"
#include "base/string_util.h"

#include <gtest/gtest.h>

// Note: headers before import. The reverse order trips a declaration-merging
// bug in AppleClang 21 (libc++'s dependent alias template __promote_t:
// "type alias template redefinition with different types") when the same
// libc++ declarations arrive first from the module's GMF and then textually.
import scada.base;

namespace scada_base_module {
namespace {

TEST(ScadaBaseModuleMixed, ImportAndIncludeCoexist) {
  // Callable via the imported name and the included declaration alike;
  // both must resolve to the same entity.
  base::Check(true, "mixed mode");
  EXPECT_EQ(Format(123456), "123456");
}

TEST(ScadaBaseModuleMixed, OutOfLineEntitiesLink) {
  std::vector<std::string_view> parts = SplitString("x;y;z", ';');
  ASSERT_EQ(parts.size(), 3u);
  EXPECT_EQ(parts[2], "z");
  EXPECT_EQ(JoinStrings(parts, "-"), "x-y-z");
}

}  // namespace
}  // namespace scada_base_module
