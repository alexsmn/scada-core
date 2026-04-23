scada_register_package(
  NAME scada-core
  REPO core
  VERSION "${SCADA_PACKAGING_VERSION}"
  METADATA_ONLY
  DESCRIPTION "Core runtime package metadata"
  DISTRIBUTION_KIND "metadata-only"
  STANDALONE_INSTALLER_DEFAULT OFF
  RUNTIME_DEPENDENCY_POLICY "provided-transitively")
