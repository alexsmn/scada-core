#pragma once

#include <filesystem>

struct BoostLogParams {
  std::filesystem::path path;
  size_t rotation_size = 10485760;
  size_t max_size = 104857600;
  size_t max_files = 1000;
  bool console = false;

  BoostLogParams& set_console(bool console) {
    this->console = console;
    return *this;
  }
};

void InitBoostLogging(const BoostLogParams& params);
void ShutdownBoostLogging();
