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
using System.Xml;
using System.Runtime.Serialization;
using Opc.Ua;
using Telecontrol.Scada;

namespace Telecontrol.FileSystem
{
    #region FileDirectoryState Class
    #if (!OPCUA_EXCLUDE_FileDirectoryState)
    /// <summary>
    /// Stores an instance of the FileDirectoryType ObjectType.
    /// </summary>
    /// <exclude />
    [System.CodeDom.Compiler.GeneratedCodeAttribute("Opc.Ua.ModelCompiler", "1.0.0.0")]
    public partial class FileDirectoryState : BaseObjectState
    {
        #region Constructors
        /// <summary>
        /// Initializes the type with its default attribute values.
        /// </summary>
        public FileDirectoryState(NodeState parent) : base(parent)
        {
        }

        /// <summary>
        /// Returns the id of the default type definition node for the instance.
        /// </summary>
        protected override NodeId GetDefaultTypeDefinitionId(NamespaceTable namespaceUris)
        {
            return Opc.Ua.NodeId.Create(Telecontrol.FileSystem.ObjectTypes.FileDirectoryType, Telecontrol.FileSystem.Namespaces.TelecontrolFileSystem, namespaceUris);
        }

        #if (!OPCUA_EXCLUDE_InitializationStrings)
        /// <summary>
        /// Initializes the instance.
        /// </summary>
        protected override void Initialize(ISystemContext context)
        {
            Initialize(context, InitializationString);
            InitializeOptionalChildren(context);
        }

        /// <summary>
        /// Initializes the instance with a node.
        /// </summary>
        protected override void Initialize(ISystemContext context, NodeState source)
        {
            InitializeOptionalChildren(context);
            base.Initialize(context, source);
        }

        /// <summary>
        /// Initializes the any option children defined for the instance.
        /// </summary>
        protected override void InitializeOptionalChildren(ISystemContext context)
        {
            base.InitializeOptionalChildren(context);
        }

        #region Initialization String
        private const string InitializationString =
           "AgAAACEAAABodHRwOi8vdGVsZWNvbnRyb2wucnUvb3BjdWEvc2NhZGEmAAAAaHR0cDovL3RlbGVjb250" +
           "cm9sLnJ1L29wY3VhL2ZpbGVzeXN0ZW3/////BGCAAgEAAAACABkAAABGaWxlRGlyZWN0b3J5VHlwZUlu" +
           "c3RhbmNlAQKpOgECqTqpOgAAAQAAAAEBFwAAAQKpOgAAAAA=";
        #endregion
        #endif
        #endregion

        #region Public Properties
        #endregion

        #region Overridden Methods
        #endregion

        #region Private Fields
        #endregion
    }
    #endif
    #endregion

    #region FileState Class
    #if (!OPCUA_EXCLUDE_FileState)
    /// <summary>
    /// Stores an instance of the FileType VariableType.
    /// </summary>
    /// <exclude />
    [System.CodeDom.Compiler.GeneratedCodeAttribute("Opc.Ua.ModelCompiler", "1.0.0.0")]
    public partial class FileState : BaseVariableState<byte[]>
    {
        #region Constructors
        /// <summary>
        /// Initializes the type with its default attribute values.
        /// </summary>
        public FileState(NodeState parent) : base(parent)
        {
        }

        /// <summary>
        /// Returns the id of the default type definition node for the instance.
        /// </summary>
        protected override NodeId GetDefaultTypeDefinitionId(NamespaceTable namespaceUris)
        {
            return Opc.Ua.NodeId.Create(Telecontrol.FileSystem.VariableTypes.FileType, Telecontrol.FileSystem.Namespaces.TelecontrolFileSystem, namespaceUris);
        }

        /// <summary>
        /// Returns the id of the default data type node for the instance.
        /// </summary>
        protected override NodeId GetDefaultDataTypeId(NamespaceTable namespaceUris)
        {
            return Opc.Ua.NodeId.Create(Opc.Ua.DataTypes.ByteString, Opc.Ua.Namespaces.OpcUa, namespaceUris);
        }

        /// <summary>
        /// Returns the id of the default value rank for the instance.
        /// </summary>
        protected override int GetDefaultValueRank()
        {
            return ValueRanks.Scalar;
        }

        #if (!OPCUA_EXCLUDE_InitializationStrings)
        /// <summary>
        /// Initializes the instance.
        /// </summary>
        protected override void Initialize(ISystemContext context)
        {
            Initialize(context, InitializationString);
            InitializeOptionalChildren(context);
        }

        /// <summary>
        /// Initializes the instance with a node.
        /// </summary>
        protected override void Initialize(ISystemContext context, NodeState source)
        {
            InitializeOptionalChildren(context);
            base.Initialize(context, source);
        }

        /// <summary>
        /// Initializes the any option children defined for the instance.
        /// </summary>
        protected override void InitializeOptionalChildren(ISystemContext context)
        {
            base.InitializeOptionalChildren(context);
        }

        #region Initialization String
        private const string InitializationString =
           "AgAAACEAAABodHRwOi8vdGVsZWNvbnRyb2wucnUvb3BjdWEvc2NhZGEmAAAAaHR0cDovL3RlbGVjb250" +
           "cm9sLnJ1L29wY3VhL2ZpbGVzeXN0ZW3/////FWCJAgIAAAACABAAAABGaWxlVHlwZUluc3RhbmNlAQKq" +
           "OgECqjqqOgAAAA/+////AQEBAAAAAQEXAAEBAqk6AwAAABVgiQoCAAAAAgAMAAAAUmVsYXRpdmVQYXRo" +
           "AQKZOgAuAESZOgAAABX/////AQH/////AAAAABVgiQoCAAAAAgAOAAAATGFzdFVwZGF0ZVRpbWUBAqs6" +
           "AC4ARKs6AAAADf////8BAf////8AAAAAFWCJCgIAAAACAAQAAABTaXplAQKsOgAuAESsOgAAAAn/////" +
           "AQH/////AAAAAA==";
        #endregion
        #endif
        #endregion

        #region Public Properties
        /// <remarks />
        public PropertyState<LocalizedText> RelativePath
        {
            get
            {
                return m_relativePath;
            }

            set
            {
                if (!Object.ReferenceEquals(m_relativePath, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_relativePath = value;
            }
        }

        /// <remarks />
        public PropertyState<DateTime> LastUpdateTime
        {
            get
            {
                return m_lastUpdateTime;
            }

            set
            {
                if (!Object.ReferenceEquals(m_lastUpdateTime, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_lastUpdateTime = value;
            }
        }

        /// <remarks />
        public PropertyState<ulong> Size
        {
            get
            {
                return m_size;
            }

            set
            {
                if (!Object.ReferenceEquals(m_size, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_size = value;
            }
        }
        #endregion

        #region Overridden Methods
        /// <summary>
        /// Populates a list with the children that belong to the node.
        /// </summary>
        /// <param name="context">The context for the system being accessed.</param>
        /// <param name="children">The list of children to populate.</param>
        public override void GetChildren(
            ISystemContext context,
            IList<BaseInstanceState> children)
        {
            if (m_relativePath != null)
            {
                children.Add(m_relativePath);
            }

            if (m_lastUpdateTime != null)
            {
                children.Add(m_lastUpdateTime);
            }

            if (m_size != null)
            {
                children.Add(m_size);
            }

            base.GetChildren(context, children);
        }

        /// <summary>
        /// Finds the child with the specified browse name.
        /// </summary>
        protected override BaseInstanceState FindChild(
            ISystemContext context,
            QualifiedName browseName,
            bool createOrReplace,
            BaseInstanceState replacement)
        {
            if (QualifiedName.IsNull(browseName))
            {
                return null;
            }

            BaseInstanceState instance = null;

            switch (browseName.Name)
            {
                case Telecontrol.FileSystem.BrowseNames.RelativePath:
                {
                    if (createOrReplace)
                    {
                        if (RelativePath == null)
                        {
                            if (replacement == null)
                            {
                                RelativePath = new PropertyState<LocalizedText>(this);
                            }
                            else
                            {
                                RelativePath = (PropertyState<LocalizedText>)replacement;
                            }
                        }
                    }

                    instance = RelativePath;
                    break;
                }

                case Telecontrol.FileSystem.BrowseNames.LastUpdateTime:
                {
                    if (createOrReplace)
                    {
                        if (LastUpdateTime == null)
                        {
                            if (replacement == null)
                            {
                                LastUpdateTime = new PropertyState<DateTime>(this);
                            }
                            else
                            {
                                LastUpdateTime = (PropertyState<DateTime>)replacement;
                            }
                        }
                    }

                    instance = LastUpdateTime;
                    break;
                }

                case Telecontrol.FileSystem.BrowseNames.Size:
                {
                    if (createOrReplace)
                    {
                        if (Size == null)
                        {
                            if (replacement == null)
                            {
                                Size = new PropertyState<ulong>(this);
                            }
                            else
                            {
                                Size = (PropertyState<ulong>)replacement;
                            }
                        }
                    }

                    instance = Size;
                    break;
                }
            }

            if (instance != null)
            {
                return instance;
            }

            return base.FindChild(context, browseName, createOrReplace, replacement);
        }
        #endregion

        #region Private Fields
        private PropertyState<LocalizedText> m_relativePath;
        private PropertyState<DateTime> m_lastUpdateTime;
        private PropertyState<ulong> m_size;
        #endregion
    }
    #endif
    #endregion
}