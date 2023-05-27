#include "base/boost_log_init.h"
#include "base/file_path_util.h"

#include <base/files/file_path.h>
#include <base/path_service.h>
#include <gmock/gmock.h>

int main(int argc, char** argv) {
  {
    base::FilePath path;
    if (!base::PathService::Get(base::DIR_EXE, &path))
      return 1;
    path = path.AppendASCII("logs");
    InitBoostLogging({.path = AsFilesystemPath(path), .console = true});
  }

  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}