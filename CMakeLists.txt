include(gtest.cmake)
include(scada_module.cmake)

add_subdirectory("${deps}/icu" icu)

set(CHROME_BASE_ROOT "${deps}/chromebase")
add_subdirectory(${CHROME_BASE_ROOT} chromebase)

add_subdirectory("${deps}/net" net)

add_subdirectory(base)
add_subdirectory(core)
add_subdirectory(model)
add_subdirectory(remote)

if(WIN32)
  file(GLOB VC_REDIST_FILES "${VC_REDIST}/*")
  if(NOT VC_REDIST_FILES)
    message(FATAL_ERROR "No VC++ redistributives were found in provided VC_REDIST: ${VC_REDIST}")
  endif()
  install(FILES ${VC_REDIST_FILES}
    DESTINATION bin
    COMPONENT Common)

  set(BOOST_COMPONENTS thread log log_setup filesystem date_time regex program_options)
  find_package(Boost REQUIRED COMPONENTS ${BOOST_COMPONENTS})

  set(BOOST_DEBUG_SUFFIX "vc142-mt-gd-x32-${Boost_LIB_VERSION}")
  set(BOOST_RELEASE_SUFFIX "vc142-mt-x32-${Boost_LIB_VERSION}")

  foreach(BOOST_COMPONENT ${BOOST_COMPONENTS})
    if(NOT ${BOOST_COMPONENT} MATCHES "date_time")
      install(FILES
          "$<$<CONFIG:Debug>:${LIBRARY_OUTPUT_PATH}/Debug/boost_${BOOST_COMPONENT}-${BOOST_DEBUG_SUFFIX}.dll>"
          "$<$<CONFIG:RelWithDebInfo>:${LIBRARY_OUTPUT_PATH}/RelWithDebInfo/boost_${BOOST_COMPONENT}-${BOOST_RELEASE_SUFFIX}.dll>"
        DESTINATION bin
        COMPONENT Common)
    endif()
  endforeach()

  install(FILES
      "$<$<CONFIG:Debug>:${LIBRARY_OUTPUT_PATH}/Debug/libprotobufd.dll>"
      "$<$<CONFIG:RelWithDebInfo>:${LIBRARY_OUTPUT_PATH}/RelWithDebInfo/libprotobuf.dll>"
      "$<$<CONFIG:Debug>:${LIBRARY_OUTPUT_PATH}/Debug/OPCUA.dll>"
      "$<$<CONFIG:RelWithDebInfo>:${LIBRARY_OUTPUT_PATH}/RelWithDebInfo/OPCUA.dll>"
      "$<$<CONFIG:Debug>:${LIBRARY_OUTPUT_PATH}/Debug/libssl-1_1.dll>"
      "$<$<CONFIG:RelWithDebInfo>:${LIBRARY_OUTPUT_PATH}/RelWithDebInfo/libssl-1_1.dll>"
      "$<$<CONFIG:Debug>:${LIBRARY_OUTPUT_PATH}/Debug/libcrypto-1_1.dll>"
      "$<$<CONFIG:RelWithDebInfo>:${LIBRARY_OUTPUT_PATH}/RelWithDebInfo/libcrypto-1_1.dll>"
    DESTINATION bin
    COMPONENT Common)

endif(WIN32)