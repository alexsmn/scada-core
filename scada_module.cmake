function(scada_module_unittests MODULE_NAME)
  cmake_parse_arguments(ARG "" "" "SOURCES" ${ARGN})

  # TODO: Only add UTs when there are cpp files (not stubs or mocks).
  if (ARG_SOURCES)
    message("scada_module_unittests(${MODULE_NAME})")

    if (NOT TARGET ${MODULE_NAME}_unittests)
      add_executable(${MODULE_NAME}_unittests)
      target_link_libraries(${MODULE_NAME}_unittests PRIVATE ${MODULE_NAME} base_unittest)
      set_property(TARGET ${MODULE_NAME}_unittests PROPERTY FOLDER ${scada_folder})
      include(GoogleTest)
      # Temporary workaround of "no cmake script provided" error. Also,
      # `gtest_discover_tests` is not correctly handled by CTest. So you cannot test Linux builds.
      # In opposite, `gtest_add_tests` triggers generation on each UT modification.
      gtest_discover_tests(${MODULE_NAME}_unittests)
      # gtest_discover_tests(${MODULE_NAME}_unittests DISCOVERY_MODE PRE_TEST)
      # gtest_add_tests(TARGET ${MODULE_NAME}_unittests)
    endif()

    target_sources(${MODULE_NAME}_unittests PRIVATE ${ARG_SOURCES})
  endif()
endfunction()

function(scada_module_sources_helper MODULE_NAME SCOPE SOURCE_DIR)
  set(OPTIONS "RECURSE")
  set(ONE_VALUE_ARGS "")
  set(MULTI_VALUE_ARGS "")
  cmake_parse_arguments(SCADA_MODULE "${OPTIONS}" "${ONE_VALUE_ARGS}" "${MULTI_VALUE_ARGS}" ${ARGN})

  if(SCADA_MODULE_RECURSE)
    set(SCADA_MODULE_GLOB "GLOB_RECURSE")
  else()
    set(SCADA_MODULE_GLOB "GLOB")
  endif()

  message("scada_module_sources(${MODULE_NAME} ${SCOPE} ${SOURCE_DIR} ${SCADA_MODULE_GLOB})")

  file(${SCADA_MODULE_GLOB} ${MODULE_NAME}_SOURCES CONFIGURE_DEPENDS
    "${SOURCE_DIR}/*.cpp"
    "${SOURCE_DIR}/*.h")

  file(${SCADA_MODULE_GLOB} ${MODULE_NAME}_IGNORE CONFIGURE_DEPENDS
    "${SOURCE_DIR}/*_mock.*")
  
  file(${SCADA_MODULE_GLOB} ${MODULE_NAME}_UT_SOURCES CONFIGURE_DEPENDS
    "${SOURCE_DIR}/*_unittest.*")

  if (WIN32)
    file(GLOB ${MODULE_NAME}_SOURCES_WIN CONFIGURE_DEPENDS "${SOURCE_DIR}/win/*.cpp" "${SOURCE_DIR}/win/*.h")
    file(GLOB ${MODULE_NAME}_IGNORE_WIN CONFIGURE_DEPENDS "${SOURCE_DIR}/win/*_mock.*")
    file(GLOB ${MODULE_NAME}_UT_SOURCES_WIN CONFIGURE_DEPENDS "${SOURCE_DIR}/win/*_unittest.*")
    list(APPEND ${MODULE_NAME}_SOURCES ${${MODULE_NAME}_SOURCES_WIN})
    list(APPEND ${MODULE_NAME}_IGNORE ${${MODULE_NAME}_IGNORE_WIN})
    list(APPEND ${MODULE_NAME}_UT_SOURCES ${${MODULE_NAME}_UT_SOURCES_WIN})
  endif()

  if (${MODULE_NAME}_IGNORE)
    list(REMOVE_ITEM ${MODULE_NAME}_SOURCES ${${MODULE_NAME}_IGNORE})
  endif()

  if (${MODULE_NAME}_UT_SOURCES)
    list(REMOVE_ITEM ${MODULE_NAME}_SOURCES ${${MODULE_NAME}_UT_SOURCES})
  endif()

  target_sources(${MODULE_NAME} ${SCOPE} ${${MODULE_NAME}_SOURCES})

  scada_module_unittests(${MODULE_NAME} SOURCES ${${MODULE_NAME}_UT_SOURCES})
endfunction()

function(scada_module_sources MODULE_NAME)
  cmake_parse_arguments(ARG "" "" "PRIVATE;PUBLIC;INTERFACE" ${ARGN})
  foreach(SOURCE_DIR ${ARG_PRIVATE})
	scada_module_sources_helper(${MODULE_NAME} PRIVATE ${SOURCE_DIR} ${ARG_UNPARSED_ARGUMENTS})
  endforeach()
  foreach(SOURCE_DIR ${ARG_PUBLIC})
	scada_module_sources_helper(${MODULE_NAME} PUBLIC ${SOURCE_DIR} ${ARG_UNPARSED_ARGUMENTS})
  endforeach()
  foreach(SOURCE_DIR ${ARG_INTERFACE})
	scada_module_sources_helper(${MODULE_NAME} INTERFACE ${SOURCE_DIR} ${ARG_UNPARSED_ARGUMENTS})
  endforeach()
endfunction()

function(scada_module MODULE_NAME)
  cmake_parse_arguments(ARG "INTERFACE" "" "" ${ARGN})

  set(SOURCE_SCOPE "PRIVATE")
  if (ARG_INTERFACE)
    set(LIBRARY_TYPE "INTERFACE")
    set(SOURCE_SCOPE "INTERFACE")
  endif()

  message("scada_module(${MODULE_NAME} ${LIBRARY_TYPE} ${ARG_UNPARSED_ARGUMENTS})")

  add_library(${MODULE_NAME} ${LIBRARY_TYPE})
  set_property(TARGET ${MODULE_NAME} PROPERTY FOLDER ${scada_folder})

  scada_module_sources(${MODULE_NAME} ${ARG_UNPARSED_ARGUMENTS} ${SOURCE_SCOPE} ${CMAKE_CURRENT_SOURCE_DIR})
endfunction()
