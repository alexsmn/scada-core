include(boost.cmake)
include(scada_module.cmake)
include(install_dll.cmake)

add_subdirectory(base)
add_subdirectory(metrics)
add_subdirectory(scada)
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
      "$<$<CONFIG:Debug>:${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug/abseil_dll.dll>"
      "$<$<CONFIG:RelWithDebInfo>:${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/RelWithDebInfo/abseil_dll.dll>"
      "$<$<CONFIG:Debug>:${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug/libprotobufd.dll>"
      "$<$<CONFIG:RelWithDebInfo>:${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/RelWithDebInfo/libprotobuf.dll>"
      "$<$<CONFIG:Debug>:${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug/OPCUA.dll>"
      "$<$<CONFIG:RelWithDebInfo>:${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/RelWithDebInfo/OPCUA.dll>"
      "$<$<CONFIG:Debug>:${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug/libssl${LIB_SUFFIX}.dll>"
      "$<$<CONFIG:RelWithDebInfo>:${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/RelWithDebInfo/libssl${LIB_SUFFIX}.dll>"
      "$<$<CONFIG:Debug>:${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug/libcrypto${LIB_SUFFIX}.dll>"
      "$<$<CONFIG:RelWithDebInfo>:${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/RelWithDebInfo/libcrypto${LIB_SUFFIX}.dll>"
    DESTINATION bin
    COMPONENT Common)

endif(WIN32)
