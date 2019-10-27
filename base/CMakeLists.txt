if(NOT BOOST_FOUND)
  find_package(Boost REQUIRED COMPONENTS system date_time log log_setup regex)
endif()

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
)