#include "scada/localizing_services.h"

#include "scada/locale_negotiation.h"
#include "scada/service_context.h"
#include "scada/variant.h"

#include <span>
#include <utility>
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

}  // namespace

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
