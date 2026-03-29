#include "base/boost_log_init.h"
#include "base/path_service.h"

#include <boost/program_options.hpp>
#include <gmock/gmock.h>

int main(int argc, char** argv) {
  namespace po = boost::program_options;
  po::options_description desc;
  desc.add_options()("log-severity", po::value<std::string>(),
                     "Log severity");
  po::variables_map vm;
  po::store(
      po::command_line_parser(argc, argv).options(desc).allow_unregistered().run(),
      vm);
  po::notify(vm);

  {
    std::filesystem::path path;
    if (!base::PathService::Get(base::DIR_EXE, &path))
      return 2;

    path /= "logs";

    auto log_severity_str = vm.count("log-severity")
                                ? vm["log-severity"].as<std::string>()
                                : std::string{};

    auto log_severity =
        ParseLogSeverity(log_severity_str).value_or(BoostLogSeverity::info);

    InitBoostLogging({.path = path,
                      .console = true,
                      .console_log_severity = log_severity});
  }

  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
