scada_module(scada_model)
target_link_libraries(scada_model PUBLIC scada_core)

set_target_properties(scada_model_unittests PROPERTIES
  EXCLUDE_FROM_ALL 1
  EXCLUDE_FROM_DEFAULT_BUILD 1
)
