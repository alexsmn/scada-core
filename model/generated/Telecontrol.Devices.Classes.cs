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

namespace Telecontrol.Devices
{
    #region DeviceState Class
    #if (!OPCUA_EXCLUDE_DeviceState)
    /// <summary>
    /// Stores an instance of the DeviceType ObjectType.
    /// </summary>
    /// <exclude />
    [System.CodeDom.Compiler.GeneratedCodeAttribute("Opc.Ua.ModelCompiler", "1.0.0.0")]
    public partial class DeviceState : BaseObjectState
    {
        #region Constructors
        /// <summary>
        /// Initializes the type with its default attribute values.
        /// </summary>
        public DeviceState(NodeState parent) : base(parent)
        {
        }

        /// <summary>
        /// Returns the id of the default type definition node for the instance.
        /// </summary>
        protected override NodeId GetDefaultTypeDefinitionId(NamespaceTable namespaceUris)
        {
            return Opc.Ua.NodeId.Create(Telecontrol.Devices.ObjectTypes.DeviceType, Telecontrol.Devices.Namespaces.TelecontrolDevices, namespaceUris);
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
           "cm9sLnJ1L29wY3VhL2RldmljZXP/////BGCAAgEAAAACABIAAABEZXZpY2VUeXBlSW5zdGFuY2UBAh8A" +
           "AQIfAB8AAAD/////CQAAABVgiQoCAAAAAgAIAAAARGlzYWJsZWQBAskAAC4ARMkAAAAAAf////8BAf//" +
           "//8AAAAAFWCJCgIAAAACAAYAAABPbmxpbmUBAsoAAC8AP8oAAAAAAf////8BAf////8AAAAAFWCJCgIA" +
           "AAACAAcAAABFbmFibGVkAQLLAAAvAD/LAAAAAAH/////AQH/////AAAAAARhggoEAAAAAgALAAAASW50" +
           "ZXJyb2dhdGUBAusAAC8BAusA6wAAAAEB/////wAAAAAEYYIKBAAAAAIAEgAAAEludGVycm9nYXRlQ2hh" +
           "bm5lbAECFgEALwECFgEWAQAAAQH/////AAAAAARhggoEAAAAAgAJAAAAU3luY0Nsb2NrAQLsAAAvAQLs" +
           "AOwAAAABAf////8AAAAABGGCCgQAAAACAAYAAABTZWxlY3QBAgABAC8BAgABAAEAAAEB/////wAAAAAE" +
           "YYIKBAAAAAIABQAAAFdyaXRlAQIBAQAvAQIBAQEBAAABAf////8AAAAABGGCCgQAAAACAAoAAABXcml0" +
           "ZVBhcmFtAQICAQAvAQICAQIBAAABAf////8AAAAA";
        #endregion
        #endif
        #endregion

        #region Public Properties
        /// <remarks />
        public PropertyState<bool> Disabled
        {
            get
            {
                return m_disabled;
            }

            set
            {
                if (!Object.ReferenceEquals(m_disabled, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_disabled = value;
            }
        }

        /// <remarks />
        public BaseDataVariableState<bool> Online
        {
            get
            {
                return m_online;
            }

            set
            {
                if (!Object.ReferenceEquals(m_online, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_online = value;
            }
        }

        /// <remarks />
        public BaseDataVariableState<bool> Enabled
        {
            get
            {
                return m_enabled;
            }

            set
            {
                if (!Object.ReferenceEquals(m_enabled, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_enabled = value;
            }
        }

        /// <remarks />
        public MethodState Interrogate
        {
            get
            {
                return m_interrogateMethod;
            }

            set
            {
                if (!Object.ReferenceEquals(m_interrogateMethod, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_interrogateMethod = value;
            }
        }

        /// <remarks />
        public MethodState InterrogateChannel
        {
            get
            {
                return m_interrogateChannelMethod;
            }

            set
            {
                if (!Object.ReferenceEquals(m_interrogateChannelMethod, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_interrogateChannelMethod = value;
            }
        }

        /// <remarks />
        public MethodState SyncClock
        {
            get
            {
                return m_syncClockMethod;
            }

            set
            {
                if (!Object.ReferenceEquals(m_syncClockMethod, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_syncClockMethod = value;
            }
        }

        /// <remarks />
        public MethodState Select
        {
            get
            {
                return m_selectMethod;
            }

            set
            {
                if (!Object.ReferenceEquals(m_selectMethod, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_selectMethod = value;
            }
        }

        /// <remarks />
        public MethodState Write
        {
            get
            {
                return m_writeMethod;
            }

            set
            {
                if (!Object.ReferenceEquals(m_writeMethod, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_writeMethod = value;
            }
        }

        /// <remarks />
        public MethodState WriteParam
        {
            get
            {
                return m_writeParamMethod;
            }

            set
            {
                if (!Object.ReferenceEquals(m_writeParamMethod, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_writeParamMethod = value;
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
            if (m_disabled != null)
            {
                children.Add(m_disabled);
            }

            if (m_online != null)
            {
                children.Add(m_online);
            }

            if (m_enabled != null)
            {
                children.Add(m_enabled);
            }

            if (m_interrogateMethod != null)
            {
                children.Add(m_interrogateMethod);
            }

            if (m_interrogateChannelMethod != null)
            {
                children.Add(m_interrogateChannelMethod);
            }

            if (m_syncClockMethod != null)
            {
                children.Add(m_syncClockMethod);
            }

            if (m_selectMethod != null)
            {
                children.Add(m_selectMethod);
            }

            if (m_writeMethod != null)
            {
                children.Add(m_writeMethod);
            }

            if (m_writeParamMethod != null)
            {
                children.Add(m_writeParamMethod);
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
                case Telecontrol.Devices.BrowseNames.Disabled:
                {
                    if (createOrReplace)
                    {
                        if (Disabled == null)
                        {
                            if (replacement == null)
                            {
                                Disabled = new PropertyState<bool>(this);
                            }
                            else
                            {
                                Disabled = (PropertyState<bool>)replacement;
                            }
                        }
                    }

                    instance = Disabled;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.Online:
                {
                    if (createOrReplace)
                    {
                        if (Online == null)
                        {
                            if (replacement == null)
                            {
                                Online = new BaseDataVariableState<bool>(this);
                            }
                            else
                            {
                                Online = (BaseDataVariableState<bool>)replacement;
                            }
                        }
                    }

                    instance = Online;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.Enabled:
                {
                    if (createOrReplace)
                    {
                        if (Enabled == null)
                        {
                            if (replacement == null)
                            {
                                Enabled = new BaseDataVariableState<bool>(this);
                            }
                            else
                            {
                                Enabled = (BaseDataVariableState<bool>)replacement;
                            }
                        }
                    }

                    instance = Enabled;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.Interrogate:
                {
                    if (createOrReplace)
                    {
                        if (Interrogate == null)
                        {
                            if (replacement == null)
                            {
                                Interrogate = new MethodState(this);
                            }
                            else
                            {
                                Interrogate = (MethodState)replacement;
                            }
                        }
                    }

                    instance = Interrogate;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.InterrogateChannel:
                {
                    if (createOrReplace)
                    {
                        if (InterrogateChannel == null)
                        {
                            if (replacement == null)
                            {
                                InterrogateChannel = new MethodState(this);
                            }
                            else
                            {
                                InterrogateChannel = (MethodState)replacement;
                            }
                        }
                    }

                    instance = InterrogateChannel;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.SyncClock:
                {
                    if (createOrReplace)
                    {
                        if (SyncClock == null)
                        {
                            if (replacement == null)
                            {
                                SyncClock = new MethodState(this);
                            }
                            else
                            {
                                SyncClock = (MethodState)replacement;
                            }
                        }
                    }

                    instance = SyncClock;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.Select:
                {
                    if (createOrReplace)
                    {
                        if (Select == null)
                        {
                            if (replacement == null)
                            {
                                Select = new MethodState(this);
                            }
                            else
                            {
                                Select = (MethodState)replacement;
                            }
                        }
                    }

                    instance = Select;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.Write:
                {
                    if (createOrReplace)
                    {
                        if (Write == null)
                        {
                            if (replacement == null)
                            {
                                Write = new MethodState(this);
                            }
                            else
                            {
                                Write = (MethodState)replacement;
                            }
                        }
                    }

                    instance = Write;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.WriteParam:
                {
                    if (createOrReplace)
                    {
                        if (WriteParam == null)
                        {
                            if (replacement == null)
                            {
                                WriteParam = new MethodState(this);
                            }
                            else
                            {
                                WriteParam = (MethodState)replacement;
                            }
                        }
                    }

                    instance = WriteParam;
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
        private PropertyState<bool> m_disabled;
        private BaseDataVariableState<bool> m_online;
        private BaseDataVariableState<bool> m_enabled;
        private MethodState m_interrogateMethod;
        private MethodState m_interrogateChannelMethod;
        private MethodState m_syncClockMethod;
        private MethodState m_selectMethod;
        private MethodState m_writeMethod;
        private MethodState m_writeParamMethod;
        #endregion
    }
    #endif
    #endregion

    #region LinkState Class
    #if (!OPCUA_EXCLUDE_LinkState)
    /// <summary>
    /// Stores an instance of the LinkType ObjectType.
    /// </summary>
    /// <exclude />
    [System.CodeDom.Compiler.GeneratedCodeAttribute("Opc.Ua.ModelCompiler", "1.0.0.0")]
    public partial class LinkState : DeviceState
    {
        #region Constructors
        /// <summary>
        /// Initializes the type with its default attribute values.
        /// </summary>
        public LinkState(NodeState parent) : base(parent)
        {
        }

        /// <summary>
        /// Returns the id of the default type definition node for the instance.
        /// </summary>
        protected override NodeId GetDefaultTypeDefinitionId(NamespaceTable namespaceUris)
        {
            return Opc.Ua.NodeId.Create(Telecontrol.Devices.ObjectTypes.LinkType, Telecontrol.Devices.Namespaces.TelecontrolDevices, namespaceUris);
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
           "cm9sLnJ1L29wY3VhL2RldmljZXP/////BGCAAgEAAAACABAAAABMaW5rVHlwZUluc3RhbmNlAQIoAAEC" +
           "KAAoAAAAAQAAAAEBFwABAQIeABAAAAAVYIkKAgAAAAIACAAAAERpc2FibGVkAQLMAAAuAETMAAAAAAH/" +
           "////AQH/////AAAAABVgiQoCAAAAAgAGAAAAT25saW5lAQLNAAAvAD/NAAAAAAH/////AQH/////AAAA" +
           "ABVgiQoCAAAAAgAHAAAARW5hYmxlZAECzgAALwA/zgAAAAAB/////wEB/////wAAAAAEYYIKBAAAAAIA" +
           "CwAAAEludGVycm9nYXRlAQLtAAAvAQLrAO0AAAABAf////8AAAAABGGCCgQAAAACABIAAABJbnRlcnJv" +
           "Z2F0ZUNoYW5uZWwBAhcBAC8BAhYBFwEAAAEB/////wAAAAAEYYIKBAAAAAIACQAAAFN5bmNDbG9jawEC" +
           "7gAALwEC7ADuAAAAAQH/////AAAAAARhggoEAAAAAgAGAAAAU2VsZWN0AQIDAQAvAQIAAQMBAAABAf//" +
           "//8AAAAABGGCCgQAAAACAAUAAABXcml0ZQECBAEALwECAQEEAQAAAQH/////AAAAAARhggoEAAAAAgAK" +
           "AAAAV3JpdGVQYXJhbQECBQEALwECAgEFAQAAAQH/////AAAAABVgiQoCAAAAAgAJAAAAVHJhbnNwb3J0" +
           "AQIqAAAuAEQqAAAAAAz/////AQH/////AAAAABVgiQoCAAAAAgAMAAAAQ29ubmVjdENvdW50AQIrAAAv" +
           "AD8rAAAAAAf/////AQH/////AAAAABVgiQoCAAAAAgARAAAAQWN0aXZlQ29ubmVjdGlvbnMBAiwAAC8A" +
           "PywAAAAAB/////8BAf////8AAAAAFWCJCgIAAAACAAsAAABNZXNzYWdlc091dAECLQAALwA/LQAAAAAH" +
           "/////wEB/////wAAAAAVYIkKAgAAAAIACgAAAE1lc3NhZ2VzSW4BAi4AAC8APy4AAAAAB/////8BAf//" +
           "//8AAAAAFWCJCgIAAAACAAgAAABCeXRlc091dAECLwAALwA/LwAAAAAH/////wEB/////wAAAAAVYIkK" +
           "AgAAAAIABwAAAEJ5dGVzSW4BAjAAAC8APzAAAAAAB/////8BAf////8AAAAA";
        #endregion
        #endif
        #endregion

        #region Public Properties
        /// <remarks />
        public PropertyState<string> Transport
        {
            get
            {
                return m_transport;
            }

            set
            {
                if (!Object.ReferenceEquals(m_transport, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_transport = value;
            }
        }

        /// <remarks />
        public BaseDataVariableState<uint> ConnectCount
        {
            get
            {
                return m_connectCount;
            }

            set
            {
                if (!Object.ReferenceEquals(m_connectCount, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_connectCount = value;
            }
        }

        /// <remarks />
        public BaseDataVariableState<uint> ActiveConnections
        {
            get
            {
                return m_activeConnections;
            }

            set
            {
                if (!Object.ReferenceEquals(m_activeConnections, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_activeConnections = value;
            }
        }

        /// <remarks />
        public BaseDataVariableState<uint> MessagesOut
        {
            get
            {
                return m_messagesOut;
            }

            set
            {
                if (!Object.ReferenceEquals(m_messagesOut, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_messagesOut = value;
            }
        }

        /// <remarks />
        public BaseDataVariableState<uint> MessagesIn
        {
            get
            {
                return m_messagesIn;
            }

            set
            {
                if (!Object.ReferenceEquals(m_messagesIn, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_messagesIn = value;
            }
        }

        /// <remarks />
        public BaseDataVariableState<uint> BytesOut
        {
            get
            {
                return m_bytesOut;
            }

            set
            {
                if (!Object.ReferenceEquals(m_bytesOut, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_bytesOut = value;
            }
        }

        /// <remarks />
        public BaseDataVariableState<uint> BytesIn
        {
            get
            {
                return m_bytesIn;
            }

            set
            {
                if (!Object.ReferenceEquals(m_bytesIn, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_bytesIn = value;
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
            if (m_transport != null)
            {
                children.Add(m_transport);
            }

            if (m_connectCount != null)
            {
                children.Add(m_connectCount);
            }

            if (m_activeConnections != null)
            {
                children.Add(m_activeConnections);
            }

            if (m_messagesOut != null)
            {
                children.Add(m_messagesOut);
            }

            if (m_messagesIn != null)
            {
                children.Add(m_messagesIn);
            }

            if (m_bytesOut != null)
            {
                children.Add(m_bytesOut);
            }

            if (m_bytesIn != null)
            {
                children.Add(m_bytesIn);
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
                case Telecontrol.Devices.BrowseNames.Transport:
                {
                    if (createOrReplace)
                    {
                        if (Transport == null)
                        {
                            if (replacement == null)
                            {
                                Transport = new PropertyState<string>(this);
                            }
                            else
                            {
                                Transport = (PropertyState<string>)replacement;
                            }
                        }
                    }

                    instance = Transport;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.ConnectCount:
                {
                    if (createOrReplace)
                    {
                        if (ConnectCount == null)
                        {
                            if (replacement == null)
                            {
                                ConnectCount = new BaseDataVariableState<uint>(this);
                            }
                            else
                            {
                                ConnectCount = (BaseDataVariableState<uint>)replacement;
                            }
                        }
                    }

                    instance = ConnectCount;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.ActiveConnections:
                {
                    if (createOrReplace)
                    {
                        if (ActiveConnections == null)
                        {
                            if (replacement == null)
                            {
                                ActiveConnections = new BaseDataVariableState<uint>(this);
                            }
                            else
                            {
                                ActiveConnections = (BaseDataVariableState<uint>)replacement;
                            }
                        }
                    }

                    instance = ActiveConnections;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.MessagesOut:
                {
                    if (createOrReplace)
                    {
                        if (MessagesOut == null)
                        {
                            if (replacement == null)
                            {
                                MessagesOut = new BaseDataVariableState<uint>(this);
                            }
                            else
                            {
                                MessagesOut = (BaseDataVariableState<uint>)replacement;
                            }
                        }
                    }

                    instance = MessagesOut;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.MessagesIn:
                {
                    if (createOrReplace)
                    {
                        if (MessagesIn == null)
                        {
                            if (replacement == null)
                            {
                                MessagesIn = new BaseDataVariableState<uint>(this);
                            }
                            else
                            {
                                MessagesIn = (BaseDataVariableState<uint>)replacement;
                            }
                        }
                    }

                    instance = MessagesIn;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.BytesOut:
                {
                    if (createOrReplace)
                    {
                        if (BytesOut == null)
                        {
                            if (replacement == null)
                            {
                                BytesOut = new BaseDataVariableState<uint>(this);
                            }
                            else
                            {
                                BytesOut = (BaseDataVariableState<uint>)replacement;
                            }
                        }
                    }

                    instance = BytesOut;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.BytesIn:
                {
                    if (createOrReplace)
                    {
                        if (BytesIn == null)
                        {
                            if (replacement == null)
                            {
                                BytesIn = new BaseDataVariableState<uint>(this);
                            }
                            else
                            {
                                BytesIn = (BaseDataVariableState<uint>)replacement;
                            }
                        }
                    }

                    instance = BytesIn;
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
        private PropertyState<string> m_transport;
        private BaseDataVariableState<uint> m_connectCount;
        private BaseDataVariableState<uint> m_activeConnections;
        private BaseDataVariableState<uint> m_messagesOut;
        private BaseDataVariableState<uint> m_messagesIn;
        private BaseDataVariableState<uint> m_bytesOut;
        private BaseDataVariableState<uint> m_bytesIn;
        #endregion
    }
    #endif
    #endregion

    #region DeviceWatchEventState Class
    #if (!OPCUA_EXCLUDE_DeviceWatchEventState)
    /// <summary>
    /// Stores an instance of the DeviceWatchEventType ObjectType.
    /// </summary>
    /// <exclude />
    [System.CodeDom.Compiler.GeneratedCodeAttribute("Opc.Ua.ModelCompiler", "1.0.0.0")]
    public partial class DeviceWatchEventState : BaseEventState
    {
        #region Constructors
        /// <summary>
        /// Initializes the type with its default attribute values.
        /// </summary>
        public DeviceWatchEventState(NodeState parent) : base(parent)
        {
        }

        /// <summary>
        /// Returns the id of the default type definition node for the instance.
        /// </summary>
        protected override NodeId GetDefaultTypeDefinitionId(NamespaceTable namespaceUris)
        {
            return Opc.Ua.NodeId.Create(Telecontrol.Devices.ObjectTypes.DeviceWatchEventType, Telecontrol.Devices.Namespaces.TelecontrolDevices, namespaceUris);
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
           "cm9sLnJ1L29wY3VhL2RldmljZXP/////BGCAAgEAAAACABwAAABEZXZpY2VXYXRjaEV2ZW50VHlwZUlu" +
           "c3RhbmNlAQLbAAEC2wDbAAAA/////wgAAAAVYIkKAgAAAAAABwAAAEV2ZW50SWQBAtwAAC4ARNwAAAAA" +
           "D/////8BAf////8AAAAAFWCJCgIAAAAAAAkAAABFdmVudFR5cGUBAt0AAC4ARN0AAAAAEf////8BAf//" +
           "//8AAAAAFWCJCgIAAAAAAAoAAABTb3VyY2VOb2RlAQLeAAAuAETeAAAAABH/////AQH/////AAAAABVg" +
           "iQoCAAAAAAAKAAAAU291cmNlTmFtZQEC3wAALgBE3wAAAAAM/////wEB/////wAAAAAVYIkKAgAAAAAA" +
           "BAAAAFRpbWUBAuAAAC4AROAAAAABACYB/////wEB/////wAAAAAVYIkKAgAAAAAACwAAAFJlY2VpdmVU" +
           "aW1lAQLhAAAuAEThAAAAAQAmAf////8BAf////8AAAAAFWCJCgIAAAAAAAcAAABNZXNzYWdlAQLjAAAu" +
           "AETjAAAAABX/////AQH/////AAAAABVgiQoCAAAAAAAIAAAAU2V2ZXJpdHkBAuQAAC4AROQAAAAABf//" +
           "//8BAf////8AAAAA";
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

    #region Iec60870LinkState Class
    #if (!OPCUA_EXCLUDE_Iec60870LinkState)
    /// <summary>
    /// Stores an instance of the Iec60870LinkType ObjectType.
    /// </summary>
    /// <exclude />
    [System.CodeDom.Compiler.GeneratedCodeAttribute("Opc.Ua.ModelCompiler", "1.0.0.0")]
    public partial class Iec60870LinkState : LinkState
    {
        #region Constructors
        /// <summary>
        /// Initializes the type with its default attribute values.
        /// </summary>
        public Iec60870LinkState(NodeState parent) : base(parent)
        {
        }

        /// <summary>
        /// Returns the id of the default type definition node for the instance.
        /// </summary>
        protected override NodeId GetDefaultTypeDefinitionId(NamespaceTable namespaceUris)
        {
            return Opc.Ua.NodeId.Create(Telecontrol.Devices.ObjectTypes.Iec60870LinkType, Telecontrol.Devices.Namespaces.TelecontrolDevices, namespaceUris);
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
           "cm9sLnJ1L29wY3VhL2RldmljZXP/////BGCAAgEAAAACABgAAABJZWM2MDg3MExpbmtUeXBlSW5zdGFu" +
           "Y2UBAikBAQIpASkBAAD/////IQAAABVgiQoCAAAAAgAIAAAARGlzYWJsZWQBAioBAC4ARCoBAAAAAf//" +
           "//8BAf////8AAAAAFWCJCgIAAAACAAYAAABPbmxpbmUBAisBAC8APysBAAAAAf////8BAf////8AAAAA" +
           "FWCJCgIAAAACAAcAAABFbmFibGVkAQIsAQAvAD8sAQAAAAH/////AQH/////AAAAAARhggoEAAAAAgAL" +
           "AAAASW50ZXJyb2dhdGUBAi0BAC8BAusALQEAAAEB/////wAAAAAEYYIKBAAAAAIAEgAAAEludGVycm9n" +
           "YXRlQ2hhbm5lbAECLgEALwECFgEuAQAAAQH/////AAAAAARhggoEAAAAAgAJAAAAU3luY0Nsb2NrAQIv" +
           "AQAvAQLsAC8BAAABAf////8AAAAABGGCCgQAAAACAAYAAABTZWxlY3QBAjABAC8BAgABMAEAAAEB////" +
           "/wAAAAAEYYIKBAAAAAIABQAAAFdyaXRlAQIxAQAvAQIBATEBAAABAf////8AAAAABGGCCgQAAAACAAoA" +
           "AABXcml0ZVBhcmFtAQIyAQAvAQICATIBAAABAf////8AAAAAFWCJCgIAAAACAAkAAABUcmFuc3BvcnQB" +
           "AjMBAC4ARDMBAAAADP////8BAf////8AAAAAFWCJCgIAAAACAAwAAABDb25uZWN0Q291bnQBAjQBAC8A" +
           "PzQBAAAAB/////8BAf////8AAAAAFWCJCgIAAAACABEAAABBY3RpdmVDb25uZWN0aW9ucwECNQEALwA/" +
           "NQEAAAAH/////wEB/////wAAAAAVYIkKAgAAAAIACwAAAE1lc3NhZ2VzT3V0AQI2AQAvAD82AQAAAAf/" +
           "////AQH/////AAAAABVgiQoCAAAAAgAKAAAATWVzc2FnZXNJbgECNwEALwA/NwEAAAAH/////wEB////" +
           "/wAAAAAVYIkKAgAAAAIACAAAAEJ5dGVzT3V0AQI4AQAvAD84AQAAAAf/////AQH/////AAAAABVgiQoC" +
           "AAAAAgAHAAAAQnl0ZXNJbgECOQEALwA/OQEAAAAH/////wEB/////wAAAAAVYIkKAgAAAAIACAAAAFBy" +
           "b3RvY29sAQI6AQAuAEQ6AQAAAQKvOv////8BAf////8AAAAAFWCJCgIAAAACAAQAAABNb2RlAQI7AQAu" +
           "AEQ7AQAAAAf/////AQH/////AAAAABVgqQoCAAAAAgANAAAAU2VuZFF1ZXVlU2l6ZQECPAEALgBEPAEA" +
           "AAcMAAAAAAf/////AQH/////AAAAABVgqQoCAAAAAgAQAAAAUmVjZWl2ZVF1ZXVlU2l6ZQECPQEALgBE" +
           "PQEAAAcIAAAAAAf/////AQH/////AAAAABVgqQoCAAAAAgAOAAAAQ29ubmVjdFRpbWVvdXQBAj4BAC4A" +
           "RD4BAAAHHgAAAAAH/////wEB/////wAAAAAVYKkKAgAAAAIAEwAAAENvbmZpcm1hdGlvblRpbWVvdXQB" +
           "Aj8BAC4ARD8BAAAHBQAAAAAH/////wEB/////wAAAAAVYKkKAgAAAAIAEgAAAFRlcm1pbmF0aW9uVGlt" +
           "ZW91dAECQAEALgBEQAEAAAcUAAAAAAf/////AQH/////AAAAABVgqQoCAAAAAgARAAAARGV2aWNlQWRk" +
           "cmVzc1NpemUBAkEBAC4AREEBAAAHAgAAAAAH/////wEB/////wAAAAAVYKkKAgAAAAIABwAAAENPVFNp" +
           "emUBAkIBAC4AREIBAAAHAgAAAAAH/////wEB/////wAAAAAVYKkKAgAAAAIADwAAAEluZm9BZGRyZXNz" +
           "U2l6ZQECQwEALgBEQwEAAAcDAAAAAAf/////AQH/////AAAAABVgqQoCAAAAAgAOAAAARGF0YUNvbGxl" +
           "Y3Rpb24BAkQBAC4AREQBAAABAQAB/////wEB/////wAAAAAVYKkKAgAAAAIADgAAAFNlbmRSZXRyeUNv" +
           "dW50AQJFAQAuAERFAQAABwAAAAAAB/////8BAf////8AAAAAFWCpCgIAAAACAA0AAABDUkNQcm90ZWN0" +
           "aW9uAQJGAQAuAERGAQAAAQAAAf////8BAf////8AAAAAFWCpCgIAAAACAAsAAABTZW5kVGltZW91dAEC" +
           "RwEALgBERwEAAAcFAAAAAAf/////AQH/////AAAAABVgqQoCAAAAAgAOAAAAUmVjZWl2ZVRpbWVvdXQB" +
           "AkgBAC4AREgBAAAHCgAAAAAH/////wEB/////wAAAAAVYKkKAgAAAAIACwAAAElkbGVUaW1lb3V0AQJJ" +
           "AQAuAERJAQAABx4AAAAAB/////8BAf////8AAAAAFWCpCgIAAAACAA0AAABBbm9ueW1vdXNNb2RlAQJK" +
           "AQAuAERKAQAAAQAAAf////8BAf////8AAAAA";
        #endregion
        #endif
        #endregion

        #region Public Properties
        /// <remarks />
        public PropertyState<Iec60870ProtocolDataType> Protocol
        {
            get
            {
                return m_protocol;
            }

            set
            {
                if (!Object.ReferenceEquals(m_protocol, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_protocol = value;
            }
        }

        /// <remarks />
        public PropertyState<uint> Mode
        {
            get
            {
                return m_mode;
            }

            set
            {
                if (!Object.ReferenceEquals(m_mode, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_mode = value;
            }
        }

        /// <remarks />
        public PropertyState<uint> SendQueueSize
        {
            get
            {
                return m_sendQueueSize;
            }

            set
            {
                if (!Object.ReferenceEquals(m_sendQueueSize, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_sendQueueSize = value;
            }
        }

        /// <remarks />
        public PropertyState<uint> ReceiveQueueSize
        {
            get
            {
                return m_receiveQueueSize;
            }

            set
            {
                if (!Object.ReferenceEquals(m_receiveQueueSize, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_receiveQueueSize = value;
            }
        }

        /// <remarks />
        public PropertyState<uint> ConnectTimeout
        {
            get
            {
                return m_connectTimeout;
            }

            set
            {
                if (!Object.ReferenceEquals(m_connectTimeout, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_connectTimeout = value;
            }
        }

        /// <remarks />
        public PropertyState<uint> ConfirmationTimeout
        {
            get
            {
                return m_confirmationTimeout;
            }

            set
            {
                if (!Object.ReferenceEquals(m_confirmationTimeout, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_confirmationTimeout = value;
            }
        }

        /// <remarks />
        public PropertyState<uint> TerminationTimeout
        {
            get
            {
                return m_terminationTimeout;
            }

            set
            {
                if (!Object.ReferenceEquals(m_terminationTimeout, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_terminationTimeout = value;
            }
        }

        /// <remarks />
        public PropertyState<uint> DeviceAddressSize
        {
            get
            {
                return m_deviceAddressSize;
            }

            set
            {
                if (!Object.ReferenceEquals(m_deviceAddressSize, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_deviceAddressSize = value;
            }
        }

        /// <remarks />
        public PropertyState<uint> COTSize
        {
            get
            {
                return m_cOTSize;
            }

            set
            {
                if (!Object.ReferenceEquals(m_cOTSize, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_cOTSize = value;
            }
        }

        /// <remarks />
        public PropertyState<uint> InfoAddressSize
        {
            get
            {
                return m_infoAddressSize;
            }

            set
            {
                if (!Object.ReferenceEquals(m_infoAddressSize, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_infoAddressSize = value;
            }
        }

        /// <remarks />
        public PropertyState<bool> DataCollection
        {
            get
            {
                return m_dataCollection;
            }

            set
            {
                if (!Object.ReferenceEquals(m_dataCollection, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_dataCollection = value;
            }
        }

        /// <remarks />
        public PropertyState<uint> SendRetryCount
        {
            get
            {
                return m_sendRetryCount;
            }

            set
            {
                if (!Object.ReferenceEquals(m_sendRetryCount, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_sendRetryCount = value;
            }
        }

        /// <remarks />
        public PropertyState<bool> CRCProtection
        {
            get
            {
                return m_cRCProtection;
            }

            set
            {
                if (!Object.ReferenceEquals(m_cRCProtection, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_cRCProtection = value;
            }
        }

        /// <remarks />
        public PropertyState<uint> SendTimeout
        {
            get
            {
                return m_sendTimeout;
            }

            set
            {
                if (!Object.ReferenceEquals(m_sendTimeout, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_sendTimeout = value;
            }
        }

        /// <remarks />
        public PropertyState<uint> ReceiveTimeout
        {
            get
            {
                return m_receiveTimeout;
            }

            set
            {
                if (!Object.ReferenceEquals(m_receiveTimeout, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_receiveTimeout = value;
            }
        }

        /// <remarks />
        public PropertyState<uint> IdleTimeout
        {
            get
            {
                return m_idleTimeout;
            }

            set
            {
                if (!Object.ReferenceEquals(m_idleTimeout, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_idleTimeout = value;
            }
        }

        /// <remarks />
        public PropertyState<bool> AnonymousMode
        {
            get
            {
                return m_anonymousMode;
            }

            set
            {
                if (!Object.ReferenceEquals(m_anonymousMode, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_anonymousMode = value;
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
            if (m_protocol != null)
            {
                children.Add(m_protocol);
            }

            if (m_mode != null)
            {
                children.Add(m_mode);
            }

            if (m_sendQueueSize != null)
            {
                children.Add(m_sendQueueSize);
            }

            if (m_receiveQueueSize != null)
            {
                children.Add(m_receiveQueueSize);
            }

            if (m_connectTimeout != null)
            {
                children.Add(m_connectTimeout);
            }

            if (m_confirmationTimeout != null)
            {
                children.Add(m_confirmationTimeout);
            }

            if (m_terminationTimeout != null)
            {
                children.Add(m_terminationTimeout);
            }

            if (m_deviceAddressSize != null)
            {
                children.Add(m_deviceAddressSize);
            }

            if (m_cOTSize != null)
            {
                children.Add(m_cOTSize);
            }

            if (m_infoAddressSize != null)
            {
                children.Add(m_infoAddressSize);
            }

            if (m_dataCollection != null)
            {
                children.Add(m_dataCollection);
            }

            if (m_sendRetryCount != null)
            {
                children.Add(m_sendRetryCount);
            }

            if (m_cRCProtection != null)
            {
                children.Add(m_cRCProtection);
            }

            if (m_sendTimeout != null)
            {
                children.Add(m_sendTimeout);
            }

            if (m_receiveTimeout != null)
            {
                children.Add(m_receiveTimeout);
            }

            if (m_idleTimeout != null)
            {
                children.Add(m_idleTimeout);
            }

            if (m_anonymousMode != null)
            {
                children.Add(m_anonymousMode);
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
                case Telecontrol.Devices.BrowseNames.Protocol:
                {
                    if (createOrReplace)
                    {
                        if (Protocol == null)
                        {
                            if (replacement == null)
                            {
                                Protocol = new PropertyState<Iec60870ProtocolDataType>(this);
                            }
                            else
                            {
                                Protocol = (PropertyState<Iec60870ProtocolDataType>)replacement;
                            }
                        }
                    }

                    instance = Protocol;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.Mode:
                {
                    if (createOrReplace)
                    {
                        if (Mode == null)
                        {
                            if (replacement == null)
                            {
                                Mode = new PropertyState<uint>(this);
                            }
                            else
                            {
                                Mode = (PropertyState<uint>)replacement;
                            }
                        }
                    }

                    instance = Mode;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.SendQueueSize:
                {
                    if (createOrReplace)
                    {
                        if (SendQueueSize == null)
                        {
                            if (replacement == null)
                            {
                                SendQueueSize = new PropertyState<uint>(this);
                            }
                            else
                            {
                                SendQueueSize = (PropertyState<uint>)replacement;
                            }
                        }
                    }

                    instance = SendQueueSize;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.ReceiveQueueSize:
                {
                    if (createOrReplace)
                    {
                        if (ReceiveQueueSize == null)
                        {
                            if (replacement == null)
                            {
                                ReceiveQueueSize = new PropertyState<uint>(this);
                            }
                            else
                            {
                                ReceiveQueueSize = (PropertyState<uint>)replacement;
                            }
                        }
                    }

                    instance = ReceiveQueueSize;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.ConnectTimeout:
                {
                    if (createOrReplace)
                    {
                        if (ConnectTimeout == null)
                        {
                            if (replacement == null)
                            {
                                ConnectTimeout = new PropertyState<uint>(this);
                            }
                            else
                            {
                                ConnectTimeout = (PropertyState<uint>)replacement;
                            }
                        }
                    }

                    instance = ConnectTimeout;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.ConfirmationTimeout:
                {
                    if (createOrReplace)
                    {
                        if (ConfirmationTimeout == null)
                        {
                            if (replacement == null)
                            {
                                ConfirmationTimeout = new PropertyState<uint>(this);
                            }
                            else
                            {
                                ConfirmationTimeout = (PropertyState<uint>)replacement;
                            }
                        }
                    }

                    instance = ConfirmationTimeout;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.TerminationTimeout:
                {
                    if (createOrReplace)
                    {
                        if (TerminationTimeout == null)
                        {
                            if (replacement == null)
                            {
                                TerminationTimeout = new PropertyState<uint>(this);
                            }
                            else
                            {
                                TerminationTimeout = (PropertyState<uint>)replacement;
                            }
                        }
                    }

                    instance = TerminationTimeout;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.DeviceAddressSize:
                {
                    if (createOrReplace)
                    {
                        if (DeviceAddressSize == null)
                        {
                            if (replacement == null)
                            {
                                DeviceAddressSize = new PropertyState<uint>(this);
                            }
                            else
                            {
                                DeviceAddressSize = (PropertyState<uint>)replacement;
                            }
                        }
                    }

                    instance = DeviceAddressSize;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.COTSize:
                {
                    if (createOrReplace)
                    {
                        if (COTSize == null)
                        {
                            if (replacement == null)
                            {
                                COTSize = new PropertyState<uint>(this);
                            }
                            else
                            {
                                COTSize = (PropertyState<uint>)replacement;
                            }
                        }
                    }

                    instance = COTSize;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.InfoAddressSize:
                {
                    if (createOrReplace)
                    {
                        if (InfoAddressSize == null)
                        {
                            if (replacement == null)
                            {
                                InfoAddressSize = new PropertyState<uint>(this);
                            }
                            else
                            {
                                InfoAddressSize = (PropertyState<uint>)replacement;
                            }
                        }
                    }

                    instance = InfoAddressSize;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.DataCollection:
                {
                    if (createOrReplace)
                    {
                        if (DataCollection == null)
                        {
                            if (replacement == null)
                            {
                                DataCollection = new PropertyState<bool>(this);
                            }
                            else
                            {
                                DataCollection = (PropertyState<bool>)replacement;
                            }
                        }
                    }

                    instance = DataCollection;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.SendRetryCount:
                {
                    if (createOrReplace)
                    {
                        if (SendRetryCount == null)
                        {
                            if (replacement == null)
                            {
                                SendRetryCount = new PropertyState<uint>(this);
                            }
                            else
                            {
                                SendRetryCount = (PropertyState<uint>)replacement;
                            }
                        }
                    }

                    instance = SendRetryCount;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.CRCProtection:
                {
                    if (createOrReplace)
                    {
                        if (CRCProtection == null)
                        {
                            if (replacement == null)
                            {
                                CRCProtection = new PropertyState<bool>(this);
                            }
                            else
                            {
                                CRCProtection = (PropertyState<bool>)replacement;
                            }
                        }
                    }

                    instance = CRCProtection;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.SendTimeout:
                {
                    if (createOrReplace)
                    {
                        if (SendTimeout == null)
                        {
                            if (replacement == null)
                            {
                                SendTimeout = new PropertyState<uint>(this);
                            }
                            else
                            {
                                SendTimeout = (PropertyState<uint>)replacement;
                            }
                        }
                    }

                    instance = SendTimeout;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.ReceiveTimeout:
                {
                    if (createOrReplace)
                    {
                        if (ReceiveTimeout == null)
                        {
                            if (replacement == null)
                            {
                                ReceiveTimeout = new PropertyState<uint>(this);
                            }
                            else
                            {
                                ReceiveTimeout = (PropertyState<uint>)replacement;
                            }
                        }
                    }

                    instance = ReceiveTimeout;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.IdleTimeout:
                {
                    if (createOrReplace)
                    {
                        if (IdleTimeout == null)
                        {
                            if (replacement == null)
                            {
                                IdleTimeout = new PropertyState<uint>(this);
                            }
                            else
                            {
                                IdleTimeout = (PropertyState<uint>)replacement;
                            }
                        }
                    }

                    instance = IdleTimeout;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.AnonymousMode:
                {
                    if (createOrReplace)
                    {
                        if (AnonymousMode == null)
                        {
                            if (replacement == null)
                            {
                                AnonymousMode = new PropertyState<bool>(this);
                            }
                            else
                            {
                                AnonymousMode = (PropertyState<bool>)replacement;
                            }
                        }
                    }

                    instance = AnonymousMode;
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
        private PropertyState<Iec60870ProtocolDataType> m_protocol;
        private PropertyState<uint> m_mode;
        private PropertyState<uint> m_sendQueueSize;
        private PropertyState<uint> m_receiveQueueSize;
        private PropertyState<uint> m_connectTimeout;
        private PropertyState<uint> m_confirmationTimeout;
        private PropertyState<uint> m_terminationTimeout;
        private PropertyState<uint> m_deviceAddressSize;
        private PropertyState<uint> m_cOTSize;
        private PropertyState<uint> m_infoAddressSize;
        private PropertyState<bool> m_dataCollection;
        private PropertyState<uint> m_sendRetryCount;
        private PropertyState<bool> m_cRCProtection;
        private PropertyState<uint> m_sendTimeout;
        private PropertyState<uint> m_receiveTimeout;
        private PropertyState<uint> m_idleTimeout;
        private PropertyState<bool> m_anonymousMode;
        #endregion
    }
    #endif
    #endregion

    #region Iec60870DeviceState Class
    #if (!OPCUA_EXCLUDE_Iec60870DeviceState)
    /// <summary>
    /// Stores an instance of the Iec60870DeviceType ObjectType.
    /// </summary>
    /// <exclude />
    [System.CodeDom.Compiler.GeneratedCodeAttribute("Opc.Ua.ModelCompiler", "1.0.0.0")]
    public partial class Iec60870DeviceState : DeviceState
    {
        #region Constructors
        /// <summary>
        /// Initializes the type with its default attribute values.
        /// </summary>
        public Iec60870DeviceState(NodeState parent) : base(parent)
        {
        }

        /// <summary>
        /// Returns the id of the default type definition node for the instance.
        /// </summary>
        protected override NodeId GetDefaultTypeDefinitionId(NamespaceTable namespaceUris)
        {
            return Opc.Ua.NodeId.Create(Telecontrol.Devices.ObjectTypes.Iec60870DeviceType, Telecontrol.Devices.Namespaces.TelecontrolDevices, namespaceUris);
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
           "cm9sLnJ1L29wY3VhL2RldmljZXP/////BGCAAgEAAAACABoAAABJZWM2MDg3MERldmljZVR5cGVJbnN0" +
           "YW5jZQECSwEBAksBSwEAAAEAAAABARcAAQECKQEgAAAAFWCJCgIAAAACAAgAAABEaXNhYmxlZAECTAEA" +
           "LgBETAEAAAAB/////wEB/////wAAAAAVYIkKAgAAAAIABgAAAE9ubGluZQECTQEALwA/TQEAAAAB////" +
           "/wEB/////wAAAAAVYIkKAgAAAAIABwAAAEVuYWJsZWQBAk4BAC8AP04BAAAAAf////8BAf////8AAAAA" +
           "BGGCCgQAAAACAAsAAABJbnRlcnJvZ2F0ZQECTwEALwEC6wBPAQAAAQH/////AAAAAARhggoEAAAAAgAS" +
           "AAAASW50ZXJyb2dhdGVDaGFubmVsAQJQAQAvAQIWAVABAAABAf////8AAAAABGGCCgQAAAACAAkAAABT" +
           "eW5jQ2xvY2sBAlEBAC8BAuwAUQEAAAEB/////wAAAAAEYYIKBAAAAAIABgAAAFNlbGVjdAECUgEALwEC" +
           "AAFSAQAAAQH/////AAAAAARhggoEAAAAAgAFAAAAV3JpdGUBAlMBAC8BAgEBUwEAAAEB/////wAAAAAE" +
           "YYIKBAAAAAIACgAAAFdyaXRlUGFyYW0BAlQBAC8BAgIBVAEAAAEB/////wAAAAAVYKkKAgAAAAIABwAA" +
           "AEFkZHJlc3MBAlUBAC4ARFUBAAAHAQAAAAAH/////wEB/////wAAAAAVYKkKAgAAAAIACwAAAExpbmtB" +
           "ZGRyZXNzAQJWAQAuAERWAQAABwEAAAAAB/////8BAf////8AAAAAFWCpCgIAAAACABQAAABTdGFydHVw" +
           "SW50ZXJyb2dhdGlvbgECVwEALgBEVwEAAAEBAAH/////AQH/////AAAAABVgqQoCAAAAAgATAAAASW50" +
           "ZXJyb2dhdGlvblBlcmlvZAECWAEALgBEWAEAAAcAAAAAAAf/////AQH/////AAAAABVgqQoCAAAAAgAQ" +
           "AAAAU3RhcnR1cENsb2NrU3luYwECWQEALgBEWQEAAAEBAAH/////AQH/////AAAAABVgqQoCAAAAAgAP" +
           "AAAAQ2xvY2tTeW5jUGVyaW9kAQJaAQAuAERaAQAABwAAAAAAB/////8BAf////8AAAAAFWCJCgIAAAAC" +
           "AAcAAABVdGNUaW1lAQKiOgAuAESiOgAAAAH/////AQH/////AAAAABVgiQoCAAAAAgAZAAAASW50ZXJy" +
           "b2dhdGlvblBlcmlvZEdyb3VwMQECWwEALgBEWwEAAAAH/////wEB/////wAAAAAVYIkKAgAAAAIAGQAA" +
           "AEludGVycm9nYXRpb25QZXJpb2RHcm91cDIBAlwBAC4ARFwBAAAAB/////8BAf////8AAAAAFWCJCgIA" +
           "AAACABkAAABJbnRlcnJvZ2F0aW9uUGVyaW9kR3JvdXAzAQJdAQAuAERdAQAAAAf/////AQH/////AAAA" +
           "ABVgiQoCAAAAAgAZAAAASW50ZXJyb2dhdGlvblBlcmlvZEdyb3VwNAECXgEALgBEXgEAAAAH/////wEB" +
           "/////wAAAAAVYIkKAgAAAAIAGQAAAEludGVycm9nYXRpb25QZXJpb2RHcm91cDUBAl8BAC4ARF8BAAAA" +
           "B/////8BAf////8AAAAAFWCJCgIAAAACABkAAABJbnRlcnJvZ2F0aW9uUGVyaW9kR3JvdXA2AQJgAQAu" +
           "AERgAQAAAAf/////AQH/////AAAAABVgiQoCAAAAAgAZAAAASW50ZXJyb2dhdGlvblBlcmlvZEdyb3Vw" +
           "NwECYQEALgBEYQEAAAAH/////wEB/////wAAAAAVYIkKAgAAAAIAGQAAAEludGVycm9nYXRpb25QZXJp" +
           "b2RHcm91cDgBAmIBAC4ARGIBAAAAB/////8BAf////8AAAAAFWCJCgIAAAACABkAAABJbnRlcnJvZ2F0" +
           "aW9uUGVyaW9kR3JvdXA5AQJjAQAuAERjAQAAAAf/////AQH/////AAAAABVgiQoCAAAAAgAaAAAASW50" +
           "ZXJyb2dhdGlvblBlcmlvZEdyb3VwMTABAmQBAC4ARGQBAAAAB/////8BAf////8AAAAAFWCJCgIAAAAC" +
           "ABoAAABJbnRlcnJvZ2F0aW9uUGVyaW9kR3JvdXAxMQECZQEALgBEZQEAAAAH/////wEB/////wAAAAAV" +
           "YIkKAgAAAAIAGgAAAEludGVycm9nYXRpb25QZXJpb2RHcm91cDEyAQJmAQAuAERmAQAAAAf/////AQH/" +
           "////AAAAABVgiQoCAAAAAgAaAAAASW50ZXJyb2dhdGlvblBlcmlvZEdyb3VwMTMBAmcBAC4ARGcBAAAA" +
           "B/////8BAf////8AAAAAFWCJCgIAAAACABoAAABJbnRlcnJvZ2F0aW9uUGVyaW9kR3JvdXAxNAECaAEA" +
           "LgBEaAEAAAAH/////wEB/////wAAAAAVYIkKAgAAAAIAGgAAAEludGVycm9nYXRpb25QZXJpb2RHcm91" +
           "cDE1AQJpAQAuAERpAQAAAAf/////AQH/////AAAAABVgiQoCAAAAAgAaAAAASW50ZXJyb2dhdGlvblBl" +
           "cmlvZEdyb3VwMTYBAmoBAC4ARGoBAAAAB/////8BAf////8AAAAA";
        #endregion
        #endif
        #endregion

        #region Public Properties
        /// <remarks />
        public PropertyState<uint> Address
        {
            get
            {
                return m_address;
            }

            set
            {
                if (!Object.ReferenceEquals(m_address, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_address = value;
            }
        }

        /// <remarks />
        public PropertyState<uint> LinkAddress
        {
            get
            {
                return m_linkAddress;
            }

            set
            {
                if (!Object.ReferenceEquals(m_linkAddress, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_linkAddress = value;
            }
        }

        /// <remarks />
        public PropertyState<bool> StartupInterrogation
        {
            get
            {
                return m_startupInterrogation;
            }

            set
            {
                if (!Object.ReferenceEquals(m_startupInterrogation, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_startupInterrogation = value;
            }
        }

        /// <remarks />
        public PropertyState<uint> InterrogationPeriod
        {
            get
            {
                return m_interrogationPeriod;
            }

            set
            {
                if (!Object.ReferenceEquals(m_interrogationPeriod, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_interrogationPeriod = value;
            }
        }

        /// <remarks />
        public PropertyState<bool> StartupClockSync
        {
            get
            {
                return m_startupClockSync;
            }

            set
            {
                if (!Object.ReferenceEquals(m_startupClockSync, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_startupClockSync = value;
            }
        }

        /// <remarks />
        public PropertyState<uint> ClockSyncPeriod
        {
            get
            {
                return m_clockSyncPeriod;
            }

            set
            {
                if (!Object.ReferenceEquals(m_clockSyncPeriod, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_clockSyncPeriod = value;
            }
        }

        /// <remarks />
        public PropertyState<bool> UtcTime
        {
            get
            {
                return m_utcTime;
            }

            set
            {
                if (!Object.ReferenceEquals(m_utcTime, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_utcTime = value;
            }
        }

        /// <remarks />
        public PropertyState<uint> InterrogationPeriodGroup1
        {
            get
            {
                return m_interrogationPeriodGroup1;
            }

            set
            {
                if (!Object.ReferenceEquals(m_interrogationPeriodGroup1, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_interrogationPeriodGroup1 = value;
            }
        }

        /// <remarks />
        public PropertyState<uint> InterrogationPeriodGroup2
        {
            get
            {
                return m_interrogationPeriodGroup2;
            }

            set
            {
                if (!Object.ReferenceEquals(m_interrogationPeriodGroup2, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_interrogationPeriodGroup2 = value;
            }
        }

        /// <remarks />
        public PropertyState<uint> InterrogationPeriodGroup3
        {
            get
            {
                return m_interrogationPeriodGroup3;
            }

            set
            {
                if (!Object.ReferenceEquals(m_interrogationPeriodGroup3, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_interrogationPeriodGroup3 = value;
            }
        }

        /// <remarks />
        public PropertyState<uint> InterrogationPeriodGroup4
        {
            get
            {
                return m_interrogationPeriodGroup4;
            }

            set
            {
                if (!Object.ReferenceEquals(m_interrogationPeriodGroup4, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_interrogationPeriodGroup4 = value;
            }
        }

        /// <remarks />
        public PropertyState<uint> InterrogationPeriodGroup5
        {
            get
            {
                return m_interrogationPeriodGroup5;
            }

            set
            {
                if (!Object.ReferenceEquals(m_interrogationPeriodGroup5, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_interrogationPeriodGroup5 = value;
            }
        }

        /// <remarks />
        public PropertyState<uint> InterrogationPeriodGroup6
        {
            get
            {
                return m_interrogationPeriodGroup6;
            }

            set
            {
                if (!Object.ReferenceEquals(m_interrogationPeriodGroup6, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_interrogationPeriodGroup6 = value;
            }
        }

        /// <remarks />
        public PropertyState<uint> InterrogationPeriodGroup7
        {
            get
            {
                return m_interrogationPeriodGroup7;
            }

            set
            {
                if (!Object.ReferenceEquals(m_interrogationPeriodGroup7, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_interrogationPeriodGroup7 = value;
            }
        }

        /// <remarks />
        public PropertyState<uint> InterrogationPeriodGroup8
        {
            get
            {
                return m_interrogationPeriodGroup8;
            }

            set
            {
                if (!Object.ReferenceEquals(m_interrogationPeriodGroup8, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_interrogationPeriodGroup8 = value;
            }
        }

        /// <remarks />
        public PropertyState<uint> InterrogationPeriodGroup9
        {
            get
            {
                return m_interrogationPeriodGroup9;
            }

            set
            {
                if (!Object.ReferenceEquals(m_interrogationPeriodGroup9, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_interrogationPeriodGroup9 = value;
            }
        }

        /// <remarks />
        public PropertyState<uint> InterrogationPeriodGroup10
        {
            get
            {
                return m_interrogationPeriodGroup10;
            }

            set
            {
                if (!Object.ReferenceEquals(m_interrogationPeriodGroup10, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_interrogationPeriodGroup10 = value;
            }
        }

        /// <remarks />
        public PropertyState<uint> InterrogationPeriodGroup11
        {
            get
            {
                return m_interrogationPeriodGroup11;
            }

            set
            {
                if (!Object.ReferenceEquals(m_interrogationPeriodGroup11, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_interrogationPeriodGroup11 = value;
            }
        }

        /// <remarks />
        public PropertyState<uint> InterrogationPeriodGroup12
        {
            get
            {
                return m_interrogationPeriodGroup12;
            }

            set
            {
                if (!Object.ReferenceEquals(m_interrogationPeriodGroup12, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_interrogationPeriodGroup12 = value;
            }
        }

        /// <remarks />
        public PropertyState<uint> InterrogationPeriodGroup13
        {
            get
            {
                return m_interrogationPeriodGroup13;
            }

            set
            {
                if (!Object.ReferenceEquals(m_interrogationPeriodGroup13, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_interrogationPeriodGroup13 = value;
            }
        }

        /// <remarks />
        public PropertyState<uint> InterrogationPeriodGroup14
        {
            get
            {
                return m_interrogationPeriodGroup14;
            }

            set
            {
                if (!Object.ReferenceEquals(m_interrogationPeriodGroup14, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_interrogationPeriodGroup14 = value;
            }
        }

        /// <remarks />
        public PropertyState<uint> InterrogationPeriodGroup15
        {
            get
            {
                return m_interrogationPeriodGroup15;
            }

            set
            {
                if (!Object.ReferenceEquals(m_interrogationPeriodGroup15, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_interrogationPeriodGroup15 = value;
            }
        }

        /// <remarks />
        public PropertyState<uint> InterrogationPeriodGroup16
        {
            get
            {
                return m_interrogationPeriodGroup16;
            }

            set
            {
                if (!Object.ReferenceEquals(m_interrogationPeriodGroup16, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_interrogationPeriodGroup16 = value;
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
            if (m_address != null)
            {
                children.Add(m_address);
            }

            if (m_linkAddress != null)
            {
                children.Add(m_linkAddress);
            }

            if (m_startupInterrogation != null)
            {
                children.Add(m_startupInterrogation);
            }

            if (m_interrogationPeriod != null)
            {
                children.Add(m_interrogationPeriod);
            }

            if (m_startupClockSync != null)
            {
                children.Add(m_startupClockSync);
            }

            if (m_clockSyncPeriod != null)
            {
                children.Add(m_clockSyncPeriod);
            }

            if (m_utcTime != null)
            {
                children.Add(m_utcTime);
            }

            if (m_interrogationPeriodGroup1 != null)
            {
                children.Add(m_interrogationPeriodGroup1);
            }

            if (m_interrogationPeriodGroup2 != null)
            {
                children.Add(m_interrogationPeriodGroup2);
            }

            if (m_interrogationPeriodGroup3 != null)
            {
                children.Add(m_interrogationPeriodGroup3);
            }

            if (m_interrogationPeriodGroup4 != null)
            {
                children.Add(m_interrogationPeriodGroup4);
            }

            if (m_interrogationPeriodGroup5 != null)
            {
                children.Add(m_interrogationPeriodGroup5);
            }

            if (m_interrogationPeriodGroup6 != null)
            {
                children.Add(m_interrogationPeriodGroup6);
            }

            if (m_interrogationPeriodGroup7 != null)
            {
                children.Add(m_interrogationPeriodGroup7);
            }

            if (m_interrogationPeriodGroup8 != null)
            {
                children.Add(m_interrogationPeriodGroup8);
            }

            if (m_interrogationPeriodGroup9 != null)
            {
                children.Add(m_interrogationPeriodGroup9);
            }

            if (m_interrogationPeriodGroup10 != null)
            {
                children.Add(m_interrogationPeriodGroup10);
            }

            if (m_interrogationPeriodGroup11 != null)
            {
                children.Add(m_interrogationPeriodGroup11);
            }

            if (m_interrogationPeriodGroup12 != null)
            {
                children.Add(m_interrogationPeriodGroup12);
            }

            if (m_interrogationPeriodGroup13 != null)
            {
                children.Add(m_interrogationPeriodGroup13);
            }

            if (m_interrogationPeriodGroup14 != null)
            {
                children.Add(m_interrogationPeriodGroup14);
            }

            if (m_interrogationPeriodGroup15 != null)
            {
                children.Add(m_interrogationPeriodGroup15);
            }

            if (m_interrogationPeriodGroup16 != null)
            {
                children.Add(m_interrogationPeriodGroup16);
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
                case Telecontrol.Devices.BrowseNames.Address:
                {
                    if (createOrReplace)
                    {
                        if (Address == null)
                        {
                            if (replacement == null)
                            {
                                Address = new PropertyState<uint>(this);
                            }
                            else
                            {
                                Address = (PropertyState<uint>)replacement;
                            }
                        }
                    }

                    instance = Address;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.LinkAddress:
                {
                    if (createOrReplace)
                    {
                        if (LinkAddress == null)
                        {
                            if (replacement == null)
                            {
                                LinkAddress = new PropertyState<uint>(this);
                            }
                            else
                            {
                                LinkAddress = (PropertyState<uint>)replacement;
                            }
                        }
                    }

                    instance = LinkAddress;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.StartupInterrogation:
                {
                    if (createOrReplace)
                    {
                        if (StartupInterrogation == null)
                        {
                            if (replacement == null)
                            {
                                StartupInterrogation = new PropertyState<bool>(this);
                            }
                            else
                            {
                                StartupInterrogation = (PropertyState<bool>)replacement;
                            }
                        }
                    }

                    instance = StartupInterrogation;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.InterrogationPeriod:
                {
                    if (createOrReplace)
                    {
                        if (InterrogationPeriod == null)
                        {
                            if (replacement == null)
                            {
                                InterrogationPeriod = new PropertyState<uint>(this);
                            }
                            else
                            {
                                InterrogationPeriod = (PropertyState<uint>)replacement;
                            }
                        }
                    }

                    instance = InterrogationPeriod;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.StartupClockSync:
                {
                    if (createOrReplace)
                    {
                        if (StartupClockSync == null)
                        {
                            if (replacement == null)
                            {
                                StartupClockSync = new PropertyState<bool>(this);
                            }
                            else
                            {
                                StartupClockSync = (PropertyState<bool>)replacement;
                            }
                        }
                    }

                    instance = StartupClockSync;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.ClockSyncPeriod:
                {
                    if (createOrReplace)
                    {
                        if (ClockSyncPeriod == null)
                        {
                            if (replacement == null)
                            {
                                ClockSyncPeriod = new PropertyState<uint>(this);
                            }
                            else
                            {
                                ClockSyncPeriod = (PropertyState<uint>)replacement;
                            }
                        }
                    }

                    instance = ClockSyncPeriod;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.UtcTime:
                {
                    if (createOrReplace)
                    {
                        if (UtcTime == null)
                        {
                            if (replacement == null)
                            {
                                UtcTime = new PropertyState<bool>(this);
                            }
                            else
                            {
                                UtcTime = (PropertyState<bool>)replacement;
                            }
                        }
                    }

                    instance = UtcTime;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.InterrogationPeriodGroup1:
                {
                    if (createOrReplace)
                    {
                        if (InterrogationPeriodGroup1 == null)
                        {
                            if (replacement == null)
                            {
                                InterrogationPeriodGroup1 = new PropertyState<uint>(this);
                            }
                            else
                            {
                                InterrogationPeriodGroup1 = (PropertyState<uint>)replacement;
                            }
                        }
                    }

                    instance = InterrogationPeriodGroup1;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.InterrogationPeriodGroup2:
                {
                    if (createOrReplace)
                    {
                        if (InterrogationPeriodGroup2 == null)
                        {
                            if (replacement == null)
                            {
                                InterrogationPeriodGroup2 = new PropertyState<uint>(this);
                            }
                            else
                            {
                                InterrogationPeriodGroup2 = (PropertyState<uint>)replacement;
                            }
                        }
                    }

                    instance = InterrogationPeriodGroup2;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.InterrogationPeriodGroup3:
                {
                    if (createOrReplace)
                    {
                        if (InterrogationPeriodGroup3 == null)
                        {
                            if (replacement == null)
                            {
                                InterrogationPeriodGroup3 = new PropertyState<uint>(this);
                            }
                            else
                            {
                                InterrogationPeriodGroup3 = (PropertyState<uint>)replacement;
                            }
                        }
                    }

                    instance = InterrogationPeriodGroup3;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.InterrogationPeriodGroup4:
                {
                    if (createOrReplace)
                    {
                        if (InterrogationPeriodGroup4 == null)
                        {
                            if (replacement == null)
                            {
                                InterrogationPeriodGroup4 = new PropertyState<uint>(this);
                            }
                            else
                            {
                                InterrogationPeriodGroup4 = (PropertyState<uint>)replacement;
                            }
                        }
                    }

                    instance = InterrogationPeriodGroup4;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.InterrogationPeriodGroup5:
                {
                    if (createOrReplace)
                    {
                        if (InterrogationPeriodGroup5 == null)
                        {
                            if (replacement == null)
                            {
                                InterrogationPeriodGroup5 = new PropertyState<uint>(this);
                            }
                            else
                            {
                                InterrogationPeriodGroup5 = (PropertyState<uint>)replacement;
                            }
                        }
                    }

                    instance = InterrogationPeriodGroup5;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.InterrogationPeriodGroup6:
                {
                    if (createOrReplace)
                    {
                        if (InterrogationPeriodGroup6 == null)
                        {
                            if (replacement == null)
                            {
                                InterrogationPeriodGroup6 = new PropertyState<uint>(this);
                            }
                            else
                            {
                                InterrogationPeriodGroup6 = (PropertyState<uint>)replacement;
                            }
                        }
                    }

                    instance = InterrogationPeriodGroup6;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.InterrogationPeriodGroup7:
                {
                    if (createOrReplace)
                    {
                        if (InterrogationPeriodGroup7 == null)
                        {
                            if (replacement == null)
                            {
                                InterrogationPeriodGroup7 = new PropertyState<uint>(this);
                            }
                            else
                            {
                                InterrogationPeriodGroup7 = (PropertyState<uint>)replacement;
                            }
                        }
                    }

                    instance = InterrogationPeriodGroup7;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.InterrogationPeriodGroup8:
                {
                    if (createOrReplace)
                    {
                        if (InterrogationPeriodGroup8 == null)
                        {
                            if (replacement == null)
                            {
                                InterrogationPeriodGroup8 = new PropertyState<uint>(this);
                            }
                            else
                            {
                                InterrogationPeriodGroup8 = (PropertyState<uint>)replacement;
                            }
                        }
                    }

                    instance = InterrogationPeriodGroup8;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.InterrogationPeriodGroup9:
                {
                    if (createOrReplace)
                    {
                        if (InterrogationPeriodGroup9 == null)
                        {
                            if (replacement == null)
                            {
                                InterrogationPeriodGroup9 = new PropertyState<uint>(this);
                            }
                            else
                            {
                                InterrogationPeriodGroup9 = (PropertyState<uint>)replacement;
                            }
                        }
                    }

                    instance = InterrogationPeriodGroup9;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.InterrogationPeriodGroup10:
                {
                    if (createOrReplace)
                    {
                        if (InterrogationPeriodGroup10 == null)
                        {
                            if (replacement == null)
                            {
                                InterrogationPeriodGroup10 = new PropertyState<uint>(this);
                            }
                            else
                            {
                                InterrogationPeriodGroup10 = (PropertyState<uint>)replacement;
                            }
                        }
                    }

                    instance = InterrogationPeriodGroup10;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.InterrogationPeriodGroup11:
                {
                    if (createOrReplace)
                    {
                        if (InterrogationPeriodGroup11 == null)
                        {
                            if (replacement == null)
                            {
                                InterrogationPeriodGroup11 = new PropertyState<uint>(this);
                            }
                            else
                            {
                                InterrogationPeriodGroup11 = (PropertyState<uint>)replacement;
                            }
                        }
                    }

                    instance = InterrogationPeriodGroup11;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.InterrogationPeriodGroup12:
                {
                    if (createOrReplace)
                    {
                        if (InterrogationPeriodGroup12 == null)
                        {
                            if (replacement == null)
                            {
                                InterrogationPeriodGroup12 = new PropertyState<uint>(this);
                            }
                            else
                            {
                                InterrogationPeriodGroup12 = (PropertyState<uint>)replacement;
                            }
                        }
                    }

                    instance = InterrogationPeriodGroup12;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.InterrogationPeriodGroup13:
                {
                    if (createOrReplace)
                    {
                        if (InterrogationPeriodGroup13 == null)
                        {
                            if (replacement == null)
                            {
                                InterrogationPeriodGroup13 = new PropertyState<uint>(this);
                            }
                            else
                            {
                                InterrogationPeriodGroup13 = (PropertyState<uint>)replacement;
                            }
                        }
                    }

                    instance = InterrogationPeriodGroup13;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.InterrogationPeriodGroup14:
                {
                    if (createOrReplace)
                    {
                        if (InterrogationPeriodGroup14 == null)
                        {
                            if (replacement == null)
                            {
                                InterrogationPeriodGroup14 = new PropertyState<uint>(this);
                            }
                            else
                            {
                                InterrogationPeriodGroup14 = (PropertyState<uint>)replacement;
                            }
                        }
                    }

                    instance = InterrogationPeriodGroup14;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.InterrogationPeriodGroup15:
                {
                    if (createOrReplace)
                    {
                        if (InterrogationPeriodGroup15 == null)
                        {
                            if (replacement == null)
                            {
                                InterrogationPeriodGroup15 = new PropertyState<uint>(this);
                            }
                            else
                            {
                                InterrogationPeriodGroup15 = (PropertyState<uint>)replacement;
                            }
                        }
                    }

                    instance = InterrogationPeriodGroup15;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.InterrogationPeriodGroup16:
                {
                    if (createOrReplace)
                    {
                        if (InterrogationPeriodGroup16 == null)
                        {
                            if (replacement == null)
                            {
                                InterrogationPeriodGroup16 = new PropertyState<uint>(this);
                            }
                            else
                            {
                                InterrogationPeriodGroup16 = (PropertyState<uint>)replacement;
                            }
                        }
                    }

                    instance = InterrogationPeriodGroup16;
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
        private PropertyState<uint> m_address;
        private PropertyState<uint> m_linkAddress;
        private PropertyState<bool> m_startupInterrogation;
        private PropertyState<uint> m_interrogationPeriod;
        private PropertyState<bool> m_startupClockSync;
        private PropertyState<uint> m_clockSyncPeriod;
        private PropertyState<bool> m_utcTime;
        private PropertyState<uint> m_interrogationPeriodGroup1;
        private PropertyState<uint> m_interrogationPeriodGroup2;
        private PropertyState<uint> m_interrogationPeriodGroup3;
        private PropertyState<uint> m_interrogationPeriodGroup4;
        private PropertyState<uint> m_interrogationPeriodGroup5;
        private PropertyState<uint> m_interrogationPeriodGroup6;
        private PropertyState<uint> m_interrogationPeriodGroup7;
        private PropertyState<uint> m_interrogationPeriodGroup8;
        private PropertyState<uint> m_interrogationPeriodGroup9;
        private PropertyState<uint> m_interrogationPeriodGroup10;
        private PropertyState<uint> m_interrogationPeriodGroup11;
        private PropertyState<uint> m_interrogationPeriodGroup12;
        private PropertyState<uint> m_interrogationPeriodGroup13;
        private PropertyState<uint> m_interrogationPeriodGroup14;
        private PropertyState<uint> m_interrogationPeriodGroup15;
        private PropertyState<uint> m_interrogationPeriodGroup16;
        #endregion
    }
    #endif
    #endregion

    #region Iec61850LogicalNodeState Class
    #if (!OPCUA_EXCLUDE_Iec61850LogicalNodeState)
    /// <summary>
    /// Stores an instance of the Iec61850LogicalNodeType ObjectType.
    /// </summary>
    /// <exclude />
    [System.CodeDom.Compiler.GeneratedCodeAttribute("Opc.Ua.ModelCompiler", "1.0.0.0")]
    public partial class Iec61850LogicalNodeState : BaseObjectState
    {
        #region Constructors
        /// <summary>
        /// Initializes the type with its default attribute values.
        /// </summary>
        public Iec61850LogicalNodeState(NodeState parent) : base(parent)
        {
        }

        /// <summary>
        /// Returns the id of the default type definition node for the instance.
        /// </summary>
        protected override NodeId GetDefaultTypeDefinitionId(NamespaceTable namespaceUris)
        {
            return Opc.Ua.NodeId.Create(Telecontrol.Devices.ObjectTypes.Iec61850LogicalNodeType, Telecontrol.Devices.Namespaces.TelecontrolDevices, namespaceUris);
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
           "cm9sLnJ1L29wY3VhL2RldmljZXP/////BGCAAgEAAAACAB8AAABJZWM2MTg1MExvZ2ljYWxOb2RlVHlw" +
           "ZUluc3RhbmNlAQJyAQECcgFyAQAA/////wAAAAA=";
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

    #region Iec61850DataVariableState Class
    #if (!OPCUA_EXCLUDE_Iec61850DataVariableState)
    /// <summary>
    /// Stores an instance of the Iec61850DataVariableType VariableType.
    /// </summary>
    /// <exclude />
    [System.CodeDom.Compiler.GeneratedCodeAttribute("Opc.Ua.ModelCompiler", "1.0.0.0")]
    public partial class Iec61850DataVariableState : BaseVariableState
    {
        #region Constructors
        /// <summary>
        /// Initializes the type with its default attribute values.
        /// </summary>
        public Iec61850DataVariableState(NodeState parent) : base(parent)
        {
        }

        /// <summary>
        /// Returns the id of the default type definition node for the instance.
        /// </summary>
        protected override NodeId GetDefaultTypeDefinitionId(NamespaceTable namespaceUris)
        {
            return Opc.Ua.NodeId.Create(Telecontrol.Devices.VariableTypes.Iec61850DataVariableType, Telecontrol.Devices.Namespaces.TelecontrolDevices, namespaceUris);
        }

        /// <summary>
        /// Returns the id of the default data type node for the instance.
        /// </summary>
        protected override NodeId GetDefaultDataTypeId(NamespaceTable namespaceUris)
        {
            return Opc.Ua.NodeId.Create(Opc.Ua.DataTypes.BaseDataType, Opc.Ua.Namespaces.OpcUa, namespaceUris);
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
           "AgAAACEAAABodHRwOi8vdGVsZWNvbnRyb2wucnUvb3BjdWEvc2NhZGEjAAAAaHR0cDovL3RlbGVjb250" +
           "cm9sLnJ1L29wY3VhL2RldmljZXP/////FWCJAgIAAAACACAAAABJZWM2MTg1MERhdGFWYXJpYWJsZVR5" +
           "cGVJbnN0YW5jZQECozoBAqM6ozoAAAAY/v///wEB/////wAAAAA=";
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

    #region Iec61850DataVariableState<T> Class
    /// <summary>
    /// A typed version of the Iec61850DataVariableType variable.
    /// </summary>
    /// <exclude />
    [System.CodeDom.Compiler.GeneratedCodeAttribute("Opc.Ua.ModelCompiler", "1.0.0.0")]
    public class Iec61850DataVariableState<T> : Iec61850DataVariableState
    {
        #region Constructors
        /// <summary>
        /// Initializes the instance with its defalt attribute values.
        /// </summary>
        public Iec61850DataVariableState(NodeState parent) : base(parent)
        {
            Value = default(T);
        }

        /// <summary>
        /// Initializes the instance with the default values.
        /// </summary>
        protected override void Initialize(ISystemContext context)
        {
            base.Initialize(context);

            Value = default(T);
            DataType = TypeInfo.GetDataTypeId(typeof(T));
            ValueRank = TypeInfo.GetValueRank(typeof(T));
        }

        /// <summary>
        /// Initializes the instance with a node.
        /// </summary>
        protected override void Initialize(ISystemContext context, NodeState source)
        {
            InitializeOptionalChildren(context);
            base.Initialize(context, source);
        }
        #endregion

        #region Public Members
        /// <summary>
        /// The value of the variable.
        /// </summary>
        public new T Value
        {
            get
            {
                return CheckTypeBeforeCast<T>(base.Value, true);
            }

            set
            {
                base.Value = value;
            }
        }
        #endregion
    }
    #endregion
    #endif
    #endregion

    #region Iec61850ControlObjectState Class
    #if (!OPCUA_EXCLUDE_Iec61850ControlObjectState)
    /// <summary>
    /// Stores an instance of the Iec61850ControlObjectType ObjectType.
    /// </summary>
    /// <exclude />
    [System.CodeDom.Compiler.GeneratedCodeAttribute("Opc.Ua.ModelCompiler", "1.0.0.0")]
    public partial class Iec61850ControlObjectState : BaseObjectState
    {
        #region Constructors
        /// <summary>
        /// Initializes the type with its default attribute values.
        /// </summary>
        public Iec61850ControlObjectState(NodeState parent) : base(parent)
        {
        }

        /// <summary>
        /// Returns the id of the default type definition node for the instance.
        /// </summary>
        protected override NodeId GetDefaultTypeDefinitionId(NamespaceTable namespaceUris)
        {
            return Opc.Ua.NodeId.Create(Telecontrol.Devices.ObjectTypes.Iec61850ControlObjectType, Telecontrol.Devices.Namespaces.TelecontrolDevices, namespaceUris);
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
           "cm9sLnJ1L29wY3VhL2RldmljZXP/////BGCAAgEAAAACACEAAABJZWM2MTg1MENvbnRyb2xPYmplY3RU" +
           "eXBlSW5zdGFuY2UBAqQ6AQKkOqQ6AAD/////AAAAAA==";
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

    #region Iec61850DeviceState Class
    #if (!OPCUA_EXCLUDE_Iec61850DeviceState)
    /// <summary>
    /// Stores an instance of the Iec61850DeviceType ObjectType.
    /// </summary>
    /// <exclude />
    [System.CodeDom.Compiler.GeneratedCodeAttribute("Opc.Ua.ModelCompiler", "1.0.0.0")]
    public partial class Iec61850DeviceState : DeviceState
    {
        #region Constructors
        /// <summary>
        /// Initializes the type with its default attribute values.
        /// </summary>
        public Iec61850DeviceState(NodeState parent) : base(parent)
        {
        }

        /// <summary>
        /// Returns the id of the default type definition node for the instance.
        /// </summary>
        protected override NodeId GetDefaultTypeDefinitionId(NamespaceTable namespaceUris)
        {
            return Opc.Ua.NodeId.Create(Telecontrol.Devices.ObjectTypes.Iec61850DeviceType, Telecontrol.Devices.Namespaces.TelecontrolDevices, namespaceUris);
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
           "cm9sLnJ1L29wY3VhL2RldmljZXP/////BGCAAgEAAAACABoAAABJZWM2MTg1MERldmljZVR5cGVJbnN0" +
           "YW5jZQECcwEBAnMBcwEAAAEAAAABARcAAQECHgAMAAAAFWCJCgIAAAACAAgAAABEaXNhYmxlZAECdAEA" +
           "LgBEdAEAAAAB/////wEB/////wAAAAAVYIkKAgAAAAIABgAAAE9ubGluZQECdQEALwA/dQEAAAAB////" +
           "/wEB/////wAAAAAVYIkKAgAAAAIABwAAAEVuYWJsZWQBAnYBAC8AP3YBAAAAAf////8BAf////8AAAAA" +
           "BGGCCgQAAAACAAsAAABJbnRlcnJvZ2F0ZQECdwEALwEC6wB3AQAAAQH/////AAAAAARhggoEAAAAAgAS" +
           "AAAASW50ZXJyb2dhdGVDaGFubmVsAQJ4AQAvAQIWAXgBAAABAf////8AAAAABGGCCgQAAAACAAkAAABT" +
           "eW5jQ2xvY2sBAnkBAC8BAuwAeQEAAAEB/////wAAAAAEYYIKBAAAAAIABgAAAFNlbGVjdAECegEALwEC" +
           "AAF6AQAAAQH/////AAAAAARhggoEAAAAAgAFAAAAV3JpdGUBAnsBAC8BAgEBewEAAAEB/////wAAAAAE" +
           "YYIKBAAAAAIACgAAAFdyaXRlUGFyYW0BAnwBAC8BAgIBfAEAAAEB/////wAAAAAEYIAKAQAAAAIABQAA" +
           "AE1vZGVsAQJ9AQAvAQJyAX0BAAD/////AAAAABVgiQoCAAAAAgAEAAAASG9zdAECfgEALgBEfgEAAAAM" +
           "/////wEB/////wAAAAAVYKkKAgAAAAIABAAAAFBvcnQBAn8BAC4ARH8BAAAFZgAABf////8BAf////8A" +
           "AAAA";
        #endregion
        #endif
        #endregion

        #region Public Properties
        /// <remarks />
        public Iec61850LogicalNodeState Model
        {
            get
            {
                return m_model;
            }

            set
            {
                if (!Object.ReferenceEquals(m_model, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_model = value;
            }
        }

        /// <remarks />
        public PropertyState<string> Host
        {
            get
            {
                return m_host;
            }

            set
            {
                if (!Object.ReferenceEquals(m_host, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_host = value;
            }
        }

        /// <remarks />
        public PropertyState<ushort> Port
        {
            get
            {
                return m_port;
            }

            set
            {
                if (!Object.ReferenceEquals(m_port, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_port = value;
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
            if (m_model != null)
            {
                children.Add(m_model);
            }

            if (m_host != null)
            {
                children.Add(m_host);
            }

            if (m_port != null)
            {
                children.Add(m_port);
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
                case Telecontrol.Devices.BrowseNames.Model:
                {
                    if (createOrReplace)
                    {
                        if (Model == null)
                        {
                            if (replacement == null)
                            {
                                Model = new Iec61850LogicalNodeState(this);
                            }
                            else
                            {
                                Model = (Iec61850LogicalNodeState)replacement;
                            }
                        }
                    }

                    instance = Model;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.Host:
                {
                    if (createOrReplace)
                    {
                        if (Host == null)
                        {
                            if (replacement == null)
                            {
                                Host = new PropertyState<string>(this);
                            }
                            else
                            {
                                Host = (PropertyState<string>)replacement;
                            }
                        }
                    }

                    instance = Host;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.Port:
                {
                    if (createOrReplace)
                    {
                        if (Port == null)
                        {
                            if (replacement == null)
                            {
                                Port = new PropertyState<ushort>(this);
                            }
                            else
                            {
                                Port = (PropertyState<ushort>)replacement;
                            }
                        }
                    }

                    instance = Port;
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
        private Iec61850LogicalNodeState m_model;
        private PropertyState<string> m_host;
        private PropertyState<ushort> m_port;
        #endregion
    }
    #endif
    #endregion

    #region Iec61850ConfigurableObjectState Class
    #if (!OPCUA_EXCLUDE_Iec61850ConfigurableObjectState)
    /// <summary>
    /// Stores an instance of the Iec61850ConfigurableObjectType ObjectType.
    /// </summary>
    /// <exclude />
    [System.CodeDom.Compiler.GeneratedCodeAttribute("Opc.Ua.ModelCompiler", "1.0.0.0")]
    public partial class Iec61850ConfigurableObjectState : BaseObjectState
    {
        #region Constructors
        /// <summary>
        /// Initializes the type with its default attribute values.
        /// </summary>
        public Iec61850ConfigurableObjectState(NodeState parent) : base(parent)
        {
        }

        /// <summary>
        /// Returns the id of the default type definition node for the instance.
        /// </summary>
        protected override NodeId GetDefaultTypeDefinitionId(NamespaceTable namespaceUris)
        {
            return Opc.Ua.NodeId.Create(Telecontrol.Devices.ObjectTypes.Iec61850ConfigurableObjectType, Telecontrol.Devices.Namespaces.TelecontrolDevices, namespaceUris);
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
           "cm9sLnJ1L29wY3VhL2RldmljZXP/////BGCAAgEAAAACACYAAABJZWM2MTg1MENvbmZpZ3VyYWJsZU9i" +
           "amVjdFR5cGVJbnN0YW5jZQECgAEBAoABgAEAAP////8BAAAAFWCJCgIAAAACAAkAAABSZWZlcmVuY2UB" +
           "AoEBAC4ARIEBAAAADP////8BAf////8AAAAA";
        #endregion
        #endif
        #endregion

        #region Public Properties
        /// <remarks />
        public PropertyState<string> Reference
        {
            get
            {
                return m_reference;
            }

            set
            {
                if (!Object.ReferenceEquals(m_reference, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_reference = value;
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
            if (m_reference != null)
            {
                children.Add(m_reference);
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
                case Telecontrol.Devices.BrowseNames.Reference:
                {
                    if (createOrReplace)
                    {
                        if (Reference == null)
                        {
                            if (replacement == null)
                            {
                                Reference = new PropertyState<string>(this);
                            }
                            else
                            {
                                Reference = (PropertyState<string>)replacement;
                            }
                        }
                    }

                    instance = Reference;
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
        private PropertyState<string> m_reference;
        #endregion
    }
    #endif
    #endregion

    #region Iec61850RcbState Class
    #if (!OPCUA_EXCLUDE_Iec61850RcbState)
    /// <summary>
    /// Stores an instance of the Iec61850RcbType ObjectType.
    /// </summary>
    /// <exclude />
    [System.CodeDom.Compiler.GeneratedCodeAttribute("Opc.Ua.ModelCompiler", "1.0.0.0")]
    public partial class Iec61850RcbState : Iec61850ConfigurableObjectState
    {
        #region Constructors
        /// <summary>
        /// Initializes the type with its default attribute values.
        /// </summary>
        public Iec61850RcbState(NodeState parent) : base(parent)
        {
        }

        /// <summary>
        /// Returns the id of the default type definition node for the instance.
        /// </summary>
        protected override NodeId GetDefaultTypeDefinitionId(NamespaceTable namespaceUris)
        {
            return Opc.Ua.NodeId.Create(Telecontrol.Devices.ObjectTypes.Iec61850RcbType, Telecontrol.Devices.Namespaces.TelecontrolDevices, namespaceUris);
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
           "cm9sLnJ1L29wY3VhL2RldmljZXP/////BGCAAgEAAAACABcAAABJZWM2MTg1MFJjYlR5cGVJbnN0YW5j" +
           "ZQECggEBAoIBggEAAAEAAAABARcAAQECcwEBAAAAFWCJCgIAAAACAAkAAABSZWZlcmVuY2UBAoMBAC4A" +
           "RIMBAAAADP////8BAf////8AAAAA";
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

    #region Iec61850ServerState Class
    #if (!OPCUA_EXCLUDE_Iec61850ServerState)
    /// <summary>
    /// Stores an instance of the Iec61850ServerType ObjectType.
    /// </summary>
    /// <exclude />
    [System.CodeDom.Compiler.GeneratedCodeAttribute("Opc.Ua.ModelCompiler", "1.0.0.0")]
    public partial class Iec61850ServerState : DeviceState
    {
        #region Constructors
        /// <summary>
        /// Initializes the type with its default attribute values.
        /// </summary>
        public Iec61850ServerState(NodeState parent) : base(parent)
        {
        }

        /// <summary>
        /// Returns the id of the default type definition node for the instance.
        /// </summary>
        protected override NodeId GetDefaultTypeDefinitionId(NamespaceTable namespaceUris)
        {
            return Opc.Ua.NodeId.Create(Telecontrol.Devices.ObjectTypes.Iec61850ServerType, Telecontrol.Devices.Namespaces.TelecontrolDevices, namespaceUris);
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
           "cm9sLnJ1L29wY3VhL2RldmljZXP/////BGCAAgEAAAACABoAAABJZWM2MTg1MFNlcnZlclR5cGVJbnN0" +
           "YW5jZQECmzoBAps6mzoAAAEAAAABARcAAQECHgALAAAAFWCJCgIAAAACAAgAAABEaXNhYmxlZAECnDoA" +
           "LgBEnDoAAAAB/////wEB/////wAAAAAVYIkKAgAAAAIABgAAAE9ubGluZQECpToALwA/pToAAAAB////" +
           "/wEB/////wAAAAAVYIkKAgAAAAIABwAAAEVuYWJsZWQBAqY6AC8AP6Y6AAAAAf////8BAf////8AAAAA" +
           "BGGCCgQAAAACAAsAAABJbnRlcnJvZ2F0ZQECpzoALwEC6wCnOgAAAQH/////AAAAAARhggoEAAAAAgAS" +
           "AAAASW50ZXJyb2dhdGVDaGFubmVsAQKoOgAvAQIWAag6AAABAf////8AAAAABGGCCgQAAAACAAkAAABT" +
           "eW5jQ2xvY2sBAqk6AC8BAuwAqToAAAEB/////wAAAAAEYYIKBAAAAAIABgAAAFNlbGVjdAECqjoALwEC" +
           "AAGqOgAAAQH/////AAAAAARhggoEAAAAAgAFAAAAV3JpdGUBAqs6AC8BAgEBqzoAAAEB/////wAAAAAE" +
           "YYIKBAAAAAIACgAAAFdyaXRlUGFyYW0BAqw6AC8BAgIBrDoAAAEB/////wAAAAAVYIkKAgAAAAIABAAA" +
           "AEhvc3QBAq06AC4ARK06AAAADP////8BAf////8AAAAAFWCpCgIAAAACAAQAAABQb3J0AQKuOgAuAESu" +
           "OgAABWYAAAX/////AQH/////AAAAAA==";
        #endregion
        #endif
        #endregion

        #region Public Properties
        /// <remarks />
        public PropertyState<string> Host
        {
            get
            {
                return m_host;
            }

            set
            {
                if (!Object.ReferenceEquals(m_host, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_host = value;
            }
        }

        /// <remarks />
        public PropertyState<ushort> Port
        {
            get
            {
                return m_port;
            }

            set
            {
                if (!Object.ReferenceEquals(m_port, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_port = value;
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
            if (m_host != null)
            {
                children.Add(m_host);
            }

            if (m_port != null)
            {
                children.Add(m_port);
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
                case Telecontrol.Devices.BrowseNames.Host:
                {
                    if (createOrReplace)
                    {
                        if (Host == null)
                        {
                            if (replacement == null)
                            {
                                Host = new PropertyState<string>(this);
                            }
                            else
                            {
                                Host = (PropertyState<string>)replacement;
                            }
                        }
                    }

                    instance = Host;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.Port:
                {
                    if (createOrReplace)
                    {
                        if (Port == null)
                        {
                            if (replacement == null)
                            {
                                Port = new PropertyState<ushort>(this);
                            }
                            else
                            {
                                Port = (PropertyState<ushort>)replacement;
                            }
                        }
                    }

                    instance = Port;
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
        private PropertyState<string> m_host;
        private PropertyState<ushort> m_port;
        #endregion
    }
    #endif
    #endregion

    #region ModbusLinkState Class
    #if (!OPCUA_EXCLUDE_ModbusLinkState)
    /// <summary>
    /// Stores an instance of the ModbusLinkType ObjectType.
    /// </summary>
    /// <exclude />
    [System.CodeDom.Compiler.GeneratedCodeAttribute("Opc.Ua.ModelCompiler", "1.0.0.0")]
    public partial class ModbusLinkState : LinkState
    {
        #region Constructors
        /// <summary>
        /// Initializes the type with its default attribute values.
        /// </summary>
        public ModbusLinkState(NodeState parent) : base(parent)
        {
        }

        /// <summary>
        /// Returns the id of the default type definition node for the instance.
        /// </summary>
        protected override NodeId GetDefaultTypeDefinitionId(NamespaceTable namespaceUris)
        {
            return Opc.Ua.NodeId.Create(Telecontrol.Devices.ObjectTypes.ModbusLinkType, Telecontrol.Devices.Namespaces.TelecontrolDevices, namespaceUris);
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
           "cm9sLnJ1L29wY3VhL2RldmljZXP/////BGCAAgEAAAACABYAAABNb2RidXNMaW5rVHlwZUluc3RhbmNl" +
           "AQJsAAECbABsAAAA/////xIAAAAVYIkKAgAAAAIACAAAAERpc2FibGVkAQLVAAAuAETVAAAAAAH/////" +
           "AQH/////AAAAABVgiQoCAAAAAgAGAAAAT25saW5lAQLWAAAvAD/WAAAAAAH/////AQH/////AAAAABVg" +
           "iQoCAAAAAgAHAAAARW5hYmxlZAEC1wAALwA/1wAAAAAB/////wEB/////wAAAAAEYYIKBAAAAAIACwAA" +
           "AEludGVycm9nYXRlAQLzAAAvAQLrAPMAAAABAf////8AAAAABGGCCgQAAAACABIAAABJbnRlcnJvZ2F0" +
           "ZUNoYW5uZWwBAhoBAC8BAhYBGgEAAAEB/////wAAAAAEYYIKBAAAAAIACQAAAFN5bmNDbG9jawEC9AAA" +
           "LwEC7AD0AAAAAQH/////AAAAAARhggoEAAAAAgAGAAAAU2VsZWN0AQIMAQAvAQIAAQwBAAABAf////8A" +
           "AAAABGGCCgQAAAACAAUAAABXcml0ZQECDQEALwECAQENAQAAAQH/////AAAAAARhggoEAAAAAgAKAAAA" +
           "V3JpdGVQYXJhbQECDgEALwECAgEOAQAAAQH/////AAAAABVgiQoCAAAAAgAJAAAAVHJhbnNwb3J0AQJu" +
           "AAAuAERuAAAAAAz/////AQH/////AAAAABVgiQoCAAAAAgAMAAAAQ29ubmVjdENvdW50AQJvAAAvAD9v" +
           "AAAAAAf/////AQH/////AAAAABVgiQoCAAAAAgARAAAAQWN0aXZlQ29ubmVjdGlvbnMBAnAAAC8AP3AA" +
           "AAAAB/////8BAf////8AAAAAFWCJCgIAAAACAAsAAABNZXNzYWdlc091dAECcQAALwA/cQAAAAAH////" +
           "/wEB/////wAAAAAVYIkKAgAAAAIACgAAAE1lc3NhZ2VzSW4BAnIAAC8AP3IAAAAAB/////8BAf////8A" +
           "AAAAFWCJCgIAAAACAAgAAABCeXRlc091dAECcwAALwA/cwAAAAAH/////wEB/////wAAAAAVYIkKAgAA" +
           "AAIABwAAAEJ5dGVzSW4BAnQAAC8AP3QAAAAAB/////8BAf////8AAAAAFWCJCgIAAAACAAgAAABQcm90" +
           "b2NvbAECnzoALgBEnzoAAAECnTr/////AQH/////AAAAABVgiQoCAAAAAgAMAAAAUmVxdWVzdERlbGF5" +
           "AQKeOgAuAESeOgAAAAf/////AQH/////AAAAAA==";
        #endregion
        #endif
        #endregion

        #region Public Properties
        /// <remarks />
        public PropertyState<ModbusLinkProtocol> Protocol
        {
            get
            {
                return m_protocol;
            }

            set
            {
                if (!Object.ReferenceEquals(m_protocol, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_protocol = value;
            }
        }

        /// <remarks />
        public PropertyState<uint> RequestDelay
        {
            get
            {
                return m_requestDelay;
            }

            set
            {
                if (!Object.ReferenceEquals(m_requestDelay, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_requestDelay = value;
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
            if (m_protocol != null)
            {
                children.Add(m_protocol);
            }

            if (m_requestDelay != null)
            {
                children.Add(m_requestDelay);
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
                case Telecontrol.Devices.BrowseNames.Protocol:
                {
                    if (createOrReplace)
                    {
                        if (Protocol == null)
                        {
                            if (replacement == null)
                            {
                                Protocol = new PropertyState<ModbusLinkProtocol>(this);
                            }
                            else
                            {
                                Protocol = (PropertyState<ModbusLinkProtocol>)replacement;
                            }
                        }
                    }

                    instance = Protocol;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.RequestDelay:
                {
                    if (createOrReplace)
                    {
                        if (RequestDelay == null)
                        {
                            if (replacement == null)
                            {
                                RequestDelay = new PropertyState<uint>(this);
                            }
                            else
                            {
                                RequestDelay = (PropertyState<uint>)replacement;
                            }
                        }
                    }

                    instance = RequestDelay;
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
        private PropertyState<ModbusLinkProtocol> m_protocol;
        private PropertyState<uint> m_requestDelay;
        #endregion
    }
    #endif
    #endregion

    #region ModbusDeviceState Class
    #if (!OPCUA_EXCLUDE_ModbusDeviceState)
    /// <summary>
    /// Stores an instance of the ModbusDeviceType ObjectType.
    /// </summary>
    /// <exclude />
    [System.CodeDom.Compiler.GeneratedCodeAttribute("Opc.Ua.ModelCompiler", "1.0.0.0")]
    public partial class ModbusDeviceState : DeviceState
    {
        #region Constructors
        /// <summary>
        /// Initializes the type with its default attribute values.
        /// </summary>
        public ModbusDeviceState(NodeState parent) : base(parent)
        {
        }

        /// <summary>
        /// Returns the id of the default type definition node for the instance.
        /// </summary>
        protected override NodeId GetDefaultTypeDefinitionId(NamespaceTable namespaceUris)
        {
            return Opc.Ua.NodeId.Create(Telecontrol.Devices.ObjectTypes.ModbusDeviceType, Telecontrol.Devices.Namespaces.TelecontrolDevices, namespaceUris);
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
           "cm9sLnJ1L29wY3VhL2RldmljZXP/////BGCAAgEAAAACABgAAABNb2RidXNEZXZpY2VUeXBlSW5zdGFu" +
           "Y2UBAnYAAQJ2AHYAAAABAAAAAQEXAAEBAmwADAAAABVgiQoCAAAAAgAIAAAARGlzYWJsZWQBAtgAAC4A" +
           "RNgAAAAAAf////8BAf////8AAAAAFWCJCgIAAAACAAYAAABPbmxpbmUBAtkAAC8AP9kAAAAAAf////8B" +
           "Af////8AAAAAFWCJCgIAAAACAAcAAABFbmFibGVkAQLaAAAvAD/aAAAAAAH/////AQH/////AAAAAARh" +
           "ggoEAAAAAgALAAAASW50ZXJyb2dhdGUBAvUAAC8BAusA9QAAAAEB/////wAAAAAEYYIKBAAAAAIAEgAA" +
           "AEludGVycm9nYXRlQ2hhbm5lbAECGwEALwECFgEbAQAAAQH/////AAAAAARhggoEAAAAAgAJAAAAU3lu" +
           "Y0Nsb2NrAQL2AAAvAQLsAPYAAAABAf////8AAAAABGGCCgQAAAACAAYAAABTZWxlY3QBAg8BAC8BAgAB" +
           "DwEAAAEB/////wAAAAAEYYIKBAAAAAIABQAAAFdyaXRlAQIQAQAvAQIBARABAAABAf////8AAAAABGGC" +
           "CgQAAAACAAoAAABXcml0ZVBhcmFtAQIRAQAvAQICAREBAAABAf////8AAAAAFWCpCgIAAAACAAcAAABB" +
           "ZGRyZXNzAQJ4AAAuAER4AAAABwEAAAAAB/////8BAf////8AAAAAFWCpCgIAAAACAA4AAABTZW5kUmV0" +
           "cnlDb3VudAECeQAALgBEeQAAAAcDAAAAAAf/////AQH/////AAAAABVgqQoCAAAAAgAPAAAAUmVzcG9u" +
           "c2VUaW1lb3V0AQKhOgAuAEShOgAAB+gDAAAAB/////8BAf////8AAAAA";
        #endregion
        #endif
        #endregion

        #region Public Properties
        /// <remarks />
        public PropertyState<uint> Address
        {
            get
            {
                return m_address;
            }

            set
            {
                if (!Object.ReferenceEquals(m_address, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_address = value;
            }
        }

        /// <remarks />
        public PropertyState<uint> SendRetryCount
        {
            get
            {
                return m_sendRetryCount;
            }

            set
            {
                if (!Object.ReferenceEquals(m_sendRetryCount, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_sendRetryCount = value;
            }
        }

        /// <remarks />
        public PropertyState<uint> ResponseTimeout
        {
            get
            {
                return m_responseTimeout;
            }

            set
            {
                if (!Object.ReferenceEquals(m_responseTimeout, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_responseTimeout = value;
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
            if (m_address != null)
            {
                children.Add(m_address);
            }

            if (m_sendRetryCount != null)
            {
                children.Add(m_sendRetryCount);
            }

            if (m_responseTimeout != null)
            {
                children.Add(m_responseTimeout);
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
                case Telecontrol.Devices.BrowseNames.Address:
                {
                    if (createOrReplace)
                    {
                        if (Address == null)
                        {
                            if (replacement == null)
                            {
                                Address = new PropertyState<uint>(this);
                            }
                            else
                            {
                                Address = (PropertyState<uint>)replacement;
                            }
                        }
                    }

                    instance = Address;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.SendRetryCount:
                {
                    if (createOrReplace)
                    {
                        if (SendRetryCount == null)
                        {
                            if (replacement == null)
                            {
                                SendRetryCount = new PropertyState<uint>(this);
                            }
                            else
                            {
                                SendRetryCount = (PropertyState<uint>)replacement;
                            }
                        }
                    }

                    instance = SendRetryCount;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.ResponseTimeout:
                {
                    if (createOrReplace)
                    {
                        if (ResponseTimeout == null)
                        {
                            if (replacement == null)
                            {
                                ResponseTimeout = new PropertyState<uint>(this);
                            }
                            else
                            {
                                ResponseTimeout = (PropertyState<uint>)replacement;
                            }
                        }
                    }

                    instance = ResponseTimeout;
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
        private PropertyState<uint> m_address;
        private PropertyState<uint> m_sendRetryCount;
        private PropertyState<uint> m_responseTimeout;
        #endregion
    }
    #endif
    #endregion

    #region OpcServerState Class
    #if (!OPCUA_EXCLUDE_OpcServerState)
    /// <summary>
    /// Stores an instance of the OpcServerType ObjectType.
    /// </summary>
    /// <exclude />
    [System.CodeDom.Compiler.GeneratedCodeAttribute("Opc.Ua.ModelCompiler", "1.0.0.0")]
    public partial class OpcServerState : DeviceState
    {
        #region Constructors
        /// <summary>
        /// Initializes the type with its default attribute values.
        /// </summary>
        public OpcServerState(NodeState parent) : base(parent)
        {
        }

        /// <summary>
        /// Returns the id of the default type definition node for the instance.
        /// </summary>
        protected override NodeId GetDefaultTypeDefinitionId(NamespaceTable namespaceUris)
        {
            return Opc.Ua.NodeId.Create(Telecontrol.Devices.ObjectTypes.OpcServerType, Telecontrol.Devices.Namespaces.TelecontrolDevices, namespaceUris);
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
           "cm9sLnJ1L29wY3VhL2RldmljZXP/////BGCAAgEAAAACABUAAABPcGNTZXJ2ZXJUeXBlSW5zdGFuY2UB" +
           "ArY6AQK2OrY6AAABAAAAAQEXAAEBAh4ACwAAABVgiQoCAAAAAgAIAAAARGlzYWJsZWQBArM6AC4ARLM6" +
           "AAAAAf////8BAf////8AAAAAFWCJCgIAAAACAAYAAABPbmxpbmUBArQ6AC8AP7Q6AAAAAf////8BAf//" +
           "//8AAAAAFWCJCgIAAAACAAcAAABFbmFibGVkAQK1OgAvAD+1OgAAAAH/////AQH/////AAAAAARhggoE" +
           "AAAAAgALAAAASW50ZXJyb2dhdGUBArk6AC8BAusAuToAAAEB/////wAAAAAEYYIKBAAAAAIAEgAAAElu" +
           "dGVycm9nYXRlQ2hhbm5lbAECujoALwECFgG6OgAAAQH/////AAAAAARhggoEAAAAAgAJAAAAU3luY0Ns" +
           "b2NrAQK7OgAvAQLsALs6AAABAf////8AAAAABGGCCgQAAAACAAYAAABTZWxlY3QBArw6AC8BAgABvDoA" +
           "AAEB/////wAAAAAEYYIKBAAAAAIABQAAAFdyaXRlAQK9OgAvAQIBAb06AAABAf////8AAAAABGGCCgQA" +
           "AAACAAoAAABXcml0ZVBhcmFtAQK+OgAvAQICAb46AAABAf////8AAAAAFWCJCgIAAAACAAsAAABNYWNo" +
           "aW5lTmFtZQECtzoALgBEtzoAAAAV/////wEB/////wAAAAAVYIkKAgAAAAIABgAAAFByb2dJZAECuDoA" +
           "LgBEuDoAAAAV/////wEB/////wAAAAA=";
        #endregion
        #endif
        #endregion

        #region Public Properties
        /// <remarks />
        public PropertyState<LocalizedText> MachineName
        {
            get
            {
                return m_machineName;
            }

            set
            {
                if (!Object.ReferenceEquals(m_machineName, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_machineName = value;
            }
        }

        /// <remarks />
        public PropertyState<LocalizedText> ProgId
        {
            get
            {
                return m_progId;
            }

            set
            {
                if (!Object.ReferenceEquals(m_progId, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_progId = value;
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
            if (m_machineName != null)
            {
                children.Add(m_machineName);
            }

            if (m_progId != null)
            {
                children.Add(m_progId);
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
                case Telecontrol.Devices.BrowseNames.MachineName:
                {
                    if (createOrReplace)
                    {
                        if (MachineName == null)
                        {
                            if (replacement == null)
                            {
                                MachineName = new PropertyState<LocalizedText>(this);
                            }
                            else
                            {
                                MachineName = (PropertyState<LocalizedText>)replacement;
                            }
                        }
                    }

                    instance = MachineName;
                    break;
                }

                case Telecontrol.Devices.BrowseNames.ProgId:
                {
                    if (createOrReplace)
                    {
                        if (ProgId == null)
                        {
                            if (replacement == null)
                            {
                                ProgId = new PropertyState<LocalizedText>(this);
                            }
                            else
                            {
                                ProgId = (PropertyState<LocalizedText>)replacement;
                            }
                        }
                    }

                    instance = ProgId;
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
        private PropertyState<LocalizedText> m_machineName;
        private PropertyState<LocalizedText> m_progId;
        #endregion
    }
    #endif
    #endregion

    #region TransmissionItemState Class
    #if (!OPCUA_EXCLUDE_TransmissionItemState)
    /// <summary>
    /// Stores an instance of the TransmissionItemType ObjectType.
    /// </summary>
    /// <exclude />
    [System.CodeDom.Compiler.GeneratedCodeAttribute("Opc.Ua.ModelCompiler", "1.0.0.0")]
    public partial class TransmissionItemState : BaseObjectState
    {
        #region Constructors
        /// <summary>
        /// Initializes the type with its default attribute values.
        /// </summary>
        public TransmissionItemState(NodeState parent) : base(parent)
        {
        }

        /// <summary>
        /// Returns the id of the default type definition node for the instance.
        /// </summary>
        protected override NodeId GetDefaultTypeDefinitionId(NamespaceTable namespaceUris)
        {
            return Opc.Ua.NodeId.Create(Telecontrol.Devices.ObjectTypes.TransmissionItemType, Telecontrol.Devices.Namespaces.TelecontrolDevices, namespaceUris);
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
           "cm9sLnJ1L29wY3VhL2RldmljZXP/////BGCAAgEAAAACABwAAABUcmFuc21pc3Npb25JdGVtVHlwZUlu" +
           "c3RhbmNlAQIiAAECIgAiAAAAAwAAAAECrAAAAD4BAq0AAAECHwABARcAAQECIQABAAAAFWCpCgIAAAAC" +
           "AA0AAABTb3VyY2VBZGRyZXNzAQIjAAAuAEQjAAAABwAAAAAAB/////8BAf////8AAAAA";
        #endregion
        #endif
        #endregion

        #region Public Properties
        /// <remarks />
        public PropertyState<uint> SourceAddress
        {
            get
            {
                return m_sourceAddress;
            }

            set
            {
                if (!Object.ReferenceEquals(m_sourceAddress, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_sourceAddress = value;
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
            if (m_sourceAddress != null)
            {
                children.Add(m_sourceAddress);
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
                case Telecontrol.Devices.BrowseNames.SourceAddress:
                {
                    if (createOrReplace)
                    {
                        if (SourceAddress == null)
                        {
                            if (replacement == null)
                            {
                                SourceAddress = new PropertyState<uint>(this);
                            }
                            else
                            {
                                SourceAddress = (PropertyState<uint>)replacement;
                            }
                        }
                    }

                    instance = SourceAddress;
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
        private PropertyState<uint> m_sourceAddress;
        #endregion
    }
    #endif
    #endregion
}