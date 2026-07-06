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
    target_include_directories(${MODULE_NAME}_unittests PRIVATE
      ${CMAKE_SOURCE_DIR}
      ${CMAKE_SOURCE_DIR}/server
      ${CMAKE_SOURCE_DIR}/server/modules)

    if(MSVC)
      # Debug and RelWithDebInfo already use embedded object debug info via
      # the preset, so disabling linker-generated debug info avoids large
      # test-bin PDB failures without producing an invalid /PDB:NONE output
      # path.
      target_link_options(${MODULE_NAME}_unittests PRIVATE
        $<$<CONFIG:Debug>:/DEBUG:NONE>
        $<$<CONFIG:Debug>:/INCREMENTAL:NO>
        $<$<CONFIG:Debug>:/PDB:NONE>
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

  # Only *.cpp/*.h are globbed. C++20 module interface units (*.cppm, e.g.
  # base/scada_base.cppm) rely on being invisible here: they are attached to
  # dedicated module targets via FILE_SET CXX_MODULES and must never join a
  # module's regular source list.
  file(${glob_mode} sources CONFIGURE_DEPENDS
    "${SOURCE_DIR}/*.cpp"
    "${SOURCE_DIR}/*.h")

  if(WIN32)
    file(GLOB win_sources CONFIGURE_DEPENDS
      "${SOURCE_DIR}/win/*.cpp"
      "${SOURCE_DIR}/win/*.h")
    list(APPEND sources ${win_sources})
    list(FILTER sources EXCLUDE REGEX "_posix\\.(cpp|h)$")
  else()
    list(FILTER sources EXCLUDE REGEX "_win\\.(cpp|h)$")
  endif()

  if(MODULE_NAME MATCHES "_qt($|_)")
    list(FILTER sources EXCLUDE REGEX "_wt(_unittest)?\\.(cpp|h)$")
  elseif(MODULE_NAME MATCHES "_wt($|_)")
    list(FILTER sources EXCLUDE REGEX "_qt(_unittest)?\\.(cpp|h)$")
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
  cmake_parse_arguments(ARG "RECURSE" "" "PRIVATE;PUBLIC;INTERFACE" ${ARGN})
  set(forwarded_args)
  if(ARG_RECURSE)
    list(APPEND forwarded_args RECURSE)
  endif()
  foreach(dir ${ARG_PRIVATE})
    _scada_module_add_sources(${MODULE_NAME} PRIVATE ${dir} ${forwarded_args} ${ARG_UNPARSED_ARGUMENTS})
  endforeach()
  foreach(dir ${ARG_PUBLIC})
    _scada_module_add_sources(${MODULE_NAME} PUBLIC ${dir} ${forwarded_args} ${ARG_UNPARSED_ARGUMENTS})
  endforeach()
  foreach(dir ${ARG_INTERFACE})
    _scada_module_add_sources(${MODULE_NAME} INTERFACE ${dir} ${forwarded_args} ${ARG_UNPARSED_ARGUMENTS})
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

#[=======================================================================[
C++20 named-module facade helpers (SCADA_CXX_MODULES pilot).

All three functions are no-ops (or gate their module-specific parts) when
SCADA_CXX_MODULES is OFF, so the default header build is untouched. See
docs/cxx-modules.md for the facade design and the consumer rules.
#]=======================================================================]

# Resolves the IDE folder for module targets: each repo defines its own
# folder variable (core: scada_core_folder, common: scada_common_folder),
# visible only in that repo's directory scope.
function(_scada_module_ide_folder OUT_VAR)
  if(DEFINED scada_core_folder)
    set(${OUT_VAR} ${scada_core_folder} PARENT_SCOPE)
  elseif(DEFINED scada_common_folder)
    set(${OUT_VAR} ${scada_common_folder} PARENT_SCOPE)
  else()
    set(${OUT_VAR} "Modules" PARENT_SCOPE)
  endif()
endfunction()

# scada_add_module_facade(<lib> FILES <name.cppm> [IMPORTS <module_targets...>])
#
# Creates <lib>_module: a STATIC library holding the named-module facade over
# <lib>'s headers. IMPORTS lists the facade targets of <lib>'s PUBLIC link
# dependencies (the .cppm `export import`s their modules, so their BMIs must
# propagate to consumers).
function(scada_add_module_facade LIB)
  if(NOT SCADA_CXX_MODULES)
    return()
  endif()
  # NAME overrides the facade target name (default <lib>_module) for libs
  # whose name would collide with an existing target (e.g. device ->
  # device_module is already the devices/module library).
  cmake_parse_arguments(ARG "" "NAME" "FILES;IMPORTS" ${ARGN})
  if(NOT ARG_NAME)
    set(ARG_NAME ${LIB}_module)
  endif()
  add_library(${ARG_NAME} STATIC)
  # NOTE: *.cppm is deliberately invisible to scada_module()'s *.cpp/*.h GLOB.
  target_sources(${ARG_NAME} PUBLIC FILE_SET CXX_MODULES FILES ${ARG_FILES})
  # PUBLIC <lib>: include dirs/defines for the GMF + link the implementation.
  # PUBLIC IMPORTS: BMIs of export-imported facades must reach consumers.
  target_link_libraries(${ARG_NAME} PUBLIC ${LIB} ${ARG_IMPORTS})
  target_compile_features(${ARG_NAME} PUBLIC cxx_std_23)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    # AppleClang (checked on 21.0) parses named C++20 modules only behind
    # -fcxx-modules; without it `module;` / `export module` fail to parse.
    # PUBLIC so importing consumers get the flag too.
    target_compile_options(${ARG_NAME} PUBLIC -fcxx-modules)
  endif()
  set_target_properties(${ARG_NAME} PROPERTIES
    CXX_CPPCHECK ""  # cppcheck cannot parse module interface units
    # ccache is UNSAFE for module compiles, not merely a cache miss: it does
    # not treat the BMI referenced through the @modmap response file as an
    # input, so it can restore a stale object (observed: an importing TU kept
    # an old inlined function body after the module was rebuilt), and a
    # cache hit on the interface unit itself would skip regenerating the
    # -fmodule-output .pcm side output.
    CXX_COMPILER_LAUNCHER "")
  _scada_module_ide_folder(_module_folder)
  set_target_properties(${ARG_NAME} PROPERTIES FOLDER ${_module_folder})
endfunction()

# scada_module_import_pilot(<lib> IMPORTS <module_targets...> DEFINES <macros...>)
#
# Opts an existing library into importing facade modules: its pilot TUs are
# gated in-source by the DEFINES macros (e.g. SCADA_USE_BASE_MODULE). The
# caller must also wrap its target_precompile_headers(... REUSE_FROM ...) in
# `if(NOT SCADA_CXX_MODULES)` - clang-scan-deps cannot read the AppleClang
# PCH and silently emits an empty dependency scan.
function(scada_module_import_pilot LIB)
  if(NOT SCADA_CXX_MODULES)
    return()
  endif()
  cmake_parse_arguments(ARG "" "" "IMPORTS;DEFINES" ${ARGN})
  target_link_libraries(${LIB} PRIVATE ${ARG_IMPORTS})
  target_compile_definitions(${LIB} PRIVATE ${ARG_DEFINES})
  set_target_properties(${LIB} PROPERTIES
    # The tree's policy version predates CMP0155, so ordinary sources are
    # not scanned for imports by default; importing TUs must opt in.
    CXX_SCAN_FOR_MODULES ON
    # ccache does not hash the imported BMI (see scada_add_module_facade).
    CXX_COMPILER_LAUNCHER "")
endfunction()

# scada_add_module_smoke_test(<lib> SOURCES <files...> [LINK <extra targets...>])
#
# Creates <lib>_module_unittests from the lib's module_test/ sources. Only
# call under `if(SCADA_CXX_MODULES)` (typically via the module_test
# subdirectory added there).
function(scada_add_module_smoke_test LIB)
  cmake_parse_arguments(ARG "" "MODULE_TARGET" "SOURCES;LINK" ${ARGN})
  if(NOT ARG_MODULE_TARGET)
    set(ARG_MODULE_TARGET ${LIB}_module)
  endif()
  add_executable(${ARG_MODULE_TARGET}_unittests ${ARG_SOURCES})
  _scada_module_ide_folder(_module_folder)
  set_target_properties(${ARG_MODULE_TARGET}_unittests PROPERTIES
    CXX_SCAN_FOR_MODULES ON
    CXX_CPPCHECK ""
    CXX_COMPILER_LAUNCHER ""
    FOLDER ${_module_folder})
  target_link_libraries(${ARG_MODULE_TARGET}_unittests PRIVATE
    ${ARG_MODULE_TARGET} base_unittest ${ARG_LINK})
  include(GoogleTest)
  # PRE_TEST defers test discovery to ctest runtime (see scada_module_unittests).
  gtest_discover_tests(${ARG_MODULE_TARGET}_unittests DISCOVERY_MODE PRE_TEST
                       PROPERTIES TIMEOUT 60)
endfunction()
