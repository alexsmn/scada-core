if(NOT BOOST_FOUND)
  find_package(Boost REQUIRED COMPONENTS system date_time log log_setup regex)
endif()

add_subdirectory(test)

file(GLOB SOURCES "*.cpp" "*.h")
file(GLOB UNITTESTS "*_unittest.*")
list(REMOVE_ITEM SOURCES ${UNITTESTS})

if(WIN32)
  file(GLOB SOURCES_WIN "win/*.cpp" "win/*.h")
  list(APPEND SOURCES ${SOURCES_WIN})
endif()

add_library(scada_base ${SOURCES})

if(NOT WIN32)
  target_link_libraries(scada_base PUBLIC stdc++fs pthread)
endif()

target_include_directories(scada_base PUBLIC "..")
target_link_libraries(scada_base PUBLIC
  base
  Boost::boost
  Boost::log
  Boost::log_setup
  Boost::system
)
set_property(TARGET scada_base PROPERTY FOLDER ${scada_folder})

# Unit Tests

add_executable(scada_base_unittests ${UNITTESTS})
target_link_libraries(scada_base_unittests PUBLIC base_unittest)

include(GoogleTest)
gtest_discover_tests(scada_base_unittests)

set_property(TARGET scada_base_unittests PROPERTY FOLDER ${scada_folder})
