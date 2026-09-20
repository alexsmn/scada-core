#pragma once

// Service-boundary wrappers that resolve outbound LocalizedText against the
// calling session's locale preferences. This is the server half of OPC UA
// locale negotiation (Part 4 §5.4,
// https://reference.opcfoundation.org/Core/Part4/v105/docs/5.4): the address
// space stores every translation it has — as a packed "mul" LocalizedText
// (Part 3 §8.5.2.2) where there is more than one — and the value a particular
// client sees is chosen here, from `ServiceContext::locale_ids()`.
//
// It is a boundary wrapper rather than a change to each node manager on
// purpose. Display names are produced in many places (the configuration
// database, the static address space, aggregate declarations, a remote tier's
// cache), every one of them caches across sessions, and a per-session answer
// cannot be cached. Resolving once, on the way out to a client, keeps the
// stored form locale-complete and makes a node manager added later
// locale-correct without knowing this exists.
//
// Scope: every LocalizedText a Read returns, scalar or array — which covers
// DisplayName and Description without naming them, so a LocalizedText-valued
// Variable attribute is resolved too — and the display name on every Browse
// reference description. A value of another type passes through untouched,
// and so does a LocalizedText that is not packed: resolving a plain value is
// a no-op by Part 4 §5.4's "return an available locale" rule, which is what
// makes it safe to run over everything rather than over a list of attribute
// ids somebody has to keep current.

#include "scada/attribute_service.h"
#include "scada/co_result.h"
#include "scada/monitored_item_service.h"
#include "scada/view_service.h"

#include <any>
#include <memory>
#include <span>

namespace scada {

// Resolves the Message of an event carried in a `std::any` against a
// session's locale preferences, returning it unchanged when it carries none.
//
// An event Message is a LocalizedText and is explicitly localizable (OPC UA
// Part 5 §6.4.2 BaseEventType,
// https://reference.opcfoundation.org/Core/Part5/v105/docs/6.4.2), so it is
// resolved per session like any other — but it travels inside a `std::any`,
// which is why this exists rather than the value simply passing through a
// field. Only `Event` and `DeviceFrameEvent` carry a message; the model- and
// semantic-change events have none and come back untouched.
std::any ResolveEventMessage(std::any event, std::span<const String> requested);

// Wraps a ViewService, resolving the display name of every reference a Browse
// returns.
class LocalizingViewService : public scada::ViewService {
 public:
  explicit LocalizingViewService(scada::ViewService& inner) : inner_{inner} {}

  scada::CoStatusOr<std::vector<scada::BrowseResult>> Browse(
      scada::ServiceContext context,
      std::vector<scada::BrowseDescription> inputs) override;

  scada::CoStatusOr<std::vector<scada::BrowsePathResult>> TranslateBrowsePaths(
      std::vector<scada::BrowsePath> inputs) override;

 private:
  scada::ViewService& inner_;
};

// Wraps an AttributeService, resolving LocalizedText values a Read returns.
// Write is forwarded unchanged: Part 4 §5.4 forbids the special locales in
// Write, and a client writing a plain value is writing exactly what it said.
class LocalizingAttributeService : public scada::AttributeService {
 public:
  explicit LocalizingAttributeService(scada::AttributeService& inner)
      : inner_{inner} {}

  scada::CoStatusOr<std::vector<scada::DataValue>> Read(
      scada::ServiceContext context,
      std::vector<scada::ReadValueId> inputs) override;

  scada::CoStatusOr<std::vector<scada::StatusCode>> Write(
      scada::ServiceContext context,
      std::vector<scada::WriteValue> inputs) override;

 private:
  scada::AttributeService& inner_;
};

// Wraps a MonitoredItemService so every event notification a subscription
// delivers carries its Message in the subscribing session's language.
//
// The locales are captured when the subscription is created, because
// `ReadNext` has no ServiceContext to carry them — a subscription belongs to
// one session and its language is a property of that session. A client that
// re-activates with a different language (Part 4 §5.7.3.2) therefore keeps
// the old language on subscriptions it already holds, and gets the new one on
// subscriptions it creates afterwards; a client that wants its journal to
// follow a live language change re-subscribes, which the web client does.
class LocalizingMonitoredItemService : public scada::MonitoredItemService {
 public:
  explicit LocalizingMonitoredItemService(scada::MonitoredItemService& inner)
      : inner_{inner} {}

  StatusOr<std::unique_ptr<MonitoredItemSubscription>> CreateSubscription(
      ServiceContext context,
      MonitoredItemSubscriptionOptions options) override;

 private:
  scada::MonitoredItemService& inner_;
};

}  // namespace scada
