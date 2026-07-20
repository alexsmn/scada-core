#include "metrics/trace_sink_impl.h"

#include <boost/asio/io_context.hpp>
#include <gtest/gtest.h>

#include <chrono>

namespace {

using namespace std::chrono_literals;

// Long enough that a still-armed watchdog cannot expire during the test, so a
// leaked wait shows up as outstanding io_context work rather than as a timeout.
constexpr auto kWatchdogTimeout = 10min;

// How long the drain is allowed to take. The fixed behavior returns
// immediately; the broken one never returns within this bound.
constexpr auto kDrainBudget = 2s;

void StartSpan(TraceSinkImpl& sink, const TraceSpanId& span_id) {
  sink.StartSpan(span_id, "span", /*parent_span_id=*/{},
                 TraceSpanKind::kInternal, /*remote_parent=*/{});
}

// Regression: EndSpan() used to erase the span but leave its watchdog wait
// armed, so every completed span kept outstanding io_context work for the full
// timeout. That stalled ServerProcess's shutdown drain, which runs before
// modules (and therefore the sink) are destroyed.
TEST(TraceSinkImplTest, EndSpanReleasesOutstandingWork) {
  boost::asio::io_context io;
  TraceSinkImpl sink{io.get_executor(), kWatchdogTimeout};

  StartSpan(sink, "span-1");
  sink.EndSpan("span-1");

  io.run_for(kDrainBudget);

  EXPECT_TRUE(io.stopped())
      << "io_context still has work after the span ended - the watchdog wait "
         "was left armed";
}

// The sink's destructor must cancel watchdogs for spans that never ended,
// otherwise a genuinely in-flight span holds the context open for the full
// timeout.
TEST(TraceSinkImplTest, DestructionCancelsWatchdogOfUnendedSpan) {
  boost::asio::io_context io;

  {
    TraceSinkImpl sink{io.get_executor(), kWatchdogTimeout};
    StartSpan(sink, "span-1");
  }

  io.run_for(kDrainBudget);

  EXPECT_TRUE(io.stopped())
      << "io_context still has work after the sink was destroyed";
}

// A span left open while the sink is alive must keep its watchdog armed --
// that pending wait is what lets OnTimeout fire. This pins the boundary so the
// cancellation above is not widened into "never arm the watchdog".
TEST(TraceSinkImplTest, OpenSpanKeepsWatchdogArmed) {
  boost::asio::io_context io;
  TraceSinkImpl sink{io.get_executor(), kWatchdogTimeout};

  StartSpan(sink, "span-1");

  io.run_for(kDrainBudget);

  EXPECT_FALSE(io.stopped())
      << "the watchdog for a still-open span should remain armed";
}

// A zero timeout disables the watchdog entirely, so no wait is ever armed.
TEST(TraceSinkImplTest, DisabledWatchdogArmsNothing) {
  boost::asio::io_context io;
  TraceSinkImpl sink{io.get_executor(), std::chrono::milliseconds::zero()};

  StartSpan(sink, "span-1");

  io.run_for(kDrainBudget);

  EXPECT_TRUE(io.stopped());
}

}  // namespace
