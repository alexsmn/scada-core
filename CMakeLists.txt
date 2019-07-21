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
set(OPCUA_PREDEFINED_NODES ${OPCUA_PREDEFINED_NODES} PARENT_SCOPE)

add_subdirectory(remote)

if(WIN32)
  file(GLOB VC_REDIST_FILES "${VC_REDIST}/*")
  install(FILES ${VC_REDIST_FILES}
    DESTINATION bin
    COMPONENT Common)

  set(BOOST_COMPONENTS thread log log_setup filesystem date_time regex program_options)
  find_package(Boost REQUIRED COMPONENTS ${BOOST_COMPONENTS})

  include(install-boost.cmake)
  list_boost_dlls(BOOST_DLLS ${BOOST_COMPONENTS})
  install(FILES ${BOOST_DLLS}
    DESTINATION bin
    COMPONENT Common)

  install(FILES
      "$<$<CONFIG:Debug>:${LIBRARY_OUTPUT_PATH}/Debug/libprotobufd.dll>"
      "$<$<CONFIG:RelWithDebInfo>:${LIBRARY_OUTPUT_PATH}/RelWithDebInfo/libprotobuf.dll>"
    DESTINATION bin
    COMPONENT Common)
endif()
