file(GLOB_RECURSE SOURCES "*.cpp" "*.h")
file(GLOB_RECURSE UNITTESTS "*_unittest.*")
list(REMOVE_ITEM SOURCES ${UNITTESTS})

add_library(scada_model ${SOURCES})
target_link_libraries(scada_model PUBLIC scada_core)
set_property(TARGET scada_model PROPERTY FOLDER ${scada_folder})

add_library(scada_model_unittests ${UNITTESTS})
target_link_libraries(scada_model_unittests PUBLIC scada_model base_unittest)
set_property(TARGET scada_model_unittests PROPERTY FOLDER ${scada_folder})
