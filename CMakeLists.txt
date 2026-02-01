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

include(scada_module.cmake)

add_subdirectory(base)
add_subdirectory(metrics)
add_subdirectory(scada)
add_subdirectory(model)
add_subdirectory(remote)
