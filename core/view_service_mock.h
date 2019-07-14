#pragma once

#include "core/view_service.h"

#include <gmock/gmock.h>

namespace scada {

class MockViewService : public ViewService {
 public:
  MOCK_METHOD2(Browse,
               void(const std::vector<BrowseDescription>& descriptions,
                    const BrowseCallback& callback));

  MOCK_METHOD2(TranslateBrowsePaths,
               void(const std::vector<BrowsePath>& browse_paths,
                    const TranslateBrowsePathsCallback& callback));
};

}  // namespace scada
