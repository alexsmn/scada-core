find_package(Boost REQUIRED COMPONENTS system date_time json log log_setup regex)
find_package(PromiseHpp REQUIRED COMPONENTS system date_time json log log_setup regex)

add_subdirectory(test)

scada_module(scada_base)

if(NOT WIN32)
  target_link_libraries(scada_base PUBLIC stdc++fs pthread)
endif()

target_include_directories(scada_base PUBLIC "..")

find_package(ChromiumBase REQUIRED)

target_link_libraries(scada_base PUBLIC
  ChromiumBase::base
  Boost::boost
  Boost::json
  Boost::log
  Boost::log_setup
  Boost::system
  promise.hpp
)

# std::format
target_compile_features(scada_base PRIVATE cxx_std_20)

if(WIN32)
  target_compile_options(scada_base PUBLIC
    /permissive-
    /W4
    /wd4100 # unreferenced formal parameter
    /wd4458 # declaration of 'x' hides class member
    /wd4459 # declaration of 'x' hides global declaration
    /wd4267 # conversion from 'size_t' to 'type', possible loss of data
    # /wd4244 # conversion from 'type1' to 'type2', possible loss of data
    $<$<NOT:$<CONFIG:Debug>>:
      /wd4189 # local variable is initialized but not referenced
    >
  )
else()
  target_compile_options(scada_base PUBLIC -Werror)
endif(WIN32)
