cmake_minimum_required(VERSION 3.21)
project(scada-core VERSION 2.5.5 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
  add_definitions(
    # Required for `DiscardVirtualMemory`.
    -DWINBLUE_KBSPRING14
    -DWINVER=0x0603
    -D_WIN32_WINNT=0x0603
    -D_MSVC_STL_HARDENING
  )
endif()

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
