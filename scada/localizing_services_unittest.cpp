#include "scada/localizing_services.h"

#include "base/test/awaitable_test.h"
#include "base/test/test_executor.h"

#include "scada/co_result.h"
#include "scada/locale_negotiation.h"
#include "scada/service_context.h"

#include <gtest/gtest.h>

#include <string>
#include <vector>

namespace scada {
namespace {

// Returns a canned response and records what it was asked, in the same shape
// as remapping_services_unittest.cpp's fakes — a plain implementation of the
// service rather than a mock, so nothing here asserts on call shape.
class FakeViewService : public scada::ViewService {
 public:
  std::vector<scada::BrowseDescription> recorded_browse;
  std::vector<scada::BrowseResult> browse_response;
  std::vector<scada::BrowsePath> recorded_paths;
  std::vector<scada::BrowsePathResult> translate_response;
  std::vector<std::string> recorded_locale_ids;

  scada::CoStatusOr<std::vector<scada::BrowseResult>> Browse(
      scada::ServiceContext context,
      std::vector<scada::BrowseDescription> inputs) override {
    recorded_browse = std::move(inputs);
    recorded_locale_ids = context.locale_ids();
    co_return browse_response;
  }

  scada::CoStatusOr<std::vector<scada::BrowsePathResult>> TranslateBrowsePaths(
      std::vector<scada::BrowsePath> inputs) override {
    recorded_paths = std::move(inputs);
    co_return translate_response;
  }
};

class FakeAttributeService : public scada::AttributeService {
 public:
  std::vector<scada::ReadValueId> recorded_read;
  std::vector<scada::DataValue> read_response;
  std::vector<scada::WriteValue> recorded_write;
  std::vector<scada::StatusCode> write_response;
  scada::Status read_status{scada::StatusCode::Good};

  scada::CoStatusOr<std::vector<scada::DataValue>> Read(
      scada::ServiceContext,
      std::vector<scada::ReadValueId> inputs) override {
    recorded_read = std::move(inputs);
    if (!read_status.good())
      co_return read_status;
    co_return read_response;
  }

  scada::CoStatusOr<std::vector<scada::StatusCode>> Write(
      scada::ServiceContext,
      std::vector<scada::WriteValue> inputs) override {
    recorded_write = std::move(inputs);
    co_return write_response;
  }
};

LocalizedText Ru() {
  return LocalizedText{"ru", u"Отрадная 110 КВ"};
}
LocalizedText En() {
  return LocalizedText{"en", u"Otradnaya 110 kV"};
}
LocalizedText Packed() {
  const std::vector<LocalizedText> translations{Ru(), En()};
  return EncodeMultiLanguage(translations);
}

ServiceContext ContextFor(std::vector<std::string> locale_ids) {
  return ServiceContext{}.with_locale_ids(std::move(locale_ids));
}

// --- Browse ---------------------------------------------------------------

TEST(LocalizingViewServiceTest, ReferenceDisplayNameIsResolvedForTheSession) {
  FakeViewService inner;
  inner.browse_response = {
      scada::BrowseResult{.references = {scada::ReferenceDescription{
                              .display_name = Packed()}}}};

  LocalizingViewService service{inner};
  TestExecutor executor;
  auto result = WaitAwaitable(
      executor, service.Browse(ContextFor({"en"}), {scada::BrowseDescription{}}));

  ASSERT_TRUE(result.ok());
  ASSERT_EQ(1u, result->size());
  ASSERT_EQ(1u, (*result)[0].references.size());
  EXPECT_EQ(En(), (*result)[0].references[0].display_name);
}

TEST(LocalizingViewServiceTest, ADifferentSessionGetsADifferentLanguage) {
  // The whole point: two sessions, one stored address space, two answers.
  FakeViewService inner;
  inner.browse_response = {
      scada::BrowseResult{.references = {scada::ReferenceDescription{
                              .display_name = Packed()}}}};

  LocalizingViewService service{inner};
  TestExecutor executor;
  const auto english = WaitAwaitable(
      executor, service.Browse(ContextFor({"en"}), {scada::BrowseDescription{}}));
  const auto russian = WaitAwaitable(
      executor, service.Browse(ContextFor({"ru"}), {scada::BrowseDescription{}}));

  ASSERT_TRUE(english.ok());
  ASSERT_TRUE(russian.ok());
  EXPECT_EQ(En(), (*english)[0].references[0].display_name);
  EXPECT_EQ(Ru(), (*russian)[0].references[0].display_name);
}

TEST(LocalizingViewServiceTest, EveryReferenceOfEveryResultIsResolved) {
  FakeViewService inner;
  inner.browse_response = {
      scada::BrowseResult{
          .references = {scada::ReferenceDescription{.display_name = Packed()},
                         scada::ReferenceDescription{.display_name = Packed()}}},
      scada::BrowseResult{.references = {
                              scada::ReferenceDescription{.display_name =
                                                              Packed()}}}};

  LocalizingViewService service{inner};
  TestExecutor executor;
  auto result = WaitAwaitable(
      executor, service.Browse(ContextFor({"en"}), {scada::BrowseDescription{},
                                                    scada::BrowseDescription{}}));

  ASSERT_TRUE(result.ok());
  for (const scada::BrowseResult& browse_result : *result) {
    for (const scada::ReferenceDescription& reference :
         browse_result.references) {
      EXPECT_EQ(En(), reference.display_name);
    }
  }
}

TEST(LocalizingViewServiceTest, APlainDisplayNameIsUnchanged) {
  // Most of the address space has one translation; it must survive untouched
  // whatever the session asked for.
  FakeViewService inner;
  inner.browse_response = {
      scada::BrowseResult{.references = {scada::ReferenceDescription{
                              .display_name = LocalizedText{u"Server"}}}}};

  LocalizingViewService service{inner};
  TestExecutor executor;
  auto result = WaitAwaitable(
      executor, service.Browse(ContextFor({"de"}), {scada::BrowseDescription{}}));

  ASSERT_TRUE(result.ok());
  EXPECT_EQ(LocalizedText{u"Server"}, (*result)[0].references[0].display_name);
}

TEST(LocalizingViewServiceTest, TheSessionContextReachesTheInnerService) {
  // The wrapper must not swallow the context: a node manager below it may
  // need the user id or the locales for its own reasons.
  FakeViewService inner;
  LocalizingViewService service{inner};
  TestExecutor executor;
  auto result = WaitAwaitable(
      executor, service.Browse(ContextFor({"en", "ru"}),
                               {scada::BrowseDescription{}}));

  ASSERT_TRUE(result.ok());
  EXPECT_EQ((std::vector<std::string>{"en", "ru"}), inner.recorded_locale_ids);
}

TEST(LocalizingViewServiceTest, TranslateBrowsePathsIsForwardedUnchanged) {
  FakeViewService inner;
  inner.translate_response = {scada::BrowsePathResult{}};
  LocalizingViewService service{inner};
  TestExecutor executor;
  auto result =
      WaitAwaitable(executor, service.TranslateBrowsePaths({scada::BrowsePath{}}));

  ASSERT_TRUE(result.ok());
  EXPECT_EQ(1u, result->size());
  EXPECT_EQ(1u, inner.recorded_paths.size());
}

// --- Read -----------------------------------------------------------------

TEST(LocalizingAttributeServiceTest, AScalarLocalizedTextValueIsResolved) {
  FakeAttributeService inner;
  scada::DataValue value;
  value.value = Packed();
  inner.read_response = {value};

  LocalizingAttributeService service{inner};
  TestExecutor executor;
  auto result = WaitAwaitable(
      executor, service.Read(ContextFor({"en"}), {scada::ReadValueId{}}));

  ASSERT_TRUE(result.ok());
  ASSERT_EQ(1u, result->size());
  ASSERT_NE(nullptr, (*result)[0].value.get_if<LocalizedText>());
  EXPECT_EQ(En(), *(*result)[0].value.get_if<LocalizedText>());
}

TEST(LocalizingAttributeServiceTest, AnArrayOfLocalizedTextIsResolvedElementwise) {
  FakeAttributeService inner;
  scada::DataValue value;
  value.value = std::vector<LocalizedText>{Packed(), LocalizedText{u"plain"}};
  inner.read_response = {value};

  LocalizingAttributeService service{inner};
  TestExecutor executor;
  auto result = WaitAwaitable(
      executor, service.Read(ContextFor({"ru"}), {scada::ReadValueId{}}));

  ASSERT_TRUE(result.ok());
  const auto* texts = (*result)[0].value.get_if<std::vector<LocalizedText>>();
  ASSERT_NE(nullptr, texts);
  ASSERT_EQ(2u, texts->size());
  EXPECT_EQ(Ru(), (*texts)[0]);
  EXPECT_EQ(LocalizedText{u"plain"}, (*texts)[1]);
}

TEST(LocalizingAttributeServiceTest, AValueOfAnotherTypeIsUntouched) {
  FakeAttributeService inner;
  scada::DataValue value;
  value.value = scada::Variant{42.0};
  inner.read_response = {value};

  LocalizingAttributeService service{inner};
  TestExecutor executor;
  auto result = WaitAwaitable(
      executor, service.Read(ContextFor({"en"}), {scada::ReadValueId{}}));

  ASSERT_TRUE(result.ok());
  EXPECT_EQ(scada::Variant{42.0}, (*result)[0].value);
}

TEST(LocalizingAttributeServiceTest, NoRequestedLocaleStillYieldsAPlainValue) {
  // Part 4 §5.4: with no LocaleIds the server returns any one it has — what it
  // must never do is hand the client the packed JSON.
  FakeAttributeService inner;
  scada::DataValue value;
  value.value = Packed();
  inner.read_response = {value};

  LocalizingAttributeService service{inner};
  TestExecutor executor;
  auto result = WaitAwaitable(
      executor, service.Read(scada::ServiceContext{}, {scada::ReadValueId{}}));

  ASSERT_TRUE(result.ok());
  const auto* text = (*result)[0].value.get_if<LocalizedText>();
  ASSERT_NE(nullptr, text);
  EXPECT_NE(String{"mul"}, text->locale);
  EXPECT_EQ(Ru(), *text);
}

TEST(LocalizingAttributeServiceTest, AFailedReadIsPassedThroughUnchanged) {
  FakeAttributeService inner;
  inner.read_status = scada::Status{scada::StatusCode::Bad_ViewIdUnknown};

  LocalizingAttributeService service{inner};
  TestExecutor executor;
  auto result = WaitAwaitable(
      executor, service.Read(ContextFor({"en"}), {scada::ReadValueId{}}));

  EXPECT_FALSE(result.ok());
  EXPECT_EQ(scada::StatusCode::Bad_ViewIdUnknown, result.status().code());
}

TEST(LocalizingAttributeServiceTest, WriteIsForwardedUnchanged) {
  // Part 4 §5.4 forbids the special locales in Write, and a plain value a
  // client wrote is exactly what it meant — so this direction never resolves.
  FakeAttributeService inner;
  inner.write_response = {scada::StatusCode::Good};

  LocalizingAttributeService service{inner};
  TestExecutor executor;
  scada::WriteValue write;
  write.value = Packed();
  auto result =
      WaitAwaitable(executor, service.Write(ContextFor({"en"}), {write}));

  ASSERT_TRUE(result.ok());
  ASSERT_EQ(1u, inner.recorded_write.size());
  EXPECT_EQ(Packed(), *inner.recorded_write[0].value.get_if<LocalizedText>());
}

}  // namespace
}  // namespace scada
