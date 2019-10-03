/* ========================================================================
 * Copyright (c) 2005-2019 The OPC Foundation, Inc. All rights reserved.
 *
 * OPC Foundation MIT License 1.00
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * The complete license agreement can be found here:
 * http://opcfoundation.org/License/MIT/1.00/
 * ======================================================================*/

using System;
using System.Collections.Generic;
using System.Text;
using System.Reflection;
using System.Xml;
using System.Runtime.Serialization;
using Opc.Ua;
using Telecontrol.Scada;

namespace Telecontrol.Devices
{
    #region DataType Identifiers
    /// <summary>
    /// A class that declares constants for all DataTypes in the Model Design.
    /// </summary>
    /// <exclude />
    [System.CodeDom.Compiler.GeneratedCodeAttribute("Opc.Ua.ModelCompiler", "1.0.0.0")]
    public static partial class DataTypes
    {
        /// <summary>
        /// The identifier for the Iec60870ProtocolDataType DataType.
        /// </summary>
        public const uint Iec60870ProtocolDataType = 15023;

        /// <summary>
        /// The identifier for the Iec60870ModeDataType DataType.
        /// </summary>
        public const uint Iec60870ModeDataType = 15025;

        /// <summary>
        /// The identifier for the ModbusLinkProtocol DataType.
        /// </summary>
        public const uint ModbusLinkProtocol = 15005;
    }
    #endregion

    #region Method Identifiers
    /// <summary>
    /// A class that declares constants for all Methods in the Model Design.
    /// </summary>
    /// <exclude />
    [System.CodeDom.Compiler.GeneratedCodeAttribute("Opc.Ua.ModelCompiler", "1.0.0.0")]
    public static partial class Methods
    {
        /// <summary>
        /// The identifier for the DeviceType_Interrogate Method.
        /// </summary>
        public const uint DeviceType_Interrogate = 235;

        /// <summary>
        /// The identifier for the DeviceType_InterrogateChannel Method.
        /// </summary>
        public const uint DeviceType_InterrogateChannel = 278;

        /// <summary>
        /// The identifier for the DeviceType_SyncClock Method.
        /// </summary>
        public const uint DeviceType_SyncClock = 236;

        /// <summary>
        /// The identifier for the DeviceType_Select Method.
        /// </summary>
        public const uint DeviceType_Select = 256;

        /// <summary>
        /// The identifier for the DeviceType_Write Method.
        /// </summary>
        public const uint DeviceType_Write = 257;

        /// <summary>
        /// The identifier for the DeviceType_WriteParam Method.
        /// </summary>
        public const uint DeviceType_WriteParam = 258;
    }
    #endregion

    #region Object Identifiers
    /// <summary>
    /// A class that declares constants for all Objects in the Model Design.
    /// </summary>
    /// <exclude />
    [System.CodeDom.Compiler.GeneratedCodeAttribute("Opc.Ua.ModelCompiler", "1.0.0.0")]
    public static partial class Objects
    {
        /// <summary>
        /// The identifier for the Devices Object.
        /// </summary>
        public const uint Devices = 30;

        /// <summary>
        /// The identifier for the Iec61850DeviceType_Model Object.
        /// </summary>
        public const uint Iec61850DeviceType_Model = 381;

        /// <summary>
        /// The identifier for the TransmissionItems Object.
        /// </summary>
        public const uint TransmissionItems = 33;
    }
    #endregion

    #region ObjectType Identifiers
    /// <summary>
    /// A class that declares constants for all ObjectTypes in the Model Design.
    /// </summary>
    /// <exclude />
    [System.CodeDom.Compiler.GeneratedCodeAttribute("Opc.Ua.ModelCompiler", "1.0.0.0")]
    public static partial class ObjectTypes
    {
        /// <summary>
        /// The identifier for the DeviceType ObjectType.
        /// </summary>
        public const uint DeviceType = 31;

        /// <summary>
        /// The identifier for the LinkType ObjectType.
        /// </summary>
        public const uint LinkType = 40;

        /// <summary>
        /// The identifier for the DeviceWatchEventType ObjectType.
        /// </summary>
        public const uint DeviceWatchEventType = 219;

        /// <summary>
        /// The identifier for the Iec60870LinkType ObjectType.
        /// </summary>
        public const uint Iec60870LinkType = 297;

        /// <summary>
        /// The identifier for the Iec60870DeviceType ObjectType.
        /// </summary>
        public const uint Iec60870DeviceType = 331;

        /// <summary>
        /// The identifier for the Iec61850LogicalNodeType ObjectType.
        /// </summary>
        public const uint Iec61850LogicalNodeType = 370;

        /// <summary>
        /// The identifier for the Iec61850ControlObjectType ObjectType.
        /// </summary>
        public const uint Iec61850ControlObjectType = 15012;

        /// <summary>
        /// The identifier for the Iec61850DeviceType ObjectType.
        /// </summary>
        public const uint Iec61850DeviceType = 371;

        /// <summary>
        /// The identifier for the Iec61850ConfigurableObjectType ObjectType.
        /// </summary>
        public const uint Iec61850ConfigurableObjectType = 384;

        /// <summary>
        /// The identifier for the Iec61850RcbType ObjectType.
        /// </summary>
        public const uint Iec61850RcbType = 386;

        /// <summary>
        /// The identifier for the Iec61850ServerType ObjectType.
        /// </summary>
        public const uint Iec61850ServerType = 15003;

        /// <summary>
        /// The identifier for the ModbusLinkType ObjectType.
        /// </summary>
        public const uint ModbusLinkType = 108;

        /// <summary>
        /// The identifier for the ModbusDeviceType ObjectType.
        /// </summary>
        public const uint ModbusDeviceType = 118;

        /// <summary>
        /// The identifier for the OpcServerType ObjectType.
        /// </summary>
        public const uint OpcServerType = 15030;

        /// <summary>
        /// The identifier for the TransmissionItemType ObjectType.
        /// </summary>
        public const uint TransmissionItemType = 34;
    }
    #endregion

    #region ReferenceType Identifiers
    /// <summary>
    /// A class that declares constants for all ReferenceTypes in the Model Design.
    /// </summary>
    /// <exclude />
    [System.CodeDom.Compiler.GeneratedCodeAttribute("Opc.Ua.ModelCompiler", "1.0.0.0")]
    public static partial class ReferenceTypes
    {
        /// <summary>
        /// The identifier for the HasTransmissionSource ReferenceType.
        /// </summary>
        public const uint HasTransmissionSource = 172;

        /// <summary>
        /// The identifier for the HasTransmissionTarget ReferenceType.
        /// </summary>
        public const uint HasTransmissionTarget = 173;
    }
    #endregion

    #region Variable Identifiers
    /// <summary>
    /// A class that declares constants for all Variables in the Model Design.
    /// </summary>
    /// <exclude />
    [System.CodeDom.Compiler.GeneratedCodeAttribute("Opc.Ua.ModelCompiler", "1.0.0.0")]
    public static partial class Variables
    {
        /// <summary>
        /// The identifier for the DeviceType_Disabled Variable.
        /// </summary>
        public const uint DeviceType_Disabled = 201;

        /// <summary>
        /// The identifier for the DeviceType_Online Variable.
        /// </summary>
        public const uint DeviceType_Online = 202;

        /// <summary>
        /// The identifier for the DeviceType_Enabled Variable.
        /// </summary>
        public const uint DeviceType_Enabled = 203;

        /// <summary>
        /// The identifier for the LinkType_Transport Variable.
        /// </summary>
        public const uint LinkType_Transport = 42;

        /// <summary>
        /// The identifier for the LinkType_ConnectCount Variable.
        /// </summary>
        public const uint LinkType_ConnectCount = 43;

        /// <summary>
        /// The identifier for the LinkType_ActiveConnections Variable.
        /// </summary>
        public const uint LinkType_ActiveConnections = 44;

        /// <summary>
        /// The identifier for the LinkType_MessagesOut Variable.
        /// </summary>
        public const uint LinkType_MessagesOut = 45;

        /// <summary>
        /// The identifier for the LinkType_MessagesIn Variable.
        /// </summary>
        public const uint LinkType_MessagesIn = 46;

        /// <summary>
        /// The identifier for the LinkType_BytesOut Variable.
        /// </summary>
        public const uint LinkType_BytesOut = 47;

        /// <summary>
        /// The identifier for the LinkType_BytesIn Variable.
        /// </summary>
        public const uint LinkType_BytesIn = 48;

        /// <summary>
        /// The identifier for the Iec60870ProtocolDataType_EnumStrings Variable.
        /// </summary>
        public const uint Iec60870ProtocolDataType_EnumStrings = 15024;

        /// <summary>
        /// The identifier for the Iec60870ModeDataType_EnumStrings Variable.
        /// </summary>
        public const uint Iec60870ModeDataType_EnumStrings = 15026;

        /// <summary>
        /// The identifier for the Iec60870LinkType_Protocol Variable.
        /// </summary>
        public const uint Iec60870LinkType_Protocol = 314;

        /// <summary>
        /// The identifier for the Iec60870LinkType_Mode Variable.
        /// </summary>
        public const uint Iec60870LinkType_Mode = 315;

        /// <summary>
        /// The identifier for the Iec60870LinkType_SendQueueSize Variable.
        /// </summary>
        public const uint Iec60870LinkType_SendQueueSize = 316;

        /// <summary>
        /// The identifier for the Iec60870LinkType_ReceiveQueueSize Variable.
        /// </summary>
        public const uint Iec60870LinkType_ReceiveQueueSize = 317;

        /// <summary>
        /// The identifier for the Iec60870LinkType_ConnectTimeout Variable.
        /// </summary>
        public const uint Iec60870LinkType_ConnectTimeout = 318;

        /// <summary>
        /// The identifier for the Iec60870LinkType_ConfirmationTimeout Variable.
        /// </summary>
        public const uint Iec60870LinkType_ConfirmationTimeout = 319;

        /// <summary>
        /// The identifier for the Iec60870LinkType_TerminationTimeout Variable.
        /// </summary>
        public const uint Iec60870LinkType_TerminationTimeout = 320;

        /// <summary>
        /// The identifier for the Iec60870LinkType_DeviceAddressSize Variable.
        /// </summary>
        public const uint Iec60870LinkType_DeviceAddressSize = 321;

        /// <summary>
        /// The identifier for the Iec60870LinkType_COTSize Variable.
        /// </summary>
        public const uint Iec60870LinkType_COTSize = 322;

        /// <summary>
        /// The identifier for the Iec60870LinkType_InfoAddressSize Variable.
        /// </summary>
        public const uint Iec60870LinkType_InfoAddressSize = 323;

        /// <summary>
        /// The identifier for the Iec60870LinkType_DataCollection Variable.
        /// </summary>
        public const uint Iec60870LinkType_DataCollection = 324;

        /// <summary>
        /// The identifier for the Iec60870LinkType_SendRetryCount Variable.
        /// </summary>
        public const uint Iec60870LinkType_SendRetryCount = 325;

        /// <summary>
        /// The identifier for the Iec60870LinkType_CRCProtection Variable.
        /// </summary>
        public const uint Iec60870LinkType_CRCProtection = 326;

        /// <summary>
        /// The identifier for the Iec60870LinkType_SendTimeout Variable.
        /// </summary>
        public const uint Iec60870LinkType_SendTimeout = 327;

        /// <summary>
        /// The identifier for the Iec60870LinkType_ReceiveTimeout Variable.
        /// </summary>
        public const uint Iec60870LinkType_ReceiveTimeout = 328;

        /// <summary>
        /// The identifier for the Iec60870LinkType_IdleTimeout Variable.
        /// </summary>
        public const uint Iec60870LinkType_IdleTimeout = 329;

        /// <summary>
        /// The identifier for the Iec60870LinkType_AnonymousMode Variable.
        /// </summary>
        public const uint Iec60870LinkType_AnonymousMode = 330;

        /// <summary>
        /// The identifier for the Iec60870DeviceType_Address Variable.
        /// </summary>
        public const uint Iec60870DeviceType_Address = 341;

        /// <summary>
        /// The identifier for the Iec60870DeviceType_LinkAddress Variable.
        /// </summary>
        public const uint Iec60870DeviceType_LinkAddress = 342;

        /// <summary>
        /// The identifier for the Iec60870DeviceType_StartupInterrogation Variable.
        /// </summary>
        public const uint Iec60870DeviceType_StartupInterrogation = 343;

        /// <summary>
        /// The identifier for the Iec60870DeviceType_InterrogationPeriod Variable.
        /// </summary>
        public const uint Iec60870DeviceType_InterrogationPeriod = 344;

        /// <summary>
        /// The identifier for the Iec60870DeviceType_StartupClockSync Variable.
        /// </summary>
        public const uint Iec60870DeviceType_StartupClockSync = 345;

        /// <summary>
        /// The identifier for the Iec60870DeviceType_ClockSyncPeriod Variable.
        /// </summary>
        public const uint Iec60870DeviceType_ClockSyncPeriod = 346;

        /// <summary>
        /// The identifier for the Iec60870DeviceType_UtcTime Variable.
        /// </summary>
        public const uint Iec60870DeviceType_UtcTime = 15010;

        /// <summary>
        /// The identifier for the Iec60870DeviceType_InterrogationPeriodGroup1 Variable.
        /// </summary>
        public const uint Iec60870DeviceType_InterrogationPeriodGroup1 = 347;

        /// <summary>
        /// The identifier for the Iec60870DeviceType_InterrogationPeriodGroup2 Variable.
        /// </summary>
        public const uint Iec60870DeviceType_InterrogationPeriodGroup2 = 348;

        /// <summary>
        /// The identifier for the Iec60870DeviceType_InterrogationPeriodGroup3 Variable.
        /// </summary>
        public const uint Iec60870DeviceType_InterrogationPeriodGroup3 = 349;

        /// <summary>
        /// The identifier for the Iec60870DeviceType_InterrogationPeriodGroup4 Variable.
        /// </summary>
        public const uint Iec60870DeviceType_InterrogationPeriodGroup4 = 350;

        /// <summary>
        /// The identifier for the Iec60870DeviceType_InterrogationPeriodGroup5 Variable.
        /// </summary>
        public const uint Iec60870DeviceType_InterrogationPeriodGroup5 = 351;

        /// <summary>
        /// The identifier for the Iec60870DeviceType_InterrogationPeriodGroup6 Variable.
        /// </summary>
        public const uint Iec60870DeviceType_InterrogationPeriodGroup6 = 352;

        /// <summary>
        /// The identifier for the Iec60870DeviceType_InterrogationPeriodGroup7 Variable.
        /// </summary>
        public const uint Iec60870DeviceType_InterrogationPeriodGroup7 = 353;

        /// <summary>
        /// The identifier for the Iec60870DeviceType_InterrogationPeriodGroup8 Variable.
        /// </summary>
        public const uint Iec60870DeviceType_InterrogationPeriodGroup8 = 354;

        /// <summary>
        /// The identifier for the Iec60870DeviceType_InterrogationPeriodGroup9 Variable.
        /// </summary>
        public const uint Iec60870DeviceType_InterrogationPeriodGroup9 = 355;

        /// <summary>
        /// The identifier for the Iec60870DeviceType_InterrogationPeriodGroup10 Variable.
        /// </summary>
        public const uint Iec60870DeviceType_InterrogationPeriodGroup10 = 356;

        /// <summary>
        /// The identifier for the Iec60870DeviceType_InterrogationPeriodGroup11 Variable.
        /// </summary>
        public const uint Iec60870DeviceType_InterrogationPeriodGroup11 = 357;

        /// <summary>
        /// The identifier for the Iec60870DeviceType_InterrogationPeriodGroup12 Variable.
        /// </summary>
        public const uint Iec60870DeviceType_InterrogationPeriodGroup12 = 358;

        /// <summary>
        /// The identifier for the Iec60870DeviceType_InterrogationPeriodGroup13 Variable.
        /// </summary>
        public const uint Iec60870DeviceType_InterrogationPeriodGroup13 = 359;

        /// <summary>
        /// The identifier for the Iec60870DeviceType_InterrogationPeriodGroup14 Variable.
        /// </summary>
        public const uint Iec60870DeviceType_InterrogationPeriodGroup14 = 360;

        /// <summary>
        /// The identifier for the Iec60870DeviceType_InterrogationPeriodGroup15 Variable.
        /// </summary>
        public const uint Iec60870DeviceType_InterrogationPeriodGroup15 = 361;

        /// <summary>
        /// The identifier for the Iec60870DeviceType_InterrogationPeriodGroup16 Variable.
        /// </summary>
        public const uint Iec60870DeviceType_InterrogationPeriodGroup16 = 362;

        /// <summary>
        /// The identifier for the Iec61850DeviceType_Host Variable.
        /// </summary>
        public const uint Iec61850DeviceType_Host = 382;

        /// <summary>
        /// The identifier for the Iec61850DeviceType_Port Variable.
        /// </summary>
        public const uint Iec61850DeviceType_Port = 383;

        /// <summary>
        /// The identifier for the Iec61850ConfigurableObjectType_Reference Variable.
        /// </summary>
        public const uint Iec61850ConfigurableObjectType_Reference = 385;

        /// <summary>
        /// The identifier for the Iec61850ServerType_Host Variable.
        /// </summary>
        public const uint Iec61850ServerType_Host = 15021;

        /// <summary>
        /// The identifier for the Iec61850ServerType_Port Variable.
        /// </summary>
        public const uint Iec61850ServerType_Port = 15022;

        /// <summary>
        /// The identifier for the ModbusLinkProtocol_EnumStrings Variable.
        /// </summary>
        public const uint ModbusLinkProtocol_EnumStrings = 15008;

        /// <summary>
        /// The identifier for the ModbusLinkType_Protocol Variable.
        /// </summary>
        public const uint ModbusLinkType_Protocol = 15007;

        /// <summary>
        /// The identifier for the ModbusLinkType_RequestDelay Variable.
        /// </summary>
        public const uint ModbusLinkType_RequestDelay = 15006;

        /// <summary>
        /// The identifier for the ModbusDeviceType_Address Variable.
        /// </summary>
        public const uint ModbusDeviceType_Address = 120;

        /// <summary>
        /// The identifier for the ModbusDeviceType_SendRetryCount Variable.
        /// </summary>
        public const uint ModbusDeviceType_SendRetryCount = 121;

        /// <summary>
        /// The identifier for the ModbusDeviceType_ResponseTimeout Variable.
        /// </summary>
        public const uint ModbusDeviceType_ResponseTimeout = 15009;

        /// <summary>
        /// The identifier for the OpcServerType_MachineName Variable.
        /// </summary>
        public const uint OpcServerType_MachineName = 15031;

        /// <summary>
        /// The identifier for the OpcServerType_ProgId Variable.
        /// </summary>
        public const uint OpcServerType_ProgId = 15032;

        /// <summary>
        /// The identifier for the TransmissionItemType_SourceAddress Variable.
        /// </summary>
        public const uint TransmissionItemType_SourceAddress = 35;

        /// <summary>
        /// The identifier for the TelecontrolDevices_BinarySchema Variable.
        /// </summary>
        public const uint TelecontrolDevices_BinarySchema = 15001;

        /// <summary>
        /// The identifier for the TelecontrolDevices_BinarySchema_NamespaceUri Variable.
        /// </summary>
        public const uint TelecontrolDevices_BinarySchema_NamespaceUri = 15040;

        /// <summary>
        /// The identifier for the TelecontrolDevices_BinarySchema_Deprecated Variable.
        /// </summary>
        public const uint TelecontrolDevices_BinarySchema_Deprecated = 15041;

        /// <summary>
        /// The identifier for the TelecontrolDevices_XmlSchema Variable.
        /// </summary>
        public const uint TelecontrolDevices_XmlSchema = 15042;

        /// <summary>
        /// The identifier for the TelecontrolDevices_XmlSchema_NamespaceUri Variable.
        /// </summary>
        public const uint TelecontrolDevices_XmlSchema_NamespaceUri = 15044;

        /// <summary>
        /// The identifier for the TelecontrolDevices_XmlSchema_Deprecated Variable.
        /// </summary>
        public const uint TelecontrolDevices_XmlSchema_Deprecated = 15045;
    }
    #endregion

    #region VariableType Identifiers
    /// <summary>
    /// A class that declares constants for all VariableTypes in the Model Design.
    /// </summary>
    /// <exclude />
    [System.CodeDom.Compiler.GeneratedCodeAttribute("Opc.Ua.ModelCompiler", "1.0.0.0")]
    public static partial class VariableTypes
    {
        /// <summary>
        /// The identifier for the Iec61850DataVariableType VariableType.
        /// </summary>
        public const uint Iec61850DataVariableType = 15011;
    }
    #endregion

    #region DataType Node Identifiers
    /// <summary>
    /// A class that declares constants for all DataTypes in the Model Design.
    /// </summary>
    /// <exclude />
    [System.CodeDom.Compiler.GeneratedCodeAttribute("Opc.Ua.ModelCompiler", "1.0.0.0")]
    public static partial class DataTypeIds
    {
        /// <summary>
        /// The identifier for the Iec60870ProtocolDataType DataType.
        /// </summary>
        public static readonly ExpandedNodeId Iec60870ProtocolDataType = new ExpandedNodeId(Telecontrol.Devices.DataTypes.Iec60870ProtocolDataType, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec60870ModeDataType DataType.
        /// </summary>
        public static readonly ExpandedNodeId Iec60870ModeDataType = new ExpandedNodeId(Telecontrol.Devices.DataTypes.Iec60870ModeDataType, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the ModbusLinkProtocol DataType.
        /// </summary>
        public static readonly ExpandedNodeId ModbusLinkProtocol = new ExpandedNodeId(Telecontrol.Devices.DataTypes.ModbusLinkProtocol, Telecontrol.Devices.Namespaces.TelecontrolDevices);
    }
    #endregion

    #region Method Node Identifiers
    /// <summary>
    /// A class that declares constants for all Methods in the Model Design.
    /// </summary>
    /// <exclude />
    [System.CodeDom.Compiler.GeneratedCodeAttribute("Opc.Ua.ModelCompiler", "1.0.0.0")]
    public static partial class MethodIds
    {
        /// <summary>
        /// The identifier for the DeviceType_Interrogate Method.
        /// </summary>
        public static readonly ExpandedNodeId DeviceType_Interrogate = new ExpandedNodeId(Telecontrol.Devices.Methods.DeviceType_Interrogate, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the DeviceType_InterrogateChannel Method.
        /// </summary>
        public static readonly ExpandedNodeId DeviceType_InterrogateChannel = new ExpandedNodeId(Telecontrol.Devices.Methods.DeviceType_InterrogateChannel, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the DeviceType_SyncClock Method.
        /// </summary>
        public static readonly ExpandedNodeId DeviceType_SyncClock = new ExpandedNodeId(Telecontrol.Devices.Methods.DeviceType_SyncClock, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the DeviceType_Select Method.
        /// </summary>
        public static readonly ExpandedNodeId DeviceType_Select = new ExpandedNodeId(Telecontrol.Devices.Methods.DeviceType_Select, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the DeviceType_Write Method.
        /// </summary>
        public static readonly ExpandedNodeId DeviceType_Write = new ExpandedNodeId(Telecontrol.Devices.Methods.DeviceType_Write, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the DeviceType_WriteParam Method.
        /// </summary>
        public static readonly ExpandedNodeId DeviceType_WriteParam = new ExpandedNodeId(Telecontrol.Devices.Methods.DeviceType_WriteParam, Telecontrol.Devices.Namespaces.TelecontrolDevices);
    }
    #endregion

    #region Object Node Identifiers
    /// <summary>
    /// A class that declares constants for all Objects in the Model Design.
    /// </summary>
    /// <exclude />
    [System.CodeDom.Compiler.GeneratedCodeAttribute("Opc.Ua.ModelCompiler", "1.0.0.0")]
    public static partial class ObjectIds
    {
        /// <summary>
        /// The identifier for the Devices Object.
        /// </summary>
        public static readonly ExpandedNodeId Devices = new ExpandedNodeId(Telecontrol.Devices.Objects.Devices, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec61850DeviceType_Model Object.
        /// </summary>
        public static readonly ExpandedNodeId Iec61850DeviceType_Model = new ExpandedNodeId(Telecontrol.Devices.Objects.Iec61850DeviceType_Model, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the TransmissionItems Object.
        /// </summary>
        public static readonly ExpandedNodeId TransmissionItems = new ExpandedNodeId(Telecontrol.Devices.Objects.TransmissionItems, Telecontrol.Devices.Namespaces.TelecontrolDevices);
    }
    #endregion

    #region ObjectType Node Identifiers
    /// <summary>
    /// A class that declares constants for all ObjectTypes in the Model Design.
    /// </summary>
    /// <exclude />
    [System.CodeDom.Compiler.GeneratedCodeAttribute("Opc.Ua.ModelCompiler", "1.0.0.0")]
    public static partial class ObjectTypeIds
    {
        /// <summary>
        /// The identifier for the DeviceType ObjectType.
        /// </summary>
        public static readonly ExpandedNodeId DeviceType = new ExpandedNodeId(Telecontrol.Devices.ObjectTypes.DeviceType, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the LinkType ObjectType.
        /// </summary>
        public static readonly ExpandedNodeId LinkType = new ExpandedNodeId(Telecontrol.Devices.ObjectTypes.LinkType, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the DeviceWatchEventType ObjectType.
        /// </summary>
        public static readonly ExpandedNodeId DeviceWatchEventType = new ExpandedNodeId(Telecontrol.Devices.ObjectTypes.DeviceWatchEventType, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec60870LinkType ObjectType.
        /// </summary>
        public static readonly ExpandedNodeId Iec60870LinkType = new ExpandedNodeId(Telecontrol.Devices.ObjectTypes.Iec60870LinkType, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec60870DeviceType ObjectType.
        /// </summary>
        public static readonly ExpandedNodeId Iec60870DeviceType = new ExpandedNodeId(Telecontrol.Devices.ObjectTypes.Iec60870DeviceType, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec61850LogicalNodeType ObjectType.
        /// </summary>
        public static readonly ExpandedNodeId Iec61850LogicalNodeType = new ExpandedNodeId(Telecontrol.Devices.ObjectTypes.Iec61850LogicalNodeType, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec61850ControlObjectType ObjectType.
        /// </summary>
        public static readonly ExpandedNodeId Iec61850ControlObjectType = new ExpandedNodeId(Telecontrol.Devices.ObjectTypes.Iec61850ControlObjectType, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec61850DeviceType ObjectType.
        /// </summary>
        public static readonly ExpandedNodeId Iec61850DeviceType = new ExpandedNodeId(Telecontrol.Devices.ObjectTypes.Iec61850DeviceType, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec61850ConfigurableObjectType ObjectType.
        /// </summary>
        public static readonly ExpandedNodeId Iec61850ConfigurableObjectType = new ExpandedNodeId(Telecontrol.Devices.ObjectTypes.Iec61850ConfigurableObjectType, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec61850RcbType ObjectType.
        /// </summary>
        public static readonly ExpandedNodeId Iec61850RcbType = new ExpandedNodeId(Telecontrol.Devices.ObjectTypes.Iec61850RcbType, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec61850ServerType ObjectType.
        /// </summary>
        public static readonly ExpandedNodeId Iec61850ServerType = new ExpandedNodeId(Telecontrol.Devices.ObjectTypes.Iec61850ServerType, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the ModbusLinkType ObjectType.
        /// </summary>
        public static readonly ExpandedNodeId ModbusLinkType = new ExpandedNodeId(Telecontrol.Devices.ObjectTypes.ModbusLinkType, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the ModbusDeviceType ObjectType.
        /// </summary>
        public static readonly ExpandedNodeId ModbusDeviceType = new ExpandedNodeId(Telecontrol.Devices.ObjectTypes.ModbusDeviceType, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the OpcServerType ObjectType.
        /// </summary>
        public static readonly ExpandedNodeId OpcServerType = new ExpandedNodeId(Telecontrol.Devices.ObjectTypes.OpcServerType, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the TransmissionItemType ObjectType.
        /// </summary>
        public static readonly ExpandedNodeId TransmissionItemType = new ExpandedNodeId(Telecontrol.Devices.ObjectTypes.TransmissionItemType, Telecontrol.Devices.Namespaces.TelecontrolDevices);
    }
    #endregion

    #region ReferenceType Node Identifiers
    /// <summary>
    /// A class that declares constants for all ReferenceTypes in the Model Design.
    /// </summary>
    /// <exclude />
    [System.CodeDom.Compiler.GeneratedCodeAttribute("Opc.Ua.ModelCompiler", "1.0.0.0")]
    public static partial class ReferenceTypeIds
    {
        /// <summary>
        /// The identifier for the HasTransmissionSource ReferenceType.
        /// </summary>
        public static readonly ExpandedNodeId HasTransmissionSource = new ExpandedNodeId(Telecontrol.Devices.ReferenceTypes.HasTransmissionSource, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the HasTransmissionTarget ReferenceType.
        /// </summary>
        public static readonly ExpandedNodeId HasTransmissionTarget = new ExpandedNodeId(Telecontrol.Devices.ReferenceTypes.HasTransmissionTarget, Telecontrol.Devices.Namespaces.TelecontrolDevices);
    }
    #endregion

    #region Variable Node Identifiers
    /// <summary>
    /// A class that declares constants for all Variables in the Model Design.
    /// </summary>
    /// <exclude />
    [System.CodeDom.Compiler.GeneratedCodeAttribute("Opc.Ua.ModelCompiler", "1.0.0.0")]
    public static partial class VariableIds
    {
        /// <summary>
        /// The identifier for the DeviceType_Disabled Variable.
        /// </summary>
        public static readonly ExpandedNodeId DeviceType_Disabled = new ExpandedNodeId(Telecontrol.Devices.Variables.DeviceType_Disabled, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the DeviceType_Online Variable.
        /// </summary>
        public static readonly ExpandedNodeId DeviceType_Online = new ExpandedNodeId(Telecontrol.Devices.Variables.DeviceType_Online, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the DeviceType_Enabled Variable.
        /// </summary>
        public static readonly ExpandedNodeId DeviceType_Enabled = new ExpandedNodeId(Telecontrol.Devices.Variables.DeviceType_Enabled, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the LinkType_Transport Variable.
        /// </summary>
        public static readonly ExpandedNodeId LinkType_Transport = new ExpandedNodeId(Telecontrol.Devices.Variables.LinkType_Transport, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the LinkType_ConnectCount Variable.
        /// </summary>
        public static readonly ExpandedNodeId LinkType_ConnectCount = new ExpandedNodeId(Telecontrol.Devices.Variables.LinkType_ConnectCount, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the LinkType_ActiveConnections Variable.
        /// </summary>
        public static readonly ExpandedNodeId LinkType_ActiveConnections = new ExpandedNodeId(Telecontrol.Devices.Variables.LinkType_ActiveConnections, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the LinkType_MessagesOut Variable.
        /// </summary>
        public static readonly ExpandedNodeId LinkType_MessagesOut = new ExpandedNodeId(Telecontrol.Devices.Variables.LinkType_MessagesOut, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the LinkType_MessagesIn Variable.
        /// </summary>
        public static readonly ExpandedNodeId LinkType_MessagesIn = new ExpandedNodeId(Telecontrol.Devices.Variables.LinkType_MessagesIn, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the LinkType_BytesOut Variable.
        /// </summary>
        public static readonly ExpandedNodeId LinkType_BytesOut = new ExpandedNodeId(Telecontrol.Devices.Variables.LinkType_BytesOut, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the LinkType_BytesIn Variable.
        /// </summary>
        public static readonly ExpandedNodeId LinkType_BytesIn = new ExpandedNodeId(Telecontrol.Devices.Variables.LinkType_BytesIn, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec60870ProtocolDataType_EnumStrings Variable.
        /// </summary>
        public static readonly ExpandedNodeId Iec60870ProtocolDataType_EnumStrings = new ExpandedNodeId(Telecontrol.Devices.Variables.Iec60870ProtocolDataType_EnumStrings, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec60870ModeDataType_EnumStrings Variable.
        /// </summary>
        public static readonly ExpandedNodeId Iec60870ModeDataType_EnumStrings = new ExpandedNodeId(Telecontrol.Devices.Variables.Iec60870ModeDataType_EnumStrings, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec60870LinkType_Protocol Variable.
        /// </summary>
        public static readonly ExpandedNodeId Iec60870LinkType_Protocol = new ExpandedNodeId(Telecontrol.Devices.Variables.Iec60870LinkType_Protocol, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec60870LinkType_Mode Variable.
        /// </summary>
        public static readonly ExpandedNodeId Iec60870LinkType_Mode = new ExpandedNodeId(Telecontrol.Devices.Variables.Iec60870LinkType_Mode, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec60870LinkType_SendQueueSize Variable.
        /// </summary>
        public static readonly ExpandedNodeId Iec60870LinkType_SendQueueSize = new ExpandedNodeId(Telecontrol.Devices.Variables.Iec60870LinkType_SendQueueSize, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec60870LinkType_ReceiveQueueSize Variable.
        /// </summary>
        public static readonly ExpandedNodeId Iec60870LinkType_ReceiveQueueSize = new ExpandedNodeId(Telecontrol.Devices.Variables.Iec60870LinkType_ReceiveQueueSize, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec60870LinkType_ConnectTimeout Variable.
        /// </summary>
        public static readonly ExpandedNodeId Iec60870LinkType_ConnectTimeout = new ExpandedNodeId(Telecontrol.Devices.Variables.Iec60870LinkType_ConnectTimeout, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec60870LinkType_ConfirmationTimeout Variable.
        /// </summary>
        public static readonly ExpandedNodeId Iec60870LinkType_ConfirmationTimeout = new ExpandedNodeId(Telecontrol.Devices.Variables.Iec60870LinkType_ConfirmationTimeout, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec60870LinkType_TerminationTimeout Variable.
        /// </summary>
        public static readonly ExpandedNodeId Iec60870LinkType_TerminationTimeout = new ExpandedNodeId(Telecontrol.Devices.Variables.Iec60870LinkType_TerminationTimeout, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec60870LinkType_DeviceAddressSize Variable.
        /// </summary>
        public static readonly ExpandedNodeId Iec60870LinkType_DeviceAddressSize = new ExpandedNodeId(Telecontrol.Devices.Variables.Iec60870LinkType_DeviceAddressSize, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec60870LinkType_COTSize Variable.
        /// </summary>
        public static readonly ExpandedNodeId Iec60870LinkType_COTSize = new ExpandedNodeId(Telecontrol.Devices.Variables.Iec60870LinkType_COTSize, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec60870LinkType_InfoAddressSize Variable.
        /// </summary>
        public static readonly ExpandedNodeId Iec60870LinkType_InfoAddressSize = new ExpandedNodeId(Telecontrol.Devices.Variables.Iec60870LinkType_InfoAddressSize, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec60870LinkType_DataCollection Variable.
        /// </summary>
        public static readonly ExpandedNodeId Iec60870LinkType_DataCollection = new ExpandedNodeId(Telecontrol.Devices.Variables.Iec60870LinkType_DataCollection, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec60870LinkType_SendRetryCount Variable.
        /// </summary>
        public static readonly ExpandedNodeId Iec60870LinkType_SendRetryCount = new ExpandedNodeId(Telecontrol.Devices.Variables.Iec60870LinkType_SendRetryCount, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec60870LinkType_CRCProtection Variable.
        /// </summary>
        public static readonly ExpandedNodeId Iec60870LinkType_CRCProtection = new ExpandedNodeId(Telecontrol.Devices.Variables.Iec60870LinkType_CRCProtection, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec60870LinkType_SendTimeout Variable.
        /// </summary>
        public static readonly ExpandedNodeId Iec60870LinkType_SendTimeout = new ExpandedNodeId(Telecontrol.Devices.Variables.Iec60870LinkType_SendTimeout, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec60870LinkType_ReceiveTimeout Variable.
        /// </summary>
        public static readonly ExpandedNodeId Iec60870LinkType_ReceiveTimeout = new ExpandedNodeId(Telecontrol.Devices.Variables.Iec60870LinkType_ReceiveTimeout, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec60870LinkType_IdleTimeout Variable.
        /// </summary>
        public static readonly ExpandedNodeId Iec60870LinkType_IdleTimeout = new ExpandedNodeId(Telecontrol.Devices.Variables.Iec60870LinkType_IdleTimeout, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec60870LinkType_AnonymousMode Variable.
        /// </summary>
        public static readonly ExpandedNodeId Iec60870LinkType_AnonymousMode = new ExpandedNodeId(Telecontrol.Devices.Variables.Iec60870LinkType_AnonymousMode, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec60870DeviceType_Address Variable.
        /// </summary>
        public static readonly ExpandedNodeId Iec60870DeviceType_Address = new ExpandedNodeId(Telecontrol.Devices.Variables.Iec60870DeviceType_Address, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec60870DeviceType_LinkAddress Variable.
        /// </summary>
        public static readonly ExpandedNodeId Iec60870DeviceType_LinkAddress = new ExpandedNodeId(Telecontrol.Devices.Variables.Iec60870DeviceType_LinkAddress, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec60870DeviceType_StartupInterrogation Variable.
        /// </summary>
        public static readonly ExpandedNodeId Iec60870DeviceType_StartupInterrogation = new ExpandedNodeId(Telecontrol.Devices.Variables.Iec60870DeviceType_StartupInterrogation, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec60870DeviceType_InterrogationPeriod Variable.
        /// </summary>
        public static readonly ExpandedNodeId Iec60870DeviceType_InterrogationPeriod = new ExpandedNodeId(Telecontrol.Devices.Variables.Iec60870DeviceType_InterrogationPeriod, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec60870DeviceType_StartupClockSync Variable.
        /// </summary>
        public static readonly ExpandedNodeId Iec60870DeviceType_StartupClockSync = new ExpandedNodeId(Telecontrol.Devices.Variables.Iec60870DeviceType_StartupClockSync, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec60870DeviceType_ClockSyncPeriod Variable.
        /// </summary>
        public static readonly ExpandedNodeId Iec60870DeviceType_ClockSyncPeriod = new ExpandedNodeId(Telecontrol.Devices.Variables.Iec60870DeviceType_ClockSyncPeriod, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec60870DeviceType_UtcTime Variable.
        /// </summary>
        public static readonly ExpandedNodeId Iec60870DeviceType_UtcTime = new ExpandedNodeId(Telecontrol.Devices.Variables.Iec60870DeviceType_UtcTime, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec60870DeviceType_InterrogationPeriodGroup1 Variable.
        /// </summary>
        public static readonly ExpandedNodeId Iec60870DeviceType_InterrogationPeriodGroup1 = new ExpandedNodeId(Telecontrol.Devices.Variables.Iec60870DeviceType_InterrogationPeriodGroup1, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec60870DeviceType_InterrogationPeriodGroup2 Variable.
        /// </summary>
        public static readonly ExpandedNodeId Iec60870DeviceType_InterrogationPeriodGroup2 = new ExpandedNodeId(Telecontrol.Devices.Variables.Iec60870DeviceType_InterrogationPeriodGroup2, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec60870DeviceType_InterrogationPeriodGroup3 Variable.
        /// </summary>
        public static readonly ExpandedNodeId Iec60870DeviceType_InterrogationPeriodGroup3 = new ExpandedNodeId(Telecontrol.Devices.Variables.Iec60870DeviceType_InterrogationPeriodGroup3, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec60870DeviceType_InterrogationPeriodGroup4 Variable.
        /// </summary>
        public static readonly ExpandedNodeId Iec60870DeviceType_InterrogationPeriodGroup4 = new ExpandedNodeId(Telecontrol.Devices.Variables.Iec60870DeviceType_InterrogationPeriodGroup4, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec60870DeviceType_InterrogationPeriodGroup5 Variable.
        /// </summary>
        public static readonly ExpandedNodeId Iec60870DeviceType_InterrogationPeriodGroup5 = new ExpandedNodeId(Telecontrol.Devices.Variables.Iec60870DeviceType_InterrogationPeriodGroup5, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec60870DeviceType_InterrogationPeriodGroup6 Variable.
        /// </summary>
        public static readonly ExpandedNodeId Iec60870DeviceType_InterrogationPeriodGroup6 = new ExpandedNodeId(Telecontrol.Devices.Variables.Iec60870DeviceType_InterrogationPeriodGroup6, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec60870DeviceType_InterrogationPeriodGroup7 Variable.
        /// </summary>
        public static readonly ExpandedNodeId Iec60870DeviceType_InterrogationPeriodGroup7 = new ExpandedNodeId(Telecontrol.Devices.Variables.Iec60870DeviceType_InterrogationPeriodGroup7, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec60870DeviceType_InterrogationPeriodGroup8 Variable.
        /// </summary>
        public static readonly ExpandedNodeId Iec60870DeviceType_InterrogationPeriodGroup8 = new ExpandedNodeId(Telecontrol.Devices.Variables.Iec60870DeviceType_InterrogationPeriodGroup8, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec60870DeviceType_InterrogationPeriodGroup9 Variable.
        /// </summary>
        public static readonly ExpandedNodeId Iec60870DeviceType_InterrogationPeriodGroup9 = new ExpandedNodeId(Telecontrol.Devices.Variables.Iec60870DeviceType_InterrogationPeriodGroup9, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec60870DeviceType_InterrogationPeriodGroup10 Variable.
        /// </summary>
        public static readonly ExpandedNodeId Iec60870DeviceType_InterrogationPeriodGroup10 = new ExpandedNodeId(Telecontrol.Devices.Variables.Iec60870DeviceType_InterrogationPeriodGroup10, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec60870DeviceType_InterrogationPeriodGroup11 Variable.
        /// </summary>
        public static readonly ExpandedNodeId Iec60870DeviceType_InterrogationPeriodGroup11 = new ExpandedNodeId(Telecontrol.Devices.Variables.Iec60870DeviceType_InterrogationPeriodGroup11, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec60870DeviceType_InterrogationPeriodGroup12 Variable.
        /// </summary>
        public static readonly ExpandedNodeId Iec60870DeviceType_InterrogationPeriodGroup12 = new ExpandedNodeId(Telecontrol.Devices.Variables.Iec60870DeviceType_InterrogationPeriodGroup12, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec60870DeviceType_InterrogationPeriodGroup13 Variable.
        /// </summary>
        public static readonly ExpandedNodeId Iec60870DeviceType_InterrogationPeriodGroup13 = new ExpandedNodeId(Telecontrol.Devices.Variables.Iec60870DeviceType_InterrogationPeriodGroup13, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec60870DeviceType_InterrogationPeriodGroup14 Variable.
        /// </summary>
        public static readonly ExpandedNodeId Iec60870DeviceType_InterrogationPeriodGroup14 = new ExpandedNodeId(Telecontrol.Devices.Variables.Iec60870DeviceType_InterrogationPeriodGroup14, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec60870DeviceType_InterrogationPeriodGroup15 Variable.
        /// </summary>
        public static readonly ExpandedNodeId Iec60870DeviceType_InterrogationPeriodGroup15 = new ExpandedNodeId(Telecontrol.Devices.Variables.Iec60870DeviceType_InterrogationPeriodGroup15, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec60870DeviceType_InterrogationPeriodGroup16 Variable.
        /// </summary>
        public static readonly ExpandedNodeId Iec60870DeviceType_InterrogationPeriodGroup16 = new ExpandedNodeId(Telecontrol.Devices.Variables.Iec60870DeviceType_InterrogationPeriodGroup16, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec61850DeviceType_Host Variable.
        /// </summary>
        public static readonly ExpandedNodeId Iec61850DeviceType_Host = new ExpandedNodeId(Telecontrol.Devices.Variables.Iec61850DeviceType_Host, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec61850DeviceType_Port Variable.
        /// </summary>
        public static readonly ExpandedNodeId Iec61850DeviceType_Port = new ExpandedNodeId(Telecontrol.Devices.Variables.Iec61850DeviceType_Port, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec61850ConfigurableObjectType_Reference Variable.
        /// </summary>
        public static readonly ExpandedNodeId Iec61850ConfigurableObjectType_Reference = new ExpandedNodeId(Telecontrol.Devices.Variables.Iec61850ConfigurableObjectType_Reference, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec61850ServerType_Host Variable.
        /// </summary>
        public static readonly ExpandedNodeId Iec61850ServerType_Host = new ExpandedNodeId(Telecontrol.Devices.Variables.Iec61850ServerType_Host, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the Iec61850ServerType_Port Variable.
        /// </summary>
        public static readonly ExpandedNodeId Iec61850ServerType_Port = new ExpandedNodeId(Telecontrol.Devices.Variables.Iec61850ServerType_Port, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the ModbusLinkProtocol_EnumStrings Variable.
        /// </summary>
        public static readonly ExpandedNodeId ModbusLinkProtocol_EnumStrings = new ExpandedNodeId(Telecontrol.Devices.Variables.ModbusLinkProtocol_EnumStrings, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the ModbusLinkType_Protocol Variable.
        /// </summary>
        public static readonly ExpandedNodeId ModbusLinkType_Protocol = new ExpandedNodeId(Telecontrol.Devices.Variables.ModbusLinkType_Protocol, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the ModbusLinkType_RequestDelay Variable.
        /// </summary>
        public static readonly ExpandedNodeId ModbusLinkType_RequestDelay = new ExpandedNodeId(Telecontrol.Devices.Variables.ModbusLinkType_RequestDelay, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the ModbusDeviceType_Address Variable.
        /// </summary>
        public static readonly ExpandedNodeId ModbusDeviceType_Address = new ExpandedNodeId(Telecontrol.Devices.Variables.ModbusDeviceType_Address, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the ModbusDeviceType_SendRetryCount Variable.
        /// </summary>
        public static readonly ExpandedNodeId ModbusDeviceType_SendRetryCount = new ExpandedNodeId(Telecontrol.Devices.Variables.ModbusDeviceType_SendRetryCount, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the ModbusDeviceType_ResponseTimeout Variable.
        /// </summary>
        public static readonly ExpandedNodeId ModbusDeviceType_ResponseTimeout = new ExpandedNodeId(Telecontrol.Devices.Variables.ModbusDeviceType_ResponseTimeout, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the OpcServerType_MachineName Variable.
        /// </summary>
        public static readonly ExpandedNodeId OpcServerType_MachineName = new ExpandedNodeId(Telecontrol.Devices.Variables.OpcServerType_MachineName, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the OpcServerType_ProgId Variable.
        /// </summary>
        public static readonly ExpandedNodeId OpcServerType_ProgId = new ExpandedNodeId(Telecontrol.Devices.Variables.OpcServerType_ProgId, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the TransmissionItemType_SourceAddress Variable.
        /// </summary>
        public static readonly ExpandedNodeId TransmissionItemType_SourceAddress = new ExpandedNodeId(Telecontrol.Devices.Variables.TransmissionItemType_SourceAddress, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the TelecontrolDevices_BinarySchema Variable.
        /// </summary>
        public static readonly ExpandedNodeId TelecontrolDevices_BinarySchema = new ExpandedNodeId(Telecontrol.Devices.Variables.TelecontrolDevices_BinarySchema, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the TelecontrolDevices_BinarySchema_NamespaceUri Variable.
        /// </summary>
        public static readonly ExpandedNodeId TelecontrolDevices_BinarySchema_NamespaceUri = new ExpandedNodeId(Telecontrol.Devices.Variables.TelecontrolDevices_BinarySchema_NamespaceUri, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the TelecontrolDevices_BinarySchema_Deprecated Variable.
        /// </summary>
        public static readonly ExpandedNodeId TelecontrolDevices_BinarySchema_Deprecated = new ExpandedNodeId(Telecontrol.Devices.Variables.TelecontrolDevices_BinarySchema_Deprecated, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the TelecontrolDevices_XmlSchema Variable.
        /// </summary>
        public static readonly ExpandedNodeId TelecontrolDevices_XmlSchema = new ExpandedNodeId(Telecontrol.Devices.Variables.TelecontrolDevices_XmlSchema, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the TelecontrolDevices_XmlSchema_NamespaceUri Variable.
        /// </summary>
        public static readonly ExpandedNodeId TelecontrolDevices_XmlSchema_NamespaceUri = new ExpandedNodeId(Telecontrol.Devices.Variables.TelecontrolDevices_XmlSchema_NamespaceUri, Telecontrol.Devices.Namespaces.TelecontrolDevices);

        /// <summary>
        /// The identifier for the TelecontrolDevices_XmlSchema_Deprecated Variable.
        /// </summary>
        public static readonly ExpandedNodeId TelecontrolDevices_XmlSchema_Deprecated = new ExpandedNodeId(Telecontrol.Devices.Variables.TelecontrolDevices_XmlSchema_Deprecated, Telecontrol.Devices.Namespaces.TelecontrolDevices);
    }
    #endregion

    #region VariableType Node Identifiers
    /// <summary>
    /// A class that declares constants for all VariableTypes in the Model Design.
    /// </summary>
    /// <exclude />
    [System.CodeDom.Compiler.GeneratedCodeAttribute("Opc.Ua.ModelCompiler", "1.0.0.0")]
    public static partial class VariableTypeIds
    {
        /// <summary>
        /// The identifier for the Iec61850DataVariableType VariableType.
        /// </summary>
        public static readonly ExpandedNodeId Iec61850DataVariableType = new ExpandedNodeId(Telecontrol.Devices.VariableTypes.Iec61850DataVariableType, Telecontrol.Devices.Namespaces.TelecontrolDevices);
    }
    #endregion

    #region BrowseName Declarations
    /// <summary>
    /// Declares all of the BrowseNames used in the Model Design.
    /// </summary>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("Opc.Ua.ModelCompiler", "1.0.0.0")]
    public static partial class BrowseNames
    {
        /// <summary>
        /// The BrowseName for the ActiveConnections component.
        /// </summary>
        public const string ActiveConnections = "ActiveConnections";

        /// <summary>
        /// The BrowseName for the Address component.
        /// </summary>
        public const string Address = "Address";

        /// <summary>
        /// The BrowseName for the AnonymousMode component.
        /// </summary>
        public const string AnonymousMode = "AnonymousMode";

        /// <summary>
        /// The BrowseName for the BytesIn component.
        /// </summary>
        public const string BytesIn = "BytesIn";

        /// <summary>
        /// The BrowseName for the BytesOut component.
        /// </summary>
        public const string BytesOut = "BytesOut";

        /// <summary>
        /// The BrowseName for the ClockSyncPeriod component.
        /// </summary>
        public const string ClockSyncPeriod = "ClockSyncPeriod";

        /// <summary>
        /// The BrowseName for the ConfirmationTimeout component.
        /// </summary>
        public const string ConfirmationTimeout = "ConfirmationTimeout";

        /// <summary>
        /// The BrowseName for the ConnectCount component.
        /// </summary>
        public const string ConnectCount = "ConnectCount";

        /// <summary>
        /// The BrowseName for the ConnectTimeout component.
        /// </summary>
        public const string ConnectTimeout = "ConnectTimeout";

        /// <summary>
        /// The BrowseName for the COTSize component.
        /// </summary>
        public const string COTSize = "COTSize";

        /// <summary>
        /// The BrowseName for the CRCProtection component.
        /// </summary>
        public const string CRCProtection = "CRCProtection";

        /// <summary>
        /// The BrowseName for the DataCollection component.
        /// </summary>
        public const string DataCollection = "DataCollection";

        /// <summary>
        /// The BrowseName for the DeviceAddressSize component.
        /// </summary>
        public const string DeviceAddressSize = "DeviceAddressSize";

        /// <summary>
        /// The BrowseName for the Devices component.
        /// </summary>
        public const string Devices = "Devices";

        /// <summary>
        /// The BrowseName for the DeviceType component.
        /// </summary>
        public const string DeviceType = "DeviceType";

        /// <summary>
        /// The BrowseName for the DeviceWatchEventType component.
        /// </summary>
        public const string DeviceWatchEventType = "DeviceWatchEventType";

        /// <summary>
        /// The BrowseName for the Disabled component.
        /// </summary>
        public const string Disabled = "Disabled";

        /// <summary>
        /// The BrowseName for the Enabled component.
        /// </summary>
        public const string Enabled = "Enabled";

        /// <summary>
        /// The BrowseName for the HasTransmissionSource component.
        /// </summary>
        public const string HasTransmissionSource = "HasTransmissionSource";

        /// <summary>
        /// The BrowseName for the HasTransmissionTarget component.
        /// </summary>
        public const string HasTransmissionTarget = "HasTransmissionTarget";

        /// <summary>
        /// The BrowseName for the Host component.
        /// </summary>
        public const string Host = "Host";

        /// <summary>
        /// The BrowseName for the IdleTimeout component.
        /// </summary>
        public const string IdleTimeout = "IdleTimeout";

        /// <summary>
        /// The BrowseName for the Iec60870DeviceType component.
        /// </summary>
        public const string Iec60870DeviceType = "Iec60870DeviceType";

        /// <summary>
        /// The BrowseName for the Iec60870LinkType component.
        /// </summary>
        public const string Iec60870LinkType = "Iec60870LinkType";

        /// <summary>
        /// The BrowseName for the Iec60870ModeDataType component.
        /// </summary>
        public const string Iec60870ModeDataType = "Iec60870ModeDataType";

        /// <summary>
        /// The BrowseName for the Iec60870ProtocolDataType component.
        /// </summary>
        public const string Iec60870ProtocolDataType = "Iec60870ProtocolDataType";

        /// <summary>
        /// The BrowseName for the Iec61850ConfigurableObjectType component.
        /// </summary>
        public const string Iec61850ConfigurableObjectType = "Iec61850ConfigurableObjectType";

        /// <summary>
        /// The BrowseName for the Iec61850ControlObjectType component.
        /// </summary>
        public const string Iec61850ControlObjectType = "Iec61850ControlObjectType";

        /// <summary>
        /// The BrowseName for the Iec61850DataVariableType component.
        /// </summary>
        public const string Iec61850DataVariableType = "Iec61850DataVariableType";

        /// <summary>
        /// The BrowseName for the Iec61850DeviceType component.
        /// </summary>
        public const string Iec61850DeviceType = "Iec61850DeviceType";

        /// <summary>
        /// The BrowseName for the Iec61850LogicalNodeType component.
        /// </summary>
        public const string Iec61850LogicalNodeType = "Iec61850LogicalNodeType";

        /// <summary>
        /// The BrowseName for the Iec61850RcbType component.
        /// </summary>
        public const string Iec61850RcbType = "Iec61850RcbType";

        /// <summary>
        /// The BrowseName for the Iec61850ServerType component.
        /// </summary>
        public const string Iec61850ServerType = "Iec61850ServerType";

        /// <summary>
        /// The BrowseName for the InfoAddressSize component.
        /// </summary>
        public const string InfoAddressSize = "InfoAddressSize";

        /// <summary>
        /// The BrowseName for the Interrogate component.
        /// </summary>
        public const string Interrogate = "Interrogate";

        /// <summary>
        /// The BrowseName for the InterrogateChannel component.
        /// </summary>
        public const string InterrogateChannel = "InterrogateChannel";

        /// <summary>
        /// The BrowseName for the InterrogationPeriod component.
        /// </summary>
        public const string InterrogationPeriod = "InterrogationPeriod";

        /// <summary>
        /// The BrowseName for the InterrogationPeriodGroup1 component.
        /// </summary>
        public const string InterrogationPeriodGroup1 = "InterrogationPeriodGroup1";

        /// <summary>
        /// The BrowseName for the InterrogationPeriodGroup10 component.
        /// </summary>
        public const string InterrogationPeriodGroup10 = "InterrogationPeriodGroup10";

        /// <summary>
        /// The BrowseName for the InterrogationPeriodGroup11 component.
        /// </summary>
        public const string InterrogationPeriodGroup11 = "InterrogationPeriodGroup11";

        /// <summary>
        /// The BrowseName for the InterrogationPeriodGroup12 component.
        /// </summary>
        public const string InterrogationPeriodGroup12 = "InterrogationPeriodGroup12";

        /// <summary>
        /// The BrowseName for the InterrogationPeriodGroup13 component.
        /// </summary>
        public const string InterrogationPeriodGroup13 = "InterrogationPeriodGroup13";

        /// <summary>
        /// The BrowseName for the InterrogationPeriodGroup14 component.
        /// </summary>
        public const string InterrogationPeriodGroup14 = "InterrogationPeriodGroup14";

        /// <summary>
        /// The BrowseName for the InterrogationPeriodGroup15 component.
        /// </summary>
        public const string InterrogationPeriodGroup15 = "InterrogationPeriodGroup15";

        /// <summary>
        /// The BrowseName for the InterrogationPeriodGroup16 component.
        /// </summary>
        public const string InterrogationPeriodGroup16 = "InterrogationPeriodGroup16";

        /// <summary>
        /// The BrowseName for the InterrogationPeriodGroup2 component.
        /// </summary>
        public const string InterrogationPeriodGroup2 = "InterrogationPeriodGroup2";

        /// <summary>
        /// The BrowseName for the InterrogationPeriodGroup3 component.
        /// </summary>
        public const string InterrogationPeriodGroup3 = "InterrogationPeriodGroup3";

        /// <summary>
        /// The BrowseName for the InterrogationPeriodGroup4 component.
        /// </summary>
        public const string InterrogationPeriodGroup4 = "InterrogationPeriodGroup4";

        /// <summary>
        /// The BrowseName for the InterrogationPeriodGroup5 component.
        /// </summary>
        public const string InterrogationPeriodGroup5 = "InterrogationPeriodGroup5";

        /// <summary>
        /// The BrowseName for the InterrogationPeriodGroup6 component.
        /// </summary>
        public const string InterrogationPeriodGroup6 = "InterrogationPeriodGroup6";

        /// <summary>
        /// The BrowseName for the InterrogationPeriodGroup7 component.
        /// </summary>
        public const string InterrogationPeriodGroup7 = "InterrogationPeriodGroup7";

        /// <summary>
        /// The BrowseName for the InterrogationPeriodGroup8 component.
        /// </summary>
        public const string InterrogationPeriodGroup8 = "InterrogationPeriodGroup8";

        /// <summary>
        /// The BrowseName for the InterrogationPeriodGroup9 component.
        /// </summary>
        public const string InterrogationPeriodGroup9 = "InterrogationPeriodGroup9";

        /// <summary>
        /// The BrowseName for the LinkAddress component.
        /// </summary>
        public const string LinkAddress = "LinkAddress";

        /// <summary>
        /// The BrowseName for the LinkType component.
        /// </summary>
        public const string LinkType = "LinkType";

        /// <summary>
        /// The BrowseName for the MachineName component.
        /// </summary>
        public const string MachineName = "MachineName";

        /// <summary>
        /// The BrowseName for the MessagesIn component.
        /// </summary>
        public const string MessagesIn = "MessagesIn";

        /// <summary>
        /// The BrowseName for the MessagesOut component.
        /// </summary>
        public const string MessagesOut = "MessagesOut";

        /// <summary>
        /// The BrowseName for the ModbusDeviceType component.
        /// </summary>
        public const string ModbusDeviceType = "ModbusDeviceType";

        /// <summary>
        /// The BrowseName for the ModbusLinkProtocol component.
        /// </summary>
        public const string ModbusLinkProtocol = "ModbusLinkProtocol";

        /// <summary>
        /// The BrowseName for the ModbusLinkType component.
        /// </summary>
        public const string ModbusLinkType = "ModbusLinkType";

        /// <summary>
        /// The BrowseName for the Mode component.
        /// </summary>
        public const string Mode = "Mode";

        /// <summary>
        /// The BrowseName for the Model component.
        /// </summary>
        public const string Model = "Model";

        /// <summary>
        /// The BrowseName for the Online component.
        /// </summary>
        public const string Online = "Online";

        /// <summary>
        /// The BrowseName for the OpcServerType component.
        /// </summary>
        public const string OpcServerType = "OpcServerType";

        /// <summary>
        /// The BrowseName for the Port component.
        /// </summary>
        public const string Port = "Port";

        /// <summary>
        /// The BrowseName for the ProgId component.
        /// </summary>
        public const string ProgId = "ProgId";

        /// <summary>
        /// The BrowseName for the Protocol component.
        /// </summary>
        public const string Protocol = "Protocol";

        /// <summary>
        /// The BrowseName for the ReceiveQueueSize component.
        /// </summary>
        public const string ReceiveQueueSize = "ReceiveQueueSize";

        /// <summary>
        /// The BrowseName for the ReceiveTimeout component.
        /// </summary>
        public const string ReceiveTimeout = "ReceiveTimeout";

        /// <summary>
        /// The BrowseName for the Reference component.
        /// </summary>
        public const string Reference = "Reference";

        /// <summary>
        /// The BrowseName for the RequestDelay component.
        /// </summary>
        public const string RequestDelay = "RequestDelay";

        /// <summary>
        /// The BrowseName for the ResponseTimeout component.
        /// </summary>
        public const string ResponseTimeout = "ResponseTimeout";

        /// <summary>
        /// The BrowseName for the Select component.
        /// </summary>
        public const string Select = "Select";

        /// <summary>
        /// The BrowseName for the SendQueueSize component.
        /// </summary>
        public const string SendQueueSize = "SendQueueSize";

        /// <summary>
        /// The BrowseName for the SendRetryCount component.
        /// </summary>
        public const string SendRetryCount = "SendRetryCount";

        /// <summary>
        /// The BrowseName for the SendTimeout component.
        /// </summary>
        public const string SendTimeout = "SendTimeout";

        /// <summary>
        /// The BrowseName for the SourceAddress component.
        /// </summary>
        public const string SourceAddress = "SourceAddress";

        /// <summary>
        /// The BrowseName for the StartupClockSync component.
        /// </summary>
        public const string StartupClockSync = "StartupClockSync";

        /// <summary>
        /// The BrowseName for the StartupInterrogation component.
        /// </summary>
        public const string StartupInterrogation = "StartupInterrogation";

        /// <summary>
        /// The BrowseName for the SyncClock component.
        /// </summary>
        public const string SyncClock = "SyncClock";

        /// <summary>
        /// The BrowseName for the TelecontrolDevices_BinarySchema component.
        /// </summary>
        public const string TelecontrolDevices_BinarySchema = "Telecontrol.Devices";

        /// <summary>
        /// The BrowseName for the TelecontrolDevices_XmlSchema component.
        /// </summary>
        public const string TelecontrolDevices_XmlSchema = "Telecontrol.Devices";

        /// <summary>
        /// The BrowseName for the TerminationTimeout component.
        /// </summary>
        public const string TerminationTimeout = "TerminationTimeout";

        /// <summary>
        /// The BrowseName for the TransmissionItems component.
        /// </summary>
        public const string TransmissionItems = "TransmissionItems";

        /// <summary>
        /// The BrowseName for the TransmissionItemType component.
        /// </summary>
        public const string TransmissionItemType = "TransmissionItemType";

        /// <summary>
        /// The BrowseName for the Transport component.
        /// </summary>
        public const string Transport = "Transport";

        /// <summary>
        /// The BrowseName for the UtcTime component.
        /// </summary>
        public const string UtcTime = "UtcTime";

        /// <summary>
        /// The BrowseName for the Write component.
        /// </summary>
        public const string Write = "Write";

        /// <summary>
        /// The BrowseName for the WriteParam component.
        /// </summary>
        public const string WriteParam = "WriteParam";
    }
    #endregion

    #region Namespace Declarations
    /// <summary>
    /// Defines constants for all namespaces referenced by the model design.
    /// </summary>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("Opc.Ua.ModelCompiler", "1.0.0.0")]
    public static partial class Namespaces
    {
        /// <summary>
        /// The URI for the OpcUa namespace (.NET code namespace is 'Opc.Ua').
        /// </summary>
        public const string OpcUa = "http://opcfoundation.org/UA/";

        /// <summary>
        /// The URI for the OpcUaXsd namespace (.NET code namespace is 'Opc.Ua').
        /// </summary>
        public const string OpcUaXsd = "http://opcfoundation.org/UA/2008/02/Types.xsd";

        /// <summary>
        /// The URI for the TelecontrolScada namespace (.NET code namespace is 'Telecontrol.Scada').
        /// </summary>
        public const string TelecontrolScada = "http://telecontrol.ru/opcua/scada";

        /// <summary>
        /// The URI for the TelecontrolDevices namespace (.NET code namespace is 'Telecontrol.Devices').
        /// </summary>
        public const string TelecontrolDevices = "http://telecontrol.ru/opcua/devices";
    }
    #endregion
}