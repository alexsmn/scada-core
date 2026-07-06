find_package(Boost REQUIRED COMPONENTS system date_time json log log_setup program_options regex)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../third_party/net/cmake")
add_subdirectory(test)

scada_module(scada_base)
scada_module_sources(scada_base PRIVATE "metrics")
scada_module_sources(scada_base PRIVATE "time")

if(APPLE)
  target_link_libraries(scada_base PUBLIC pthread)
elseif(NOT WIN32)
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
target_compile_features(scada_base PRIVATE cxx_std_23)

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
elseif(NOT APPLE)
  target_compile_options(scada_base PUBLIC -Werror)
  # GCC 13/14 emit false-positive hardening diagnostics inside Boost/libstdc++
  # (e.g. boost::log's record_ostream operator<<(char), std::string copies) that
  # would otherwise trip -Werror. Exempt just those from -Werror on GCC; Clang
  # does not raise them.
  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(scada_base PUBLIC
      -Wno-error=stringop-overread
      -Wno-error=stringop-overflow
      -Wno-error=array-bounds
      -Wno-error=restrict
      -Wno-error=maybe-uninitialized
      -Wno-error=dangling-reference)
  endif()
endif(WIN32)

# scada.base named-module facade. A separate target rather than a FILE_SET
# on scada_base itself so that the default header build - including the ~68
# targets reusing scada_base's PCH - is completely unaffected.
scada_add_module_facade(scada_base FILES scada_base.cppm)
if(SCADA_CXX_MODULES)
  add_subdirectory(module_test)
endif()
