cmake_minimum_required(VERSION 3.25)
project(scada-core VERSION 2.5.6 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
  add_definitions(
    -DWINVER=0x0603
    -D_WIN32_WINNT=0x0603
    -D_MSVC_STL_HARDENING
  )
endif()

# IDE folder for targets (parent project may override).
if(NOT DEFINED scada_core_folder)
  set(scada_core_folder "Core")
endif()

enable_testing()

include(scada_module.cmake)

add_subdirectory(base)
add_subdirectory(metrics)
add_subdirectory(net)
add_subdirectory(scada)
add_subdirectory(model)
add_subdirectory(remote)
