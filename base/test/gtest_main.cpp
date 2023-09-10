#include "base/boost_log_init.h"
#include "base/command_line.h"
#include "base/file_path_util.h"

#include <base/files/file_path.h>
#include <base/path_service.h>
#include <gmock/gmock.h>

int main(int argc, char** argv) {
  if (!base::CommandLine::Init(argc, argv)) {
    return 1;
  }

  {
    base::FilePath path;
    if (!base::PathService::Get(base::DIR_EXE, &path))
      return 2;

    path = path.AppendASCII("logs");

    auto log_severity =
        ParseLogSeverity(
            base::CommandLine::ForCurrentProcess()->GetSwitchValueASCII(
                "log-severity"))
            .value_or(BoostLogSeverity::info);

    InitBoostLogging({.path = AsFilesystemPath(path),
                      .console = true,
                      .console_log_severity = log_severity});
  }

  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}