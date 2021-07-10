find_package(Boost REQUIRED COMPONENTS system date_time log log_setup regex)

add_subdirectory("${deps}/promise.hpp" "promise.hpp")
add_subdirectory(test)

scada_module(scada_base)

if(NOT WIN32)
  target_link_libraries(scada_base PUBLIC stdc++fs pthread)
endif()

target_include_directories(scada_base PUBLIC "..")

target_link_libraries(scada_base PUBLIC
  base
  Boost::boost
  Boost::log
  Boost::log_setup
  Boost::system
  promise.hpp
)

if(WIN32)
  target_compile_options(scada_base PUBLIC
    /permissive-
    /W4
    /wd4100 # unreferenced formal parameter
    /wd4458 # declaration of 'x' hides class member
    /wd4459 # declaration of 'x' hides global declaration
    $<$<NOT:$<CONFIG:Debug>>:
      /wd4189 # local variable is initialized but not referenced
    >
  )
endif(WIN32)
