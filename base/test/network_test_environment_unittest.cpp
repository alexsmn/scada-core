#include "base/test/network_test_environment.h"

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <gtest/gtest.h>

#include <set>
#include <string>
#include <vector>

namespace {

// Binds an acceptor to `port` on the loopback the fixtures use, reporting
// whether the caller could have taken the port for itself.
bool CanBind(int port) {
  boost::asio::io_context io_context;
  boost::asio::ip::tcp::acceptor acceptor{io_context};
  boost::system::error_code error;
  acceptor.open(boost::asio::ip::tcp::v4(), error);
  if (error) {
    return false;
  }
  acceptor.bind({boost::asio::ip::address_v4::loopback(),
                 static_cast<unsigned short>(port)},
                error);
  return !error;
}

TEST(GenerateTestNetworkPortTest, ReturnsDistinctPorts) {
  std::set<int> ports;
  for (int i = 0; i < 32; ++i) {
    EXPECT_TRUE(ports.insert(GenerateTestNetworkPort()).second);
  }
}

TEST(GenerateTestNetworkPortTest, ReturnsAPortTheCallerCanBind) {
  for (int i = 0; i < 8; ++i) {
    EXPECT_TRUE(CanBind(GenerateTestNetworkPort()));
  }
}

// The defect this helper was rewritten for: ports used to be drawn at random
// from a fixed range and deduped only within the drawing process, so a port
// another test binary was already listening on could still be handed out.
// Holding ports open here stands in for that other binary.
TEST(GenerateTestNetworkPortTest, NeverReturnsAPortThatIsAlreadyBound) {
  boost::asio::io_context io_context;
  std::vector<boost::asio::ip::tcp::acceptor> occupied;
  std::set<int> occupied_ports;
  for (int i = 0; i < 16; ++i) {
    boost::asio::ip::tcp::acceptor acceptor{io_context};
    acceptor.open(boost::asio::ip::tcp::v4());
    acceptor.bind({boost::asio::ip::address_v4::loopback(), 0});
    acceptor.listen();
    occupied_ports.insert(acceptor.local_endpoint().port());
    occupied.push_back(std::move(acceptor));
  }

  for (int i = 0; i < 32; ++i) {
    EXPECT_FALSE(occupied_ports.contains(GenerateTestNetworkPort()));
  }
}

TEST(NetworkTestEnvironmentTest, TransportStringsAgreeOnOnePort) {
  const NetworkTestEnvironment environment;
  const std::string port = std::to_string(environment.port);

  EXPECT_EQ(environment.server_transport_string,
            "TCP;Passive;Host=127.0.0.1;Port=" + port);
  EXPECT_EQ(environment.client_transport_string,
            "TCP;Active;Host=127.0.0.1;Port=" + port);
}

TEST(NetworkTestEnvironmentTest, TwoEnvironmentsDoNotShareAPort) {
  const NetworkTestEnvironment first;
  const NetworkTestEnvironment second;

  EXPECT_NE(first.port, second.port);
}

}  // namespace
