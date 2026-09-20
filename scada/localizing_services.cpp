#include "scada/localizing_services.h"

#include "scada/event.h"
#include "scada/locale_negotiation.h"
#include "scada/service_context.h"
#include "scada/variant.h"

#include <any>
#include <memory>
#include <span>
#include <utility>
#include <variant>
#include <vector>

namespace scada {

namespace {

// Resolves in place every LocalizedText a value carries — the scalar case and
// the array case, which a Read of a LocalizedText-valued array attribute
// produces. Anything else is left alone.
void ResolveValue(Variant& value, std::span<const String> locale_ids) {
  if (auto* text = value.get_if<LocalizedText>()) {
    *text = ResolveLocalizedText(*text, locale_ids);
    return;
  }
  if (auto* texts = value.get_if<std::vector<LocalizedText>>()) {
    for (LocalizedText& text : *texts)
      text = ResolveLocalizedText(text, locale_ids);
  }
}

// The base event carrying the Message, for each concrete type that has one.
scada::Event& EventBase(scada::Event& event) {
  return event;
}
scada::Event& EventBase(scada::DeviceFrameEvent& event) {
  return event.base;
}

// Resolves the message of one concrete event type in place.
template <class EventT>
bool ResolveIn(std::any& event, std::span<const String> requested) {
  auto* typed = std::any_cast<EventT>(&event);
  if (!typed)
    return false;
  scada::Event& base = EventBase(*typed);
  base.message = ResolveLocalizedText(base.message, requested);
  return true;
}

// A subscription whose event notifications are resolved into one session's
// language on the way out. Everything else is forwarded untouched.
class LocalizingSubscription final : public scada::MonitoredItemSubscription {
 public:
  LocalizingSubscription(std::unique_ptr<MonitoredItemSubscription> inner,
                         std::vector<String> locale_ids)
      : inner_{std::move(inner)}, locale_ids_{std::move(locale_ids)} {}

  Awaitable<std::vector<MonitoredItemCreateResult>> AddItems(
      std::vector<MonitoredItemCreateRequest> requests) override {
    return inner_->AddItems(std::move(requests));
  }

  Awaitable<std::vector<Status>> RemoveItems(
      std::span<const MonitoredItemId> item_ids) override {
    return inner_->RemoveItems(item_ids);
  }

  CoStatusOr<std::vector<MonitoredItemNotification>> ReadNext(
      std::size_t max_count) override {
    auto notifications = co_await inner_->ReadNext(max_count);
    if (!notifications.ok())
      co_return notifications;
    for (MonitoredItemNotification& notification : *notifications) {
      if (auto* event = std::get_if<EventNotification>(&notification))
        event->event = ResolveEventMessage(std::move(event->event), locale_ids_);
    }
    co_return notifications;
  }

  void Close(Status status) override { inner_->Close(status); }

 private:
  const std::unique_ptr<MonitoredItemSubscription> inner_;
  // Captured at subscription time; see the class comment in the header.
  const std::vector<String> locale_ids_;
};

}  // namespace

std::any ResolveEventMessage(std::any event,
                             std::span<const String> requested) {
  if (!event.has_value())
    return event;
  if (ResolveIn<scada::Event>(event, requested) ||
      ResolveIn<scada::DeviceFrameEvent>(event, requested)) {
    return event;
  }
  // ModelChangeEvent and SemanticChangeEvent carry no message, and an event
  // type this does not know is returned untouched rather than rejected — a
  // message nobody resolved is a worse outcome than one nobody could.
  return event;
}

StatusOr<std::unique_ptr<MonitoredItemSubscription>>
LocalizingMonitoredItemService::CreateSubscription(
    ServiceContext context,
    MonitoredItemSubscriptionOptions options) {
  std::vector<String> locale_ids = context.locale_ids();
  auto subscription = inner_.CreateSubscription(context, std::move(options));
  if (!subscription.ok())
    return subscription;
  // Nothing to resolve against: skip the wrapper entirely so a server with no
  // locale-aware clients pays nothing for this.
  if (locale_ids.empty())
    return subscription;
  return std::unique_ptr<MonitoredItemSubscription>{
      std::make_unique<LocalizingSubscription>(std::move(*subscription),
                                               std::move(locale_ids))};
}

scada::CoStatusOr<std::vector<scada::BrowseResult>>
LocalizingViewService::Browse(scada::ServiceContext context,
                              std::vector<scada::BrowseDescription> inputs) {
  auto results = co_await inner_.Browse(context, std::move(inputs));
  if (!results.ok())
    co_return results;
  const std::span<const String> locale_ids{context.locale_ids()};
  for (scada::BrowseResult& result : *results) {
    for (scada::ReferenceDescription& reference : result.references) {
      reference.display_name =
          ResolveLocalizedText(reference.display_name, locale_ids);
    }
  }
  co_return results;
}

scada::CoStatusOr<std::vector<scada::BrowsePathResult>>
LocalizingViewService::TranslateBrowsePaths(
    std::vector<scada::BrowsePath> inputs) {
  // BrowsePathResult carries NodeIds and no text, so there is nothing to
  // resolve — and the service signature carries no ServiceContext to resolve
  // against.
  return inner_.TranslateBrowsePaths(std::move(inputs));
}

scada::CoStatusOr<std::vector<scada::DataValue>>
LocalizingAttributeService::Read(scada::ServiceContext context,
                                 std::vector<scada::ReadValueId> inputs) {
  auto results = co_await inner_.Read(context, std::move(inputs));
  if (!results.ok())
    co_return results;
  const std::span<const String> locale_ids{context.locale_ids()};
  for (scada::DataValue& result : *results)
    ResolveValue(result.value, locale_ids);
  co_return results;
}

scada::CoStatusOr<std::vector<scada::StatusCode>>
LocalizingAttributeService::Write(scada::ServiceContext context,
                                  std::vector<scada::WriteValue> inputs) {
  return inner_.Write(std::move(context), std::move(inputs));
}

}  // namespace scada
