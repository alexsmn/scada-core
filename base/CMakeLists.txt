find_package(Boost REQUIRED COMPONENTS system date_time json log log_setup program_options regex)
find_package(PromiseHpp REQUIRED COMPONENTS system date_time json log log_setup regex)

add_subdirectory(test)

scada_module(scada_base)
scada_module_sources(scada_base PRIVATE "metrics")
scada_module_sources(scada_base PRIVATE "time")

if(NOT WIN32)
  target_link_libraries(scada_base PUBLIC stdc++fs pthread)
endif()

target_include_directories(scada_base PUBLIC "..")

target_link_libraries(scada_base PUBLIC
  Boost::boost
  Boost::json
  Boost::log
  Boost::log_setup
  Boost::program_options
  Boost::system
  promise.hpp
)

target_precompile_headers(scada_base PRIVATE
  <algorithm>
  <cassert>
  <chrono>
  <cstdint>
  <functional>
  <map>
  <memory>
  <optional>
  <ostream>
  <span>
  <sstream>
  <string>
  <string_view>
  <unordered_map>
  <variant>
  <vector>
)

# std::format
target_compile_features(scada_base PRIVATE cxx_std_20)

if(WIN32)
  target_link_libraries(scada_base PUBLIC shell32)

  target_compile_definitions(scada_base PUBLIC
    NOMINMAX
    UNICODE
    WIN32_LEAN_AND_MEAN
    _WIN32_WINNT=0x0602 # Windows 8; required for boost::winapi::WaitOnAddress
  )

  target_compile_options(scada_base PUBLIC
    /permissive-
    /W4
    /wd4100 # unreferenced formal parameter
    /wd4458 # declaration of 'x' hides class member
    /wd4459 # declaration of 'x' hides global declaration
    /wd4267 # conversion from 'size_t' to 'type', possible loss of data
    # /wd4244 # conversion from 'type1' to 'type2', possible loss of data
    $<$<NOT:$<CONFIG:Debug>>:
      /wd4189 # local variable is initialized but not referenced
    >
  )
else()
  target_compile_options(scada_base PUBLIC -Werror)
endif(WIN32)
