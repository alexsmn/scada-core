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
  // An unstubbed `Awaitable<T>` return is gmock's `T()`: a null-frame
  // awaitable whose `await_ready()` still answers false, so `co_await` on it
  // segfaults inside the AWAITING coroutine, naming no mock. Every awaitable
  // method therefore completes by default with the emptiest honest answer.
  // See CLAUDE.md, "Unit Test Guidance"; the `*_mock_unittest.cpp` beside
  // this header pins it.
  MockViewService() {
    ON_CALL(*this, Browse)
        .WillByDefault(
            [](scada::ServiceContext, std::vector<BrowseDescription> inputs)
                -> CoStatusOr<std::vector<BrowseResult>> {
              co_return std::vector<BrowseResult>(inputs.size());
            });
    ON_CALL(*this, TranslateBrowsePaths)
        .WillByDefault([](std::vector<BrowsePath> browse_paths)
                           -> CoStatusOr<std::vector<BrowsePathResult>> {
          co_return std::vector<BrowsePathResult>(browse_paths.size());
        });
  }

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
