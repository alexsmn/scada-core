file(GLOB sources "*.cpp" "*.h")

add_library(base_unittest ${sources})

find_package(GTest REQUIRED)
find_package(Transport REQUIRED)

target_link_libraries(base_unittest PUBLIC
  scada_base
  GTest::gmock
  Transport::transport
)

# Mitigate unreachable code in gmock-actions.h.
if(WIN32)
target_compile_options(base_unittest PUBLIC /wd4702)
endif()

set_property(TARGET base_unittest PROPERTY FOLDER ${scada_core_folder})

add_executable(base_awaitable_promise_unittests
  "async_cache_unittest.cpp"
  "async_completion_unittest.cpp"
  "callback_awaitable_unittest.cpp"
  "awaitable_promise_unittest.cpp"
  "awaitable_unittest.cpp"
  "executor_conversions_unittest.cpp"
  "gtest_main.cpp"
)

target_link_libraries(base_awaitable_promise_unittests PRIVATE
  scada_base
  GTest::gmock
  Transport::transport
)

include(GoogleTest)
gtest_discover_tests(base_awaitable_promise_unittests)

set_property(TARGET base_awaitable_promise_unittests PROPERTY FOLDER
  ${scada_core_folder})
