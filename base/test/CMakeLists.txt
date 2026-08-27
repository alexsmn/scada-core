# `base_unittest` is the tree's shared test-support library, not a test binary:
# every `scada_module()`'s `<module>_unittests` links it (core/scada_module.cmake),
# as do the client, framework, tier and e2e test executables, for the gtest main
# and the helpers in this directory. So it must carry helpers only. A
# `*_unittest.cpp` swept in here is compiled into every one of those binaries and
# *run* by none of them, because only `base_awaitable_unittests` below discovers
# tests -- the orphan shape `scada_orphan_unittests_check` exists to catch,
# arriving from the direction it cannot see (the file is built, so nothing looks
# wrong).
file(GLOB sources "*.cpp" "*.h")
file(GLOB unittest_sources CONFIGURE_DEPENDS "*_unittest.cpp")
list(REMOVE_ITEM sources ${unittest_sources})

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

# Glob rather than name each file: an explicit list drops a newly added
# `*_unittest.cpp` from the run silently, while the glob above still compiles it
# into `base_unittest`. That combination is what made a missing test look like a
# passing one. `gtest_main.cpp` arrives with `base_unittest`, which is how the
# other gtest-main consumers in this tree take it.
add_executable(base_awaitable_unittests ${unittest_sources})

target_link_libraries(base_awaitable_unittests PRIVATE
  base_unittest
  scada_net
)

include(GoogleTest)
gtest_discover_tests(base_awaitable_unittests)

set_property(TARGET base_awaitable_unittests PROPERTY FOLDER
  ${scada_core_folder})
