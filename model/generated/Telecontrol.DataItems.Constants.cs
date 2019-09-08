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
using Telecontrol.History;

namespace Telecontrol.DataItems
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
        /// The identifier for the AnalogConversionDataType DataType.
        /// </summary>
        public const uint AnalogConversionDataType = 15038;

        /// <summary>
        /// The identifier for the SimulationFunctionDataType DataType.
        /// </summary>
        public const uint SimulationFunctionDataType = 15022;
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
        /// The identifier for the DataItemType_WriteManual Method.
        /// </summary>
        public const uint DataItemType_WriteManual = 285;
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
        /// The identifier for the ChannelsPropertyCategory Object.
        /// </summary>
        public const uint ChannelsPropertyCategory = 15028;

        /// <summary>
        /// The identifier for the ConversionPropertyCategory Object.
        /// </summary>
        public const uint ConversionPropertyCategory = 15029;

        /// <summary>
        /// The identifier for the FilteringPropertyCategory Object.
        /// </summary>
        public const uint FilteringPropertyCategory = 15030;

        /// <summary>
        /// The identifier for the DisplayPropertyCategory Object.
        /// </summary>
        public const uint DisplayPropertyCategory = 15031;

        /// <summary>
        /// The identifier for the HistoryPropertyCategory Object.
        /// </summary>
        public const uint HistoryPropertyCategory = 15032;

        /// <summary>
        /// The identifier for the SimulationPropertyCategory Object.
        /// </summary>
        public const uint SimulationPropertyCategory = 15033;

        /// <summary>
        /// The identifier for the LimitsPropertyCategory Object.
        /// </summary>
        public const uint LimitsPropertyCategory = 15034;

        /// <summary>
        /// The identifier for the Statistics Object.
        /// </summary>
        public const uint Statistics = 363;

        /// <summary>
        /// The identifier for the DataItems Object.
        /// </summary>
        public const uint DataItems = 24;

        /// <summary>
        /// The identifier for the TsFormats Object.
        /// </summary>
        public const uint TsFormats = 27;

        /// <summary>
        /// The identifier for the SimulationSignals Object.
        /// </summary>
        public const uint SimulationSignals = 28;

        /// <summary>
        /// The identifier for the Aliases Object.
        /// </summary>
        public const uint Aliases = 15019;
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
        /// The identifier for the DataGroupType ObjectType.
        /// </summary>
        public const uint DataGroupType = 62;

        /// <summary>
        /// The identifier for the TsFormatType ObjectType.
        /// </summary>
        public const uint TsFormatType = 80;

        /// <summary>
        /// The identifier for the AliasType ObjectType.
        /// </summary>
        public const uint AliasType = 15021;
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
        /// The identifier for the HasSimulationSignal ReferenceType.
        /// </summary>
        public const uint HasSimulationSignal = 69;

        /// <summary>
        /// The identifier for the HasTsFormat ReferenceType.
        /// </summary>
        public const uint HasTsFormat = 71;

        /// <summary>
        /// The identifier for the AliasOf ReferenceType.
        /// </summary>
        public const uint AliasOf = 15020;
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
        /// The identifier for the Statistics_ServerCPUUsage Variable.
        /// </summary>
        public const uint Statistics_ServerCPUUsage = 366;

        /// <summary>
        /// The identifier for the Statistics_ServerMemoryUsage Variable.
        /// </summary>
        public const uint Statistics_ServerMemoryUsage = 367;

        /// <summary>
        /// The identifier for the Statistics_TotalCPUUsage Variable.
        /// </summary>
        public const uint Statistics_TotalCPUUsage = 368;

        /// <summary>
        /// The identifier for the Statistics_TotalMemoryUsage Variable.
        /// </summary>
        public const uint Statistics_TotalMemoryUsage = 369;

        /// <summary>
        /// The identifier for the DataGroupType_Simulated Variable.
        /// </summary>
        public const uint DataGroupType_Simulated = 63;

        /// <summary>
        /// The identifier for the DataItemType_Alias Variable.
        /// </summary>
        public const uint DataItemType_Alias = 19;

        /// <summary>
        /// The identifier for the DataItemType_Severity Variable.
        /// </summary>
        public const uint DataItemType_Severity = 174;

        /// <summary>
        /// The identifier for the DataItemType_Input1 Variable.
        /// </summary>
        public const uint DataItemType_Input1 = 175;

        /// <summary>
        /// The identifier for the DataItemType_Input2 Variable.
        /// </summary>
        public const uint DataItemType_Input2 = 176;

        /// <summary>
        /// The identifier for the DataItemType_Output Variable.
        /// </summary>
        public const uint DataItemType_Output = 177;

        /// <summary>
        /// The identifier for the DataItemType_OutputCondition Variable.
        /// </summary>
        public const uint DataItemType_OutputCondition = 178;

        /// <summary>
        /// The identifier for the DataItemType_OutputTwoStaged Variable.
        /// </summary>
        public const uint DataItemType_OutputTwoStaged = 15025;

        /// <summary>
        /// The identifier for the DataItemType_StalePeriod Variable.
        /// </summary>
        public const uint DataItemType_StalePeriod = 179;

        /// <summary>
        /// The identifier for the DataItemType_Simulated Variable.
        /// </summary>
        public const uint DataItemType_Simulated = 66;

        /// <summary>
        /// The identifier for the DataItemType_Locked Variable.
        /// </summary>
        public const uint DataItemType_Locked = 180;

        /// <summary>
        /// The identifier for the DiscreteItemType_Inversion Variable.
        /// </summary>
        public const uint DiscreteItemType_Inversion = 188;

        /// <summary>
        /// The identifier for the AnalogConversionDataType_EnumStrings Variable.
        /// </summary>
        public const uint AnalogConversionDataType_EnumStrings = 15039;

        /// <summary>
        /// The identifier for the AnalogItemType_DisplayFormat Variable.
        /// </summary>
        public const uint AnalogItemType_DisplayFormat = 189;

        /// <summary>
        /// The identifier for the AnalogItemType_Conversion Variable.
        /// </summary>
        public const uint AnalogItemType_Conversion = 190;

        /// <summary>
        /// The identifier for the AnalogItemType_Clamping Variable.
        /// </summary>
        public const uint AnalogItemType_Clamping = 191;

        /// <summary>
        /// The identifier for the AnalogItemType_EuLo Variable.
        /// </summary>
        public const uint AnalogItemType_EuLo = 192;

        /// <summary>
        /// The identifier for the AnalogItemType_EuHi Variable.
        /// </summary>
        public const uint AnalogItemType_EuHi = 193;

        /// <summary>
        /// The identifier for the AnalogItemType_IrLo Variable.
        /// </summary>
        public const uint AnalogItemType_IrLo = 194;

        /// <summary>
        /// The identifier for the AnalogItemType_IrHi Variable.
        /// </summary>
        public const uint AnalogItemType_IrHi = 195;

        /// <summary>
        /// The identifier for the AnalogItemType_LimitLo Variable.
        /// </summary>
        public const uint AnalogItemType_LimitLo = 196;

        /// <summary>
        /// The identifier for the AnalogItemType_LimitHi Variable.
        /// </summary>
        public const uint AnalogItemType_LimitHi = 197;

        /// <summary>
        /// The identifier for the AnalogItemType_LimitLoLo Variable.
        /// </summary>
        public const uint AnalogItemType_LimitLoLo = 198;

        /// <summary>
        /// The identifier for the AnalogItemType_LimitHiHi Variable.
        /// </summary>
        public const uint AnalogItemType_LimitHiHi = 199;

        /// <summary>
        /// The identifier for the AnalogItemType_EngineeringUnits Variable.
        /// </summary>
        public const uint AnalogItemType_EngineeringUnits = 200;

        /// <summary>
        /// The identifier for the AnalogItemType_Aperture Variable.
        /// </summary>
        public const uint AnalogItemType_Aperture = 15003;

        /// <summary>
        /// The identifier for the AnalogItemType_Deadband Variable.
        /// </summary>
        public const uint AnalogItemType_Deadband = 15004;

        /// <summary>
        /// The identifier for the TsFormatType_OpenLabel Variable.
        /// </summary>
        public const uint TsFormatType_OpenLabel = 81;

        /// <summary>
        /// The identifier for the TsFormatType_CloseLabel Variable.
        /// </summary>
        public const uint TsFormatType_CloseLabel = 82;

        /// <summary>
        /// The identifier for the TsFormatType_OpenColor Variable.
        /// </summary>
        public const uint TsFormatType_OpenColor = 83;

        /// <summary>
        /// The identifier for the TsFormatType_CloseColor Variable.
        /// </summary>
        public const uint TsFormatType_CloseColor = 84;

        /// <summary>
        /// The identifier for the SimulationFunctionDataType_EnumStrings Variable.
        /// </summary>
        public const uint SimulationFunctionDataType_EnumStrings = 15023;

        /// <summary>
        /// The identifier for the SimulationSignalType_Function Variable.
        /// </summary>
        public const uint SimulationSignalType_Function = 15024;

        /// <summary>
        /// The identifier for the SimulationSignalType_Period Variable.
        /// </summary>
        public const uint SimulationSignalType_Period = 230;

        /// <summary>
        /// The identifier for the SimulationSignalType_Phase Variable.
        /// </summary>
        public const uint SimulationSignalType_Phase = 231;

        /// <summary>
        /// The identifier for the SimulationSignalType_UpdateInterval Variable.
        /// </summary>
        public const uint SimulationSignalType_UpdateInterval = 232;

        /// <summary>
        /// The identifier for the TelecontrolDataItems_BinarySchema Variable.
        /// </summary>
        public const uint TelecontrolDataItems_BinarySchema = 15001;

        /// <summary>
        /// The identifier for the TelecontrolDataItems_BinarySchema_NamespaceUri Variable.
        /// </summary>
        public const uint TelecontrolDataItems_BinarySchema_NamespaceUri = 15005;

        /// <summary>
        /// The identifier for the TelecontrolDataItems_BinarySchema_Deprecated Variable.
        /// </summary>
        public const uint TelecontrolDataItems_BinarySchema_Deprecated = 15006;

        /// <summary>
        /// The identifier for the TelecontrolDataItems_XmlSchema Variable.
        /// </summary>
        public const uint TelecontrolDataItems_XmlSchema = 15007;

        /// <summary>
        /// The identifier for the TelecontrolDataItems_XmlSchema_NamespaceUri Variable.
        /// </summary>
        public const uint TelecontrolDataItems_XmlSchema_NamespaceUri = 15009;

        /// <summary>
        /// The identifier for the TelecontrolDataItems_XmlSchema_Deprecated Variable.
        /// </summary>
        public const uint TelecontrolDataItems_XmlSchema_Deprecated = 15010;
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
        /// The identifier for the DataItemType VariableType.
        /// </summary>
        public const uint DataItemType = 18;

        /// <summary>
        /// The identifier for the DiscreteItemType VariableType.
        /// </summary>
        public const uint DiscreteItemType = 72;

        /// <summary>
        /// The identifier for the AnalogItemType VariableType.
        /// </summary>
        public const uint AnalogItemType = 76;

        /// <summary>
        /// The identifier for the SimulationSignalType VariableType.
        /// </summary>
        public const uint SimulationSignalType = 67;
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
        /// The identifier for the AnalogConversionDataType DataType.
        /// </summary>
        public static readonly ExpandedNodeId AnalogConversionDataType = new ExpandedNodeId(Telecontrol.DataItems.DataTypes.AnalogConversionDataType, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the SimulationFunctionDataType DataType.
        /// </summary>
        public static readonly ExpandedNodeId SimulationFunctionDataType = new ExpandedNodeId(Telecontrol.DataItems.DataTypes.SimulationFunctionDataType, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);
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
        /// The identifier for the DataItemType_WriteManual Method.
        /// </summary>
        public static readonly ExpandedNodeId DataItemType_WriteManual = new ExpandedNodeId(Telecontrol.DataItems.Methods.DataItemType_WriteManual, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);
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
        /// The identifier for the ChannelsPropertyCategory Object.
        /// </summary>
        public static readonly ExpandedNodeId ChannelsPropertyCategory = new ExpandedNodeId(Telecontrol.DataItems.Objects.ChannelsPropertyCategory, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the ConversionPropertyCategory Object.
        /// </summary>
        public static readonly ExpandedNodeId ConversionPropertyCategory = new ExpandedNodeId(Telecontrol.DataItems.Objects.ConversionPropertyCategory, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the FilteringPropertyCategory Object.
        /// </summary>
        public static readonly ExpandedNodeId FilteringPropertyCategory = new ExpandedNodeId(Telecontrol.DataItems.Objects.FilteringPropertyCategory, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the DisplayPropertyCategory Object.
        /// </summary>
        public static readonly ExpandedNodeId DisplayPropertyCategory = new ExpandedNodeId(Telecontrol.DataItems.Objects.DisplayPropertyCategory, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the HistoryPropertyCategory Object.
        /// </summary>
        public static readonly ExpandedNodeId HistoryPropertyCategory = new ExpandedNodeId(Telecontrol.DataItems.Objects.HistoryPropertyCategory, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the SimulationPropertyCategory Object.
        /// </summary>
        public static readonly ExpandedNodeId SimulationPropertyCategory = new ExpandedNodeId(Telecontrol.DataItems.Objects.SimulationPropertyCategory, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the LimitsPropertyCategory Object.
        /// </summary>
        public static readonly ExpandedNodeId LimitsPropertyCategory = new ExpandedNodeId(Telecontrol.DataItems.Objects.LimitsPropertyCategory, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the Statistics Object.
        /// </summary>
        public static readonly ExpandedNodeId Statistics = new ExpandedNodeId(Telecontrol.DataItems.Objects.Statistics, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the DataItems Object.
        /// </summary>
        public static readonly ExpandedNodeId DataItems = new ExpandedNodeId(Telecontrol.DataItems.Objects.DataItems, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the TsFormats Object.
        /// </summary>
        public static readonly ExpandedNodeId TsFormats = new ExpandedNodeId(Telecontrol.DataItems.Objects.TsFormats, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the SimulationSignals Object.
        /// </summary>
        public static readonly ExpandedNodeId SimulationSignals = new ExpandedNodeId(Telecontrol.DataItems.Objects.SimulationSignals, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the Aliases Object.
        /// </summary>
        public static readonly ExpandedNodeId Aliases = new ExpandedNodeId(Telecontrol.DataItems.Objects.Aliases, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);
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
        /// The identifier for the DataGroupType ObjectType.
        /// </summary>
        public static readonly ExpandedNodeId DataGroupType = new ExpandedNodeId(Telecontrol.DataItems.ObjectTypes.DataGroupType, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the TsFormatType ObjectType.
        /// </summary>
        public static readonly ExpandedNodeId TsFormatType = new ExpandedNodeId(Telecontrol.DataItems.ObjectTypes.TsFormatType, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the AliasType ObjectType.
        /// </summary>
        public static readonly ExpandedNodeId AliasType = new ExpandedNodeId(Telecontrol.DataItems.ObjectTypes.AliasType, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);
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
        /// The identifier for the HasSimulationSignal ReferenceType.
        /// </summary>
        public static readonly ExpandedNodeId HasSimulationSignal = new ExpandedNodeId(Telecontrol.DataItems.ReferenceTypes.HasSimulationSignal, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the HasTsFormat ReferenceType.
        /// </summary>
        public static readonly ExpandedNodeId HasTsFormat = new ExpandedNodeId(Telecontrol.DataItems.ReferenceTypes.HasTsFormat, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the AliasOf ReferenceType.
        /// </summary>
        public static readonly ExpandedNodeId AliasOf = new ExpandedNodeId(Telecontrol.DataItems.ReferenceTypes.AliasOf, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);
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
        /// The identifier for the Statistics_ServerCPUUsage Variable.
        /// </summary>
        public static readonly ExpandedNodeId Statistics_ServerCPUUsage = new ExpandedNodeId(Telecontrol.DataItems.Variables.Statistics_ServerCPUUsage, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the Statistics_ServerMemoryUsage Variable.
        /// </summary>
        public static readonly ExpandedNodeId Statistics_ServerMemoryUsage = new ExpandedNodeId(Telecontrol.DataItems.Variables.Statistics_ServerMemoryUsage, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the Statistics_TotalCPUUsage Variable.
        /// </summary>
        public static readonly ExpandedNodeId Statistics_TotalCPUUsage = new ExpandedNodeId(Telecontrol.DataItems.Variables.Statistics_TotalCPUUsage, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the Statistics_TotalMemoryUsage Variable.
        /// </summary>
        public static readonly ExpandedNodeId Statistics_TotalMemoryUsage = new ExpandedNodeId(Telecontrol.DataItems.Variables.Statistics_TotalMemoryUsage, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the DataGroupType_Simulated Variable.
        /// </summary>
        public static readonly ExpandedNodeId DataGroupType_Simulated = new ExpandedNodeId(Telecontrol.DataItems.Variables.DataGroupType_Simulated, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the DataItemType_Alias Variable.
        /// </summary>
        public static readonly ExpandedNodeId DataItemType_Alias = new ExpandedNodeId(Telecontrol.DataItems.Variables.DataItemType_Alias, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the DataItemType_Severity Variable.
        /// </summary>
        public static readonly ExpandedNodeId DataItemType_Severity = new ExpandedNodeId(Telecontrol.DataItems.Variables.DataItemType_Severity, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the DataItemType_Input1 Variable.
        /// </summary>
        public static readonly ExpandedNodeId DataItemType_Input1 = new ExpandedNodeId(Telecontrol.DataItems.Variables.DataItemType_Input1, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the DataItemType_Input2 Variable.
        /// </summary>
        public static readonly ExpandedNodeId DataItemType_Input2 = new ExpandedNodeId(Telecontrol.DataItems.Variables.DataItemType_Input2, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the DataItemType_Output Variable.
        /// </summary>
        public static readonly ExpandedNodeId DataItemType_Output = new ExpandedNodeId(Telecontrol.DataItems.Variables.DataItemType_Output, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the DataItemType_OutputCondition Variable.
        /// </summary>
        public static readonly ExpandedNodeId DataItemType_OutputCondition = new ExpandedNodeId(Telecontrol.DataItems.Variables.DataItemType_OutputCondition, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the DataItemType_OutputTwoStaged Variable.
        /// </summary>
        public static readonly ExpandedNodeId DataItemType_OutputTwoStaged = new ExpandedNodeId(Telecontrol.DataItems.Variables.DataItemType_OutputTwoStaged, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the DataItemType_StalePeriod Variable.
        /// </summary>
        public static readonly ExpandedNodeId DataItemType_StalePeriod = new ExpandedNodeId(Telecontrol.DataItems.Variables.DataItemType_StalePeriod, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the DataItemType_Simulated Variable.
        /// </summary>
        public static readonly ExpandedNodeId DataItemType_Simulated = new ExpandedNodeId(Telecontrol.DataItems.Variables.DataItemType_Simulated, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the DataItemType_Locked Variable.
        /// </summary>
        public static readonly ExpandedNodeId DataItemType_Locked = new ExpandedNodeId(Telecontrol.DataItems.Variables.DataItemType_Locked, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the DiscreteItemType_Inversion Variable.
        /// </summary>
        public static readonly ExpandedNodeId DiscreteItemType_Inversion = new ExpandedNodeId(Telecontrol.DataItems.Variables.DiscreteItemType_Inversion, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the AnalogConversionDataType_EnumStrings Variable.
        /// </summary>
        public static readonly ExpandedNodeId AnalogConversionDataType_EnumStrings = new ExpandedNodeId(Telecontrol.DataItems.Variables.AnalogConversionDataType_EnumStrings, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the AnalogItemType_DisplayFormat Variable.
        /// </summary>
        public static readonly ExpandedNodeId AnalogItemType_DisplayFormat = new ExpandedNodeId(Telecontrol.DataItems.Variables.AnalogItemType_DisplayFormat, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the AnalogItemType_Conversion Variable.
        /// </summary>
        public static readonly ExpandedNodeId AnalogItemType_Conversion = new ExpandedNodeId(Telecontrol.DataItems.Variables.AnalogItemType_Conversion, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the AnalogItemType_Clamping Variable.
        /// </summary>
        public static readonly ExpandedNodeId AnalogItemType_Clamping = new ExpandedNodeId(Telecontrol.DataItems.Variables.AnalogItemType_Clamping, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the AnalogItemType_EuLo Variable.
        /// </summary>
        public static readonly ExpandedNodeId AnalogItemType_EuLo = new ExpandedNodeId(Telecontrol.DataItems.Variables.AnalogItemType_EuLo, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the AnalogItemType_EuHi Variable.
        /// </summary>
        public static readonly ExpandedNodeId AnalogItemType_EuHi = new ExpandedNodeId(Telecontrol.DataItems.Variables.AnalogItemType_EuHi, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the AnalogItemType_IrLo Variable.
        /// </summary>
        public static readonly ExpandedNodeId AnalogItemType_IrLo = new ExpandedNodeId(Telecontrol.DataItems.Variables.AnalogItemType_IrLo, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the AnalogItemType_IrHi Variable.
        /// </summary>
        public static readonly ExpandedNodeId AnalogItemType_IrHi = new ExpandedNodeId(Telecontrol.DataItems.Variables.AnalogItemType_IrHi, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the AnalogItemType_LimitLo Variable.
        /// </summary>
        public static readonly ExpandedNodeId AnalogItemType_LimitLo = new ExpandedNodeId(Telecontrol.DataItems.Variables.AnalogItemType_LimitLo, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the AnalogItemType_LimitHi Variable.
        /// </summary>
        public static readonly ExpandedNodeId AnalogItemType_LimitHi = new ExpandedNodeId(Telecontrol.DataItems.Variables.AnalogItemType_LimitHi, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the AnalogItemType_LimitLoLo Variable.
        /// </summary>
        public static readonly ExpandedNodeId AnalogItemType_LimitLoLo = new ExpandedNodeId(Telecontrol.DataItems.Variables.AnalogItemType_LimitLoLo, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the AnalogItemType_LimitHiHi Variable.
        /// </summary>
        public static readonly ExpandedNodeId AnalogItemType_LimitHiHi = new ExpandedNodeId(Telecontrol.DataItems.Variables.AnalogItemType_LimitHiHi, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the AnalogItemType_EngineeringUnits Variable.
        /// </summary>
        public static readonly ExpandedNodeId AnalogItemType_EngineeringUnits = new ExpandedNodeId(Telecontrol.DataItems.Variables.AnalogItemType_EngineeringUnits, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the AnalogItemType_Aperture Variable.
        /// </summary>
        public static readonly ExpandedNodeId AnalogItemType_Aperture = new ExpandedNodeId(Telecontrol.DataItems.Variables.AnalogItemType_Aperture, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the AnalogItemType_Deadband Variable.
        /// </summary>
        public static readonly ExpandedNodeId AnalogItemType_Deadband = new ExpandedNodeId(Telecontrol.DataItems.Variables.AnalogItemType_Deadband, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the TsFormatType_OpenLabel Variable.
        /// </summary>
        public static readonly ExpandedNodeId TsFormatType_OpenLabel = new ExpandedNodeId(Telecontrol.DataItems.Variables.TsFormatType_OpenLabel, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the TsFormatType_CloseLabel Variable.
        /// </summary>
        public static readonly ExpandedNodeId TsFormatType_CloseLabel = new ExpandedNodeId(Telecontrol.DataItems.Variables.TsFormatType_CloseLabel, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the TsFormatType_OpenColor Variable.
        /// </summary>
        public static readonly ExpandedNodeId TsFormatType_OpenColor = new ExpandedNodeId(Telecontrol.DataItems.Variables.TsFormatType_OpenColor, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the TsFormatType_CloseColor Variable.
        /// </summary>
        public static readonly ExpandedNodeId TsFormatType_CloseColor = new ExpandedNodeId(Telecontrol.DataItems.Variables.TsFormatType_CloseColor, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the SimulationFunctionDataType_EnumStrings Variable.
        /// </summary>
        public static readonly ExpandedNodeId SimulationFunctionDataType_EnumStrings = new ExpandedNodeId(Telecontrol.DataItems.Variables.SimulationFunctionDataType_EnumStrings, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the SimulationSignalType_Function Variable.
        /// </summary>
        public static readonly ExpandedNodeId SimulationSignalType_Function = new ExpandedNodeId(Telecontrol.DataItems.Variables.SimulationSignalType_Function, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the SimulationSignalType_Period Variable.
        /// </summary>
        public static readonly ExpandedNodeId SimulationSignalType_Period = new ExpandedNodeId(Telecontrol.DataItems.Variables.SimulationSignalType_Period, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the SimulationSignalType_Phase Variable.
        /// </summary>
        public static readonly ExpandedNodeId SimulationSignalType_Phase = new ExpandedNodeId(Telecontrol.DataItems.Variables.SimulationSignalType_Phase, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the SimulationSignalType_UpdateInterval Variable.
        /// </summary>
        public static readonly ExpandedNodeId SimulationSignalType_UpdateInterval = new ExpandedNodeId(Telecontrol.DataItems.Variables.SimulationSignalType_UpdateInterval, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the TelecontrolDataItems_BinarySchema Variable.
        /// </summary>
        public static readonly ExpandedNodeId TelecontrolDataItems_BinarySchema = new ExpandedNodeId(Telecontrol.DataItems.Variables.TelecontrolDataItems_BinarySchema, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the TelecontrolDataItems_BinarySchema_NamespaceUri Variable.
        /// </summary>
        public static readonly ExpandedNodeId TelecontrolDataItems_BinarySchema_NamespaceUri = new ExpandedNodeId(Telecontrol.DataItems.Variables.TelecontrolDataItems_BinarySchema_NamespaceUri, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the TelecontrolDataItems_BinarySchema_Deprecated Variable.
        /// </summary>
        public static readonly ExpandedNodeId TelecontrolDataItems_BinarySchema_Deprecated = new ExpandedNodeId(Telecontrol.DataItems.Variables.TelecontrolDataItems_BinarySchema_Deprecated, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the TelecontrolDataItems_XmlSchema Variable.
        /// </summary>
        public static readonly ExpandedNodeId TelecontrolDataItems_XmlSchema = new ExpandedNodeId(Telecontrol.DataItems.Variables.TelecontrolDataItems_XmlSchema, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the TelecontrolDataItems_XmlSchema_NamespaceUri Variable.
        /// </summary>
        public static readonly ExpandedNodeId TelecontrolDataItems_XmlSchema_NamespaceUri = new ExpandedNodeId(Telecontrol.DataItems.Variables.TelecontrolDataItems_XmlSchema_NamespaceUri, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the TelecontrolDataItems_XmlSchema_Deprecated Variable.
        /// </summary>
        public static readonly ExpandedNodeId TelecontrolDataItems_XmlSchema_Deprecated = new ExpandedNodeId(Telecontrol.DataItems.Variables.TelecontrolDataItems_XmlSchema_Deprecated, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);
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
        /// The identifier for the DataItemType VariableType.
        /// </summary>
        public static readonly ExpandedNodeId DataItemType = new ExpandedNodeId(Telecontrol.DataItems.VariableTypes.DataItemType, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the DiscreteItemType VariableType.
        /// </summary>
        public static readonly ExpandedNodeId DiscreteItemType = new ExpandedNodeId(Telecontrol.DataItems.VariableTypes.DiscreteItemType, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the AnalogItemType VariableType.
        /// </summary>
        public static readonly ExpandedNodeId AnalogItemType = new ExpandedNodeId(Telecontrol.DataItems.VariableTypes.AnalogItemType, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);

        /// <summary>
        /// The identifier for the SimulationSignalType VariableType.
        /// </summary>
        public static readonly ExpandedNodeId SimulationSignalType = new ExpandedNodeId(Telecontrol.DataItems.VariableTypes.SimulationSignalType, Telecontrol.DataItems.Namespaces.TelecontrolDataItems);
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
        /// The BrowseName for the Alias component.
        /// </summary>
        public const string Alias = "Alias";

        /// <summary>
        /// The BrowseName for the Aliases component.
        /// </summary>
        public const string Aliases = "Aliases";

        /// <summary>
        /// The BrowseName for the AliasOf component.
        /// </summary>
        public const string AliasOf = "AliasOf";

        /// <summary>
        /// The BrowseName for the AliasType component.
        /// </summary>
        public const string AliasType = "AliasType";

        /// <summary>
        /// The BrowseName for the AnalogConversionDataType component.
        /// </summary>
        public const string AnalogConversionDataType = "AnalogConversionDataType";

        /// <summary>
        /// The BrowseName for the AnalogItemType component.
        /// </summary>
        public const string AnalogItemType = "AnalogItemType";

        /// <summary>
        /// The BrowseName for the Aperture component.
        /// </summary>
        public const string Aperture = "Aperture";

        /// <summary>
        /// The BrowseName for the ChannelsPropertyCategory component.
        /// </summary>
        public const string ChannelsPropertyCategory = "ChannelsPropertyCategory";

        /// <summary>
        /// The BrowseName for the Clamping component.
        /// </summary>
        public const string Clamping = "Clamping";

        /// <summary>
        /// The BrowseName for the CloseColor component.
        /// </summary>
        public const string CloseColor = "CloseColor";

        /// <summary>
        /// The BrowseName for the CloseLabel component.
        /// </summary>
        public const string CloseLabel = "CloseLabel";

        /// <summary>
        /// The BrowseName for the Conversion component.
        /// </summary>
        public const string Conversion = "Conversion";

        /// <summary>
        /// The BrowseName for the ConversionPropertyCategory component.
        /// </summary>
        public const string ConversionPropertyCategory = "ConversionPropertyCategory";

        /// <summary>
        /// The BrowseName for the DataGroupType component.
        /// </summary>
        public const string DataGroupType = "DataGroupType";

        /// <summary>
        /// The BrowseName for the DataItems component.
        /// </summary>
        public const string DataItems = "DataItems";

        /// <summary>
        /// The BrowseName for the DataItemType component.
        /// </summary>
        public const string DataItemType = "DataItemType";

        /// <summary>
        /// The BrowseName for the Deadband component.
        /// </summary>
        public const string Deadband = "Deadband";

        /// <summary>
        /// The BrowseName for the DiscreteItemType component.
        /// </summary>
        public const string DiscreteItemType = "DiscreteItemType";

        /// <summary>
        /// The BrowseName for the DisplayFormat component.
        /// </summary>
        public const string DisplayFormat = "DisplayFormat";

        /// <summary>
        /// The BrowseName for the DisplayPropertyCategory component.
        /// </summary>
        public const string DisplayPropertyCategory = "DisplayPropertyCategory";

        /// <summary>
        /// The BrowseName for the EngineeringUnits component.
        /// </summary>
        public const string EngineeringUnits = "EngineeringUnits";

        /// <summary>
        /// The BrowseName for the EuHi component.
        /// </summary>
        public const string EuHi = "EuHi";

        /// <summary>
        /// The BrowseName for the EuLo component.
        /// </summary>
        public const string EuLo = "EuLo";

        /// <summary>
        /// The BrowseName for the FilteringPropertyCategory component.
        /// </summary>
        public const string FilteringPropertyCategory = "FilteringPropertyCategory";

        /// <summary>
        /// The BrowseName for the Function component.
        /// </summary>
        public const string Function = "Function";

        /// <summary>
        /// The BrowseName for the HasSimulationSignal component.
        /// </summary>
        public const string HasSimulationSignal = "HasSimulationSignal";

        /// <summary>
        /// The BrowseName for the HasTsFormat component.
        /// </summary>
        public const string HasTsFormat = "HasTsFormat";

        /// <summary>
        /// The BrowseName for the HistoryPropertyCategory component.
        /// </summary>
        public const string HistoryPropertyCategory = "HistoryPropertyCategory";

        /// <summary>
        /// The BrowseName for the Input1 component.
        /// </summary>
        public const string Input1 = "Input1";

        /// <summary>
        /// The BrowseName for the Input2 component.
        /// </summary>
        public const string Input2 = "Input2";

        /// <summary>
        /// The BrowseName for the Inversion component.
        /// </summary>
        public const string Inversion = "Inversion";

        /// <summary>
        /// The BrowseName for the IrHi component.
        /// </summary>
        public const string IrHi = "IrHi";

        /// <summary>
        /// The BrowseName for the IrLo component.
        /// </summary>
        public const string IrLo = "IrLo";

        /// <summary>
        /// The BrowseName for the LimitHi component.
        /// </summary>
        public const string LimitHi = "LimitHi";

        /// <summary>
        /// The BrowseName for the LimitHiHi component.
        /// </summary>
        public const string LimitHiHi = "LimitHiHi";

        /// <summary>
        /// The BrowseName for the LimitLo component.
        /// </summary>
        public const string LimitLo = "LimitLo";

        /// <summary>
        /// The BrowseName for the LimitLoLo component.
        /// </summary>
        public const string LimitLoLo = "LimitLoLo";

        /// <summary>
        /// The BrowseName for the LimitsPropertyCategory component.
        /// </summary>
        public const string LimitsPropertyCategory = "LimitsPropertyCategory";

        /// <summary>
        /// The BrowseName for the Locked component.
        /// </summary>
        public const string Locked = "Locked";

        /// <summary>
        /// The BrowseName for the OpenColor component.
        /// </summary>
        public const string OpenColor = "OpenColor";

        /// <summary>
        /// The BrowseName for the OpenLabel component.
        /// </summary>
        public const string OpenLabel = "OpenLabel";

        /// <summary>
        /// The BrowseName for the Output component.
        /// </summary>
        public const string Output = "Output";

        /// <summary>
        /// The BrowseName for the OutputCondition component.
        /// </summary>
        public const string OutputCondition = "OutputCondition";

        /// <summary>
        /// The BrowseName for the OutputTwoStaged component.
        /// </summary>
        public const string OutputTwoStaged = "OutputTwoStaged";

        /// <summary>
        /// The BrowseName for the Period component.
        /// </summary>
        public const string Period = "Period";

        /// <summary>
        /// The BrowseName for the Phase component.
        /// </summary>
        public const string Phase = "Phase";

        /// <summary>
        /// The BrowseName for the ServerCPUUsage component.
        /// </summary>
        public const string ServerCPUUsage = "ServerCPUUsage";

        /// <summary>
        /// The BrowseName for the ServerMemoryUsage component.
        /// </summary>
        public const string ServerMemoryUsage = "ServerMemoryUsage";

        /// <summary>
        /// The BrowseName for the Severity component.
        /// </summary>
        public const string Severity = "Severity";

        /// <summary>
        /// The BrowseName for the Simulated component.
        /// </summary>
        public const string Simulated = "Simulated";

        /// <summary>
        /// The BrowseName for the SimulationFunctionDataType component.
        /// </summary>
        public const string SimulationFunctionDataType = "SimulationFunctionDataType";

        /// <summary>
        /// The BrowseName for the SimulationPropertyCategory component.
        /// </summary>
        public const string SimulationPropertyCategory = "SimulationPropertyCategory";

        /// <summary>
        /// The BrowseName for the SimulationSignals component.
        /// </summary>
        public const string SimulationSignals = "SimulationSignals";

        /// <summary>
        /// The BrowseName for the SimulationSignalType component.
        /// </summary>
        public const string SimulationSignalType = "SimulationSignalType";

        /// <summary>
        /// The BrowseName for the StalePeriod component.
        /// </summary>
        public const string StalePeriod = "StalePeriod";

        /// <summary>
        /// The BrowseName for the Statistics component.
        /// </summary>
        public const string Statistics = "Statistics";

        /// <summary>
        /// The BrowseName for the TelecontrolDataItems_BinarySchema component.
        /// </summary>
        public const string TelecontrolDataItems_BinarySchema = "Telecontrol.DataItems";

        /// <summary>
        /// The BrowseName for the TelecontrolDataItems_XmlSchema component.
        /// </summary>
        public const string TelecontrolDataItems_XmlSchema = "Telecontrol.DataItems";

        /// <summary>
        /// The BrowseName for the TotalCPUUsage component.
        /// </summary>
        public const string TotalCPUUsage = "TotalCPUUsage";

        /// <summary>
        /// The BrowseName for the TotalMemoryUsage component.
        /// </summary>
        public const string TotalMemoryUsage = "TotalMemoryUsage";

        /// <summary>
        /// The BrowseName for the TsFormats component.
        /// </summary>
        public const string TsFormats = "TsFormats";

        /// <summary>
        /// The BrowseName for the TsFormatType component.
        /// </summary>
        public const string TsFormatType = "TsFormatType";

        /// <summary>
        /// The BrowseName for the UpdateInterval component.
        /// </summary>
        public const string UpdateInterval = "UpdateInterval";

        /// <summary>
        /// The BrowseName for the WriteManual component.
        /// </summary>
        public const string WriteManual = "WriteManual";
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
        /// The URI for the TelecontrolHistory namespace (.NET code namespace is 'Telecontrol.History').
        /// </summary>
        public const string TelecontrolHistory = "http://telecontrol.ru/opcua/history";

        /// <summary>
        /// The URI for the TelecontrolDataItems namespace (.NET code namespace is 'Telecontrol.DataItems').
        /// </summary>
        public const string TelecontrolDataItems = "http://telecontrol.ru/opcua/data_items";
    }
    #endregion
}