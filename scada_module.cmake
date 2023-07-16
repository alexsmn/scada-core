macro(scada_module_unittests MODULE_NAME)
  cmake_parse_arguments(ARG "" "" "SOURCES" ${ARGN})

  # TODO: Only add UTs when there are cpp files (not stubs or mocks).
  if (ARG_SOURCES)
    message("scada_module_unittests(${MODULE_NAME})")

    if (NOT TARGET ${MODULE_NAME}_unittests)
      add_executable(${MODULE_NAME}_unittests)
      target_link_libraries(${MODULE_NAME}_unittests PUBLIC ${MODULE_NAME} base_unittest)
      set_property(TARGET ${MODULE_NAME}_unittests PROPERTY FOLDER ${scada_folder})
      include(GoogleTest)
      # Temporary workaround of "no cmake script provided" error. Also,
      # `gtest_discover_tests` is not correctly handled by CTest. So you cannot test Linux builds.
      # In opposite, `gtest_add_tests` triggers generation on each UT modification.
      gtest_discover_tests(${MODULE_NAME}_unittests DISCOVERY_MODE PRE_TEST)
      # gtest_add_tests(TARGET ${MODULE_NAME}_unittests)
    endif()

    target_sources(${MODULE_NAME}_unittests PUBLIC ${ARG_SOURCES})
  endif()
endmacro()

macro(scada_module_sources MODULE_NAME SOURCE_DIR)
  set(OPTIONS "RECURSE")
  set(ONE_VALUE_ARGS "")
  set(MULTI_VALUE_ARGS "")
  cmake_parse_arguments(SCADA_MODULE "${OPTIONS}" "${ONE_VALUE_ARGS}" "${MULTI_VALUE_ARGS}" ${ARGN})

  if(SCADA_MODULE_RECURSE)
    set(SCADA_MODULE_GLOB "GLOB_RECURSE")
  else()
    set(SCADA_MODULE_GLOB "GLOB")
  endif()

  message("scada_module_sources(${MODULE_NAME} ${SOURCE_DIR} ${SCADA_MODULE_GLOB})")

  file(${SCADA_MODULE_GLOB} ${MODULE_NAME}_SOURCES CONFIGURE_DEPENDS "${SOURCE_DIR}/*.cpp" "${SOURCE_DIR}/*.h")
  file(${SCADA_MODULE_GLOB} ${MODULE_NAME}_UT_SOURCES CONFIGURE_DEPENDS "${SOURCE_DIR}/*_unittest.*" "${SOURCE_DIR}/*_mock.*")

  if (WIN32)
    file(GLOB ${MODULE_NAME}_SOURCES_WIN CONFIGURE_DEPENDS "${SOURCE_DIR}/win/*.cpp" "${SOURCE_DIR}/win/*.h")
    file(GLOB ${MODULE_NAME}_UT_SOURCES_WIN CONFIGURE_DEPENDS "${SOURCE_DIR}/win/*_unittest.*" "${SOURCE_DIR}/win/*_mock.*")
    list(APPEND ${MODULE_NAME}_SOURCES ${${MODULE_NAME}_SOURCES_WIN})
    list(APPEND ${MODULE_NAME}_UT_SOURCES ${${MODULE_NAME}_UT_SOURCES_WIN})
  endif()

  if (${MODULE_NAME}_UT_SOURCES)
    list(REMOVE_ITEM ${MODULE_NAME}_SOURCES ${${MODULE_NAME}_UT_SOURCES})
  endif()

  target_sources(${MODULE_NAME} PRIVATE ${${MODULE_NAME}_SOURCES})

  scada_module_unittests(${MODULE_NAME} SOURCES ${${MODULE_NAME}_UT_SOURCES})
endmacro()

macro(scada_module MODULE_NAME)
  message("scada_module(${MODULE_NAME})")

  add_library(${MODULE_NAME})
  set_property(TARGET ${MODULE_NAME} PROPERTY FOLDER ${scada_folder})

  scada_module_sources(${MODULE_NAME} ${CMAKE_CURRENT_SOURCE_DIR} ${ARGN})
endmacro()
