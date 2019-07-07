#pragma once

#include "model/scada_node_ids.h"

namespace data_items {

constexpr scada::NamespaceIndex kNamespaceIndex = NamespaceIndexes::SCADA;

namespace numeric_id {

const scada::NumericId SimulationSignals = 28;
const scada::NumericId TsFormats = 27;

} // numeric_id

namespace id {

const scada::NodeId AnalogConversionDataType{320, kNamespaceIndex};
const scada::NodeId AnalogConversionDataType_EnumStrings{321, kNamespaceIndex};
const scada::NodeId AnalogItemType_Aperture{295, kNamespaceIndex};
const scada::NodeId AnalogItemType_Clamping{157, kNamespaceIndex};
const scada::NodeId AnalogItemType_Conversion{156, kNamespaceIndex};
const scada::NodeId AnalogItemType_Deadband{296, kNamespaceIndex};
const scada::NodeId AnalogItemType_DisplayFormat{155, kNamespaceIndex};
const scada::NodeId AnalogItemType_EngineeringUnits{166, kNamespaceIndex};
const scada::NodeId AnalogItemType_EuHi{159, kNamespaceIndex};
const scada::NodeId AnalogItemType_EuLo{158, kNamespaceIndex};
const scada::NodeId AnalogItemType_IrHi{161, kNamespaceIndex};
const scada::NodeId AnalogItemType_IrLo{160, kNamespaceIndex};
const scada::NodeId AnalogItemType_LimitHiHi{165, kNamespaceIndex};
const scada::NodeId AnalogItemType_LimitHi{163, kNamespaceIndex};
const scada::NodeId AnalogItemType_LimitLoLo{164, kNamespaceIndex};
const scada::NodeId AnalogItemType_LimitLo{162, kNamespaceIndex};
const scada::NodeId AnalogItemType{76, kNamespaceIndex}; // :DataItemType
const scada::NodeId DataGroupType_Simulated{292, kNamespaceIndex};
const scada::NodeId DataGroupType{62, kNamespaceIndex};
const scada::NodeId DataItems{24, kNamespaceIndex};
const scada::NodeId DataItemType_Alias{140, kNamespaceIndex};
const scada::NodeId DataItemType_Input1{145, kNamespaceIndex};
const scada::NodeId DataItemType_Input2{146, kNamespaceIndex};
const scada::NodeId DataItemType_Locked{151, kNamespaceIndex};
const scada::NodeId DataItemType_OutputCondition{148, kNamespaceIndex};
const scada::NodeId DataItemType_OutputTwoStaged{309, kNamespaceIndex};
const scada::NodeId DataItemType_Output{147, kNamespaceIndex};
const scada::NodeId DataItemType_Severity{144, kNamespaceIndex};
const scada::NodeId DataItemType_Simulated{143, kNamespaceIndex};
const scada::NodeId DataItemType_StalePeriod{149, kNamespaceIndex};
const scada::NodeId DataItemType_WriteManual{230, kNamespaceIndex};
const scada::NodeId DataItemType{239, kNamespaceIndex};
const scada::NodeId DiscreteItemType_Inversion{153, kNamespaceIndex};
const scada::NodeId DiscreteItemType{72, kNamespaceIndex}; // :DataItemType
const scada::NodeId HasSimulationSignal{150, kNamespaceIndex}; // Ref
const scada::NodeId HasTsFormat{154, kNamespaceIndex}; // Ref
const scada::NodeId Server_CPU{226, kNamespaceIndex};
const scada::NodeId Server_Memory{227, kNamespaceIndex};
const scada::NodeId Server_TotalCPU{228, kNamespaceIndex};
const scada::NodeId Server_TotalMemory{229, kNamespaceIndex};
const scada::NodeId SimulationSignals{numeric_id::SimulationSignals, kNamespaceIndex};
const scada::NodeId SimulationSignalTypeEnum{326, kNamespaceIndex};
const scada::NodeId SimulationSignalTypeEnum_EnumStrings{327, kNamespaceIndex};
const scada::NodeId SimulationSignalType_Period{173, kNamespaceIndex};
const scada::NodeId SimulationSignalType_Phase{174, kNamespaceIndex};
const scada::NodeId SimulationSignalType_Type{172, kNamespaceIndex};
const scada::NodeId SimulationSignalType_UpdateInterval{175, kNamespaceIndex};
const scada::NodeId SimulationSignalType{67, kNamespaceIndex};
const scada::NodeId TsFormats{numeric_id::TsFormats, kNamespaceIndex};
const scada::NodeId TsFormatType_CloseColor{219, kNamespaceIndex};
const scada::NodeId TsFormatType_CloseLabel{217, kNamespaceIndex};
const scada::NodeId TsFormatType_OpenColor{218, kNamespaceIndex};
const scada::NodeId TsFormatType_OpenLabel{216, kNamespaceIndex};
const scada::NodeId TsFormatType{80, kNamespaceIndex};
const scada::NodeId Aliases{300, kNamespaceIndex};
const scada::NodeId AliasType{301, kNamespaceIndex};
const scada::NodeId AliasOf{302, kNamespaceIndex};

}

}