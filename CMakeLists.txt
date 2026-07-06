cmake_minimum_required(VERSION 3.25)
if(POLICY CMP0167)
  cmake_policy(SET CMP0167 NEW)
endif()
project(scada-core VERSION 2.6.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
  add_definitions(
    -DWINVER=0x0603
    -D_WIN32_WINNT=0x0603
    -D_MSVC_STL_HARDENING
  )
endif()

# IDE folder for targets (parent project may override).
if(NOT DEFINED scada_core_folder)
  set(scada_core_folder "Core")
endif()

enable_testing()

include(scada_module.cmake)

# Pilot support for named C++20 modules (the scada.base facade). OFF by
# default; the header-based build is the source of truth and must remain
# unaffected. See base/scada_base.cppm for the design notes.
option(SCADA_CXX_MODULES
  "Pilot: build the scada.base named C++20 module facade (experimental)" OFF)
if(SCADA_CXX_MODULES)
  if(CMAKE_VERSION VERSION_LESS 3.28)
    message(FATAL_ERROR "SCADA_CXX_MODULES requires CMake >= 3.28 "
      "(FILE_SET CXX_MODULES with Ninja); found ${CMAKE_VERSION}.")
  endif()
  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU"
     AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 14)
    message(FATAL_ERROR "SCADA_CXX_MODULES requires GCC >= 14 "
      "(P1689 module dependency scanning); found GCC "
      "${CMAKE_CXX_COMPILER_VERSION}. Configure with "
      "-DSCADA_CXX_MODULES=OFF on GCC 13.")
  endif()
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang"
     AND NOT CMAKE_CXX_COMPILER_CLANG_SCAN_DEPS)
    message(FATAL_ERROR "SCADA_CXX_MODULES requires clang-scan-deps, which "
      "the Apple Command Line Tools do not ship. Install full Xcode >= 16 or "
      "set CMAKE_CXX_COMPILER_CLANG_SCAN_DEPS (e.g. to "
      "/opt/homebrew/opt/llvm/bin/clang-scan-deps).")
  endif()
  if(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang" AND NOT CMAKE_CXX_SCANDEP_SOURCE)
    # CMake (through at least 4.3) wires module dependency scanning only for
    # compiler id "Clang"; Modules/Compiler/AppleClang-CXX.cmake never sets
    # CMAKE_CXX_SCANDEP_SOURCE, so a module target fails to generate even
    # with CMAKE_CXX_COMPILER_CLANG_SCAN_DEPS in the cache. Mirror the
    # Clang >= 16 block from Modules/Compiler/Clang-CXX.cmake here (P1689
    # output is compiler-agnostic; the scanner replays AppleClang's own
    # command line). Drop this once CMake supports AppleClang natively.
    execute_process(
      COMMAND "${CMAKE_CXX_COMPILER}" -print-resource-dir
      OUTPUT_VARIABLE _appleclang_resource_dir
      OUTPUT_STRIP_TRAILING_WHITESPACE)
    # AppleClang finds the SDK (and thus libc++ headers) via built-in CLT
    # defaults, but the replaying scanner has no such defaults - without an
    # explicit -isysroot it fails with "'functional' file not found".
    execute_process(
      COMMAND xcrun --show-sdk-path
      OUTPUT_VARIABLE _appleclang_sysroot
      OUTPUT_STRIP_TRAILING_WHITESPACE)
    string(CONCAT _appleclang_scandep_source
      "\"${CMAKE_CXX_COMPILER_CLANG_SCAN_DEPS}\""
      " -format=p1689"
      " --"
      " <CMAKE_CXX_COMPILER> <DEFINES> <INCLUDES> <FLAGS>"
      " -x c++ <SOURCE> -c -o <OBJECT>"
      " -resource-dir \"${_appleclang_resource_dir}\""
      " -isysroot \"${_appleclang_sysroot}\""
      " -MT <DYNDEP_FILE>"
      " -MD -MF <DEP_FILE>"
      " > <DYNDEP_FILE>.tmp"
      " && mv <DYNDEP_FILE>.tmp <DYNDEP_FILE>")
    # CACHE INTERNAL, not directory-scope set(): module facades also live in
    # sibling repos (common/), which never see this directory's variables.
    set(CMAKE_CXX_SCANDEP_SOURCE "${_appleclang_scandep_source}"
        CACHE INTERNAL "AppleClang module dependency scanning command")
    unset(_appleclang_scandep_source)
    unset(_appleclang_resource_dir)
    unset(_appleclang_sysroot)
    set(CMAKE_CXX_MODULE_MAP_FORMAT "clang"
        CACHE INTERNAL "AppleClang module map format")
    set(CMAKE_CXX_MODULE_MAP_FLAG "@<MODULE_MAP_FILE>"
        CACHE INTERNAL "AppleClang module map flag")
    set(CMAKE_CXX_COMPILE_BMI
      "<CMAKE_CXX_COMPILER> <DEFINES> <INCLUDES> <FLAGS> -o <OBJECT> --precompile <SOURCE>"
      CACHE INTERNAL "AppleClang BMI compile command")
  endif()
endif()

add_subdirectory(base)
add_subdirectory(metrics)
add_subdirectory(net)
add_subdirectory(scada)
add_subdirectory(model)
add_subdirectory(remote)
