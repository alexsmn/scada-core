include(GoogleTest)

macro(SCADA_MODULE MODULE_NAME)
  file(GLOB ${MODULE_NAME}_SOURCES "*.cpp" "*.h")
  file(GLOB ${MODULE_NAME}_UT_SOURCES "*_unittest.*" "*_mock.*")

  if (WIN32)
    file(GLOB ${MODULE_NAME}_SOURCES_WIN "win/*.cpp" "win/*.h")
    file(GLOB ${MODULE_NAME}_UT_SOURCES_WIN "win/*_unittest.*" "win/*_mock.*")
    list(APPEND ${MODULE_NAME}_SOURCES ${${MODULE_NAME}_SOURCES_WIN})
    list(APPEND ${MODULE_NAME}_UT_SOURCES ${${MODULE_NAME}_UT_SOURCES_WIN})
  endif()

  if (${MODULE_NAME}_UT_SOURCES)
    list(REMOVE_ITEM ${MODULE_NAME}_SOURCES ${${MODULE_NAME}_UT_SOURCES})
  endif()

  add_library(${MODULE_NAME} ${${MODULE_NAME}_SOURCES})
  set_property(TARGET ${MODULE_NAME} PROPERTY FOLDER ${scada_folder})

  if (${MODULE_NAME}_UT_SOURCES)
    add_executable(${MODULE_NAME}_unittests ${${MODULE_NAME}_UT_SOURCES})
    target_link_libraries(${MODULE_NAME}_unittests PUBLIC ${MODULE_NAME} base_unittest)
    set_property(TARGET ${MODULE_NAME}_unittests PROPERTY FOLDER ${scada_folder})
    # Temporary workaround of "no cmake script provided" error.
    # gtest_discover_tests(TARGET ${MODULE_NAME}_unittests)
    gtest_add_tests(TARGET ${MODULE_NAME}_unittests)
  endif()
endmacro()
