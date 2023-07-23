#pragma once

#include "scada/view_service.h"

#include <gmock/gmock.h>

namespace scada {

class MockViewService : public ViewService {
 public:
  MOCK_METHOD(void,
              Browse,
              (const std::vector<BrowseDescription>& descriptions,
               const BrowseCallback& callback),
              (override));

  MOCK_METHOD(void,
              TranslateBrowsePaths,
              (const std::vector<BrowsePath>& browse_paths,
               const TranslateBrowsePathsCallback& callback),
              (override));
};

}  // namespace scada
