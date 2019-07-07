add_subdirectory("${deps}/googletest" gtest)
add_subdirectory("${deps}/icu" icu)

set(GTEST_INCLUDE_DIR "${deps}/googletest/googletest/include")
set(GMOCK_INCLUDE_DIR "${deps}/googletest/googlemock/include")

set(CHROME_BASE_ROOT "${deps}/chromebase")
add_subdirectory(${CHROME_BASE_ROOT} chromebase)

add_subdirectory("${deps}/net" net)

add_subdirectory(base)
add_subdirectory(core)
add_subdirectory(model)
add_subdirectory(remote)

if(WIN32)
  set(BOOST_COMPONENTS thread log log_setup filesystem date_time regex program_options)
  find_package(Boost REQUIRED COMPONENTS ${BOOST_COMPONENTS})

  set(BOOST_DEBUG_SUFFIX "vc142-mt-gd-x32-${Boost_LIB_VERSION}")
  set(BOOST_RELEASE_SUFFIX "vc142-mt-x32-${Boost_LIB_VERSION}")

  foreach(BOOST_COMPONENT ${BOOST_COMPONENTS})
    install(FILES
        "$<$<CONFIG:Debug>:${LIBRARY_OUTPUT_PATH}/Debug/boost_${BOOST_COMPONENT}-${BOOST_DEBUG_SUFFIX}.dll>"
        "$<$<CONFIG:RelWithDebInfo>:${LIBRARY_OUTPUT_PATH}/RelWithDebInfo/boost_${BOOST_COMPONENT}-${BOOST_RELEASE_SUFFIX}.dll>"
      DESTINATION bin
      COMPONENT Common)
  endforeach()

  install(FILES
      "$<$<CONFIG:Debug>:${LIBRARY_OUTPUT_PATH}/Debug/libprotobufd.dll>"
      "$<$<CONFIG:RelWithDebInfo>:${LIBRARY_OUTPUT_PATH}/RelWithDebInfo/libprotobuf.dll>"
    DESTINATION bin
    COMPONENT Common)
endif()