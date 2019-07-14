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

namespace Telecontrol.History
{
    #region Object Identifiers
    /// <summary>
    /// A class that declares constants for all Objects in the Model Design.
    /// </summary>
    /// <exclude />
    [System.CodeDom.Compiler.GeneratedCodeAttribute("Opc.Ua.ModelCompiler", "1.0.0.0")]
    public static partial class Objects
    {
        /// <summary>
        /// The identifier for the HistoricalDatabases Object.
        /// </summary>
        public const uint HistoricalDatabases = 12;

        /// <summary>
        /// The identifier for the SystemDatabase Object.
        /// </summary>
        public const uint SystemDatabase = 13;
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
        /// The identifier for the HistoricalDatabaseType ObjectType.
        /// </summary>
        public const uint HistoricalDatabaseType = 20;
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
        /// The identifier for the HasHistoricalDatabase ReferenceType.
        /// </summary>
        public const uint HasHistoricalDatabase = 15001;
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
        /// The identifier for the HistoricalDatabaseType_Depth Variable.
        /// </summary>
        public const uint HistoricalDatabaseType_Depth = 36;

        /// <summary>
        /// The identifier for the HistoricalDatabaseType_WrittenValueCount Variable.
        /// </summary>
        public const uint HistoricalDatabaseType_WrittenValueCount = 15021;

        /// <summary>
        /// The identifier for the HistoricalDatabaseType_WriteValueDuration Variable.
        /// </summary>
        public const uint HistoricalDatabaseType_WriteValueDuration = 15002;

        /// <summary>
        /// The identifier for the HistoricalDatabaseType_WrittenEventCount Variable.
        /// </summary>
        public const uint HistoricalDatabaseType_WrittenEventCount = 15022;

        /// <summary>
        /// The identifier for the HistoricalDatabaseType_PendingTaskCount Variable.
        /// </summary>
        public const uint HistoricalDatabaseType_PendingTaskCount = 50;

        /// <summary>
        /// The identifier for the HistoricalDatabaseType_EventCleanupDuration Variable.
        /// </summary>
        public const uint HistoricalDatabaseType_EventCleanupDuration = 51;

        /// <summary>
        /// The identifier for the HistoricalDatabaseType_ValueCleanupDuration Variable.
        /// </summary>
        public const uint HistoricalDatabaseType_ValueCleanupDuration = 52;

        /// <summary>
        /// The identifier for the SystemDatabase_Depth Variable.
        /// </summary>
        public const uint SystemDatabase_Depth = 37;

        /// <summary>
        /// The identifier for the SystemDatabase_WrittenValueCount Variable.
        /// </summary>
        public const uint SystemDatabase_WrittenValueCount = 15023;

        /// <summary>
        /// The identifier for the SystemDatabase_WriteValueDuration Variable.
        /// </summary>
        public const uint SystemDatabase_WriteValueDuration = 15003;

        /// <summary>
        /// The identifier for the SystemDatabase_WrittenEventCount Variable.
        /// </summary>
        public const uint SystemDatabase_WrittenEventCount = 15024;

        /// <summary>
        /// The identifier for the SystemDatabase_PendingTaskCount Variable.
        /// </summary>
        public const uint SystemDatabase_PendingTaskCount = 54;

        /// <summary>
        /// The identifier for the SystemDatabase_EventCleanupDuration Variable.
        /// </summary>
        public const uint SystemDatabase_EventCleanupDuration = 55;

        /// <summary>
        /// The identifier for the SystemDatabase_ValueCleanupDuration Variable.
        /// </summary>
        public const uint SystemDatabase_ValueCleanupDuration = 56;
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
        /// The identifier for the HistoricalDatabases Object.
        /// </summary>
        public static readonly ExpandedNodeId HistoricalDatabases = new ExpandedNodeId(Telecontrol.History.Objects.HistoricalDatabases, Telecontrol.History.Namespaces.TelecontrolHistory);

        /// <summary>
        /// The identifier for the SystemDatabase Object.
        /// </summary>
        public static readonly ExpandedNodeId SystemDatabase = new ExpandedNodeId(Telecontrol.History.Objects.SystemDatabase, Telecontrol.History.Namespaces.TelecontrolHistory);
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
        /// The identifier for the HistoricalDatabaseType ObjectType.
        /// </summary>
        public static readonly ExpandedNodeId HistoricalDatabaseType = new ExpandedNodeId(Telecontrol.History.ObjectTypes.HistoricalDatabaseType, Telecontrol.History.Namespaces.TelecontrolHistory);
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
        /// The identifier for the HasHistoricalDatabase ReferenceType.
        /// </summary>
        public static readonly ExpandedNodeId HasHistoricalDatabase = new ExpandedNodeId(Telecontrol.History.ReferenceTypes.HasHistoricalDatabase, Telecontrol.History.Namespaces.TelecontrolHistory);
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
        /// The identifier for the HistoricalDatabaseType_Depth Variable.
        /// </summary>
        public static readonly ExpandedNodeId HistoricalDatabaseType_Depth = new ExpandedNodeId(Telecontrol.History.Variables.HistoricalDatabaseType_Depth, Telecontrol.History.Namespaces.TelecontrolHistory);

        /// <summary>
        /// The identifier for the HistoricalDatabaseType_WrittenValueCount Variable.
        /// </summary>
        public static readonly ExpandedNodeId HistoricalDatabaseType_WrittenValueCount = new ExpandedNodeId(Telecontrol.History.Variables.HistoricalDatabaseType_WrittenValueCount, Telecontrol.History.Namespaces.TelecontrolHistory);

        /// <summary>
        /// The identifier for the HistoricalDatabaseType_WriteValueDuration Variable.
        /// </summary>
        public static readonly ExpandedNodeId HistoricalDatabaseType_WriteValueDuration = new ExpandedNodeId(Telecontrol.History.Variables.HistoricalDatabaseType_WriteValueDuration, Telecontrol.History.Namespaces.TelecontrolHistory);

        /// <summary>
        /// The identifier for the HistoricalDatabaseType_WrittenEventCount Variable.
        /// </summary>
        public static readonly ExpandedNodeId HistoricalDatabaseType_WrittenEventCount = new ExpandedNodeId(Telecontrol.History.Variables.HistoricalDatabaseType_WrittenEventCount, Telecontrol.History.Namespaces.TelecontrolHistory);

        /// <summary>
        /// The identifier for the HistoricalDatabaseType_PendingTaskCount Variable.
        /// </summary>
        public static readonly ExpandedNodeId HistoricalDatabaseType_PendingTaskCount = new ExpandedNodeId(Telecontrol.History.Variables.HistoricalDatabaseType_PendingTaskCount, Telecontrol.History.Namespaces.TelecontrolHistory);

        /// <summary>
        /// The identifier for the HistoricalDatabaseType_EventCleanupDuration Variable.
        /// </summary>
        public static readonly ExpandedNodeId HistoricalDatabaseType_EventCleanupDuration = new ExpandedNodeId(Telecontrol.History.Variables.HistoricalDatabaseType_EventCleanupDuration, Telecontrol.History.Namespaces.TelecontrolHistory);

        /// <summary>
        /// The identifier for the HistoricalDatabaseType_ValueCleanupDuration Variable.
        /// </summary>
        public static readonly ExpandedNodeId HistoricalDatabaseType_ValueCleanupDuration = new ExpandedNodeId(Telecontrol.History.Variables.HistoricalDatabaseType_ValueCleanupDuration, Telecontrol.History.Namespaces.TelecontrolHistory);

        /// <summary>
        /// The identifier for the SystemDatabase_Depth Variable.
        /// </summary>
        public static readonly ExpandedNodeId SystemDatabase_Depth = new ExpandedNodeId(Telecontrol.History.Variables.SystemDatabase_Depth, Telecontrol.History.Namespaces.TelecontrolHistory);

        /// <summary>
        /// The identifier for the SystemDatabase_WrittenValueCount Variable.
        /// </summary>
        public static readonly ExpandedNodeId SystemDatabase_WrittenValueCount = new ExpandedNodeId(Telecontrol.History.Variables.SystemDatabase_WrittenValueCount, Telecontrol.History.Namespaces.TelecontrolHistory);

        /// <summary>
        /// The identifier for the SystemDatabase_WriteValueDuration Variable.
        /// </summary>
        public static readonly ExpandedNodeId SystemDatabase_WriteValueDuration = new ExpandedNodeId(Telecontrol.History.Variables.SystemDatabase_WriteValueDuration, Telecontrol.History.Namespaces.TelecontrolHistory);

        /// <summary>
        /// The identifier for the SystemDatabase_WrittenEventCount Variable.
        /// </summary>
        public static readonly ExpandedNodeId SystemDatabase_WrittenEventCount = new ExpandedNodeId(Telecontrol.History.Variables.SystemDatabase_WrittenEventCount, Telecontrol.History.Namespaces.TelecontrolHistory);

        /// <summary>
        /// The identifier for the SystemDatabase_PendingTaskCount Variable.
        /// </summary>
        public static readonly ExpandedNodeId SystemDatabase_PendingTaskCount = new ExpandedNodeId(Telecontrol.History.Variables.SystemDatabase_PendingTaskCount, Telecontrol.History.Namespaces.TelecontrolHistory);

        /// <summary>
        /// The identifier for the SystemDatabase_EventCleanupDuration Variable.
        /// </summary>
        public static readonly ExpandedNodeId SystemDatabase_EventCleanupDuration = new ExpandedNodeId(Telecontrol.History.Variables.SystemDatabase_EventCleanupDuration, Telecontrol.History.Namespaces.TelecontrolHistory);

        /// <summary>
        /// The identifier for the SystemDatabase_ValueCleanupDuration Variable.
        /// </summary>
        public static readonly ExpandedNodeId SystemDatabase_ValueCleanupDuration = new ExpandedNodeId(Telecontrol.History.Variables.SystemDatabase_ValueCleanupDuration, Telecontrol.History.Namespaces.TelecontrolHistory);
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
        /// The BrowseName for the Depth component.
        /// </summary>
        public const string Depth = "Depth";

        /// <summary>
        /// The BrowseName for the EventCleanupDuration component.
        /// </summary>
        public const string EventCleanupDuration = "EventCleanupDuration";

        /// <summary>
        /// The BrowseName for the HasHistoricalDatabase component.
        /// </summary>
        public const string HasHistoricalDatabase = "HasHistoricalDatabase";

        /// <summary>
        /// The BrowseName for the HistoricalDatabases component.
        /// </summary>
        public const string HistoricalDatabases = "HistoricalDatabases";

        /// <summary>
        /// The BrowseName for the HistoricalDatabaseType component.
        /// </summary>
        public const string HistoricalDatabaseType = "HistoricalDatabaseType";

        /// <summary>
        /// The BrowseName for the PendingTaskCount component.
        /// </summary>
        public const string PendingTaskCount = "PendingTaskCount";

        /// <summary>
        /// The BrowseName for the SystemDatabase component.
        /// </summary>
        public const string SystemDatabase = "System";

        /// <summary>
        /// The BrowseName for the ValueCleanupDuration component.
        /// </summary>
        public const string ValueCleanupDuration = "ValueCleanupDuration";

        /// <summary>
        /// The BrowseName for the WriteValueDuration component.
        /// </summary>
        public const string WriteValueDuration = "WriteValueDuration";

        /// <summary>
        /// The BrowseName for the WrittenEventCount component.
        /// </summary>
        public const string WrittenEventCount = "WrittenEventCount";

        /// <summary>
        /// The BrowseName for the WrittenValueCount component.
        /// </summary>
        public const string WrittenValueCount = "WrittenValueCount";
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
    }
    #endregion
}