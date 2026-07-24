#pragma once

#include "scada/co_result.h"
#include "scada/view_service.h"

#include <gmock/gmock.h>

namespace scada {

template <class T>
CoStatusOr<std::vector<T>> MakeViewResult(StatusOr<std::vector<T>> result) {
  co_return std::move(result);
}

class MockViewService : public ViewService {
 public:
  MOCK_METHOD((CoStatusOr<std::vector<BrowseResult>>),
              Browse,
              (scada::ServiceContext context,
               std::vector<BrowseDescription> inputs),
              (override));

  MOCK_METHOD((CoStatusOr<std::vector<BrowsePathResult>>),
              TranslateBrowsePaths,
              (std::vector<BrowsePath> browse_paths),
              (override));
};

}  // namespace scada
