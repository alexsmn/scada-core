include(boost.cmake)
include(gtest.cmake)
include(scada_module.cmake)
include(install_dll.cmake)

add_subdirectory(base)
add_subdirectory(core)
add_subdirectory(model)
add_subdirectory(remote)

if(WIN32)
  include(vc-redist.cmake)

  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(LIB_SUFFIX "-3-x64")
  else()
    set(LIB_SUFFIX "-3")
  endif()

  install(FILES
      "$<$<CONFIG:Debug>:${LIBRARY_OUTPUT_PATH}/Debug/libprotobufd.dll>"
      "$<$<CONFIG:RelWithDebInfo>:${LIBRARY_OUTPUT_PATH}/RelWithDebInfo/libprotobuf.dll>"
      "$<$<CONFIG:Debug>:${LIBRARY_OUTPUT_PATH}/Debug/OPCUA.dll>"
      "$<$<CONFIG:RelWithDebInfo>:${LIBRARY_OUTPUT_PATH}/RelWithDebInfo/OPCUA.dll>"
      "$<$<CONFIG:Debug>:${LIBRARY_OUTPUT_PATH}/Debug/libssl${LIB_SUFFIX}.dll>"
      "$<$<CONFIG:RelWithDebInfo>:${LIBRARY_OUTPUT_PATH}/RelWithDebInfo/libssl${LIB_SUFFIX}.dll>"
      "$<$<CONFIG:Debug>:${LIBRARY_OUTPUT_PATH}/Debug/libcrypto${LIB_SUFFIX}.dll>"
      "$<$<CONFIG:RelWithDebInfo>:${LIBRARY_OUTPUT_PATH}/RelWithDebInfo/libcrypto${LIB_SUFFIX}.dll>"
    DESTINATION bin
    COMPONENT Common)

endif(WIN32)
