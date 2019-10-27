set(OPCUA_COMPILER_ROOT "${deps}/UA-ModelCompiler")
set(OPCUA_COMPILER "${OPCUA_COMPILER_ROOT}/Bin/Release/Opc.Ua.ModelCompiler.exe")
set(OPCUA_STANDARD_NODES "${OPCUA_COMPILER_ROOT}/Core/Stack/Generated/Opc.Ua.PredefinedNodes.uanodes")

# add_custom_command(
#   OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/scada_node_ids.h"
#   COMMAND ${OPCUA_COMPILER} -d2 model.xml -c node_ids.csv -o generated
#   DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/model.xml" "${CMAKE_CURRENT_SOURCE_DIR}/node_ids.csv"
#   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
# )

file(GLOB OPCUA_PREDEFINED_NODES "generated/*.uanodes")
list(INSERT OPCUA_PREDEFINED_NODES 0 ${OPCUA_STANDARD_NODES})
set(OPCUA_PREDEFINED_NODES ${OPCUA_PREDEFINED_NODES} PARENT_SCOPE)

file(GLOB SCADA_MODEL_TRANSLATIONS "*_translations.json")
set(SCADA_MODEL_TRANSLATIONS ${SCADA_MODEL_TRANSLATIONS} PARENT_SCOPE)

scada_module(scada_model)
target_link_libraries(scada_model PUBLIC scada_core)