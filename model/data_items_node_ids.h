#pragma once

#include "core/standard_node_ids.h"
#include "model/namespaces.h"

namespace data_items {

constexpr scada::NamespaceIndex kNamespaceIndex = NamespaceIndexes::DATA_ITEMS;

namespace numeric_id {

constexpr scada::NumericId AliasOf = 15020; // ReferenceType
constexpr scada::NumericId AliasType = 15021; // ObjectType
constexpr scada::NumericId Aliases = 15019; // Object
constexpr scada::NumericId AnalogConversionDataType = 15038; // DataType
constexpr scada::NumericId AnalogConversionDataType_EnumStrings = 15039; // Variable
constexpr scada::NumericId AnalogItemType = 76; // VariableType
constexpr scada::NumericId AnalogItemType_Alias = 287; // Variable
constexpr scada::NumericId AnalogItemType_Aperture = 15003; // Variable
constexpr scada::NumericId AnalogItemType_Clamping = 191; // Variable
constexpr scada::NumericId AnalogItemType_Conversion = 190; // Variable
constexpr scada::NumericId AnalogItemType_Deadband = 15004; // Variable
constexpr scada::NumericId AnalogItemType_DisplayFormat = 189; // Variable
constexpr scada::NumericId AnalogItemType_EngineeringUnits = 200; // Variable
constexpr scada::NumericId AnalogItemType_EuHi = 193; // Variable
constexpr scada::NumericId AnalogItemType_EuLo = 192; // Variable
constexpr scada::NumericId AnalogItemType_Input1 = 289; // Variable
constexpr scada::NumericId AnalogItemType_Input2 = 290; // Variable
constexpr scada::NumericId AnalogItemType_IrHi = 195; // Variable
constexpr scada::NumericId AnalogItemType_IrLo = 194; // Variable
constexpr scada::NumericId AnalogItemType_LimitHi = 197; // Variable
constexpr scada::NumericId AnalogItemType_LimitHiHi = 199; // Variable
constexpr scada::NumericId AnalogItemType_LimitLo = 196; // Variable
constexpr scada::NumericId AnalogItemType_LimitLoLo = 198; // Variable
constexpr scada::NumericId AnalogItemType_Locked = 295; // Variable
constexpr scada::NumericId AnalogItemType_Output = 291; // Variable
constexpr scada::NumericId AnalogItemType_OutputCondition = 292; // Variable
constexpr scada::NumericId AnalogItemType_OutputTwoStaged = 15027; // Variable
constexpr scada::NumericId AnalogItemType_Severity = 288; // Variable
constexpr scada::NumericId AnalogItemType_Simulated = 294; // Variable
constexpr scada::NumericId AnalogItemType_StalePeriod = 293; // Variable
constexpr scada::NumericId AnalogItemType_WriteManual = 296; // Method
constexpr scada::NumericId DataGroupType = 62; // ObjectType
constexpr scada::NumericId DataGroupType_Simulated = 63; // Variable
constexpr scada::NumericId DataItemType = 18; // VariableType
constexpr scada::NumericId DataItemType_Alias = 19; // Variable
constexpr scada::NumericId DataItemType_Input1 = 175; // Variable
constexpr scada::NumericId DataItemType_Input2 = 176; // Variable
constexpr scada::NumericId DataItemType_Locked = 180; // Variable
constexpr scada::NumericId DataItemType_Output = 177; // Variable
constexpr scada::NumericId DataItemType_OutputCondition = 178; // Variable
constexpr scada::NumericId DataItemType_OutputTwoStaged = 15025; // Variable
constexpr scada::NumericId DataItemType_Severity = 174; // Variable
constexpr scada::NumericId DataItemType_Simulated = 66; // Variable
constexpr scada::NumericId DataItemType_StalePeriod = 179; // Variable
constexpr scada::NumericId DataItemType_WriteManual = 285; // Method
constexpr scada::NumericId DataItemWriteManualMethodType = 284; // Method
constexpr scada::NumericId DataItems = 24; // Object
constexpr scada::NumericId DiscreteItemType = 72; // VariableType
constexpr scada::NumericId DiscreteItemType_Alias = 73; // Variable
constexpr scada::NumericId DiscreteItemType_Input1 = 182; // Variable
constexpr scada::NumericId DiscreteItemType_Input2 = 183; // Variable
constexpr scada::NumericId DiscreteItemType_Inversion = 188; // Variable
constexpr scada::NumericId DiscreteItemType_Locked = 187; // Variable
constexpr scada::NumericId DiscreteItemType_Output = 184; // Variable
constexpr scada::NumericId DiscreteItemType_OutputCondition = 185; // Variable
constexpr scada::NumericId DiscreteItemType_OutputTwoStaged = 15026; // Variable
constexpr scada::NumericId DiscreteItemType_Severity = 181; // Variable
constexpr scada::NumericId DiscreteItemType_Simulated = 74; // Variable
constexpr scada::NumericId DiscreteItemType_StalePeriod = 186; // Variable
constexpr scada::NumericId DiscreteItemType_WriteManual = 286; // Method
constexpr scada::NumericId HasSimulationSignal = 69; // ReferenceType
constexpr scada::NumericId HasTsFormat = 71; // ReferenceType
constexpr scada::NumericId PropertyCategories = 15011; // Object
constexpr scada::NumericId PropertyCategories_Channels = 15012; // Object
constexpr scada::NumericId PropertyCategories_Conversion = 15013; // Object
constexpr scada::NumericId PropertyCategories_Display = 15015; // Object
constexpr scada::NumericId PropertyCategories_Filtering = 15014; // Object
constexpr scada::NumericId PropertyCategories_History = 15016; // Object
constexpr scada::NumericId PropertyCategories_Limits = 15018; // Object
constexpr scada::NumericId PropertyCategories_Simulation = 15017; // Object
constexpr scada::NumericId SimulationFunctionDataType = 15022; // DataType
constexpr scada::NumericId SimulationFunctionDataType_EnumStrings = 15023; // Variable
constexpr scada::NumericId SimulationSignalType = 67; // VariableType
constexpr scada::NumericId SimulationSignalType_Function = 15024; // Variable
constexpr scada::NumericId SimulationSignalType_Period = 230; // Variable
constexpr scada::NumericId SimulationSignalType_Phase = 231; // Variable
constexpr scada::NumericId SimulationSignalType_UpdateInterval = 232; // Variable
constexpr scada::NumericId SimulationSignals = 28; // Object
constexpr scada::NumericId Statistics = 363; // Object
constexpr scada::NumericId Statistics_ServerCPUUsage = 366; // Variable
constexpr scada::NumericId Statistics_ServerMemoryUsage = 367; // Variable
constexpr scada::NumericId Statistics_TotalCPUUsage = 368; // Variable
constexpr scada::NumericId Statistics_TotalMemoryUsage = 369; // Variable
constexpr scada::NumericId TelecontrolDataItems_BinarySchema = 15001; // Variable
constexpr scada::NumericId TelecontrolDataItems_BinarySchema_DataTypeVersion = 15002; // Variable
constexpr scada::NumericId TelecontrolDataItems_BinarySchema_Deprecated = 15006; // Variable
constexpr scada::NumericId TelecontrolDataItems_BinarySchema_NamespaceUri = 15005; // Variable
constexpr scada::NumericId TelecontrolDataItems_XmlSchema = 15007; // Variable
constexpr scada::NumericId TelecontrolDataItems_XmlSchema_DataTypeVersion = 15008; // Variable
constexpr scada::NumericId TelecontrolDataItems_XmlSchema_Deprecated = 15010; // Variable
constexpr scada::NumericId TelecontrolDataItems_XmlSchema_NamespaceUri = 15009; // Variable
constexpr scada::NumericId TsFormatType = 80; // ObjectType
constexpr scada::NumericId TsFormatType_CloseColor = 84; // Variable
constexpr scada::NumericId TsFormatType_CloseLabel = 82; // Variable
constexpr scada::NumericId TsFormatType_OpenColor = 83; // Variable
constexpr scada::NumericId TsFormatType_OpenLabel = 81; // Variable
constexpr scada::NumericId TsFormats = 27; // Object

} //  namespace numeric_id

namespace id {

const scada::NodeId AliasOf{numeric_id::AliasOf, kNamespaceIndex}; // ReferenceType
const scada::NodeId AliasType{numeric_id::AliasType, kNamespaceIndex}; // ObjectType
const scada::NodeId Aliases{numeric_id::Aliases, kNamespaceIndex}; // Object
const scada::NodeId AnalogConversionDataType{numeric_id::AnalogConversionDataType, kNamespaceIndex}; // DataType
const scada::NodeId AnalogConversionDataType_EnumStrings{numeric_id::AnalogConversionDataType_EnumStrings, kNamespaceIndex}; // Variable
const scada::NodeId AnalogItemType{numeric_id::AnalogItemType, kNamespaceIndex}; // VariableType
const scada::NodeId AnalogItemType_Alias{numeric_id::AnalogItemType_Alias, kNamespaceIndex}; // Variable
const scada::NodeId AnalogItemType_Aperture{numeric_id::AnalogItemType_Aperture, kNamespaceIndex}; // Variable
const scada::NodeId AnalogItemType_Clamping{numeric_id::AnalogItemType_Clamping, kNamespaceIndex}; // Variable
const scada::NodeId AnalogItemType_Conversion{numeric_id::AnalogItemType_Conversion, kNamespaceIndex}; // Variable
const scada::NodeId AnalogItemType_Deadband{numeric_id::AnalogItemType_Deadband, kNamespaceIndex}; // Variable
const scada::NodeId AnalogItemType_DisplayFormat{numeric_id::AnalogItemType_DisplayFormat, kNamespaceIndex}; // Variable
const scada::NodeId AnalogItemType_EngineeringUnits{numeric_id::AnalogItemType_EngineeringUnits, kNamespaceIndex}; // Variable
const scada::NodeId AnalogItemType_EuHi{numeric_id::AnalogItemType_EuHi, kNamespaceIndex}; // Variable
const scada::NodeId AnalogItemType_EuLo{numeric_id::AnalogItemType_EuLo, kNamespaceIndex}; // Variable
const scada::NodeId AnalogItemType_Input1{numeric_id::AnalogItemType_Input1, kNamespaceIndex}; // Variable
const scada::NodeId AnalogItemType_Input2{numeric_id::AnalogItemType_Input2, kNamespaceIndex}; // Variable
const scada::NodeId AnalogItemType_IrHi{numeric_id::AnalogItemType_IrHi, kNamespaceIndex}; // Variable
const scada::NodeId AnalogItemType_IrLo{numeric_id::AnalogItemType_IrLo, kNamespaceIndex}; // Variable
const scada::NodeId AnalogItemType_LimitHi{numeric_id::AnalogItemType_LimitHi, kNamespaceIndex}; // Variable
const scada::NodeId AnalogItemType_LimitHiHi{numeric_id::AnalogItemType_LimitHiHi, kNamespaceIndex}; // Variable
const scada::NodeId AnalogItemType_LimitLo{numeric_id::AnalogItemType_LimitLo, kNamespaceIndex}; // Variable
const scada::NodeId AnalogItemType_LimitLoLo{numeric_id::AnalogItemType_LimitLoLo, kNamespaceIndex}; // Variable
const scada::NodeId AnalogItemType_Locked{numeric_id::AnalogItemType_Locked, kNamespaceIndex}; // Variable
const scada::NodeId AnalogItemType_Output{numeric_id::AnalogItemType_Output, kNamespaceIndex}; // Variable
const scada::NodeId AnalogItemType_OutputCondition{numeric_id::AnalogItemType_OutputCondition, kNamespaceIndex}; // Variable
const scada::NodeId AnalogItemType_OutputTwoStaged{numeric_id::AnalogItemType_OutputTwoStaged, kNamespaceIndex}; // Variable
const scada::NodeId AnalogItemType_Severity{numeric_id::AnalogItemType_Severity, kNamespaceIndex}; // Variable
const scada::NodeId AnalogItemType_Simulated{numeric_id::AnalogItemType_Simulated, kNamespaceIndex}; // Variable
const scada::NodeId AnalogItemType_StalePeriod{numeric_id::AnalogItemType_StalePeriod, kNamespaceIndex}; // Variable
const scada::NodeId AnalogItemType_WriteManual{numeric_id::AnalogItemType_WriteManual, kNamespaceIndex}; // Method
const scada::NodeId DataGroupType{numeric_id::DataGroupType, kNamespaceIndex}; // ObjectType
const scada::NodeId DataGroupType_Simulated{numeric_id::DataGroupType_Simulated, kNamespaceIndex}; // Variable
const scada::NodeId DataItemType{numeric_id::DataItemType, kNamespaceIndex}; // VariableType
const scada::NodeId DataItemType_Alias{numeric_id::DataItemType_Alias, kNamespaceIndex}; // Variable
const scada::NodeId DataItemType_Input1{numeric_id::DataItemType_Input1, kNamespaceIndex}; // Variable
const scada::NodeId DataItemType_Input2{numeric_id::DataItemType_Input2, kNamespaceIndex}; // Variable
const scada::NodeId DataItemType_Locked{numeric_id::DataItemType_Locked, kNamespaceIndex}; // Variable
const scada::NodeId DataItemType_Output{numeric_id::DataItemType_Output, kNamespaceIndex}; // Variable
const scada::NodeId DataItemType_OutputCondition{numeric_id::DataItemType_OutputCondition, kNamespaceIndex}; // Variable
const scada::NodeId DataItemType_OutputTwoStaged{numeric_id::DataItemType_OutputTwoStaged, kNamespaceIndex}; // Variable
const scada::NodeId DataItemType_Severity{numeric_id::DataItemType_Severity, kNamespaceIndex}; // Variable
const scada::NodeId DataItemType_Simulated{numeric_id::DataItemType_Simulated, kNamespaceIndex}; // Variable
const scada::NodeId DataItemType_StalePeriod{numeric_id::DataItemType_StalePeriod, kNamespaceIndex}; // Variable
const scada::NodeId DataItemType_WriteManual{numeric_id::DataItemType_WriteManual, kNamespaceIndex}; // Method
const scada::NodeId DataItemWriteManualMethodType{numeric_id::DataItemWriteManualMethodType, kNamespaceIndex}; // Method
const scada::NodeId DataItems{numeric_id::DataItems, kNamespaceIndex}; // Object
const scada::NodeId DiscreteItemType{numeric_id::DiscreteItemType, kNamespaceIndex}; // VariableType
const scada::NodeId DiscreteItemType_Alias{numeric_id::DiscreteItemType_Alias, kNamespaceIndex}; // Variable
const scada::NodeId DiscreteItemType_Input1{numeric_id::DiscreteItemType_Input1, kNamespaceIndex}; // Variable
const scada::NodeId DiscreteItemType_Input2{numeric_id::DiscreteItemType_Input2, kNamespaceIndex}; // Variable
const scada::NodeId DiscreteItemType_Inversion{numeric_id::DiscreteItemType_Inversion, kNamespaceIndex}; // Variable
const scada::NodeId DiscreteItemType_Locked{numeric_id::DiscreteItemType_Locked, kNamespaceIndex}; // Variable
const scada::NodeId DiscreteItemType_Output{numeric_id::DiscreteItemType_Output, kNamespaceIndex}; // Variable
const scada::NodeId DiscreteItemType_OutputCondition{numeric_id::DiscreteItemType_OutputCondition, kNamespaceIndex}; // Variable
const scada::NodeId DiscreteItemType_OutputTwoStaged{numeric_id::DiscreteItemType_OutputTwoStaged, kNamespaceIndex}; // Variable
const scada::NodeId DiscreteItemType_Severity{numeric_id::DiscreteItemType_Severity, kNamespaceIndex}; // Variable
const scada::NodeId DiscreteItemType_Simulated{numeric_id::DiscreteItemType_Simulated, kNamespaceIndex}; // Variable
const scada::NodeId DiscreteItemType_StalePeriod{numeric_id::DiscreteItemType_StalePeriod, kNamespaceIndex}; // Variable
const scada::NodeId DiscreteItemType_WriteManual{numeric_id::DiscreteItemType_WriteManual, kNamespaceIndex}; // Method
const scada::NodeId HasSimulationSignal{numeric_id::HasSimulationSignal, kNamespaceIndex}; // ReferenceType
const scada::NodeId HasTsFormat{numeric_id::HasTsFormat, kNamespaceIndex}; // ReferenceType
const scada::NodeId PropertyCategories{numeric_id::PropertyCategories, kNamespaceIndex}; // Object
const scada::NodeId PropertyCategories_Channels{numeric_id::PropertyCategories_Channels, kNamespaceIndex}; // Object
const scada::NodeId PropertyCategories_Conversion{numeric_id::PropertyCategories_Conversion, kNamespaceIndex}; // Object
const scada::NodeId PropertyCategories_Display{numeric_id::PropertyCategories_Display, kNamespaceIndex}; // Object
const scada::NodeId PropertyCategories_Filtering{numeric_id::PropertyCategories_Filtering, kNamespaceIndex}; // Object
const scada::NodeId PropertyCategories_History{numeric_id::PropertyCategories_History, kNamespaceIndex}; // Object
const scada::NodeId PropertyCategories_Limits{numeric_id::PropertyCategories_Limits, kNamespaceIndex}; // Object
const scada::NodeId PropertyCategories_Simulation{numeric_id::PropertyCategories_Simulation, kNamespaceIndex}; // Object
const scada::NodeId SimulationFunctionDataType{numeric_id::SimulationFunctionDataType, kNamespaceIndex}; // DataType
const scada::NodeId SimulationFunctionDataType_EnumStrings{numeric_id::SimulationFunctionDataType_EnumStrings, kNamespaceIndex}; // Variable
const scada::NodeId SimulationSignalType{numeric_id::SimulationSignalType, kNamespaceIndex}; // VariableType
const scada::NodeId SimulationSignalType_Function{numeric_id::SimulationSignalType_Function, kNamespaceIndex}; // Variable
const scada::NodeId SimulationSignalType_Period{numeric_id::SimulationSignalType_Period, kNamespaceIndex}; // Variable
const scada::NodeId SimulationSignalType_Phase{numeric_id::SimulationSignalType_Phase, kNamespaceIndex}; // Variable
const scada::NodeId SimulationSignalType_UpdateInterval{numeric_id::SimulationSignalType_UpdateInterval, kNamespaceIndex}; // Variable
const scada::NodeId SimulationSignals{numeric_id::SimulationSignals, kNamespaceIndex}; // Object
const scada::NodeId Statistics{numeric_id::Statistics, kNamespaceIndex}; // Object
const scada::NodeId Statistics_ServerCPUUsage{numeric_id::Statistics_ServerCPUUsage, kNamespaceIndex}; // Variable
const scada::NodeId Statistics_ServerMemoryUsage{numeric_id::Statistics_ServerMemoryUsage, kNamespaceIndex}; // Variable
const scada::NodeId Statistics_TotalCPUUsage{numeric_id::Statistics_TotalCPUUsage, kNamespaceIndex}; // Variable
const scada::NodeId Statistics_TotalMemoryUsage{numeric_id::Statistics_TotalMemoryUsage, kNamespaceIndex}; // Variable
const scada::NodeId TelecontrolDataItems_BinarySchema{numeric_id::TelecontrolDataItems_BinarySchema, kNamespaceIndex}; // Variable
const scada::NodeId TelecontrolDataItems_BinarySchema_DataTypeVersion{numeric_id::TelecontrolDataItems_BinarySchema_DataTypeVersion, kNamespaceIndex}; // Variable
const scada::NodeId TelecontrolDataItems_BinarySchema_Deprecated{numeric_id::TelecontrolDataItems_BinarySchema_Deprecated, kNamespaceIndex}; // Variable
const scada::NodeId TelecontrolDataItems_BinarySchema_NamespaceUri{numeric_id::TelecontrolDataItems_BinarySchema_NamespaceUri, kNamespaceIndex}; // Variable
const scada::NodeId TelecontrolDataItems_XmlSchema{numeric_id::TelecontrolDataItems_XmlSchema, kNamespaceIndex}; // Variable
const scada::NodeId TelecontrolDataItems_XmlSchema_DataTypeVersion{numeric_id::TelecontrolDataItems_XmlSchema_DataTypeVersion, kNamespaceIndex}; // Variable
const scada::NodeId TelecontrolDataItems_XmlSchema_Deprecated{numeric_id::TelecontrolDataItems_XmlSchema_Deprecated, kNamespaceIndex}; // Variable
const scada::NodeId TelecontrolDataItems_XmlSchema_NamespaceUri{numeric_id::TelecontrolDataItems_XmlSchema_NamespaceUri, kNamespaceIndex}; // Variable
const scada::NodeId TsFormatType{numeric_id::TsFormatType, kNamespaceIndex}; // ObjectType
const scada::NodeId TsFormatType_CloseColor{numeric_id::TsFormatType_CloseColor, kNamespaceIndex}; // Variable
const scada::NodeId TsFormatType_CloseLabel{numeric_id::TsFormatType_CloseLabel, kNamespaceIndex}; // Variable
const scada::NodeId TsFormatType_OpenColor{numeric_id::TsFormatType_OpenColor, kNamespaceIndex}; // Variable
const scada::NodeId TsFormatType_OpenLabel{numeric_id::TsFormatType_OpenLabel, kNamespaceIndex}; // Variable
const scada::NodeId TsFormats{numeric_id::TsFormats, kNamespaceIndex}; // Object

} //  namespace id

} //  namespace data_items

