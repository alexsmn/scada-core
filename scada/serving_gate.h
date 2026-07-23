#pragma once

#include "scada/status.h"

#include <atomic>
#include <cstdint>
#include <functional>
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

  bool allowed() const { return allowed_.load(std::memory_order_acquire); }
  StatusCode blocked_status() const {
    return blocked_status_.load(std::memory_order_relaxed);
  }

  // Blocks delivery and immediately sweeps every registered subscription with
  // `status` (which must be a Bad code).
  void SetBlocked(StatusCode status) {
    std::vector<Sweeper> sweepers;
    {
      const std::lock_guard<std::mutex> lock{mutex_};
      blocked_status_.store(status, std::memory_order_relaxed);
      allowed_.store(false, std::memory_order_release);
      sweepers.reserve(sweepers_.size());
      for (const auto& [id, sweeper] : sweepers_)
        sweepers.push_back(sweeper);
    }
    // Invoke outside the gate lock: each sweeper takes its own subscription's
    // lock, so calling it under mutex_ would invert the lock order.
    for (const auto& sweeper : sweepers)
      sweeper(status);
  }

  // Resumes normal delivery. The next backend sample restores good quality.
  void SetAllowed() { allowed_.store(true, std::memory_order_release); }

  // RAII registration of a subscription's sweep callback, invoked on each
  // SetBlocked(). Auto-unregisters on destruction. Move-only.
  class Registration {
   public:
    Registration() = default;
    Registration(ServingGate* gate, std::uint64_t id) : gate_{gate}, id_{id} {}
    Registration(Registration&& other) noexcept
        : gate_{std::exchange(other.gate_, nullptr)}, id_{other.id_} {}
    Registration& operator=(Registration&& other) noexcept {
      if (this != &other) {
        Reset();
        gate_ = std::exchange(other.gate_, nullptr);
        id_ = other.id_;
      }
      return *this;
    }
    Registration(const Registration&) = delete;
    Registration& operator=(const Registration&) = delete;
    ~Registration() { Reset(); }

   private:
    void Reset() {
      if (gate_)
        gate_->Unregister(id_);
      gate_ = nullptr;
    }

    ServingGate* gate_ = nullptr;
    std::uint64_t id_ = 0;
  };

  [[nodiscard]] Registration Register(Sweeper sweeper) {
    const std::lock_guard<std::mutex> lock{mutex_};
    const std::uint64_t id = next_id_++;
    sweepers_.emplace(id, std::move(sweeper));
    return Registration{this, id};
  }

 private:
  friend class Registration;

  void Unregister(std::uint64_t id) {
    const std::lock_guard<std::mutex> lock{mutex_};
    sweepers_.erase(id);
  }

  std::atomic<bool> allowed_{true};
  std::atomic<StatusCode> blocked_status_{StatusCode::Bad};
  std::mutex mutex_;
  std::uint64_t next_id_ = 1;
  std::unordered_map<std::uint64_t, Sweeper> sweepers_;
};

}  // namespace scada
