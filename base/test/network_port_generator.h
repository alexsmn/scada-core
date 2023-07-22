#pragma once

#include <format>
#include <random>
#include <unordered_set>

inline int GenerateTestNetworkPort() {
  static std::mt19937 gen(std::random_device{}());
  static std::uniform_int_distribution<> distrib{30000, 40000};
  static std::unordered_set<int> seen;
  int port = distrib(gen);
  while (!seen.emplace(port).second) {
    port = distrib(gen);
  }
  return port;
}

struct NetworkTestEnvironment {
  const int port = GenerateTestNetworkPort();
  const std::string server_transport_string =
      std::format("TCP;Passive;Port={}", port);
  const std::string client_transport_string =
      std::format("TCP;Active;Port={}", port);
};
