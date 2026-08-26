#pragma once

#include "base/no_destructor.h"

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <format>
#include <mutex>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

// Returns a TCP port that the operating system has just confirmed to be free,
// and that this process has not returned before.
//
// The port is chosen by binding an acceptor to 127.0.0.1:0, reading back the
// port the OS assigned, and closing it. Asking the OS is what makes the result
// unique across *processes* -- a `ctest -j` run, or two checkouts testing at
// once, both of which this tree does routinely.
//
// The previous implementation drew at random from 30000-40000 and deduped
// through a process-local set, so two concurrent test binaries could draw the
// same port; the loser's bind failed, and a bind failure inside a device or
// protocol fixture reads as a protocol bug rather than as a port clash.
// Seeding the generator per process would not have helped: `std::random_device`
// already differs per process, so the collision was a birthday problem over a
// 10000-port range rather than a shared-sequence problem.
//
// Residual race, deliberately accepted: between the probe closing and the
// caller binding, another process can still take the port. Holding the socket
// open instead would mean handing the caller an acceptor rather than a port,
// which every fixture here is built to build for itself.
inline int GenerateTestNetworkPort() {
  static scada::base::NoDestructor<std::mutex> mutex;
  static scada::base::NoDestructor<std::unordered_set<int>> seen;

  std::lock_guard lock{*mutex};

  // Probes are held open until an unseen port turns up, so the OS cannot offer
  // the same just-released port twice within this loop.
  boost::asio::io_context io_context;
  std::vector<boost::asio::ip::tcp::acceptor> probes;
  for (;;) {
    boost::asio::ip::tcp::acceptor acceptor{io_context};
    acceptor.open(boost::asio::ip::tcp::v4());
    acceptor.bind({boost::asio::ip::address_v4::loopback(), 0});
    const int port = acceptor.local_endpoint().port();
    probes.push_back(std::move(acceptor));
    // Every probe, this one included, is released as `probes` goes out of
    // scope, so the returned port is free for the caller to bind.
    if (seen->emplace(port).second) {
      return port;
    }
  }
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
