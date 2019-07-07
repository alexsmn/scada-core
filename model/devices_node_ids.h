#pragma once

#include "model/scada_node_ids.h"

namespace devices {

constexpr scada::NamespaceIndex kNamespaceIndex = NamespaceIndexes::SCADA;

namespace numeric_id {

const scada::NumericId Iec60870DeviceType_InterrogationPeriodGroup1 = 200;

}

namespace id {

const scada::NodeId Devices{30, kNamespaceIndex};
const scada::NodeId DeviceType_Disabled{141, kNamespaceIndex};
const scada::NodeId DeviceType_Enabled{249, kNamespaceIndex};
const scada::NodeId DeviceType_InterrogateChannel{135, kNamespaceIndex};
const scada::NodeId DeviceType_Interrogate{133, kNamespaceIndex};
const scada::NodeId DeviceType_Online{248, kNamespaceIndex};
const scada::NodeId DeviceType_Select{137, kNamespaceIndex};
const scada::NodeId DeviceType_SyncClock{134, kNamespaceIndex};
const scada::NodeId DeviceType_WriteParam{138, kNamespaceIndex};
const scada::NodeId DeviceType_Write{136, kNamespaceIndex};
const scada::NodeId DeviceType{115, kNamespaceIndex}; // :BaseObjectType
const scada::NodeId Iec60870DeviceType_Address{193, kNamespaceIndex};
const scada::NodeId Iec60870DeviceType_ClockSyncPeriod{198, kNamespaceIndex};
const scada::NodeId Iec60870DeviceType_InterrogationPeriodGroup10{209, kNamespaceIndex};
const scada::NodeId Iec60870DeviceType_InterrogationPeriodGroup11{210, kNamespaceIndex};
const scada::NodeId Iec60870DeviceType_InterrogationPeriodGroup12{211, kNamespaceIndex};
const scada::NodeId Iec60870DeviceType_InterrogationPeriodGroup13{212, kNamespaceIndex};
const scada::NodeId Iec60870DeviceType_InterrogationPeriodGroup14{213, kNamespaceIndex};
const scada::NodeId Iec60870DeviceType_InterrogationPeriodGroup15{214, kNamespaceIndex};
const scada::NodeId Iec60870DeviceType_InterrogationPeriodGroup16{215, kNamespaceIndex};
const scada::NodeId Iec60870DeviceType_InterrogationPeriodGroup1{numeric_id::Iec60870DeviceType_InterrogationPeriodGroup1, kNamespaceIndex};
const scada::NodeId Iec60870DeviceType_InterrogationPeriodGroup2{201, kNamespaceIndex};
const scada::NodeId Iec60870DeviceType_InterrogationPeriodGroup3{202, kNamespaceIndex};
const scada::NodeId Iec60870DeviceType_InterrogationPeriodGroup4{203, kNamespaceIndex};
const scada::NodeId Iec60870DeviceType_InterrogationPeriodGroup5{204, kNamespaceIndex};
const scada::NodeId Iec60870DeviceType_InterrogationPeriodGroup6{205, kNamespaceIndex};
const scada::NodeId Iec60870DeviceType_InterrogationPeriodGroup7{206, kNamespaceIndex};
const scada::NodeId Iec60870DeviceType_InterrogationPeriodGroup8{207, kNamespaceIndex};
const scada::NodeId Iec60870DeviceType_InterrogationPeriodGroup9{208, kNamespaceIndex};
const scada::NodeId Iec60870DeviceType_InterrogationPeriod{196, kNamespaceIndex};
const scada::NodeId Iec60870DeviceType_LinkAddress{194, kNamespaceIndex};
const scada::NodeId Iec60870DeviceType_StartupClockSync{197, kNamespaceIndex};
const scada::NodeId Iec60870DeviceType_StartupInterrogation{195, kNamespaceIndex};
const scada::NodeId Iec60870DeviceType_UtcTime{294, kNamespaceIndex};
const scada::NodeId Iec60870DeviceType{331, kNamespaceIndex};
const scada::NodeId Iec60870ProtocolType{322, kNamespaceIndex};
const scada::NodeId Iec60870ProtocolType_EnumStrings{323, kNamespaceIndex};
const scada::NodeId Iec60870ModeType{324, kNamespaceIndex};
const scada::NodeId Iec60870ModeType_EnumStrings{325, kNamespaceIndex};
const scada::NodeId Iec60870LinkType_AnonymousMode{192, kNamespaceIndex};
const scada::NodeId Iec60870LinkType_ConfirmationTimeout{181, kNamespaceIndex};
const scada::NodeId Iec60870LinkType_ConnectTimeout{180, kNamespaceIndex};
const scada::NodeId Iec60870LinkType_COTSize{184, kNamespaceIndex};
const scada::NodeId Iec60870LinkType_CRCProtection{188, kNamespaceIndex};
const scada::NodeId Iec60870LinkType_DataCollection{186, kNamespaceIndex};
const scada::NodeId Iec60870LinkType_DeviceAddressSize{183, kNamespaceIndex};
const scada::NodeId Iec60870LinkType_IdleTimeout{191, kNamespaceIndex};
const scada::NodeId Iec60870LinkType_InfoAddressSize{185, kNamespaceIndex};
const scada::NodeId Iec60870LinkType_Mode{177, kNamespaceIndex};
const scada::NodeId Iec60870LinkType_Protocol{176, kNamespaceIndex};
const scada::NodeId Iec60870LinkType_ReceiveQueueSize{179, kNamespaceIndex};
const scada::NodeId Iec60870LinkType_ReceiveTimeout{190, kNamespaceIndex};
const scada::NodeId Iec60870LinkType_SendQueueSize{178, kNamespaceIndex};
const scada::NodeId Iec60870LinkType_SendRetryCount{187, kNamespaceIndex};
const scada::NodeId Iec60870LinkType_SendTimeout{189, kNamespaceIndex};
const scada::NodeId Iec60870LinkType_TerminationTimeout{182, kNamespaceIndex};
const scada::NodeId Iec60870LinkType{124, kNamespaceIndex};
const scada::NodeId HasTransmissionSource{221, kNamespaceIndex};
const scada::NodeId HasTransmissionTarget{224, kNamespaceIndex}; // Ref
const scada::NodeId Iec61850ConfigurableObjectType_Reference{283, kNamespaceIndex};
const scada::NodeId Iec61850ConfigurableObjectType{285, kNamespaceIndex};
const scada::NodeId Iec61850ControlObjectType{293, kNamespaceIndex};
const scada::NodeId Iec61850DataVariableType{291, kNamespaceIndex};
const scada::NodeId Iec61850DeviceType_Host{278, kNamespaceIndex};
const scada::NodeId Iec61850DeviceType_Model{288, kNamespaceIndex};
const scada::NodeId Iec61850DeviceType_Port{279, kNamespaceIndex};
const scada::NodeId Iec61850DeviceType{277, kNamespaceIndex};
const scada::NodeId Iec61850LogicalNodeType{289, kNamespaceIndex};
const scada::NodeId Iec61850RcbType{281, kNamespaceIndex};
const scada::NodeId LinkType_ActiveConnections{251, kNamespaceIndex};
const scada::NodeId LinkType_BytesIn{254, kNamespaceIndex};
const scada::NodeId LinkType_BytesOut{255, kNamespaceIndex};
const scada::NodeId LinkType_ConnectCount{250, kNamespaceIndex};
const scada::NodeId LinkType_MessagesIn{252, kNamespaceIndex};
const scada::NodeId LinkType_MessagesOut{253, kNamespaceIndex};
const scada::NodeId LinkType_Transport{142, kNamespaceIndex};
const scada::NodeId LinkType{116, kNamespaceIndex}; // :Device
const scada::NodeId ModbusDeviceType_Address{168, kNamespaceIndex};
const scada::NodeId ModbusDeviceType_SendRetryCount{169, kNamespaceIndex};
const scada::NodeId ModbusDeviceType_ResponseTimeout{298, kNamespaceIndex};
const scada::NodeId ModbusDeviceType{118, kNamespaceIndex};
const scada::NodeId ModbusLinkProtocol{130, kNamespaceIndex};
const scada::NodeId ModbusLinkProtocol_EnumStrings{303, kNamespaceIndex};
const scada::NodeId ModbusLinkType_Protocol{167, kNamespaceIndex};
const scada::NodeId ModbusLinkType_RequestDelay{299, kNamespaceIndex};
const scada::NodeId ModbusLinkType{108, kNamespaceIndex};
const scada::NodeId TransmissionItems{33, kNamespaceIndex};
const scada::NodeId TransmissionItemType_SourceAddress{225, kNamespaceIndex};
const scada::NodeId TransmissionItemType{34, kNamespaceIndex};

}

}