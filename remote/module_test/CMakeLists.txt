# Smoke tests for the scada.remote module facade (SCADA_CXX_MODULES=ON only).
scada_add_module_smoke_test(scada_core_remote SOURCES
  module_smoke_unittest.cpp   # import-only TU
  module_mixed_unittest.cpp   # import + textual protobuf headers
  LINK scada_remote_protocol Transport::transport)
