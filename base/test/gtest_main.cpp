#include "base/boost_log.h"
#include "base/files/file_path.h"
#include "base/path_service.h"

#include <gmock/gmock.h>

int main(int argc, char** argv) {
  {
    base::FilePath path;
    if (!base::PathService::Get(base::DIR_EXE, &path))
      return 1;
    path = path.AppendASCII("logs");
    BoostLogParams params;
    params.path = path.value();
    params.console = true;
    InitBoostLogging(params);
  }

  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}