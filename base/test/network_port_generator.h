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
  int port = 0;
  std::string server_transport_string;
  std::string client_transport_string;
};

inline NetworkTestEnvironment GenerateNetworkTestEnvironment() {
  int port = GenerateTestNetworkPort();
  // std::format("INPROCESS;Passive;Name={}", port)
  // std::format("INPROCESS;Active;Name={}", port)
  return {.port = port,
          .server_transport_string = std::format("TCP;Passive;Port={}", port),
          .client_transport_string = std::format("TCP;Active;Port={}", port)};
};
