# TODO: Remove `filesystem` and `thread`.
set(BOOST_COMPONENTS atomic context date_time filesystem json log log_setup program_options thread)

find_package(Boost REQUIRED COMPONENTS ${BOOST_COMPONENTS})

if(WIN32)
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(BOOST_ARCH "x64")
  else()
    set(BOOST_ARCH "x32")
  endif()
  
  set(BOOST_DEBUG_SUFFIX "vc143-mt-gd-${BOOST_ARCH}-${Boost_MAJOR_VERSION}_${Boost_MINOR_VERSION}")
  set(BOOST_RELEASE_SUFFIX "vc143-mt-${BOOST_ARCH}-${Boost_MAJOR_VERSION}_${Boost_MINOR_VERSION}")

  foreach(BOOST_COMPONENT ${BOOST_COMPONENTS})
    if(NOT ${BOOST_COMPONENT} MATCHES "atomic|date_time")
      install(FILES
          "$<$<CONFIG:Debug>:${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug/boost_${BOOST_COMPONENT}-${BOOST_DEBUG_SUFFIX}.dll>"
          "$<$<CONFIG:RelWithDebInfo>:${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/RelWithDebInfo/boost_${BOOST_COMPONENT}-${BOOST_RELEASE_SUFFIX}.dll>"
        DESTINATION bin
        COMPONENT Common)
    endif()
  endforeach()

endif(WIN32)
