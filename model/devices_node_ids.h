#pragma once

#include "model/namespaces.h"

namespace devices {

namespace numeric_id {

const scada::NumericId Iec60870DeviceType_InterrogationPeriodGroup1 = 200;

}

namespace id {

const scada::NodeId Iec60870DeviceType_Address{193, NamespaceIndexes::SCADA};
const scada::NodeId Iec60870DeviceType_ClockSyncPeriod{198, NamespaceIndexes::SCADA};
const scada::NodeId Iec60870DeviceType_InterrogationPeriodGroup10{209, NamespaceIndexes::SCADA};
const scada::NodeId Iec60870DeviceType_InterrogationPeriodGroup11{210, NamespaceIndexes::SCADA};
const scada::NodeId Iec60870DeviceType_InterrogationPeriodGroup12{211, NamespaceIndexes::SCADA};
const scada::NodeId Iec60870DeviceType_InterrogationPeriodGroup13{212, NamespaceIndexes::SCADA};
const scada::NodeId Iec60870DeviceType_InterrogationPeriodGroup14{213, NamespaceIndexes::SCADA};
const scada::NodeId Iec60870DeviceType_InterrogationPeriodGroup15{214, NamespaceIndexes::SCADA};
const scada::NodeId Iec60870DeviceType_InterrogationPeriodGroup16{215, NamespaceIndexes::SCADA};
const scada::NodeId Iec60870DeviceType_InterrogationPeriodGroup1{numeric_id::Iec60870DeviceType_InterrogationPeriodGroup1, NamespaceIndexes::SCADA};
const scada::NodeId Iec60870DeviceType_InterrogationPeriodGroup2{201, NamespaceIndexes::SCADA};
const scada::NodeId Iec60870DeviceType_InterrogationPeriodGroup3{202, NamespaceIndexes::SCADA};
const scada::NodeId Iec60870DeviceType_InterrogationPeriodGroup4{203, NamespaceIndexes::SCADA};
const scada::NodeId Iec60870DeviceType_InterrogationPeriodGroup5{204, NamespaceIndexes::SCADA};
const scada::NodeId Iec60870DeviceType_InterrogationPeriodGroup6{205, NamespaceIndexes::SCADA};
const scada::NodeId Iec60870DeviceType_InterrogationPeriodGroup7{206, NamespaceIndexes::SCADA};
const scada::NodeId Iec60870DeviceType_InterrogationPeriodGroup8{207, NamespaceIndexes::SCADA};
const scada::NodeId Iec60870DeviceType_InterrogationPeriodGroup9{208, NamespaceIndexes::SCADA};
const scada::NodeId Iec60870DeviceType_InterrogationPeriod{196, NamespaceIndexes::SCADA};
const scada::NodeId Iec60870DeviceType_LinkAddress{194, NamespaceIndexes::SCADA};
const scada::NodeId Iec60870DeviceType_StartupClockSync{197, NamespaceIndexes::SCADA};
const scada::NodeId Iec60870DeviceType_StartupInterrogation{195, NamespaceIndexes::SCADA};
const scada::NodeId Iec60870DeviceType_UtcTime{294, NamespaceIndexes::SCADA};
const scada::NodeId Iec60870DeviceType{331, NamespaceIndexes::SCADA};
const scada::NodeId Iec60870ProtocolDataType{322, NamespaceIndexes::SCADA};
const scada::NodeId Iec60870ProtocolDataType_EnumStrings{323, NamespaceIndexes::SCADA};
const scada::NodeId Iec60870ModeDataType{324, NamespaceIndexes::SCADA};
const scada::NodeId Iec60870ModeDataType_EnumStrings{325, NamespaceIndexes::SCADA};
const scada::NodeId Iec60870LinkType_AnonymousMode{192, NamespaceIndexes::SCADA};
const scada::NodeId Iec60870LinkType_ConfirmationTimeout{181, NamespaceIndexes::SCADA};
const scada::NodeId Iec60870LinkType_ConnectTimeout{180, NamespaceIndexes::SCADA};
const scada::NodeId Iec60870LinkType_COTSize{184, NamespaceIndexes::SCADA};
const scada::NodeId Iec60870LinkType_CRCProtection{188, NamespaceIndexes::SCADA};
const scada::NodeId Iec60870LinkType_DataCollection{186, NamespaceIndexes::SCADA};
const scada::NodeId Iec60870LinkType_DeviceAddressSize{183, NamespaceIndexes::SCADA};
const scada::NodeId Iec60870LinkType_IdleTimeout{191, NamespaceIndexes::SCADA};
const scada::NodeId Iec60870LinkType_InfoAddressSize{185, NamespaceIndexes::SCADA};
const scada::NodeId Iec60870LinkType_Mode{177, NamespaceIndexes::SCADA};
const scada::NodeId Iec60870LinkType_Protocol{176, NamespaceIndexes::SCADA};
const scada::NodeId Iec60870LinkType_ReceiveQueueSize{179, NamespaceIndexes::SCADA};
const scada::NodeId Iec60870LinkType_ReceiveTimeout{190, NamespaceIndexes::SCADA};
const scada::NodeId Iec60870LinkType_SendQueueSize{178, NamespaceIndexes::SCADA};
const scada::NodeId Iec60870LinkType_SendRetryCount{187, NamespaceIndexes::SCADA};
const scada::NodeId Iec60870LinkType_SendTimeout{189, NamespaceIndexes::SCADA};
const scada::NodeId Iec60870LinkType_TerminationTimeout{182, NamespaceIndexes::SCADA};
const scada::NodeId Iec60870LinkType{124, NamespaceIndexes::SCADA};
const scada::NodeId Iec61850ConfigurableObjectType_Reference{283, NamespaceIndexes::SCADA};
const scada::NodeId Iec61850ConfigurableObjectType{285, NamespaceIndexes::SCADA};
const scada::NodeId Iec61850ControlObjectType{293, NamespaceIndexes::SCADA};
const scada::NodeId Iec61850DataVariableType{291, NamespaceIndexes::SCADA};
const scada::NodeId Iec61850DeviceType_Host{278, NamespaceIndexes::SCADA};
const scada::NodeId Iec61850DeviceType_Model{288, NamespaceIndexes::SCADA};
const scada::NodeId Iec61850DeviceType_Port{279, NamespaceIndexes::SCADA};
const scada::NodeId Iec61850DeviceType{277, NamespaceIndexes::SCADA};
const scada::NodeId Iec61850LogicalNodeType{289, NamespaceIndexes::SCADA};
const scada::NodeId Iec61850RcbType{281, NamespaceIndexes::SCADA};
const scada::NodeId LinkType_ActiveConnections{251, NamespaceIndexes::SCADA};
const scada::NodeId LinkType_ConnectCount{250, NamespaceIndexes::SCADA};
const scada::NodeId LinkType_Transport{142, NamespaceIndexes::SCADA};
const scada::NodeId LinkType{116, NamespaceIndexes::SCADA}; // :Device
const scada::NodeId ModbusDeviceType_Address{168, NamespaceIndexes::SCADA};
const scada::NodeId ModbusDeviceType_SendRetryCount{169, NamespaceIndexes::SCADA};
const scada::NodeId ModbusDeviceType_ResponseTimeout{298, NamespaceIndexes::SCADA};
const scada::NodeId ModbusDeviceType{118, NamespaceIndexes::SCADA};
const scada::NodeId ModbusLinkProtocol{130, NamespaceIndexes::SCADA};
const scada::NodeId ModbusLinkProtocol_EnumStrings{303, NamespaceIndexes::SCADA};
const scada::NodeId ModbusLinkType_Protocol{167, NamespaceIndexes::SCADA};
const scada::NodeId ModbusLinkType_RequestDelay{299, NamespaceIndexes::SCADA};
const scada::NodeId ModbusLinkType_Mode{330, NamespaceIndexes::SCADA};
const scada::NodeId ModbusLinkType{108, NamespaceIndexes::SCADA};
const scada::NodeId Devices{30, NamespaceIndexes::SCADA};
const scada::NodeId DeviceType_Disabled{141, NamespaceIndexes::SCADA};
const scada::NodeId DeviceType_Enabled{249, NamespaceIndexes::SCADA};
const scada::NodeId DeviceType_InterrogateChannel{135, NamespaceIndexes::SCADA};
const scada::NodeId DeviceType_Interrogate{133, NamespaceIndexes::SCADA};
const scada::NodeId DeviceType_Online{248, NamespaceIndexes::SCADA};
const scada::NodeId DeviceType_Select{137, NamespaceIndexes::SCADA};
const scada::NodeId DeviceType_SyncClock{134, NamespaceIndexes::SCADA};
const scada::NodeId DeviceType_WriteParam{138, NamespaceIndexes::SCADA};
const scada::NodeId DeviceType_Write{136, NamespaceIndexes::SCADA};
const scada::NodeId DeviceType_BytesIn{254, NamespaceIndexes::SCADA};
const scada::NodeId DeviceType_BytesOut{255, NamespaceIndexes::SCADA};
const scada::NodeId DeviceType_MessagesIn{252, NamespaceIndexes::SCADA};
const scada::NodeId DeviceType_MessagesOut{253, NamespaceIndexes::SCADA};
const scada::NodeId DeviceType{115, NamespaceIndexes::SCADA}; // :BaseObjectType
const scada::NodeId DeviceWatchEventType{15132, NamespaceIndexes::SCADA}; // ObjectType
const scada::NodeId TransmissionItemType_SourceAddress{225, NamespaceIndexes::SCADA};
const scada::NodeId TransmissionItemType{34, NamespaceIndexes::SCADA};
const scada::NodeId HasTransmissionSource{221, NamespaceIndexes::SCADA};
const scada::NodeId HasTransmissionItem{339, NamespaceIndexes::SCADA};  // ReferenceType
const scada::NodeId ModbusModeDataType{332, NamespaceIndexes::SCADA};
const scada::NodeId ModbusModeDataType_EnumStrings{333, NamespaceIndexes::SCADA};
const scada::NodeId ModbusTransmissionItemType{342, NamespaceIndexes::SCADA};
const scada::NodeId Iec60870TransmissionItemType{343, NamespaceIndexes::SCADA};
const scada::NodeId Iec61850TransmissionItemType{344, NamespaceIndexes::SCADA};

}

}  // namespace devices