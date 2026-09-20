#include "scada/service_context.h"

#include <gtest/gtest.h>

#include <sstream>
#include <string>
#include <vector>

namespace scada {
namespace {

TEST(ServiceContextTest, LocaleIdsDefaultToEmpty) {
  // Part 4 §5.4 reads an empty list as "the Server shall return any one that
  // it has", so the default context is the no-preference case rather than a
  // preference for anything in particular.
  EXPECT_TRUE(ServiceContext{}.locale_ids().empty());
}

TEST(ServiceContextTest, LocaleIdsRoundTripInOrder) {
  // The order is the preference order, so it is part of the value.
  const std::vector<std::string> locales{"en-GB", "en", "ru"};
  const ServiceContext context = ServiceContext{}.with_locale_ids(locales);
  EXPECT_EQ(locales, context.locale_ids());
}

TEST(ServiceContextTest, WithLocaleIdsLeavesTheSourceContextAlone) {
  // The `with_` family is copy-on-write; a session's context is shared by
  // every in-flight request, so a derived context must not mutate it.
  const ServiceContext original = ServiceContext{}.with_locale_ids({"ru"});
  const ServiceContext derived = original.with_locale_ids({"en"});
  EXPECT_EQ((std::vector<std::string>{"ru"}), original.locale_ids());
  EXPECT_EQ((std::vector<std::string>{"en"}), derived.locale_ids());
}

TEST(ServiceContextTest, LocaleIdsSurviveTheOtherWithers) {
  // A context is built by chaining, and the session builds it in one
  // expression — so nothing may drop a field set earlier in the chain.
  const ServiceContext context = ServiceContext{}
                                     .with_locale_ids({"en"})
                                     .with_user_rights(7)
                                     .with_peer("10.0.0.1:1234")
                                     .with_request_id(42);
  EXPECT_EQ((std::vector<std::string>{"en"}), context.locale_ids());
  EXPECT_EQ(7u, context.user_rights());
  EXPECT_EQ("10.0.0.1:1234", context.peer());
}

TEST(ServiceContextTest, LocaleIdsAreLogged) {
  // The ostream operator already named the field while nothing could set it;
  // this pins that it prints what was set rather than always an empty list.
  std::ostringstream stream;
  stream << ServiceContext{}.with_locale_ids({"en", "ru"});
  EXPECT_NE(std::string::npos, stream.str().find("en"));
  EXPECT_NE(std::string::npos, stream.str().find("ru"));
}

}  // namespace
}  // namespace scada
