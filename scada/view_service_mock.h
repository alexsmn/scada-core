#pragma once

#include "scada/view_service.h"

#include <gmock/gmock.h>

namespace scada {

class MockViewService : public ViewService {
 public:
  MOCK_METHOD(void,
              Browse,
              (const scada::ServiceContext& context,
               const std::vector<BrowseDescription>& inputs,
               const BrowseCallback& callback),
              (override));

  MOCK_METHOD(void,
              TranslateBrowsePaths,
              (const std::vector<BrowsePath>& browse_paths,
               const TranslateBrowsePathsCallback& callback),
              (override));
};

}  // namespace scada
