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

  set(BOOST_DEBUG_SUFFIX "vc142-mt-gd-x32-${Boost_LIB_VERSION}")
  set(BOOST_RELEASE_SUFFIX "vc142-mt-x32-${Boost_LIB_VERSION}")
  set(BOOST_SUFFIX "$<IF:$<CONFIG:Debug>,${BOOST_DEBUG_SUFFIX},${BOOST_RELEASE_SUFFIX}>")
  # TODO: Remove Vcpkg dependency.
  set(BOOST_DLL_DIR "${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}$<$<CONFIG:Debug>:/debug>/bin")

  foreach(BOOST_COMPONENT ${BOOST_COMPONENTS})
    install(FILES "${BOOST_DLL_DIR}/boost_${BOOST_COMPONENT}-${BOOST_SUFFIX}.dll"
      DESTINATION bin
      COMPONENT Common)
  endforeach()

  install(FILES
      "$<$<CONFIG:Debug>:${LIBRARY_OUTPUT_PATH}/Debug/libprotobufd.dll>"
      "$<$<CONFIG:RelWithDebInfo>:${LIBRARY_OUTPUT_PATH}/RelWithDebInfo/libprotobuf.dll>"
    DESTINATION bin
    COMPONENT Common)
endif()


# CPack

set(CPACK_COMPONENT_COMMON_DISPLAY_NAME "Общие компоненты" PARENT_SCOPE)

set(CPACK_COMPONENTS_ALL ${CPACK_COMPONENTS_ALL} Common PARENT_SCOPE)
