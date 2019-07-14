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

namespace Telecontrol.Security
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
        /// The identifier for the Users Object.
        /// </summary>
        public const uint Users = 29;

        /// <summary>
        /// The identifier for the RootUser Object.
        /// </summary>
        public const uint RootUser = 38;
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
        /// The identifier for the UserType ObjectType.
        /// </summary>
        public const uint UserType = 16;
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
        /// The identifier for the UserType_AccessRights Variable.
        /// </summary>
        public const uint UserType_AccessRights = 17;

        /// <summary>
        /// The identifier for the RootUser_AccessRights Variable.
        /// </summary>
        public const uint RootUser_AccessRights = 39;
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
        /// The identifier for the Users Object.
        /// </summary>
        public static readonly ExpandedNodeId Users = new ExpandedNodeId(Telecontrol.Security.Objects.Users, Telecontrol.Security.Namespaces.TelecontrolSecurity);

        /// <summary>
        /// The identifier for the RootUser Object.
        /// </summary>
        public static readonly ExpandedNodeId RootUser = new ExpandedNodeId(Telecontrol.Security.Objects.RootUser, Telecontrol.Security.Namespaces.TelecontrolSecurity);
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
        /// The identifier for the UserType ObjectType.
        /// </summary>
        public static readonly ExpandedNodeId UserType = new ExpandedNodeId(Telecontrol.Security.ObjectTypes.UserType, Telecontrol.Security.Namespaces.TelecontrolSecurity);
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
        /// The identifier for the UserType_AccessRights Variable.
        /// </summary>
        public static readonly ExpandedNodeId UserType_AccessRights = new ExpandedNodeId(Telecontrol.Security.Variables.UserType_AccessRights, Telecontrol.Security.Namespaces.TelecontrolSecurity);

        /// <summary>
        /// The identifier for the RootUser_AccessRights Variable.
        /// </summary>
        public static readonly ExpandedNodeId RootUser_AccessRights = new ExpandedNodeId(Telecontrol.Security.Variables.RootUser_AccessRights, Telecontrol.Security.Namespaces.TelecontrolSecurity);
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
        /// The BrowseName for the AccessRights component.
        /// </summary>
        public const string AccessRights = "AccessRights";

        /// <summary>
        /// The BrowseName for the RootUser component.
        /// </summary>
        public const string RootUser = "root";

        /// <summary>
        /// The BrowseName for the Users component.
        /// </summary>
        public const string Users = "Users";

        /// <summary>
        /// The BrowseName for the UserType component.
        /// </summary>
        public const string UserType = "UserType";
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
        /// The URI for the TelecontrolSecurity namespace (.NET code namespace is 'Telecontrol.Security').
        /// </summary>
        public const string TelecontrolSecurity = "http://telecontrol.ru/opcua/security";
    }
    #endregion
}