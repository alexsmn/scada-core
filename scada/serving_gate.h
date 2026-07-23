#pragma once

#include "scada/status.h"

#include <atomic>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <utility>
#include <vector>

namespace scada {

// A shared, reversible gate that monitored-item subscriptions consult so a
// server-wide "serving blocked" condition (e.g. a runtime license expiry)
// reaches already-active subscriptions, not just new requests. While blocked:
//   * each backend data-change / event / status notification is rewritten to
//     `blocked_status()` as it is delivered, so live values do not leak, and
//   * every registered subscription is swept once on the transition into
//     blocked, so an item whose value never changes still reports the status.
// On unblock, the next real sample restores good quality, so no sweep is
// needed.
//
// Framework-agnostic: core carries no license concept. A driver (the license
// module, via RootNodeManager) calls SetBlocked()/SetAllowed(), and is expected
// to call them only on real transitions. Reads (`allowed()`) happen on every
// notification and are lock-free; registration and sweeping are the cold path.
class ServingGate {
 public:
  // Sweeps one subscription: pushes `blocked_status` to each of its active
  // items. Invoked by SetBlocked(), never with the gate lock held.
  using Sweeper = std::function<void(StatusCode blocked_status)>;

  bool allowed() const {
    return state_->allowed.load(std::memory_order_acquire);
  }
  StatusCode blocked_status() const {
    return state_->blocked_status.load(std::memory_order_relaxed);
  }

  // Blocks delivery and immediately sweeps every registered subscription with
  // `status` (which must be a Bad code).
  void SetBlocked(StatusCode status) {
    std::vector<Sweeper> sweepers;
    {
      const std::lock_guard<std::mutex> lock{state_->mutex};
      state_->blocked_status.store(status, std::memory_order_relaxed);
      state_->allowed.store(false, std::memory_order_release);
      sweepers.reserve(state_->sweepers.size());
      for (const auto& [id, sweeper] : state_->sweepers)
        sweepers.push_back(sweeper);
    }
    // Invoke outside the gate lock: each sweeper takes its own subscription's
    // lock, so calling it under the state lock would invert the lock order.
    for (const auto& sweeper : sweepers)
      sweeper(status);
  }

  // Resumes normal delivery. The next backend sample restores good quality.
  void SetAllowed() {
    state_->allowed.store(true, std::memory_order_release);
  }

 private:
  // Shared control block holding the gate's observable state (the block/allow
  // atomics) and the sweeper registry. The gate owns the sole strong reference;
  // holders (a View, a Registration) keep only what they need. Because the block
  // is refcounted it can outlive the ServingGate, so a client subscription's
  // `State` torn down after the RootNodeManager that owns the gate can still
  // read/unregister safely instead of touching a destroyed mutex or atomics.
  // See serving_gate lifetime notes / the CreateAggregateMonitoredItem teardown
  // regression.
  struct State {
    std::atomic<bool> allowed{true};
    std::atomic<StatusCode> blocked_status{StatusCode::Bad};
    std::mutex mutex;
    std::uint64_t next_id = 1;
    std::unordered_map<std::uint64_t, Sweeper> sweepers;
  };

 public:
  // Lock-free, lifetime-safe read handle a subscription captures instead of a
  // raw ServingGate*. Because it holds a strong reference to the shared state,
  // reads stay valid even after the originating gate is destroyed — they then
  // observe the last published state, which is correct since no further
  // transitions can occur once the license bridge is gone. A default-constructed
  // View means "no gate": always allowed. Copyable and cheap to pass by value.
  class View {
   public:
    View() = default;

    bool allowed() const {
      return !state_ || state_->allowed.load(std::memory_order_acquire);
    }
    StatusCode blocked_status() const {
      return state_ ? state_->blocked_status.load(std::memory_order_relaxed)
                    : StatusCode::Good;
    }

   private:
    friend class ServingGate;
    explicit View(std::shared_ptr<const State> state)
        : state_{std::move(state)} {}

    std::shared_ptr<const State> state_;
  };

  // Returns a read handle for the block/allow state. Capture this instead of a
  // raw ServingGate* when the reader may outlive the gate.
  [[nodiscard]] View view() const { return View{state_}; }

  // RAII registration of a subscription's sweep callback, invoked on each
  // SetBlocked(). Auto-unregisters on destruction. Move-only. Safe to destroy
  // after the originating ServingGate is gone (the unregister becomes a no-op).
  class Registration {
   public:
    Registration() = default;
    Registration(std::weak_ptr<State> state, std::uint64_t id)
        : state_{std::move(state)}, id_{id} {}
    // Moving a weak_ptr leaves the source empty, so the moved-from Registration
    // unregisters nothing.
    Registration(Registration&& other) noexcept = default;
    Registration& operator=(Registration&& other) noexcept {
      if (this != &other) {
        Reset();
        state_ = std::move(other.state_);
        id_ = other.id_;
      }
      return *this;
    }
    Registration(const Registration&) = delete;
    Registration& operator=(const Registration&) = delete;
    ~Registration() { Reset(); }

   private:
    void Reset() {
      // A late unregister after the gate (and thus the shared state) is
      // destroyed is a safe no-op: the weak_ptr fails to lock, so we never touch
      // a destroyed mutex.
      if (auto state = state_.lock()) {
        const std::lock_guard<std::mutex> lock{state->mutex};
        state->sweepers.erase(id_);
      }
      state_.reset();
    }

    std::weak_ptr<State> state_;
    std::uint64_t id_ = 0;
  };

  [[nodiscard]] Registration Register(Sweeper sweeper) {
    const std::lock_guard<std::mutex> lock{state_->mutex};
    const std::uint64_t id = state_->next_id++;
    state_->sweepers.emplace(id, std::move(sweeper));
    return Registration{state_, id};
  }

 private:
  const std::shared_ptr<State> state_ = std::make_shared<State>();
};

}  // namespace scada
