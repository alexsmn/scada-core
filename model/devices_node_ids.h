#pragma once

#include "core/standard_node_ids.h"
#include "model/namespaces.h"

namespace devices {

constexpr scada::NamespaceIndex kNamespaceIndex = NamespaceIndexes::DEVICES;

namespace numeric_id {

constexpr scada::NumericId DeviceInterrogateChannelMethodType = 277; // Method
constexpr scada::NumericId DeviceInterrogateMethodType = 233; // Method
constexpr scada::NumericId DeviceSelectMethodType = 274; // Method
constexpr scada::NumericId DeviceSyncClockMethodType = 234; // Method
constexpr scada::NumericId DeviceType = 31; // ObjectType
constexpr scada::NumericId DeviceType_Disabled = 201; // Variable
constexpr scada::NumericId DeviceType_Enabled = 203; // Variable
constexpr scada::NumericId DeviceType_Interrogate = 235; // Method
constexpr scada::NumericId DeviceType_InterrogateChannel = 278; // Method
constexpr scada::NumericId DeviceType_Online = 202; // Variable
constexpr scada::NumericId DeviceType_Select = 256; // Method
constexpr scada::NumericId DeviceType_SyncClock = 236; // Method
constexpr scada::NumericId DeviceType_Write = 257; // Method
constexpr scada::NumericId DeviceType_WriteParam = 258; // Method
constexpr scada::NumericId DeviceWatchEventType = 219; // ObjectType
constexpr scada::NumericId DeviceWatchEventType_EventId = 220; // Variable
constexpr scada::NumericId DeviceWatchEventType_EventType = 221; // Variable
constexpr scada::NumericId DeviceWatchEventType_LocalTime = 226; // Variable
constexpr scada::NumericId DeviceWatchEventType_Message = 227; // Variable
constexpr scada::NumericId DeviceWatchEventType_ReceiveTime = 225; // Variable
constexpr scada::NumericId DeviceWatchEventType_Severity = 228; // Variable
constexpr scada::NumericId DeviceWatchEventType_SourceName = 223; // Variable
constexpr scada::NumericId DeviceWatchEventType_SourceNode = 222; // Variable
constexpr scada::NumericId DeviceWatchEventType_Time = 224; // Variable
constexpr scada::NumericId DeviceWriteMethodType = 275; // Method
constexpr scada::NumericId DeviceWriteParamMethodType = 276; // Method
constexpr scada::NumericId Devices = 30; // Object
constexpr scada::NumericId HasTransmissionSource = 172; // ReferenceType
constexpr scada::NumericId HasTransmissionTarget = 173; // ReferenceType
constexpr scada::NumericId Iec60870DeviceType = 331; // ObjectType
constexpr scada::NumericId Iec60870DeviceType_Address = 341; // Variable
constexpr scada::NumericId Iec60870DeviceType_ClockSyncPeriod = 346; // Variable
constexpr scada::NumericId Iec60870DeviceType_Disabled = 332; // Variable
constexpr scada::NumericId Iec60870DeviceType_Enabled = 334; // Variable
constexpr scada::NumericId Iec60870DeviceType_Interrogate = 335; // Method
constexpr scada::NumericId Iec60870DeviceType_InterrogateChannel = 336; // Method
constexpr scada::NumericId Iec60870DeviceType_InterrogationPeriod = 344; // Variable
constexpr scada::NumericId Iec60870DeviceType_InterrogationPeriodGroup1 = 347; // Variable
constexpr scada::NumericId Iec60870DeviceType_InterrogationPeriodGroup10 = 356; // Variable
constexpr scada::NumericId Iec60870DeviceType_InterrogationPeriodGroup11 = 357; // Variable
constexpr scada::NumericId Iec60870DeviceType_InterrogationPeriodGroup12 = 358; // Variable
constexpr scada::NumericId Iec60870DeviceType_InterrogationPeriodGroup13 = 359; // Variable
constexpr scada::NumericId Iec60870DeviceType_InterrogationPeriodGroup14 = 360; // Variable
constexpr scada::NumericId Iec60870DeviceType_InterrogationPeriodGroup15 = 361; // Variable
constexpr scada::NumericId Iec60870DeviceType_InterrogationPeriodGroup16 = 362; // Variable
constexpr scada::NumericId Iec60870DeviceType_InterrogationPeriodGroup2 = 348; // Variable
constexpr scada::NumericId Iec60870DeviceType_InterrogationPeriodGroup3 = 349; // Variable
constexpr scada::NumericId Iec60870DeviceType_InterrogationPeriodGroup4 = 350; // Variable
constexpr scada::NumericId Iec60870DeviceType_InterrogationPeriodGroup5 = 351; // Variable
constexpr scada::NumericId Iec60870DeviceType_InterrogationPeriodGroup6 = 352; // Variable
constexpr scada::NumericId Iec60870DeviceType_InterrogationPeriodGroup7 = 353; // Variable
constexpr scada::NumericId Iec60870DeviceType_InterrogationPeriodGroup8 = 354; // Variable
constexpr scada::NumericId Iec60870DeviceType_InterrogationPeriodGroup9 = 355; // Variable
constexpr scada::NumericId Iec60870DeviceType_LinkAddress = 342; // Variable
constexpr scada::NumericId Iec60870DeviceType_Online = 333; // Variable
constexpr scada::NumericId Iec60870DeviceType_Select = 338; // Method
constexpr scada::NumericId Iec60870DeviceType_StartupClockSync = 345; // Variable
constexpr scada::NumericId Iec60870DeviceType_StartupInterrogation = 343; // Variable
constexpr scada::NumericId Iec60870DeviceType_SyncClock = 337; // Method
constexpr scada::NumericId Iec60870DeviceType_UtcTime = 15010; // Variable
constexpr scada::NumericId Iec60870DeviceType_Write = 339; // Method
constexpr scada::NumericId Iec60870DeviceType_WriteParam = 340; // Method
constexpr scada::NumericId Iec60870LinkType = 297; // ObjectType
constexpr scada::NumericId Iec60870LinkType_ActiveConnections = 309; // Variable
constexpr scada::NumericId Iec60870LinkType_AnonymousMode = 330; // Variable
constexpr scada::NumericId Iec60870LinkType_BytesIn = 313; // Variable
constexpr scada::NumericId Iec60870LinkType_BytesOut = 312; // Variable
constexpr scada::NumericId Iec60870LinkType_COTSize = 322; // Variable
constexpr scada::NumericId Iec60870LinkType_CRCProtection = 326; // Variable
constexpr scada::NumericId Iec60870LinkType_ConfirmationTimeout = 319; // Variable
constexpr scada::NumericId Iec60870LinkType_ConnectCount = 308; // Variable
constexpr scada::NumericId Iec60870LinkType_ConnectTimeout = 318; // Variable
constexpr scada::NumericId Iec60870LinkType_DataCollection = 324; // Variable
constexpr scada::NumericId Iec60870LinkType_DeviceAddressSize = 321; // Variable
constexpr scada::NumericId Iec60870LinkType_Disabled = 298; // Variable
constexpr scada::NumericId Iec60870LinkType_Enabled = 300; // Variable
constexpr scada::NumericId Iec60870LinkType_IdleTimeout = 329; // Variable
constexpr scada::NumericId Iec60870LinkType_InfoAddressSize = 323; // Variable
constexpr scada::NumericId Iec60870LinkType_Interrogate = 301; // Method
constexpr scada::NumericId Iec60870LinkType_InterrogateChannel = 302; // Method
constexpr scada::NumericId Iec60870LinkType_MessagesIn = 311; // Variable
constexpr scada::NumericId Iec60870LinkType_MessagesOut = 310; // Variable
constexpr scada::NumericId Iec60870LinkType_Mode = 315; // Variable
constexpr scada::NumericId Iec60870LinkType_Online = 299; // Variable
constexpr scada::NumericId Iec60870LinkType_Protocol = 314; // Variable
constexpr scada::NumericId Iec60870LinkType_ReceiveQueueSize = 317; // Variable
constexpr scada::NumericId Iec60870LinkType_ReceiveTimeout = 328; // Variable
constexpr scada::NumericId Iec60870LinkType_Select = 304; // Method
constexpr scada::NumericId Iec60870LinkType_SendQueueSize = 316; // Variable
constexpr scada::NumericId Iec60870LinkType_SendRetryCount = 325; // Variable
constexpr scada::NumericId Iec60870LinkType_SendTimeout = 327; // Variable
constexpr scada::NumericId Iec60870LinkType_SyncClock = 303; // Method
constexpr scada::NumericId Iec60870LinkType_TerminationTimeout = 320; // Variable
constexpr scada::NumericId Iec60870LinkType_Transport = 307; // Variable
constexpr scada::NumericId Iec60870LinkType_Write = 305; // Method
constexpr scada::NumericId Iec60870LinkType_WriteParam = 306; // Method
constexpr scada::NumericId Iec60870ModeDataType = 15025; // DataType
constexpr scada::NumericId Iec60870ModeDataType_EnumStrings = 15026; // Variable
constexpr scada::NumericId Iec60870ProtocolDataType = 15023; // DataType
constexpr scada::NumericId Iec60870ProtocolDataType_EnumStrings = 15024; // Variable
constexpr scada::NumericId Iec61850ConfigurableObjectType = 384; // ObjectType
constexpr scada::NumericId Iec61850ConfigurableObjectType_Reference = 385; // Variable
constexpr scada::NumericId Iec61850ControlObjectType = 15012; // ObjectType
constexpr scada::NumericId Iec61850DataVariableType = 15011; // VariableType
constexpr scada::NumericId Iec61850DeviceType = 371; // ObjectType
constexpr scada::NumericId Iec61850DeviceType_Disabled = 372; // Variable
constexpr scada::NumericId Iec61850DeviceType_Enabled = 374; // Variable
constexpr scada::NumericId Iec61850DeviceType_Host = 382; // Variable
constexpr scada::NumericId Iec61850DeviceType_Interrogate = 375; // Method
constexpr scada::NumericId Iec61850DeviceType_InterrogateChannel = 376; // Method
constexpr scada::NumericId Iec61850DeviceType_Model = 381; // Object
constexpr scada::NumericId Iec61850DeviceType_Online = 373; // Variable
constexpr scada::NumericId Iec61850DeviceType_Port = 383; // Variable
constexpr scada::NumericId Iec61850DeviceType_Select = 378; // Method
constexpr scada::NumericId Iec61850DeviceType_SyncClock = 377; // Method
constexpr scada::NumericId Iec61850DeviceType_Write = 379; // Method
constexpr scada::NumericId Iec61850DeviceType_WriteParam = 380; // Method
constexpr scada::NumericId Iec61850LogicalNodeType = 370; // ObjectType
constexpr scada::NumericId Iec61850RcbType = 386; // ObjectType
constexpr scada::NumericId Iec61850RcbType_Reference = 387; // Variable
constexpr scada::NumericId Iec61850ServerType = 15003; // ObjectType
constexpr scada::NumericId Iec61850ServerType_Disabled = 15004; // Variable
constexpr scada::NumericId Iec61850ServerType_Enabled = 15014; // Variable
constexpr scada::NumericId Iec61850ServerType_Host = 15021; // Variable
constexpr scada::NumericId Iec61850ServerType_Interrogate = 15015; // Method
constexpr scada::NumericId Iec61850ServerType_InterrogateChannel = 15016; // Method
constexpr scada::NumericId Iec61850ServerType_Online = 15013; // Variable
constexpr scada::NumericId Iec61850ServerType_Port = 15022; // Variable
constexpr scada::NumericId Iec61850ServerType_Select = 15018; // Method
constexpr scada::NumericId Iec61850ServerType_SyncClock = 15017; // Method
constexpr scada::NumericId Iec61850ServerType_Write = 15019; // Method
constexpr scada::NumericId Iec61850ServerType_WriteParam = 15020; // Method
constexpr scada::NumericId LinkType = 40; // ObjectType
constexpr scada::NumericId LinkType_ActiveConnections = 44; // Variable
constexpr scada::NumericId LinkType_BytesIn = 48; // Variable
constexpr scada::NumericId LinkType_BytesOut = 47; // Variable
constexpr scada::NumericId LinkType_ConnectCount = 43; // Variable
constexpr scada::NumericId LinkType_Disabled = 204; // Variable
constexpr scada::NumericId LinkType_Enabled = 206; // Variable
constexpr scada::NumericId LinkType_Interrogate = 237; // Method
constexpr scada::NumericId LinkType_InterrogateChannel = 279; // Method
constexpr scada::NumericId LinkType_MessagesIn = 46; // Variable
constexpr scada::NumericId LinkType_MessagesOut = 45; // Variable
constexpr scada::NumericId LinkType_Online = 205; // Variable
constexpr scada::NumericId LinkType_Select = 259; // Method
constexpr scada::NumericId LinkType_SyncClock = 238; // Method
constexpr scada::NumericId LinkType_Transport = 42; // Variable
constexpr scada::NumericId LinkType_Write = 260; // Method
constexpr scada::NumericId LinkType_WriteParam = 261; // Method
constexpr scada::NumericId ModbusDeviceType = 118; // ObjectType
constexpr scada::NumericId ModbusDeviceType_Address = 120; // Variable
constexpr scada::NumericId ModbusDeviceType_Disabled = 216; // Variable
constexpr scada::NumericId ModbusDeviceType_Enabled = 218; // Variable
constexpr scada::NumericId ModbusDeviceType_Interrogate = 245; // Method
constexpr scada::NumericId ModbusDeviceType_InterrogateChannel = 283; // Method
constexpr scada::NumericId ModbusDeviceType_Online = 217; // Variable
constexpr scada::NumericId ModbusDeviceType_ResponseTimeout = 15009; // Variable
constexpr scada::NumericId ModbusDeviceType_Select = 271; // Method
constexpr scada::NumericId ModbusDeviceType_SendRetryCount = 121; // Variable
constexpr scada::NumericId ModbusDeviceType_SyncClock = 246; // Method
constexpr scada::NumericId ModbusDeviceType_Write = 272; // Method
constexpr scada::NumericId ModbusDeviceType_WriteParam = 273; // Method
constexpr scada::NumericId ModbusLinkProtocol = 15005; // DataType
constexpr scada::NumericId ModbusLinkProtocol_EnumStrings = 15008; // Variable
constexpr scada::NumericId ModbusLinkType = 108; // ObjectType
constexpr scada::NumericId ModbusLinkType_ActiveConnections = 112; // Variable
constexpr scada::NumericId ModbusLinkType_BytesIn = 116; // Variable
constexpr scada::NumericId ModbusLinkType_BytesOut = 115; // Variable
constexpr scada::NumericId ModbusLinkType_ConnectCount = 111; // Variable
constexpr scada::NumericId ModbusLinkType_Disabled = 213; // Variable
constexpr scada::NumericId ModbusLinkType_Enabled = 215; // Variable
constexpr scada::NumericId ModbusLinkType_Interrogate = 243; // Method
constexpr scada::NumericId ModbusLinkType_InterrogateChannel = 282; // Method
constexpr scada::NumericId ModbusLinkType_MessagesIn = 114; // Variable
constexpr scada::NumericId ModbusLinkType_MessagesOut = 113; // Variable
constexpr scada::NumericId ModbusLinkType_Online = 214; // Variable
constexpr scada::NumericId ModbusLinkType_Protocol = 15007; // Variable
constexpr scada::NumericId ModbusLinkType_RequestDelay = 15006; // Variable
constexpr scada::NumericId ModbusLinkType_Select = 268; // Method
constexpr scada::NumericId ModbusLinkType_SyncClock = 244; // Method
constexpr scada::NumericId ModbusLinkType_Transport = 110; // Variable
constexpr scada::NumericId ModbusLinkType_Write = 269; // Method
constexpr scada::NumericId ModbusLinkType_WriteParam = 270; // Method
constexpr scada::NumericId TelecontrolDevices_BinarySchema = 15001; // Variable
constexpr scada::NumericId TelecontrolDevices_BinarySchema_DataTypeVersion = 15002; // Variable
constexpr scada::NumericId TelecontrolDevices_BinarySchema_Deprecated = 15041; // Variable
constexpr scada::NumericId TelecontrolDevices_BinarySchema_NamespaceUri = 15040; // Variable
constexpr scada::NumericId TelecontrolDevices_XmlSchema = 15042; // Variable
constexpr scada::NumericId TelecontrolDevices_XmlSchema_DataTypeVersion = 15043; // Variable
constexpr scada::NumericId TelecontrolDevices_XmlSchema_Deprecated = 15045; // Variable
constexpr scada::NumericId TelecontrolDevices_XmlSchema_NamespaceUri = 15044; // Variable
constexpr scada::NumericId TransmissionItemType = 34; // ObjectType
constexpr scada::NumericId TransmissionItemType_SourceAddress = 35; // Variable
constexpr scada::NumericId TransmissionItems = 33; // Object

} //  namespace numeric_id

namespace id {

const scada::NodeId DeviceInterrogateChannelMethodType{numeric_id::DeviceInterrogateChannelMethodType, kNamespaceIndex}; // Method
const scada::NodeId DeviceInterrogateMethodType{numeric_id::DeviceInterrogateMethodType, kNamespaceIndex}; // Method
const scada::NodeId DeviceSelectMethodType{numeric_id::DeviceSelectMethodType, kNamespaceIndex}; // Method
const scada::NodeId DeviceSyncClockMethodType{numeric_id::DeviceSyncClockMethodType, kNamespaceIndex}; // Method
const scada::NodeId DeviceType{numeric_id::DeviceType, kNamespaceIndex}; // ObjectType
const scada::NodeId DeviceType_Disabled{numeric_id::DeviceType_Disabled, kNamespaceIndex}; // Variable
const scada::NodeId DeviceType_Enabled{numeric_id::DeviceType_Enabled, kNamespaceIndex}; // Variable
const scada::NodeId DeviceType_Interrogate{numeric_id::DeviceType_Interrogate, kNamespaceIndex}; // Method
const scada::NodeId DeviceType_InterrogateChannel{numeric_id::DeviceType_InterrogateChannel, kNamespaceIndex}; // Method
const scada::NodeId DeviceType_Online{numeric_id::DeviceType_Online, kNamespaceIndex}; // Variable
const scada::NodeId DeviceType_Select{numeric_id::DeviceType_Select, kNamespaceIndex}; // Method
const scada::NodeId DeviceType_SyncClock{numeric_id::DeviceType_SyncClock, kNamespaceIndex}; // Method
const scada::NodeId DeviceType_Write{numeric_id::DeviceType_Write, kNamespaceIndex}; // Method
const scada::NodeId DeviceType_WriteParam{numeric_id::DeviceType_WriteParam, kNamespaceIndex}; // Method
const scada::NodeId DeviceWatchEventType{numeric_id::DeviceWatchEventType, kNamespaceIndex}; // ObjectType
const scada::NodeId DeviceWatchEventType_EventId{numeric_id::DeviceWatchEventType_EventId, kNamespaceIndex}; // Variable
const scada::NodeId DeviceWatchEventType_EventType{numeric_id::DeviceWatchEventType_EventType, kNamespaceIndex}; // Variable
const scada::NodeId DeviceWatchEventType_LocalTime{numeric_id::DeviceWatchEventType_LocalTime, kNamespaceIndex}; // Variable
const scada::NodeId DeviceWatchEventType_Message{numeric_id::DeviceWatchEventType_Message, kNamespaceIndex}; // Variable
const scada::NodeId DeviceWatchEventType_ReceiveTime{numeric_id::DeviceWatchEventType_ReceiveTime, kNamespaceIndex}; // Variable
const scada::NodeId DeviceWatchEventType_Severity{numeric_id::DeviceWatchEventType_Severity, kNamespaceIndex}; // Variable
const scada::NodeId DeviceWatchEventType_SourceName{numeric_id::DeviceWatchEventType_SourceName, kNamespaceIndex}; // Variable
const scada::NodeId DeviceWatchEventType_SourceNode{numeric_id::DeviceWatchEventType_SourceNode, kNamespaceIndex}; // Variable
const scada::NodeId DeviceWatchEventType_Time{numeric_id::DeviceWatchEventType_Time, kNamespaceIndex}; // Variable
const scada::NodeId DeviceWriteMethodType{numeric_id::DeviceWriteMethodType, kNamespaceIndex}; // Method
const scada::NodeId DeviceWriteParamMethodType{numeric_id::DeviceWriteParamMethodType, kNamespaceIndex}; // Method
const scada::NodeId Devices{numeric_id::Devices, kNamespaceIndex}; // Object
const scada::NodeId HasTransmissionSource{numeric_id::HasTransmissionSource, kNamespaceIndex}; // ReferenceType
const scada::NodeId HasTransmissionTarget{numeric_id::HasTransmissionTarget, kNamespaceIndex}; // ReferenceType
const scada::NodeId Iec60870DeviceType{numeric_id::Iec60870DeviceType, kNamespaceIndex}; // ObjectType
const scada::NodeId Iec60870DeviceType_Address{numeric_id::Iec60870DeviceType_Address, kNamespaceIndex}; // Variable
const scada::NodeId Iec60870DeviceType_ClockSyncPeriod{numeric_id::Iec60870DeviceType_ClockSyncPeriod, kNamespaceIndex}; // Variable
const scada::NodeId Iec60870DeviceType_Disabled{numeric_id::Iec60870DeviceType_Disabled, kNamespaceIndex}; // Variable
const scada::NodeId Iec60870DeviceType_Enabled{numeric_id::Iec60870DeviceType_Enabled, kNamespaceIndex}; // Variable
const scada::NodeId Iec60870DeviceType_Interrogate{numeric_id::Iec60870DeviceType_Interrogate, kNamespaceIndex}; // Method
const scada::NodeId Iec60870DeviceType_InterrogateChannel{numeric_id::Iec60870DeviceType_InterrogateChannel, kNamespaceIndex}; // Method
const scada::NodeId Iec60870DeviceType_InterrogationPeriod{numeric_id::Iec60870DeviceType_InterrogationPeriod, kNamespaceIndex}; // Variable
const scada::NodeId Iec60870DeviceType_InterrogationPeriodGroup1{numeric_id::Iec60870DeviceType_InterrogationPeriodGroup1, kNamespaceIndex}; // Variable
const scada::NodeId Iec60870DeviceType_InterrogationPeriodGroup10{numeric_id::Iec60870DeviceType_InterrogationPeriodGroup10, kNamespaceIndex}; // Variable
const scada::NodeId Iec60870DeviceType_InterrogationPeriodGroup11{numeric_id::Iec60870DeviceType_InterrogationPeriodGroup11, kNamespaceIndex}; // Variable
const scada::NodeId Iec60870DeviceType_InterrogationPeriodGroup12{numeric_id::Iec60870DeviceType_InterrogationPeriodGroup12, kNamespaceIndex}; // Variable
const scada::NodeId Iec60870DeviceType_InterrogationPeriodGroup13{numeric_id::Iec60870DeviceType_InterrogationPeriodGroup13, kNamespaceIndex}; // Variable
const scada::NodeId Iec60870DeviceType_InterrogationPeriodGroup14{numeric_id::Iec60870DeviceType_InterrogationPeriodGroup14, kNamespaceIndex}; // Variable
const scada::NodeId Iec60870DeviceType_InterrogationPeriodGroup15{numeric_id::Iec60870DeviceType_InterrogationPeriodGroup15, kNamespaceIndex}; // Variable
const scada::NodeId Iec60870DeviceType_InterrogationPeriodGroup16{numeric_id::Iec60870DeviceType_InterrogationPeriodGroup16, kNamespaceIndex}; // Variable
const scada::NodeId Iec60870DeviceType_InterrogationPeriodGroup2{numeric_id::Iec60870DeviceType_InterrogationPeriodGroup2, kNamespaceIndex}; // Variable
const scada::NodeId Iec60870DeviceType_InterrogationPeriodGroup3{numeric_id::Iec60870DeviceType_InterrogationPeriodGroup3, kNamespaceIndex}; // Variable
const scada::NodeId Iec60870DeviceType_InterrogationPeriodGroup4{numeric_id::Iec60870DeviceType_InterrogationPeriodGroup4, kNamespaceIndex}; // Variable
const scada::NodeId Iec60870DeviceType_InterrogationPeriodGroup5{numeric_id::Iec60870DeviceType_InterrogationPeriodGroup5, kNamespaceIndex}; // Variable
const scada::NodeId Iec60870DeviceType_InterrogationPeriodGroup6{numeric_id::Iec60870DeviceType_InterrogationPeriodGroup6, kNamespaceIndex}; // Variable
const scada::NodeId Iec60870DeviceType_InterrogationPeriodGroup7{numeric_id::Iec60870DeviceType_InterrogationPeriodGroup7, kNamespaceIndex}; // Variable
const scada::NodeId Iec60870DeviceType_InterrogationPeriodGroup8{numeric_id::Iec60870DeviceType_InterrogationPeriodGroup8, kNamespaceIndex}; // Variable
const scada::NodeId Iec60870DeviceType_InterrogationPeriodGroup9{numeric_id::Iec60870DeviceType_InterrogationPeriodGroup9, kNamespaceIndex}; // Variable
const scada::NodeId Iec60870DeviceType_LinkAddress{numeric_id::Iec60870DeviceType_LinkAddress, kNamespaceIndex}; // Variable
const scada::NodeId Iec60870DeviceType_Online{numeric_id::Iec60870DeviceType_Online, kNamespaceIndex}; // Variable
const scada::NodeId Iec60870DeviceType_Select{numeric_id::Iec60870DeviceType_Select, kNamespaceIndex}; // Method
const scada::NodeId Iec60870DeviceType_StartupClockSync{numeric_id::Iec60870DeviceType_StartupClockSync, kNamespaceIndex}; // Variable
const scada::NodeId Iec60870DeviceType_StartupInterrogation{numeric_id::Iec60870DeviceType_StartupInterrogation, kNamespaceIndex}; // Variable
const scada::NodeId Iec60870DeviceType_SyncClock{numeric_id::Iec60870DeviceType_SyncClock, kNamespaceIndex}; // Method
const scada::NodeId Iec60870DeviceType_UtcTime{numeric_id::Iec60870DeviceType_UtcTime, kNamespaceIndex}; // Variable
const scada::NodeId Iec60870DeviceType_Write{numeric_id::Iec60870DeviceType_Write, kNamespaceIndex}; // Method
const scada::NodeId Iec60870DeviceType_WriteParam{numeric_id::Iec60870DeviceType_WriteParam, kNamespaceIndex}; // Method
const scada::NodeId Iec60870LinkType{numeric_id::Iec60870LinkType, kNamespaceIndex}; // ObjectType
const scada::NodeId Iec60870LinkType_ActiveConnections{numeric_id::Iec60870LinkType_ActiveConnections, kNamespaceIndex}; // Variable
const scada::NodeId Iec60870LinkType_AnonymousMode{numeric_id::Iec60870LinkType_AnonymousMode, kNamespaceIndex}; // Variable
const scada::NodeId Iec60870LinkType_BytesIn{numeric_id::Iec60870LinkType_BytesIn, kNamespaceIndex}; // Variable
const scada::NodeId Iec60870LinkType_BytesOut{numeric_id::Iec60870LinkType_BytesOut, kNamespaceIndex}; // Variable
const scada::NodeId Iec60870LinkType_COTSize{numeric_id::Iec60870LinkType_COTSize, kNamespaceIndex}; // Variable
const scada::NodeId Iec60870LinkType_CRCProtection{numeric_id::Iec60870LinkType_CRCProtection, kNamespaceIndex}; // Variable
const scada::NodeId Iec60870LinkType_ConfirmationTimeout{numeric_id::Iec60870LinkType_ConfirmationTimeout, kNamespaceIndex}; // Variable
const scada::NodeId Iec60870LinkType_ConnectCount{numeric_id::Iec60870LinkType_ConnectCount, kNamespaceIndex}; // Variable
const scada::NodeId Iec60870LinkType_ConnectTimeout{numeric_id::Iec60870LinkType_ConnectTimeout, kNamespaceIndex}; // Variable
const scada::NodeId Iec60870LinkType_DataCollection{numeric_id::Iec60870LinkType_DataCollection, kNamespaceIndex}; // Variable
const scada::NodeId Iec60870LinkType_DeviceAddressSize{numeric_id::Iec60870LinkType_DeviceAddressSize, kNamespaceIndex}; // Variable
const scada::NodeId Iec60870LinkType_Disabled{numeric_id::Iec60870LinkType_Disabled, kNamespaceIndex}; // Variable
const scada::NodeId Iec60870LinkType_Enabled{numeric_id::Iec60870LinkType_Enabled, kNamespaceIndex}; // Variable
const scada::NodeId Iec60870LinkType_IdleTimeout{numeric_id::Iec60870LinkType_IdleTimeout, kNamespaceIndex}; // Variable
const scada::NodeId Iec60870LinkType_InfoAddressSize{numeric_id::Iec60870LinkType_InfoAddressSize, kNamespaceIndex}; // Variable
const scada::NodeId Iec60870LinkType_Interrogate{numeric_id::Iec60870LinkType_Interrogate, kNamespaceIndex}; // Method
const scada::NodeId Iec60870LinkType_InterrogateChannel{numeric_id::Iec60870LinkType_InterrogateChannel, kNamespaceIndex}; // Method
const scada::NodeId Iec60870LinkType_MessagesIn{numeric_id::Iec60870LinkType_MessagesIn, kNamespaceIndex}; // Variable
const scada::NodeId Iec60870LinkType_MessagesOut{numeric_id::Iec60870LinkType_MessagesOut, kNamespaceIndex}; // Variable
const scada::NodeId Iec60870LinkType_Mode{numeric_id::Iec60870LinkType_Mode, kNamespaceIndex}; // Variable
const scada::NodeId Iec60870LinkType_Online{numeric_id::Iec60870LinkType_Online, kNamespaceIndex}; // Variable
const scada::NodeId Iec60870LinkType_Protocol{numeric_id::Iec60870LinkType_Protocol, kNamespaceIndex}; // Variable
const scada::NodeId Iec60870LinkType_ReceiveQueueSize{numeric_id::Iec60870LinkType_ReceiveQueueSize, kNamespaceIndex}; // Variable
const scada::NodeId Iec60870LinkType_ReceiveTimeout{numeric_id::Iec60870LinkType_ReceiveTimeout, kNamespaceIndex}; // Variable
const scada::NodeId Iec60870LinkType_Select{numeric_id::Iec60870LinkType_Select, kNamespaceIndex}; // Method
const scada::NodeId Iec60870LinkType_SendQueueSize{numeric_id::Iec60870LinkType_SendQueueSize, kNamespaceIndex}; // Variable
const scada::NodeId Iec60870LinkType_SendRetryCount{numeric_id::Iec60870LinkType_SendRetryCount, kNamespaceIndex}; // Variable
const scada::NodeId Iec60870LinkType_SendTimeout{numeric_id::Iec60870LinkType_SendTimeout, kNamespaceIndex}; // Variable
const scada::NodeId Iec60870LinkType_SyncClock{numeric_id::Iec60870LinkType_SyncClock, kNamespaceIndex}; // Method
const scada::NodeId Iec60870LinkType_TerminationTimeout{numeric_id::Iec60870LinkType_TerminationTimeout, kNamespaceIndex}; // Variable
const scada::NodeId Iec60870LinkType_Transport{numeric_id::Iec60870LinkType_Transport, kNamespaceIndex}; // Variable
const scada::NodeId Iec60870LinkType_Write{numeric_id::Iec60870LinkType_Write, kNamespaceIndex}; // Method
const scada::NodeId Iec60870LinkType_WriteParam{numeric_id::Iec60870LinkType_WriteParam, kNamespaceIndex}; // Method
const scada::NodeId Iec60870ModeDataType{numeric_id::Iec60870ModeDataType, kNamespaceIndex}; // DataType
const scada::NodeId Iec60870ModeDataType_EnumStrings{numeric_id::Iec60870ModeDataType_EnumStrings, kNamespaceIndex}; // Variable
const scada::NodeId Iec60870ProtocolDataType{numeric_id::Iec60870ProtocolDataType, kNamespaceIndex}; // DataType
const scada::NodeId Iec60870ProtocolDataType_EnumStrings{numeric_id::Iec60870ProtocolDataType_EnumStrings, kNamespaceIndex}; // Variable
const scada::NodeId Iec61850ConfigurableObjectType{numeric_id::Iec61850ConfigurableObjectType, kNamespaceIndex}; // ObjectType
const scada::NodeId Iec61850ConfigurableObjectType_Reference{numeric_id::Iec61850ConfigurableObjectType_Reference, kNamespaceIndex}; // Variable
const scada::NodeId Iec61850ControlObjectType{numeric_id::Iec61850ControlObjectType, kNamespaceIndex}; // ObjectType
const scada::NodeId Iec61850DataVariableType{numeric_id::Iec61850DataVariableType, kNamespaceIndex}; // VariableType
const scada::NodeId Iec61850DeviceType{numeric_id::Iec61850DeviceType, kNamespaceIndex}; // ObjectType
const scada::NodeId Iec61850DeviceType_Disabled{numeric_id::Iec61850DeviceType_Disabled, kNamespaceIndex}; // Variable
const scada::NodeId Iec61850DeviceType_Enabled{numeric_id::Iec61850DeviceType_Enabled, kNamespaceIndex}; // Variable
const scada::NodeId Iec61850DeviceType_Host{numeric_id::Iec61850DeviceType_Host, kNamespaceIndex}; // Variable
const scada::NodeId Iec61850DeviceType_Interrogate{numeric_id::Iec61850DeviceType_Interrogate, kNamespaceIndex}; // Method
const scada::NodeId Iec61850DeviceType_InterrogateChannel{numeric_id::Iec61850DeviceType_InterrogateChannel, kNamespaceIndex}; // Method
const scada::NodeId Iec61850DeviceType_Model{numeric_id::Iec61850DeviceType_Model, kNamespaceIndex}; // Object
const scada::NodeId Iec61850DeviceType_Online{numeric_id::Iec61850DeviceType_Online, kNamespaceIndex}; // Variable
const scada::NodeId Iec61850DeviceType_Port{numeric_id::Iec61850DeviceType_Port, kNamespaceIndex}; // Variable
const scada::NodeId Iec61850DeviceType_Select{numeric_id::Iec61850DeviceType_Select, kNamespaceIndex}; // Method
const scada::NodeId Iec61850DeviceType_SyncClock{numeric_id::Iec61850DeviceType_SyncClock, kNamespaceIndex}; // Method
const scada::NodeId Iec61850DeviceType_Write{numeric_id::Iec61850DeviceType_Write, kNamespaceIndex}; // Method
const scada::NodeId Iec61850DeviceType_WriteParam{numeric_id::Iec61850DeviceType_WriteParam, kNamespaceIndex}; // Method
const scada::NodeId Iec61850LogicalNodeType{numeric_id::Iec61850LogicalNodeType, kNamespaceIndex}; // ObjectType
const scada::NodeId Iec61850RcbType{numeric_id::Iec61850RcbType, kNamespaceIndex}; // ObjectType
const scada::NodeId Iec61850RcbType_Reference{numeric_id::Iec61850RcbType_Reference, kNamespaceIndex}; // Variable
const scada::NodeId Iec61850ServerType{numeric_id::Iec61850ServerType, kNamespaceIndex}; // ObjectType
const scada::NodeId Iec61850ServerType_Disabled{numeric_id::Iec61850ServerType_Disabled, kNamespaceIndex}; // Variable
const scada::NodeId Iec61850ServerType_Enabled{numeric_id::Iec61850ServerType_Enabled, kNamespaceIndex}; // Variable
const scada::NodeId Iec61850ServerType_Host{numeric_id::Iec61850ServerType_Host, kNamespaceIndex}; // Variable
const scada::NodeId Iec61850ServerType_Interrogate{numeric_id::Iec61850ServerType_Interrogate, kNamespaceIndex}; // Method
const scada::NodeId Iec61850ServerType_InterrogateChannel{numeric_id::Iec61850ServerType_InterrogateChannel, kNamespaceIndex}; // Method
const scada::NodeId Iec61850ServerType_Online{numeric_id::Iec61850ServerType_Online, kNamespaceIndex}; // Variable
const scada::NodeId Iec61850ServerType_Port{numeric_id::Iec61850ServerType_Port, kNamespaceIndex}; // Variable
const scada::NodeId Iec61850ServerType_Select{numeric_id::Iec61850ServerType_Select, kNamespaceIndex}; // Method
const scada::NodeId Iec61850ServerType_SyncClock{numeric_id::Iec61850ServerType_SyncClock, kNamespaceIndex}; // Method
const scada::NodeId Iec61850ServerType_Write{numeric_id::Iec61850ServerType_Write, kNamespaceIndex}; // Method
const scada::NodeId Iec61850ServerType_WriteParam{numeric_id::Iec61850ServerType_WriteParam, kNamespaceIndex}; // Method
const scada::NodeId LinkType{numeric_id::LinkType, kNamespaceIndex}; // ObjectType
const scada::NodeId LinkType_ActiveConnections{numeric_id::LinkType_ActiveConnections, kNamespaceIndex}; // Variable
const scada::NodeId LinkType_BytesIn{numeric_id::LinkType_BytesIn, kNamespaceIndex}; // Variable
const scada::NodeId LinkType_BytesOut{numeric_id::LinkType_BytesOut, kNamespaceIndex}; // Variable
const scada::NodeId LinkType_ConnectCount{numeric_id::LinkType_ConnectCount, kNamespaceIndex}; // Variable
const scada::NodeId LinkType_Disabled{numeric_id::LinkType_Disabled, kNamespaceIndex}; // Variable
const scada::NodeId LinkType_Enabled{numeric_id::LinkType_Enabled, kNamespaceIndex}; // Variable
const scada::NodeId LinkType_Interrogate{numeric_id::LinkType_Interrogate, kNamespaceIndex}; // Method
const scada::NodeId LinkType_InterrogateChannel{numeric_id::LinkType_InterrogateChannel, kNamespaceIndex}; // Method
const scada::NodeId LinkType_MessagesIn{numeric_id::LinkType_MessagesIn, kNamespaceIndex}; // Variable
const scada::NodeId LinkType_MessagesOut{numeric_id::LinkType_MessagesOut, kNamespaceIndex}; // Variable
const scada::NodeId LinkType_Online{numeric_id::LinkType_Online, kNamespaceIndex}; // Variable
const scada::NodeId LinkType_Select{numeric_id::LinkType_Select, kNamespaceIndex}; // Method
const scada::NodeId LinkType_SyncClock{numeric_id::LinkType_SyncClock, kNamespaceIndex}; // Method
const scada::NodeId LinkType_Transport{numeric_id::LinkType_Transport, kNamespaceIndex}; // Variable
const scada::NodeId LinkType_Write{numeric_id::LinkType_Write, kNamespaceIndex}; // Method
const scada::NodeId LinkType_WriteParam{numeric_id::LinkType_WriteParam, kNamespaceIndex}; // Method
const scada::NodeId ModbusDeviceType{numeric_id::ModbusDeviceType, kNamespaceIndex}; // ObjectType
const scada::NodeId ModbusDeviceType_Address{numeric_id::ModbusDeviceType_Address, kNamespaceIndex}; // Variable
const scada::NodeId ModbusDeviceType_Disabled{numeric_id::ModbusDeviceType_Disabled, kNamespaceIndex}; // Variable
const scada::NodeId ModbusDeviceType_Enabled{numeric_id::ModbusDeviceType_Enabled, kNamespaceIndex}; // Variable
const scada::NodeId ModbusDeviceType_Interrogate{numeric_id::ModbusDeviceType_Interrogate, kNamespaceIndex}; // Method
const scada::NodeId ModbusDeviceType_InterrogateChannel{numeric_id::ModbusDeviceType_InterrogateChannel, kNamespaceIndex}; // Method
const scada::NodeId ModbusDeviceType_Online{numeric_id::ModbusDeviceType_Online, kNamespaceIndex}; // Variable
const scada::NodeId ModbusDeviceType_ResponseTimeout{numeric_id::ModbusDeviceType_ResponseTimeout, kNamespaceIndex}; // Variable
const scada::NodeId ModbusDeviceType_Select{numeric_id::ModbusDeviceType_Select, kNamespaceIndex}; // Method
const scada::NodeId ModbusDeviceType_SendRetryCount{numeric_id::ModbusDeviceType_SendRetryCount, kNamespaceIndex}; // Variable
const scada::NodeId ModbusDeviceType_SyncClock{numeric_id::ModbusDeviceType_SyncClock, kNamespaceIndex}; // Method
const scada::NodeId ModbusDeviceType_Write{numeric_id::ModbusDeviceType_Write, kNamespaceIndex}; // Method
const scada::NodeId ModbusDeviceType_WriteParam{numeric_id::ModbusDeviceType_WriteParam, kNamespaceIndex}; // Method
const scada::NodeId ModbusLinkProtocol{numeric_id::ModbusLinkProtocol, kNamespaceIndex}; // DataType
const scada::NodeId ModbusLinkProtocol_EnumStrings{numeric_id::ModbusLinkProtocol_EnumStrings, kNamespaceIndex}; // Variable
const scada::NodeId ModbusLinkType{numeric_id::ModbusLinkType, kNamespaceIndex}; // ObjectType
const scada::NodeId ModbusLinkType_ActiveConnections{numeric_id::ModbusLinkType_ActiveConnections, kNamespaceIndex}; // Variable
const scada::NodeId ModbusLinkType_BytesIn{numeric_id::ModbusLinkType_BytesIn, kNamespaceIndex}; // Variable
const scada::NodeId ModbusLinkType_BytesOut{numeric_id::ModbusLinkType_BytesOut, kNamespaceIndex}; // Variable
const scada::NodeId ModbusLinkType_ConnectCount{numeric_id::ModbusLinkType_ConnectCount, kNamespaceIndex}; // Variable
const scada::NodeId ModbusLinkType_Disabled{numeric_id::ModbusLinkType_Disabled, kNamespaceIndex}; // Variable
const scada::NodeId ModbusLinkType_Enabled{numeric_id::ModbusLinkType_Enabled, kNamespaceIndex}; // Variable
const scada::NodeId ModbusLinkType_Interrogate{numeric_id::ModbusLinkType_Interrogate, kNamespaceIndex}; // Method
const scada::NodeId ModbusLinkType_InterrogateChannel{numeric_id::ModbusLinkType_InterrogateChannel, kNamespaceIndex}; // Method
const scada::NodeId ModbusLinkType_MessagesIn{numeric_id::ModbusLinkType_MessagesIn, kNamespaceIndex}; // Variable
const scada::NodeId ModbusLinkType_MessagesOut{numeric_id::ModbusLinkType_MessagesOut, kNamespaceIndex}; // Variable
const scada::NodeId ModbusLinkType_Online{numeric_id::ModbusLinkType_Online, kNamespaceIndex}; // Variable
const scada::NodeId ModbusLinkType_Protocol{numeric_id::ModbusLinkType_Protocol, kNamespaceIndex}; // Variable
const scada::NodeId ModbusLinkType_RequestDelay{numeric_id::ModbusLinkType_RequestDelay, kNamespaceIndex}; // Variable
const scada::NodeId ModbusLinkType_Select{numeric_id::ModbusLinkType_Select, kNamespaceIndex}; // Method
const scada::NodeId ModbusLinkType_SyncClock{numeric_id::ModbusLinkType_SyncClock, kNamespaceIndex}; // Method
const scada::NodeId ModbusLinkType_Transport{numeric_id::ModbusLinkType_Transport, kNamespaceIndex}; // Variable
const scada::NodeId ModbusLinkType_Write{numeric_id::ModbusLinkType_Write, kNamespaceIndex}; // Method
const scada::NodeId ModbusLinkType_WriteParam{numeric_id::ModbusLinkType_WriteParam, kNamespaceIndex}; // Method
const scada::NodeId TelecontrolDevices_BinarySchema{numeric_id::TelecontrolDevices_BinarySchema, kNamespaceIndex}; // Variable
const scada::NodeId TelecontrolDevices_BinarySchema_DataTypeVersion{numeric_id::TelecontrolDevices_BinarySchema_DataTypeVersion, kNamespaceIndex}; // Variable
const scada::NodeId TelecontrolDevices_BinarySchema_Deprecated{numeric_id::TelecontrolDevices_BinarySchema_Deprecated, kNamespaceIndex}; // Variable
const scada::NodeId TelecontrolDevices_BinarySchema_NamespaceUri{numeric_id::TelecontrolDevices_BinarySchema_NamespaceUri, kNamespaceIndex}; // Variable
const scada::NodeId TelecontrolDevices_XmlSchema{numeric_id::TelecontrolDevices_XmlSchema, kNamespaceIndex}; // Variable
const scada::NodeId TelecontrolDevices_XmlSchema_DataTypeVersion{numeric_id::TelecontrolDevices_XmlSchema_DataTypeVersion, kNamespaceIndex}; // Variable
const scada::NodeId TelecontrolDevices_XmlSchema_Deprecated{numeric_id::TelecontrolDevices_XmlSchema_Deprecated, kNamespaceIndex}; // Variable
const scada::NodeId TelecontrolDevices_XmlSchema_NamespaceUri{numeric_id::TelecontrolDevices_XmlSchema_NamespaceUri, kNamespaceIndex}; // Variable
const scada::NodeId TransmissionItemType{numeric_id::TransmissionItemType, kNamespaceIndex}; // ObjectType
const scada::NodeId TransmissionItemType_SourceAddress{numeric_id::TransmissionItemType_SourceAddress, kNamespaceIndex}; // Variable
const scada::NodeId TransmissionItems{numeric_id::TransmissionItems, kNamespaceIndex}; // Object

} //  namespace id

} //  namespace devices

