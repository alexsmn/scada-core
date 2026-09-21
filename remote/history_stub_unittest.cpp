#include "remote/history_stub.h"

#include "base/test/test_executor.h"
#include "remote/message_sender_mock.h"
#include "remote/protocol.h"
#include "remote/protocol_utils.h"
#include "scada/history_service.h"
#include "scada/locale_negotiation.h"

#include <gmock/gmock.h>

using namespace testing;

namespace {

// Answers every events read with one event whose message is packed in the
// "mul" form, the way a message composed from a translated catalog is stored.
class TwoLanguageHistoryService final : public scada::HistoryService {
 public:
  scada::CoStatusOr<scada::HistoryReadRawResult> HistoryReadRaw(
      scada::HistoryReadRawDetails) override {
    co_return scada::HistoryReadRawResult{};
  }

  scada::CoStatusOr<scada::HistoryReadEventsResult> HistoryReadEvents(
      scada::NodeId,
      scada::Time,
      scada::Time,
      scada::EventFilter) override {
    const scada::LocalizedText translations[] = {
        {"ru", u"Изменение состояния"},
        {"en", u"State change"},
    };
    const scada::LocalizedText names[] = {
        {"ru", u"Статистика сервера"},
        {"en", u"Server statistics"},
    };
    scada::Event event;
    event.event_id = 1;
    event.time = scada::Time{std::chrono::seconds{1}};
    event.severity = scada::kSeverityNormal;
    event.message = scada::EncodeMultiLanguage(translations);
    event.source_name = scada::EncodeMultiLanguage(names);
    co_return scada::HistoryReadEventsResult{.events = {std::move(event)}};
  }
};

// Runs one HistoryReadEvents request through a stub built for `locale_ids`
// and returns the event the client would receive.
protocol::Event ReadOneEvent(std::vector<std::string> locale_ids) {
  TestExecutor executor;
  auto sender = std::make_shared<StrictMock<MessageSenderMock>>();
  TwoLanguageHistoryService service;

  auto stub = std::make_shared<HistoryStub>(service, sender, executor,
                                            std::move(locale_ids));

  protocol::Event received;
  EXPECT_CALL(*sender, Send(_))
      .WillOnce(Invoke([&received](protocol::Message& message) {
        ASSERT_EQ(message.responses_size(), 1);
        const auto& result = message.responses(0).history_read_events_result();
        ASSERT_EQ(result.event_size(), 1);
        received = result.event(0);
      }));

  protocol::Request request;
  request.set_request_id(7);
  auto& read = *request.mutable_history_read_events();
  Convert(scada::NodeId{2, 3}, *read.mutable_node_id());

  stub->OnRequestReceived(request);
  while (executor.GetTaskCount() != 0)
    executor.Poll();

  return received;
}

// A stored message holds every language the server could say it in; the
// session's LocaleIds pick which one crosses the wire. OPC UA Part 4 §5.4,
// https://reference.opcfoundation.org/Core/Part4/v105/docs/5.4 — Part 11
// states no exception for historical access, so a read of the journal is
// localized like a live event.
TEST(HistoryStubTest, AnEventMessageIsResolvedToTheSessionLanguage) {
  const auto russian = ReadOneEvent({"ru"});
  EXPECT_EQ(russian.message_locale(), "ru");
  EXPECT_EQ(russian.message_utf8(), "Изменение состояния");

  const auto english = ReadOneEvent({"en"});
  EXPECT_EQ(english.message_locale(), "en");
  EXPECT_EQ(english.message_utf8(), "State change");
}

// §5.4 would rather answer in some language than in none: an unmatched
// request degrades to a translation the server has, never to empty text.
TEST(HistoryStubTest, AnUnknownLanguageStillYieldsAMessage) {
  const auto received = ReadOneEvent({"de"});
  EXPECT_EQ(received.message_locale(), "ru");
  EXPECT_FALSE(received.message_utf8().empty());
}

// A session that named no locale is the pre-negotiation case, and the packed
// payload must not reach it: it would render as JSON in the event journal.
TEST(HistoryStubTest, ASessionWithNoLocalesGetsPlainTextNotThePackedForm) {
  const auto received = ReadOneEvent({});
  EXPECT_NE(received.message_locale(), scada::kMultiLanguageLocale);
  EXPECT_THAT(received.message_utf8(), Not(HasSubstr("{\"t\":")));
}

// SourceName is the source node's DisplayName captured when the event was
// produced, and it carries every language that name had — so the journal's
// object column follows the session too, not just its message column. The
// wire field is a plain string (OPC UA Part 5 §6.4.2 types SourceName as a
// `String`), so what must differ between sessions is the string itself.
//
// Regression test for backlog 802: an English session read its journal's
// object column in Russian while the same session's node tree was correct,
// because the producer resolved the name once against an empty context.
TEST(HistoryStubTest, AnEventSourceNameIsResolvedToTheSessionLanguage) {
  const auto english = ReadOneEvent({"en"});
  EXPECT_EQ(english.source_name(), "Server statistics");

  const auto russian = ReadOneEvent({"ru"});
  EXPECT_EQ(russian.source_name(), "Статистика сервера");
}

// And the packed payload must never reach the wire: the object column would
// show raw JSON, which is worse than showing the wrong language.
TEST(HistoryStubTest, ASourceNameIsNeverThePackedFormOnTheWire) {
  const auto received = ReadOneEvent({});
  EXPECT_FALSE(received.source_name().empty());
  EXPECT_THAT(received.source_name(), Not(HasSubstr("{\"t\":")));
}

}  // namespace
