#pragma once

#include "model/namespaces.h"

namespace data_items {
  
namespace numeric_id {

const scada::NumericId SimulationSignals = 28;
const scada::NumericId TsFormats = 27;

} // numeric_id

namespace id {

const scada::NodeId DisplayPropertyCategory{316, NamespaceIndexes::SCADA};
const scada::NodeId ChannelsPropertyCategory{313, NamespaceIndexes::SCADA};
const scada::NodeId ConversionPropertyCategory{314, NamespaceIndexes::SCADA};
const scada::NodeId FilteringPropertyCategory{315, NamespaceIndexes::SCADA};
const scada::NodeId SimulationPropertyCategory{318, NamespaceIndexes::SCADA};
const scada::NodeId LimitsPropertyCategory{319, NamespaceIndexes::SCADA};
const scada::NodeId AnalogConversionDataType{320, NamespaceIndexes::SCADA};
const scada::NodeId AnalogConversionDataType_EnumStrings{321, NamespaceIndexes::SCADA};
const scada::NodeId AnalogItemType_Aperture{295, NamespaceIndexes::SCADA};
const scada::NodeId AnalogItemType_Clamping{157, NamespaceIndexes::SCADA};
const scada::NodeId AnalogItemType_Conversion{156, NamespaceIndexes::SCADA};
const scada::NodeId AnalogItemType_Deadband{296, NamespaceIndexes::SCADA};
const scada::NodeId AnalogItemType_DisplayFormat{155, NamespaceIndexes::SCADA};
const scada::NodeId AnalogItemType_EngineeringUnits{166, NamespaceIndexes::SCADA};
const scada::NodeId AnalogItemType_EuHi{159, NamespaceIndexes::SCADA};
const scada::NodeId AnalogItemType_EuLo{158, NamespaceIndexes::SCADA};
const scada::NodeId AnalogItemType_IrHi{161, NamespaceIndexes::SCADA};
const scada::NodeId AnalogItemType_IrLo{160, NamespaceIndexes::SCADA};
const scada::NodeId AnalogItemType_LimitHiHi{165, NamespaceIndexes::SCADA};
const scada::NodeId AnalogItemType_LimitHi{163, NamespaceIndexes::SCADA};
const scada::NodeId AnalogItemType_LimitLoLo{164, NamespaceIndexes::SCADA};
const scada::NodeId AnalogItemType_LimitLo{162, NamespaceIndexes::SCADA};
const scada::NodeId AnalogItemType{76, NamespaceIndexes::SCADA}; // :DataItemType
const scada::NodeId DataGroupType_Simulated{292, NamespaceIndexes::SCADA};
const scada::NodeId DataGroupType{62, NamespaceIndexes::SCADA};
const scada::NodeId DataItems{24, NamespaceIndexes::SCADA};
const scada::NodeId DataItemType_Alias{140, NamespaceIndexes::SCADA};
const scada::NodeId DataItemType_Input1{145, NamespaceIndexes::SCADA};
const scada::NodeId DataItemType_Input2{146, NamespaceIndexes::SCADA};
const scada::NodeId DataItemType_Locked{151, NamespaceIndexes::SCADA};
const scada::NodeId DataItemType_OutputCondition{148, NamespaceIndexes::SCADA};
const scada::NodeId DataItemType_OutputTwoStaged{309, NamespaceIndexes::SCADA};
const scada::NodeId DataItemType_Output{147, NamespaceIndexes::SCADA};
const scada::NodeId DataItemType_Severity{144, NamespaceIndexes::SCADA};
const scada::NodeId DataItemType_Simulated{143, NamespaceIndexes::SCADA};
const scada::NodeId DataItemType_StalePeriod{149, NamespaceIndexes::SCADA};
const scada::NodeId DataItemType_WriteManual{230, NamespaceIndexes::SCADA};
const scada::NodeId DataItemType{239, NamespaceIndexes::SCADA};
const scada::NodeId DiscreteItemType_Inversion{153, NamespaceIndexes::SCADA};
const scada::NodeId DiscreteItemType{72, NamespaceIndexes::SCADA}; // :DataItemType
const scada::NodeId HasTsFormat{154, NamespaceIndexes::SCADA}; // Ref
const scada::NodeId HasSimulationSignal{150, NamespaceIndexes::SCADA}; // Ref
const scada::NodeId SimulationSignals{numeric_id::SimulationSignals, NamespaceIndexes::SCADA};
const scada::NodeId SimulationFunctionDataType{326, NamespaceIndexes::SCADA};
const scada::NodeId SimulationFunctionDataType_EnumStrings{327, NamespaceIndexes::SCADA};
const scada::NodeId SimulationSignalType_Period{173, NamespaceIndexes::SCADA};
const scada::NodeId SimulationSignalType_Phase{174, NamespaceIndexes::SCADA};
const scada::NodeId SimulationSignalType_Function{172, NamespaceIndexes::SCADA};
const scada::NodeId SimulationSignalType_UpdateInterval{175, NamespaceIndexes::SCADA};
const scada::NodeId SimulationSignalType{67, NamespaceIndexes::SCADA};
const scada::NodeId TsFormats{numeric_id::TsFormats, NamespaceIndexes::SCADA};
const scada::NodeId TsFormatType_CloseColor{219, NamespaceIndexes::SCADA};
const scada::NodeId TsFormatType_CloseLabel{217, NamespaceIndexes::SCADA};
const scada::NodeId TsFormatType_OpenColor{218, NamespaceIndexes::SCADA};
const scada::NodeId TsFormatType_OpenLabel{216, NamespaceIndexes::SCADA};
const scada::NodeId TsFormatType{80, NamespaceIndexes::SCADA};
const scada::NodeId Aliases{300, NamespaceIndexes::SCADA};
const scada::NodeId AliasType{301, NamespaceIndexes::SCADA};
const scada::NodeId AliasOf{302, NamespaceIndexes::SCADA};
const scada::NodeId Statistics_ServerCPUUsage{226, NamespaceIndexes::SCADA};
const scada::NodeId Statistics_ServerMemoryUsage{227, NamespaceIndexes::SCADA};
const scada::NodeId Statistics_TotalCPUUsage{228, NamespaceIndexes::SCADA};
const scada::NodeId Statistics_TotalMemoryUsage{229, NamespaceIndexes::SCADA};

}

}
