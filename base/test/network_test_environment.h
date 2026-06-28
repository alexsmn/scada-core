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
  // TODO: Use `transport::TransportString` instead of `std::string`.
  // Bind/connect on the explicit IPv4 loopback. Without a host the active
  // connect resolves an empty host string, which some platforms (macOS) fail to
  // resolve to localhost — so the device never connects. Pinning both ends to
  // 127.0.0.1 keeps the loopback address family matched.
  const std::string server_transport_string =
      std::format("TCP;Passive;Host=127.0.0.1;Port={}", port);
  const std::string client_transport_string =
      std::format("TCP;Active;Host=127.0.0.1;Port={}", port);
};
