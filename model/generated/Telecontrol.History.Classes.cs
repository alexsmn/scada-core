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

namespace Telecontrol.History
{
    #region HistoricalDatabaseState Class
    #if (!OPCUA_EXCLUDE_HistoricalDatabaseState)
    /// <summary>
    /// Stores an instance of the HistoricalDatabaseType ObjectType.
    /// </summary>
    /// <exclude />
    [System.CodeDom.Compiler.GeneratedCodeAttribute("Opc.Ua.ModelCompiler", "1.0.0.0")]
    public partial class HistoricalDatabaseState : BaseObjectState
    {
        #region Constructors
        /// <summary>
        /// Initializes the type with its default attribute values.
        /// </summary>
        public HistoricalDatabaseState(NodeState parent) : base(parent)
        {
        }

        /// <summary>
        /// Returns the id of the default type definition node for the instance.
        /// </summary>
        protected override NodeId GetDefaultTypeDefinitionId(NamespaceTable namespaceUris)
        {
            return Opc.Ua.NodeId.Create(Telecontrol.History.ObjectTypes.HistoricalDatabaseType, Telecontrol.History.Namespaces.TelecontrolHistory, namespaceUris);
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
           "AgAAACEAAABodHRwOi8vdGVsZWNvbnRyb2wucnUvb3BjdWEvc2NhZGEjAAAAaHR0cDovL3RlbGVjb250" +
           "cm9sLnJ1L29wY3VhL2hpc3Rvcnn/////BGCAAgEAAAACAB4AAABIaXN0b3JpY2FsRGF0YWJhc2VUeXBl" +
           "SW5zdGFuY2UBAhQAAQIUABQAAAABAAAAAQEXAAEBAgwABwAAABVgqQoCAAAAAgAFAAAARGVwdGgBAiQA" +
           "AC4ARCQAAAAHAQAAAAAH/////wEB/////wAAAAAVYIkKAgAAAAIAEQAAAFdyaXR0ZW5WYWx1ZUNvdW50" +
           "AQKtOgAvAD+tOgAAAAf/////AQH/////AAAAABVgiQoCAAAAAgASAAAAV3JpdGVWYWx1ZUR1cmF0aW9u" +
           "AQKaOgAvAD+aOgAAAAf/////AQH/////AAAAABVgiQoCAAAAAgARAAAAV3JpdHRlbkV2ZW50Q291bnQB" +
           "Aq46AC8AP646AAAAB/////8BAf////8AAAAAFWCJCgIAAAACABAAAABQZW5kaW5nVGFza0NvdW50AQIy" +
           "AAAvAD8yAAAAAAf/////AQH/////AAAAABVgiQoCAAAAAgAUAAAARXZlbnRDbGVhbnVwRHVyYXRpb24B" +
           "AjMAAC8APzMAAAAAB/////8BAf////8AAAAAFWCJCgIAAAACABQAAABWYWx1ZUNsZWFudXBEdXJhdGlv" +
           "bgECNAAALwA/NAAAAAAH/////wEB/////wAAAAA=";
        #endregion
        #endif
        #endregion

        #region Public Properties
        /// <remarks />
        public PropertyState<uint> Depth
        {
            get
            {
                return m_depth;
            }

            set
            {
                if (!Object.ReferenceEquals(m_depth, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_depth = value;
            }
        }

        /// <remarks />
        public BaseDataVariableState<uint> WrittenValueCount
        {
            get
            {
                return m_writtenValueCount;
            }

            set
            {
                if (!Object.ReferenceEquals(m_writtenValueCount, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_writtenValueCount = value;
            }
        }

        /// <remarks />
        public BaseDataVariableState<uint> WriteValueDuration
        {
            get
            {
                return m_writeValueDuration;
            }

            set
            {
                if (!Object.ReferenceEquals(m_writeValueDuration, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_writeValueDuration = value;
            }
        }

        /// <remarks />
        public BaseDataVariableState<uint> WrittenEventCount
        {
            get
            {
                return m_writtenEventCount;
            }

            set
            {
                if (!Object.ReferenceEquals(m_writtenEventCount, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_writtenEventCount = value;
            }
        }

        /// <remarks />
        public BaseDataVariableState<uint> PendingTaskCount
        {
            get
            {
                return m_pendingTaskCount;
            }

            set
            {
                if (!Object.ReferenceEquals(m_pendingTaskCount, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_pendingTaskCount = value;
            }
        }

        /// <remarks />
        public BaseDataVariableState<uint> EventCleanupDuration
        {
            get
            {
                return m_eventCleanupDuration;
            }

            set
            {
                if (!Object.ReferenceEquals(m_eventCleanupDuration, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_eventCleanupDuration = value;
            }
        }

        /// <remarks />
        public BaseDataVariableState<uint> ValueCleanupDuration
        {
            get
            {
                return m_valueCleanupDuration;
            }

            set
            {
                if (!Object.ReferenceEquals(m_valueCleanupDuration, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_valueCleanupDuration = value;
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
            if (m_depth != null)
            {
                children.Add(m_depth);
            }

            if (m_writtenValueCount != null)
            {
                children.Add(m_writtenValueCount);
            }

            if (m_writeValueDuration != null)
            {
                children.Add(m_writeValueDuration);
            }

            if (m_writtenEventCount != null)
            {
                children.Add(m_writtenEventCount);
            }

            if (m_pendingTaskCount != null)
            {
                children.Add(m_pendingTaskCount);
            }

            if (m_eventCleanupDuration != null)
            {
                children.Add(m_eventCleanupDuration);
            }

            if (m_valueCleanupDuration != null)
            {
                children.Add(m_valueCleanupDuration);
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
                case Telecontrol.History.BrowseNames.Depth:
                {
                    if (createOrReplace)
                    {
                        if (Depth == null)
                        {
                            if (replacement == null)
                            {
                                Depth = new PropertyState<uint>(this);
                            }
                            else
                            {
                                Depth = (PropertyState<uint>)replacement;
                            }
                        }
                    }

                    instance = Depth;
                    break;
                }

                case Telecontrol.History.BrowseNames.WrittenValueCount:
                {
                    if (createOrReplace)
                    {
                        if (WrittenValueCount == null)
                        {
                            if (replacement == null)
                            {
                                WrittenValueCount = new BaseDataVariableState<uint>(this);
                            }
                            else
                            {
                                WrittenValueCount = (BaseDataVariableState<uint>)replacement;
                            }
                        }
                    }

                    instance = WrittenValueCount;
                    break;
                }

                case Telecontrol.History.BrowseNames.WriteValueDuration:
                {
                    if (createOrReplace)
                    {
                        if (WriteValueDuration == null)
                        {
                            if (replacement == null)
                            {
                                WriteValueDuration = new BaseDataVariableState<uint>(this);
                            }
                            else
                            {
                                WriteValueDuration = (BaseDataVariableState<uint>)replacement;
                            }
                        }
                    }

                    instance = WriteValueDuration;
                    break;
                }

                case Telecontrol.History.BrowseNames.WrittenEventCount:
                {
                    if (createOrReplace)
                    {
                        if (WrittenEventCount == null)
                        {
                            if (replacement == null)
                            {
                                WrittenEventCount = new BaseDataVariableState<uint>(this);
                            }
                            else
                            {
                                WrittenEventCount = (BaseDataVariableState<uint>)replacement;
                            }
                        }
                    }

                    instance = WrittenEventCount;
                    break;
                }

                case Telecontrol.History.BrowseNames.PendingTaskCount:
                {
                    if (createOrReplace)
                    {
                        if (PendingTaskCount == null)
                        {
                            if (replacement == null)
                            {
                                PendingTaskCount = new BaseDataVariableState<uint>(this);
                            }
                            else
                            {
                                PendingTaskCount = (BaseDataVariableState<uint>)replacement;
                            }
                        }
                    }

                    instance = PendingTaskCount;
                    break;
                }

                case Telecontrol.History.BrowseNames.EventCleanupDuration:
                {
                    if (createOrReplace)
                    {
                        if (EventCleanupDuration == null)
                        {
                            if (replacement == null)
                            {
                                EventCleanupDuration = new BaseDataVariableState<uint>(this);
                            }
                            else
                            {
                                EventCleanupDuration = (BaseDataVariableState<uint>)replacement;
                            }
                        }
                    }

                    instance = EventCleanupDuration;
                    break;
                }

                case Telecontrol.History.BrowseNames.ValueCleanupDuration:
                {
                    if (createOrReplace)
                    {
                        if (ValueCleanupDuration == null)
                        {
                            if (replacement == null)
                            {
                                ValueCleanupDuration = new BaseDataVariableState<uint>(this);
                            }
                            else
                            {
                                ValueCleanupDuration = (BaseDataVariableState<uint>)replacement;
                            }
                        }
                    }

                    instance = ValueCleanupDuration;
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
        private PropertyState<uint> m_depth;
        private BaseDataVariableState<uint> m_writtenValueCount;
        private BaseDataVariableState<uint> m_writeValueDuration;
        private BaseDataVariableState<uint> m_writtenEventCount;
        private BaseDataVariableState<uint> m_pendingTaskCount;
        private BaseDataVariableState<uint> m_eventCleanupDuration;
        private BaseDataVariableState<uint> m_valueCleanupDuration;
        #endregion
    }
    #endif
    #endregion
}