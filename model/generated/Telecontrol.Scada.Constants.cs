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

namespace Telecontrol.Scada
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
        /// The identifier for the PropertyCategories Object.
        /// </summary>
        public const uint PropertyCategories = 15029;

        /// <summary>
        /// The identifier for the GeneralPropertyCategory Object.
        /// </summary>
        public const uint GeneralPropertyCategory = 15001;
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
        /// The identifier for the Creates ReferenceType.
        /// </summary>
        public const uint Creates = 23;

        /// <summary>
        /// The identifier for the HasPropertyCategory ReferenceType.
        /// </summary>
        public const uint HasPropertyCategory = 15028;
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
        /// The identifier for the PropertyCategories Object.
        /// </summary>
        public static readonly ExpandedNodeId PropertyCategories = new ExpandedNodeId(Telecontrol.Scada.Objects.PropertyCategories, Telecontrol.Scada.Namespaces.TelecontrolScada);

        /// <summary>
        /// The identifier for the GeneralPropertyCategory Object.
        /// </summary>
        public static readonly ExpandedNodeId GeneralPropertyCategory = new ExpandedNodeId(Telecontrol.Scada.Objects.GeneralPropertyCategory, Telecontrol.Scada.Namespaces.TelecontrolScada);
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
        /// The identifier for the Creates ReferenceType.
        /// </summary>
        public static readonly ExpandedNodeId Creates = new ExpandedNodeId(Telecontrol.Scada.ReferenceTypes.Creates, Telecontrol.Scada.Namespaces.TelecontrolScada);

        /// <summary>
        /// The identifier for the HasPropertyCategory ReferenceType.
        /// </summary>
        public static readonly ExpandedNodeId HasPropertyCategory = new ExpandedNodeId(Telecontrol.Scada.ReferenceTypes.HasPropertyCategory, Telecontrol.Scada.Namespaces.TelecontrolScada);
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
        /// The BrowseName for the Creates component.
        /// </summary>
        public const string Creates = "Creates";

        /// <summary>
        /// The BrowseName for the GeneralPropertyCategory component.
        /// </summary>
        public const string GeneralPropertyCategory = "GeneralPropertyCategory";

        /// <summary>
        /// The BrowseName for the HasPropertyCategory component.
        /// </summary>
        public const string HasPropertyCategory = "HasPropertyCategory";

        /// <summary>
        /// The BrowseName for the PropertyCategories component.
        /// </summary>
        public const string PropertyCategories = "PropertyCategories";
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
    }
    #endregion
}