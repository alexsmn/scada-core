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

namespace Telecontrol.FileSystem
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
        /// The identifier for the FileSystem Object.
        /// </summary>
        public const uint FileSystem = 15016;
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
        /// The identifier for the FileDirectoryType ObjectType.
        /// </summary>
        public const uint FileDirectoryType = 15017;
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
        /// The identifier for the FileType_RelativePath Variable.
        /// </summary>
        public const uint FileType_RelativePath = 15001;

        /// <summary>
        /// The identifier for the FileType_LastUpdateTime Variable.
        /// </summary>
        public const uint FileType_LastUpdateTime = 15019;

        /// <summary>
        /// The identifier for the FileType_Size Variable.
        /// </summary>
        public const uint FileType_Size = 15020;
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
        /// The identifier for the FileType VariableType.
        /// </summary>
        public const uint FileType = 15018;
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
        /// The identifier for the FileSystem Object.
        /// </summary>
        public static readonly ExpandedNodeId FileSystem = new ExpandedNodeId(Telecontrol.FileSystem.Objects.FileSystem, Telecontrol.FileSystem.Namespaces.TelecontrolFileSystem);
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
        /// The identifier for the FileDirectoryType ObjectType.
        /// </summary>
        public static readonly ExpandedNodeId FileDirectoryType = new ExpandedNodeId(Telecontrol.FileSystem.ObjectTypes.FileDirectoryType, Telecontrol.FileSystem.Namespaces.TelecontrolFileSystem);
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
        /// The identifier for the FileType_RelativePath Variable.
        /// </summary>
        public static readonly ExpandedNodeId FileType_RelativePath = new ExpandedNodeId(Telecontrol.FileSystem.Variables.FileType_RelativePath, Telecontrol.FileSystem.Namespaces.TelecontrolFileSystem);

        /// <summary>
        /// The identifier for the FileType_LastUpdateTime Variable.
        /// </summary>
        public static readonly ExpandedNodeId FileType_LastUpdateTime = new ExpandedNodeId(Telecontrol.FileSystem.Variables.FileType_LastUpdateTime, Telecontrol.FileSystem.Namespaces.TelecontrolFileSystem);

        /// <summary>
        /// The identifier for the FileType_Size Variable.
        /// </summary>
        public static readonly ExpandedNodeId FileType_Size = new ExpandedNodeId(Telecontrol.FileSystem.Variables.FileType_Size, Telecontrol.FileSystem.Namespaces.TelecontrolFileSystem);
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
        /// The identifier for the FileType VariableType.
        /// </summary>
        public static readonly ExpandedNodeId FileType = new ExpandedNodeId(Telecontrol.FileSystem.VariableTypes.FileType, Telecontrol.FileSystem.Namespaces.TelecontrolFileSystem);
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
        /// The BrowseName for the FileDirectoryType component.
        /// </summary>
        public const string FileDirectoryType = "FileDirectoryType";

        /// <summary>
        /// The BrowseName for the FileSystem component.
        /// </summary>
        public const string FileSystem = "FileSystem";

        /// <summary>
        /// The BrowseName for the FileType component.
        /// </summary>
        public const string FileType = "FileType";

        /// <summary>
        /// The BrowseName for the LastUpdateTime component.
        /// </summary>
        public const string LastUpdateTime = "LastUpdateTime";

        /// <summary>
        /// The BrowseName for the RelativePath component.
        /// </summary>
        public const string RelativePath = "RelativePath";

        /// <summary>
        /// The BrowseName for the Size component.
        /// </summary>
        public const string Size = "Size";
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
        /// The URI for the TelecontrolFileSystem namespace (.NET code namespace is 'Telecontrol.FileSystem').
        /// </summary>
        public const string TelecontrolFileSystem = "http://telecontrol.ru/opcua/filesystem";
    }
    #endregion
}