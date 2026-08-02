#include "base/path_service.h"

#include <gtest/gtest.h>

namespace {

enum {
  kTestPathStart = scada::base::PATH_BASE_END + 100,
  kRecursivePathKey,
  kOverridePathKey,
  kTestPathEnd,
};

bool TestPathProvider(int key, std::filesystem::path* result) {
  switch (key) {
    case kRecursivePathKey: {
      std::filesystem::path temp_path;
      if (!scada::base::PathService::Get(scada::base::DIR_TEMP, &temp_path))
        return false;
      *result = temp_path / "recursive-provider";
      return true;
    }

    case kOverridePathKey:
      *result = std::filesystem::path{"provider-value"};
      return true;

    default:
      return false;
  }
}

void EnsureTestProviderRegistered() {
  static const bool registered = [] {
    scada::base::PathService::RegisterProvider(TestPathProvider, kTestPathStart,
                                               kTestPathEnd);
    return true;
  }();
  (void)registered;
}

}  // namespace

TEST(PathServiceTest, ProviderCanResolveBuiltinPathRecursively) {
  EnsureTestProviderRegistered();

  std::filesystem::path path;
  ASSERT_TRUE(scada::base::PathService::Get(kRecursivePathKey, &path));
  EXPECT_EQ(path.filename(), "recursive-provider");
  EXPECT_FALSE(path.empty());
}

TEST(PathServiceTest, OverrideTakesPrecedenceOverProvider) {
  EnsureTestProviderRegistered();

  const auto override_path = std::filesystem::path{"override-value"};
  scada::base::PathService::Override(kOverridePathKey, override_path);

  std::filesystem::path path;
  ASSERT_TRUE(scada::base::PathService::Get(kOverridePathKey, &path));
  EXPECT_EQ(path, override_path);

  scada::base::PathService::Override(kOverridePathKey, {});
}
