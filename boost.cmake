# TODO: Remove `filesystem` and `thread`.
set(BOOST_COMPONENTS atomic context date_time filesystem json log log_setup program_options thread)

find_package(Boost REQUIRED COMPONENTS ${BOOST_COMPONENTS})
