#[=======================================================================[
scada_module.cmake — Convention-based CMake module helpers

Source files (*.cpp, *.h) are auto-discovered via GLOB CONFIGURE_DEPENDS.
Files matching *_unittest.* are compiled into a separate GoogleTest
executable (<module>_unittests). Files matching *_mock.* are excluded.
On Windows, a win/ subdirectory is also scanned automatically.

Functions
---------

  scada_module(<name> [INTERFACE] [RECURSE])

    Create a library and discover sources in CMAKE_CURRENT_SOURCE_DIR.
    RECURSE scans subdirectories recursively.

    Examples:
      scada_module(scada_base)
      scada_module(scada_server_security RECURSE)

  scada_module_sources(<name> PRIVATE|PUBLIC|INTERFACE <dir>... [RECURSE])

    Add sources from additional directories to an existing module.

    Examples:
      scada_module_sources(scada_base PRIVATE "metrics")
      scada_module_sources(iec61850_lib PRIVATE "client")

  scada_module_unittests(<name> <files>...)

    Register unit test source files for <name>. Normally called
    automatically when *_unittest.cpp files are discovered.
    Creates <name>_unittests executable linked to <name> and
    base_unittest, with gtest_discover_tests(DISCOVERY_MODE PRE_TEST).

#]=======================================================================]

function(scada_module_unittests MODULE_NAME)
  if(NOT ARGN)
    return()
  endif()

  if(NOT TARGET ${MODULE_NAME}_unittests)
    add_executable(${MODULE_NAME}_unittests)

    include(GoogleTest)
    # PRE_TEST defers test discovery to ctest runtime, so individual tests
    # are found even when only specific targets are built (without PRE_TEST,
    # unbuilt executables get _NOT_BUILT placeholders at configure time).
    gtest_discover_tests(${MODULE_NAME}_unittests DISCOVERY_MODE PRE_TEST
                         PROPERTIES TIMEOUT 60)

    target_link_libraries(${MODULE_NAME}_unittests PRIVATE ${MODULE_NAME} base_unittest)

    if(MSVC)
      # RelWithDebInfo already uses embedded object debug info via the preset,
      # so disabling linker-generated debug info avoids large test-bin PDB
      # failures without producing an invalid /PDB:NONE output path.
      target_link_options(${MODULE_NAME}_unittests PRIVATE
        $<$<CONFIG:RelWithDebInfo>:/DEBUG:NONE>)
    endif()
  endif()

  target_sources(${MODULE_NAME}_unittests PRIVATE ${ARGN})
endfunction()

function(_scada_module_add_sources MODULE_NAME SCOPE SOURCE_DIR)
  cmake_parse_arguments(ARG "RECURSE" "" "" ${ARGN})

  if(ARG_RECURSE)
    set(glob_mode GLOB_RECURSE)
  else()
    set(glob_mode GLOB)
  endif()

  file(${glob_mode} sources CONFIGURE_DEPENDS
    "${SOURCE_DIR}/*.cpp"
    "${SOURCE_DIR}/*.h")

  if(WIN32)
    file(GLOB win_sources CONFIGURE_DEPENDS
      "${SOURCE_DIR}/win/*.cpp"
      "${SOURCE_DIR}/win/*.h")
    list(APPEND sources ${win_sources})
  endif()

  # Separate unittest files and exclude mocks.
  set(ut_sources ${sources})
  list(FILTER ut_sources INCLUDE REGEX "_unittest\\.")
  list(FILTER sources EXCLUDE REGEX "_unittest\\.")
  list(FILTER sources EXCLUDE REGEX "_mock\\.")

  target_sources(${MODULE_NAME} ${SCOPE} ${sources})

  if(ut_sources)
    scada_module_unittests(${MODULE_NAME} ${ut_sources})
  endif()
endfunction()

function(scada_module_sources MODULE_NAME)
  cmake_parse_arguments(ARG "" "" "PRIVATE;PUBLIC;INTERFACE" ${ARGN})
  foreach(dir ${ARG_PRIVATE})
    _scada_module_add_sources(${MODULE_NAME} PRIVATE ${dir} ${ARG_UNPARSED_ARGUMENTS})
  endforeach()
  foreach(dir ${ARG_PUBLIC})
    _scada_module_add_sources(${MODULE_NAME} PUBLIC ${dir} ${ARG_UNPARSED_ARGUMENTS})
  endforeach()
  foreach(dir ${ARG_INTERFACE})
    _scada_module_add_sources(${MODULE_NAME} INTERFACE ${dir} ${ARG_UNPARSED_ARGUMENTS})
  endforeach()
endfunction()

function(scada_module MODULE_NAME)
  cmake_parse_arguments(ARG "INTERFACE" "" "" ${ARGN})

  if(ARG_INTERFACE)
    add_library(${MODULE_NAME} INTERFACE)
    scada_module_sources(${MODULE_NAME} ${ARG_UNPARSED_ARGUMENTS} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
  else()
    add_library(${MODULE_NAME})
    scada_module_sources(${MODULE_NAME} ${ARG_UNPARSED_ARGUMENTS} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
  endif()
endfunction()
