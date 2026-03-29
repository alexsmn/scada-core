#pragma once

// ChromiumBase defined FRIEND_TEST_ALL_PREFIXES to friend-declare a test
// across all test prefixes (DISABLED_, FLAKY_, etc.). This minimal
// replacement directly friend-declares the test fixture.
#define FRIEND_TEST_ALL_PREFIXES(test_case_name, test_name) \
  friend class test_case_name##_##test_name##_Test

#define FORWARD_DECLARE_TEST(test_case_name, test_name) \
  class test_case_name##_##test_name##_Test
