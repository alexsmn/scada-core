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
using Telecontrol.History;

namespace Telecontrol.DataItems
{
    #region DataGroupState Class
    #if (!OPCUA_EXCLUDE_DataGroupState)
    /// <summary>
    /// Stores an instance of the DataGroupType ObjectType.
    /// </summary>
    /// <exclude />
    [System.CodeDom.Compiler.GeneratedCodeAttribute("Opc.Ua.ModelCompiler", "1.0.0.0")]
    public partial class DataGroupState : BaseObjectState
    {
        #region Constructors
        /// <summary>
        /// Initializes the type with its default attribute values.
        /// </summary>
        public DataGroupState(NodeState parent) : base(parent)
        {
        }

        /// <summary>
        /// Returns the id of the default type definition node for the instance.
        /// </summary>
        protected override NodeId GetDefaultTypeDefinitionId(NamespaceTable namespaceUris)
        {
            return Opc.Ua.NodeId.Create(Telecontrol.DataItems.ObjectTypes.DataGroupType, Telecontrol.DataItems.Namespaces.TelecontrolDataItems, namespaceUris);
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
           "AwAAACEAAABodHRwOi8vdGVsZWNvbnRyb2wucnUvb3BjdWEvc2NhZGEjAAAAaHR0cDovL3RlbGVjb250" +
           "cm9sLnJ1L29wY3VhL2hpc3RvcnkmAAAAaHR0cDovL3RlbGVjb250cm9sLnJ1L29wY3VhL2RhdGFfaXRl" +
           "bXP/////BGCAAgEAAAADABUAAABEYXRhR3JvdXBUeXBlSW5zdGFuY2UBAz4AAQM+AD4AAAACAAAAAQEX" +
           "AAABAz4AAQEXAAEBAxgAAQAAABVgiQoCAAAAAwAJAAAAU2ltdWxhdGVkAQM/AAAuAEQ/AAAAAAH/////" +
           "AQEBAAAAAQG0OgABA7k6AAAAAA==";
        #endregion
        #endif
        #endregion

        #region Public Properties
        /// <remarks />
        public PropertyState<bool> Simulated
        {
            get
            {
                return m_simulated;
            }

            set
            {
                if (!Object.ReferenceEquals(m_simulated, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_simulated = value;
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
            if (m_simulated != null)
            {
                children.Add(m_simulated);
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
                case Telecontrol.DataItems.BrowseNames.Simulated:
                {
                    if (createOrReplace)
                    {
                        if (Simulated == null)
                        {
                            if (replacement == null)
                            {
                                Simulated = new PropertyState<bool>(this);
                            }
                            else
                            {
                                Simulated = (PropertyState<bool>)replacement;
                            }
                        }
                    }

                    instance = Simulated;
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
        private PropertyState<bool> m_simulated;
        #endregion
    }
    #endif
    #endregion

    #region DataItemState Class
    #if (!OPCUA_EXCLUDE_DataItemState)
    /// <summary>
    /// Stores an instance of the DataItemType VariableType.
    /// </summary>
    /// <exclude />
    [System.CodeDom.Compiler.GeneratedCodeAttribute("Opc.Ua.ModelCompiler", "1.0.0.0")]
    public partial class DataItemState : BaseDataVariableState
    {
        #region Constructors
        /// <summary>
        /// Initializes the type with its default attribute values.
        /// </summary>
        public DataItemState(NodeState parent) : base(parent)
        {
        }

        /// <summary>
        /// Returns the id of the default type definition node for the instance.
        /// </summary>
        protected override NodeId GetDefaultTypeDefinitionId(NamespaceTable namespaceUris)
        {
            return Opc.Ua.NodeId.Create(Telecontrol.DataItems.VariableTypes.DataItemType, Telecontrol.DataItems.Namespaces.TelecontrolDataItems, namespaceUris);
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
           "AwAAACEAAABodHRwOi8vdGVsZWNvbnRyb2wucnUvb3BjdWEvc2NhZGEjAAAAaHR0cDovL3RlbGVjb250" +
           "cm9sLnJ1L29wY3VhL2hpc3RvcnkmAAAAaHR0cDovL3RlbGVjb250cm9sLnJ1L29wY3VhL2RhdGFfaXRl" +
           "bXP/////FWCJAgIAAAADABQAAABEYXRhSXRlbVR5cGVJbnN0YW5jZQEDEgABAxIAEgAAAAAY/v///wEB" +
           "BAAAAAEDRQAAAQNDAAECmToAAQIUAAEBFwABAQM+AAEBFwABAQMYAAsAAAAVYIkKAgAAAAMABQAAAEFs" +
           "aWFzAQMTAAAuAEQTAAAAAAz/////AQH/////AAAAABVgqQoCAAAAAwAIAAAAU2V2ZXJpdHkBA64AAC4A" +
           "RK4AAAAHAQAAAAAH/////wEB/////wAAAAAVYIkKAgAAAAMABgAAAElucHV0MQEDrwAALgBErwAAAAAM" +
           "/////wEB/////wAAAAAVYIkKAgAAAAMABgAAAElucHV0MgEDsAAALgBEsAAAAAAM/////wEB/////wAA" +
           "AAAVYIkKAgAAAAMABgAAAE91dHB1dAEDsQAALgBEsQAAAAAM/////wEB/////wAAAAAVYIkKAgAAAAMA" +
           "DwAAAE91dHB1dENvbmRpdGlvbgEDsgAALgBEsgAAAAAM/////wEB/////wAAAAAVYIkKAgAAAAMADwAA" +
           "AE91dHB1dFR3b1N0YWdlZAEDsToALgBEsToAAAAB/////wEB/////wAAAAAVYIkKAgAAAAMACwAAAFN0" +
           "YWxlUGVyaW9kAQOzAAAuAESzAAAAAAf/////AQH/////AAAAABVgiQoCAAAAAwAJAAAAU2ltdWxhdGVk" +
           "AQNCAAAuAERCAAAAAAH/////AQH/////AAAAABVgiQoCAAAAAwAGAAAATG9ja2VkAQO0AAAuAES0AAAA" +
           "AAH/////AQH/////AAAAAARhggoEAAAAAwALAAAAV3JpdGVNYW51YWwBAx0BAC8BAx0BHQEAAAEB////" +
           "/wAAAAA=";
        #endregion
        #endif
        #endregion

        #region Public Properties
        /// <remarks />
        public PropertyState<string> Alias
        {
            get
            {
                return m_alias;
            }

            set
            {
                if (!Object.ReferenceEquals(m_alias, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_alias = value;
            }
        }

        /// <remarks />
        public PropertyState<uint> Severity
        {
            get
            {
                return m_severity;
            }

            set
            {
                if (!Object.ReferenceEquals(m_severity, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_severity = value;
            }
        }

        /// <remarks />
        public PropertyState<string> Input1
        {
            get
            {
                return m_input1;
            }

            set
            {
                if (!Object.ReferenceEquals(m_input1, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_input1 = value;
            }
        }

        /// <remarks />
        public PropertyState<string> Input2
        {
            get
            {
                return m_input2;
            }

            set
            {
                if (!Object.ReferenceEquals(m_input2, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_input2 = value;
            }
        }

        /// <remarks />
        public PropertyState<string> Output
        {
            get
            {
                return m_output;
            }

            set
            {
                if (!Object.ReferenceEquals(m_output, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_output = value;
            }
        }

        /// <remarks />
        public PropertyState<string> OutputCondition
        {
            get
            {
                return m_outputCondition;
            }

            set
            {
                if (!Object.ReferenceEquals(m_outputCondition, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_outputCondition = value;
            }
        }

        /// <remarks />
        public PropertyState<bool> OutputTwoStaged
        {
            get
            {
                return m_outputTwoStaged;
            }

            set
            {
                if (!Object.ReferenceEquals(m_outputTwoStaged, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_outputTwoStaged = value;
            }
        }

        /// <remarks />
        public PropertyState<uint> StalePeriod
        {
            get
            {
                return m_stalePeriod;
            }

            set
            {
                if (!Object.ReferenceEquals(m_stalePeriod, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_stalePeriod = value;
            }
        }

        /// <remarks />
        public PropertyState<bool> Simulated
        {
            get
            {
                return m_simulated;
            }

            set
            {
                if (!Object.ReferenceEquals(m_simulated, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_simulated = value;
            }
        }

        /// <remarks />
        public PropertyState<bool> Locked
        {
            get
            {
                return m_locked;
            }

            set
            {
                if (!Object.ReferenceEquals(m_locked, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_locked = value;
            }
        }

        /// <remarks />
        public MethodState WriteManual
        {
            get
            {
                return m_writeManualMethod;
            }

            set
            {
                if (!Object.ReferenceEquals(m_writeManualMethod, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_writeManualMethod = value;
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
            if (m_alias != null)
            {
                children.Add(m_alias);
            }

            if (m_severity != null)
            {
                children.Add(m_severity);
            }

            if (m_input1 != null)
            {
                children.Add(m_input1);
            }

            if (m_input2 != null)
            {
                children.Add(m_input2);
            }

            if (m_output != null)
            {
                children.Add(m_output);
            }

            if (m_outputCondition != null)
            {
                children.Add(m_outputCondition);
            }

            if (m_outputTwoStaged != null)
            {
                children.Add(m_outputTwoStaged);
            }

            if (m_stalePeriod != null)
            {
                children.Add(m_stalePeriod);
            }

            if (m_simulated != null)
            {
                children.Add(m_simulated);
            }

            if (m_locked != null)
            {
                children.Add(m_locked);
            }

            if (m_writeManualMethod != null)
            {
                children.Add(m_writeManualMethod);
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
                case Telecontrol.DataItems.BrowseNames.Alias:
                {
                    if (createOrReplace)
                    {
                        if (Alias == null)
                        {
                            if (replacement == null)
                            {
                                Alias = new PropertyState<string>(this);
                            }
                            else
                            {
                                Alias = (PropertyState<string>)replacement;
                            }
                        }
                    }

                    instance = Alias;
                    break;
                }

                case Telecontrol.DataItems.BrowseNames.Severity:
                {
                    if (createOrReplace)
                    {
                        if (Severity == null)
                        {
                            if (replacement == null)
                            {
                                Severity = new PropertyState<uint>(this);
                            }
                            else
                            {
                                Severity = (PropertyState<uint>)replacement;
                            }
                        }
                    }

                    instance = Severity;
                    break;
                }

                case Telecontrol.DataItems.BrowseNames.Input1:
                {
                    if (createOrReplace)
                    {
                        if (Input1 == null)
                        {
                            if (replacement == null)
                            {
                                Input1 = new PropertyState<string>(this);
                            }
                            else
                            {
                                Input1 = (PropertyState<string>)replacement;
                            }
                        }
                    }

                    instance = Input1;
                    break;
                }

                case Telecontrol.DataItems.BrowseNames.Input2:
                {
                    if (createOrReplace)
                    {
                        if (Input2 == null)
                        {
                            if (replacement == null)
                            {
                                Input2 = new PropertyState<string>(this);
                            }
                            else
                            {
                                Input2 = (PropertyState<string>)replacement;
                            }
                        }
                    }

                    instance = Input2;
                    break;
                }

                case Telecontrol.DataItems.BrowseNames.Output:
                {
                    if (createOrReplace)
                    {
                        if (Output == null)
                        {
                            if (replacement == null)
                            {
                                Output = new PropertyState<string>(this);
                            }
                            else
                            {
                                Output = (PropertyState<string>)replacement;
                            }
                        }
                    }

                    instance = Output;
                    break;
                }

                case Telecontrol.DataItems.BrowseNames.OutputCondition:
                {
                    if (createOrReplace)
                    {
                        if (OutputCondition == null)
                        {
                            if (replacement == null)
                            {
                                OutputCondition = new PropertyState<string>(this);
                            }
                            else
                            {
                                OutputCondition = (PropertyState<string>)replacement;
                            }
                        }
                    }

                    instance = OutputCondition;
                    break;
                }

                case Telecontrol.DataItems.BrowseNames.OutputTwoStaged:
                {
                    if (createOrReplace)
                    {
                        if (OutputTwoStaged == null)
                        {
                            if (replacement == null)
                            {
                                OutputTwoStaged = new PropertyState<bool>(this);
                            }
                            else
                            {
                                OutputTwoStaged = (PropertyState<bool>)replacement;
                            }
                        }
                    }

                    instance = OutputTwoStaged;
                    break;
                }

                case Telecontrol.DataItems.BrowseNames.StalePeriod:
                {
                    if (createOrReplace)
                    {
                        if (StalePeriod == null)
                        {
                            if (replacement == null)
                            {
                                StalePeriod = new PropertyState<uint>(this);
                            }
                            else
                            {
                                StalePeriod = (PropertyState<uint>)replacement;
                            }
                        }
                    }

                    instance = StalePeriod;
                    break;
                }

                case Telecontrol.DataItems.BrowseNames.Simulated:
                {
                    if (createOrReplace)
                    {
                        if (Simulated == null)
                        {
                            if (replacement == null)
                            {
                                Simulated = new PropertyState<bool>(this);
                            }
                            else
                            {
                                Simulated = (PropertyState<bool>)replacement;
                            }
                        }
                    }

                    instance = Simulated;
                    break;
                }

                case Telecontrol.DataItems.BrowseNames.Locked:
                {
                    if (createOrReplace)
                    {
                        if (Locked == null)
                        {
                            if (replacement == null)
                            {
                                Locked = new PropertyState<bool>(this);
                            }
                            else
                            {
                                Locked = (PropertyState<bool>)replacement;
                            }
                        }
                    }

                    instance = Locked;
                    break;
                }

                case Telecontrol.DataItems.BrowseNames.WriteManual:
                {
                    if (createOrReplace)
                    {
                        if (WriteManual == null)
                        {
                            if (replacement == null)
                            {
                                WriteManual = new MethodState(this);
                            }
                            else
                            {
                                WriteManual = (MethodState)replacement;
                            }
                        }
                    }

                    instance = WriteManual;
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
        private PropertyState<string> m_alias;
        private PropertyState<uint> m_severity;
        private PropertyState<string> m_input1;
        private PropertyState<string> m_input2;
        private PropertyState<string> m_output;
        private PropertyState<string> m_outputCondition;
        private PropertyState<bool> m_outputTwoStaged;
        private PropertyState<uint> m_stalePeriod;
        private PropertyState<bool> m_simulated;
        private PropertyState<bool> m_locked;
        private MethodState m_writeManualMethod;
        #endregion
    }

    #region DataItemState<T> Class
    /// <summary>
    /// A typed version of the DataItemType variable.
    /// </summary>
    /// <exclude />
    [System.CodeDom.Compiler.GeneratedCodeAttribute("Opc.Ua.ModelCompiler", "1.0.0.0")]
    public class DataItemState<T> : DataItemState
    {
        #region Constructors
        /// <summary>
        /// Initializes the instance with its defalt attribute values.
        /// </summary>
        public DataItemState(NodeState parent) : base(parent)
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

    #region DiscreteItemState Class
    #if (!OPCUA_EXCLUDE_DiscreteItemState)
    /// <summary>
    /// Stores an instance of the DiscreteItemType VariableType.
    /// </summary>
    /// <exclude />
    [System.CodeDom.Compiler.GeneratedCodeAttribute("Opc.Ua.ModelCompiler", "1.0.0.0")]
    public partial class DiscreteItemState : DataItemState<uint>
    {
        #region Constructors
        /// <summary>
        /// Initializes the type with its default attribute values.
        /// </summary>
        public DiscreteItemState(NodeState parent) : base(parent)
        {
        }

        /// <summary>
        /// Returns the id of the default type definition node for the instance.
        /// </summary>
        protected override NodeId GetDefaultTypeDefinitionId(NamespaceTable namespaceUris)
        {
            return Opc.Ua.NodeId.Create(Telecontrol.DataItems.VariableTypes.DiscreteItemType, Telecontrol.DataItems.Namespaces.TelecontrolDataItems, namespaceUris);
        }

        /// <summary>
        /// Returns the id of the default data type node for the instance.
        /// </summary>
        protected override NodeId GetDefaultDataTypeId(NamespaceTable namespaceUris)
        {
            return Opc.Ua.NodeId.Create(Opc.Ua.DataTypes.UInt32, Opc.Ua.Namespaces.OpcUa, namespaceUris);
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
           "AwAAACEAAABodHRwOi8vdGVsZWNvbnRyb2wucnUvb3BjdWEvc2NhZGEjAAAAaHR0cDovL3RlbGVjb250" +
           "cm9sLnJ1L29wY3VhL2hpc3RvcnkmAAAAaHR0cDovL3RlbGVjb250cm9sLnJ1L29wY3VhL2RhdGFfaXRl" +
           "bXP/////FWCJAgIAAAADABgAAABEaXNjcmV0ZUl0ZW1UeXBlSW5zdGFuY2UBA0gAAQNIAEgAAAAAB/7/" +
           "//8BAQEAAAABA0cAAAEDUAAMAAAAFWCJCgIAAAADAAUAAABBbGlhcwEDSQAALgBESQAAAAAM/////wEB" +
           "/////wAAAAAVYKkKAgAAAAMACAAAAFNldmVyaXR5AQO1AAAuAES1AAAABwEAAAAAB/////8BAf////8A" +
           "AAAAFWCJCgIAAAADAAYAAABJbnB1dDEBA7YAAC4ARLYAAAAADP////8BAf////8AAAAAFWCJCgIAAAAD" +
           "AAYAAABJbnB1dDIBA7cAAC4ARLcAAAAADP////8BAf////8AAAAAFWCJCgIAAAADAAYAAABPdXRwdXQB" +
           "A7gAAC4ARLgAAAAADP////8BAf////8AAAAAFWCJCgIAAAADAA8AAABPdXRwdXRDb25kaXRpb24BA7kA" +
           "AC4ARLkAAAAADP////8BAf////8AAAAAFWCJCgIAAAADAA8AAABPdXRwdXRUd29TdGFnZWQBA7I6AC4A" +
           "RLI6AAAAAf////8BAf////8AAAAAFWCJCgIAAAADAAsAAABTdGFsZVBlcmlvZAEDugAALgBEugAAAAAH" +
           "/////wEB/////wAAAAAVYIkKAgAAAAMACQAAAFNpbXVsYXRlZAEDSgAALgBESgAAAAAB/////wEB////" +
           "/wAAAAAVYIkKAgAAAAMABgAAAExvY2tlZAEDuwAALgBEuwAAAAAB/////wEB/////wAAAAAEYYIKBAAA" +
           "AAMACwAAAFdyaXRlTWFudWFsAQMeAQAvAQMdAR4BAAABAf////8AAAAAFWCJCgIAAAADAAkAAABJbnZl" +
           "cnNpb24BA7wAAC4ARLwAAAAAAf////8BAf////8AAAAA";
        #endregion
        #endif
        #endregion

        #region Public Properties
        /// <remarks />
        public PropertyState<bool> Inversion
        {
            get
            {
                return m_inversion;
            }

            set
            {
                if (!Object.ReferenceEquals(m_inversion, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_inversion = value;
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
            if (m_inversion != null)
            {
                children.Add(m_inversion);
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
                case Telecontrol.DataItems.BrowseNames.Inversion:
                {
                    if (createOrReplace)
                    {
                        if (Inversion == null)
                        {
                            if (replacement == null)
                            {
                                Inversion = new PropertyState<bool>(this);
                            }
                            else
                            {
                                Inversion = (PropertyState<bool>)replacement;
                            }
                        }
                    }

                    instance = Inversion;
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
        private PropertyState<bool> m_inversion;
        #endregion
    }
    #endif
    #endregion

    #region AnalogItemState Class
    #if (!OPCUA_EXCLUDE_AnalogItemState)
    /// <summary>
    /// Stores an instance of the AnalogItemType VariableType.
    /// </summary>
    /// <exclude />
    [System.CodeDom.Compiler.GeneratedCodeAttribute("Opc.Ua.ModelCompiler", "1.0.0.0")]
    public partial class AnalogItemState : DataItemState<double>
    {
        #region Constructors
        /// <summary>
        /// Initializes the type with its default attribute values.
        /// </summary>
        public AnalogItemState(NodeState parent) : base(parent)
        {
        }

        /// <summary>
        /// Returns the id of the default type definition node for the instance.
        /// </summary>
        protected override NodeId GetDefaultTypeDefinitionId(NamespaceTable namespaceUris)
        {
            return Opc.Ua.NodeId.Create(Telecontrol.DataItems.VariableTypes.AnalogItemType, Telecontrol.DataItems.Namespaces.TelecontrolDataItems, namespaceUris);
        }

        /// <summary>
        /// Returns the id of the default data type node for the instance.
        /// </summary>
        protected override NodeId GetDefaultDataTypeId(NamespaceTable namespaceUris)
        {
            return Opc.Ua.NodeId.Create(Opc.Ua.DataTypes.Double, Opc.Ua.Namespaces.OpcUa, namespaceUris);
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
           "AwAAACEAAABodHRwOi8vdGVsZWNvbnRyb2wucnUvb3BjdWEvc2NhZGEjAAAAaHR0cDovL3RlbGVjb250" +
           "cm9sLnJ1L29wY3VhL2hpc3RvcnkmAAAAaHR0cDovL3RlbGVjb250cm9sLnJ1L29wY3VhL2RhdGFfaXRl" +
           "bXP/////FWCJAgIAAAADABYAAABBbmFsb2dJdGVtVHlwZUluc3RhbmNlAQNMAAEDTABMAAAAAAv+////" +
           "AQH/////GQAAABVgiQoCAAAAAwAFAAAAQWxpYXMBAx8BAC4ARB8BAAAADP////8BAf////8AAAAAFWCp" +
           "CgIAAAADAAgAAABTZXZlcml0eQEDIAEALgBEIAEAAAcBAAAAAAf/////AQH/////AAAAABVgiQoCAAAA" +
           "AwAGAAAASW5wdXQxAQMhAQAuAEQhAQAAAAz/////AQH/////AAAAABVgiQoCAAAAAwAGAAAASW5wdXQy" +
           "AQMiAQAuAEQiAQAAAAz/////AQH/////AAAAABVgiQoCAAAAAwAGAAAAT3V0cHV0AQMjAQAuAEQjAQAA" +
           "AAz/////AQH/////AAAAABVgiQoCAAAAAwAPAAAAT3V0cHV0Q29uZGl0aW9uAQMkAQAuAEQkAQAAAAz/" +
           "////AQH/////AAAAABVgiQoCAAAAAwAPAAAAT3V0cHV0VHdvU3RhZ2VkAQOzOgAuAESzOgAAAAH/////" +
           "AQH/////AAAAABVgiQoCAAAAAwALAAAAU3RhbGVQZXJpb2QBAyUBAC4ARCUBAAAAB/////8BAf////8A" +
           "AAAAFWCJCgIAAAADAAkAAABTaW11bGF0ZWQBAyYBAC4ARCYBAAAAAf////8BAf////8AAAAAFWCJCgIA" +
           "AAADAAYAAABMb2NrZWQBAycBAC4ARCcBAAAAAf////8BAf////8AAAAABGGCCgQAAAADAAsAAABXcml0" +
           "ZU1hbnVhbAEDKAEALwEDHQEoAQAAAQH/////AAAAABVgiQoCAAAAAwANAAAARGlzcGxheUZvcm1hdAED" +
           "vQAALgBEvQAAAAAM/////wEB/////wAAAAAVYIkKAgAAAAMACgAAAENvbnZlcnNpb24BA74AAC4ARL4A" +
           "AAABA746/////wEB/////wAAAAAVYIkKAgAAAAMACAAAAENsYW1waW5nAQO/AAAuAES/AAAAAAH/////" +
           "AQH/////AAAAABVgqQoCAAAAAwAEAAAARXVMbwEDwAAALgBEwAAAAAsAAAAAAABZwAAL/////wEB////" +
           "/wAAAAAVYKkKAgAAAAMABAAAAEV1SGkBA8EAAC4ARMEAAAALAAAAAAAAWUAAC/////8BAf////8AAAAA" +
           "FWCpCgIAAAADAAQAAABJckxvAQPCAAAuAETCAAAACwAAAAAAAPA/AAv/////AQH/////AAAAABVgqQoC" +
           "AAAAAwAEAAAASXJIaQEDwwAALgBEwwAAAAsAAAAA4P/vQAAL/////wEB/////wAAAAAVYIkKAgAAAAMA" +
           "BwAAAExpbWl0TG8BA8QAAC4ARMQAAAAAC/////8BAf////8AAAAAFWCJCgIAAAADAAcAAABMaW1pdEhp" +
           "AQPFAAAuAETFAAAAAAv/////AQH/////AAAAABVgiQoCAAAAAwAJAAAATGltaXRMb0xvAQPGAAAuAETG" +
           "AAAAAAv/////AQH/////AAAAABVgiQoCAAAAAwAJAAAATGltaXRIaUhpAQPHAAAuAETHAAAAAAv/////" +
           "AQH/////AAAAABVgiQoCAAAAAwAQAAAARW5naW5lZXJpbmdVbml0cwEDyAAALgBEyAAAAAAM/////wEB" +
           "/////wAAAAAVYIkKAgAAAAMACAAAAEFwZXJ0dXJlAQObOgAuAESbOgAAAAv/////AQH/////AAAAABVg" +
           "iQoCAAAAAwAIAAAARGVhZGJhbmQBA5w6AC4ARJw6AAAAC/////8BAf////8AAAAA";
        #endregion
        #endif
        #endregion

        #region Public Properties
        /// <remarks />
        public PropertyState<string> DisplayFormat
        {
            get
            {
                return m_displayFormat;
            }

            set
            {
                if (!Object.ReferenceEquals(m_displayFormat, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_displayFormat = value;
            }
        }

        /// <remarks />
        public PropertyState<AnalogConversionDataType> Conversion
        {
            get
            {
                return m_conversion;
            }

            set
            {
                if (!Object.ReferenceEquals(m_conversion, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_conversion = value;
            }
        }

        /// <remarks />
        public PropertyState<bool> Clamping
        {
            get
            {
                return m_clamping;
            }

            set
            {
                if (!Object.ReferenceEquals(m_clamping, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_clamping = value;
            }
        }

        /// <remarks />
        public PropertyState<double> EuLo
        {
            get
            {
                return m_euLo;
            }

            set
            {
                if (!Object.ReferenceEquals(m_euLo, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_euLo = value;
            }
        }

        /// <remarks />
        public PropertyState<double> EuHi
        {
            get
            {
                return m_euHi;
            }

            set
            {
                if (!Object.ReferenceEquals(m_euHi, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_euHi = value;
            }
        }

        /// <remarks />
        public PropertyState<double> IrLo
        {
            get
            {
                return m_irLo;
            }

            set
            {
                if (!Object.ReferenceEquals(m_irLo, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_irLo = value;
            }
        }

        /// <remarks />
        public PropertyState<double> IrHi
        {
            get
            {
                return m_irHi;
            }

            set
            {
                if (!Object.ReferenceEquals(m_irHi, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_irHi = value;
            }
        }

        /// <remarks />
        public PropertyState<double> LimitLo
        {
            get
            {
                return m_limitLo;
            }

            set
            {
                if (!Object.ReferenceEquals(m_limitLo, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_limitLo = value;
            }
        }

        /// <remarks />
        public PropertyState<double> LimitHi
        {
            get
            {
                return m_limitHi;
            }

            set
            {
                if (!Object.ReferenceEquals(m_limitHi, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_limitHi = value;
            }
        }

        /// <remarks />
        public PropertyState<double> LimitLoLo
        {
            get
            {
                return m_limitLoLo;
            }

            set
            {
                if (!Object.ReferenceEquals(m_limitLoLo, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_limitLoLo = value;
            }
        }

        /// <remarks />
        public PropertyState<double> LimitHiHi
        {
            get
            {
                return m_limitHiHi;
            }

            set
            {
                if (!Object.ReferenceEquals(m_limitHiHi, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_limitHiHi = value;
            }
        }

        /// <remarks />
        public PropertyState<string> EngineeringUnits
        {
            get
            {
                return m_engineeringUnits;
            }

            set
            {
                if (!Object.ReferenceEquals(m_engineeringUnits, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_engineeringUnits = value;
            }
        }

        /// <remarks />
        public PropertyState<double> Aperture
        {
            get
            {
                return m_aperture;
            }

            set
            {
                if (!Object.ReferenceEquals(m_aperture, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_aperture = value;
            }
        }

        /// <remarks />
        public PropertyState<double> Deadband
        {
            get
            {
                return m_deadband;
            }

            set
            {
                if (!Object.ReferenceEquals(m_deadband, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_deadband = value;
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
            if (m_displayFormat != null)
            {
                children.Add(m_displayFormat);
            }

            if (m_conversion != null)
            {
                children.Add(m_conversion);
            }

            if (m_clamping != null)
            {
                children.Add(m_clamping);
            }

            if (m_euLo != null)
            {
                children.Add(m_euLo);
            }

            if (m_euHi != null)
            {
                children.Add(m_euHi);
            }

            if (m_irLo != null)
            {
                children.Add(m_irLo);
            }

            if (m_irHi != null)
            {
                children.Add(m_irHi);
            }

            if (m_limitLo != null)
            {
                children.Add(m_limitLo);
            }

            if (m_limitHi != null)
            {
                children.Add(m_limitHi);
            }

            if (m_limitLoLo != null)
            {
                children.Add(m_limitLoLo);
            }

            if (m_limitHiHi != null)
            {
                children.Add(m_limitHiHi);
            }

            if (m_engineeringUnits != null)
            {
                children.Add(m_engineeringUnits);
            }

            if (m_aperture != null)
            {
                children.Add(m_aperture);
            }

            if (m_deadband != null)
            {
                children.Add(m_deadband);
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
                case Telecontrol.DataItems.BrowseNames.DisplayFormat:
                {
                    if (createOrReplace)
                    {
                        if (DisplayFormat == null)
                        {
                            if (replacement == null)
                            {
                                DisplayFormat = new PropertyState<string>(this);
                            }
                            else
                            {
                                DisplayFormat = (PropertyState<string>)replacement;
                            }
                        }
                    }

                    instance = DisplayFormat;
                    break;
                }

                case Telecontrol.DataItems.BrowseNames.Conversion:
                {
                    if (createOrReplace)
                    {
                        if (Conversion == null)
                        {
                            if (replacement == null)
                            {
                                Conversion = new PropertyState<AnalogConversionDataType>(this);
                            }
                            else
                            {
                                Conversion = (PropertyState<AnalogConversionDataType>)replacement;
                            }
                        }
                    }

                    instance = Conversion;
                    break;
                }

                case Telecontrol.DataItems.BrowseNames.Clamping:
                {
                    if (createOrReplace)
                    {
                        if (Clamping == null)
                        {
                            if (replacement == null)
                            {
                                Clamping = new PropertyState<bool>(this);
                            }
                            else
                            {
                                Clamping = (PropertyState<bool>)replacement;
                            }
                        }
                    }

                    instance = Clamping;
                    break;
                }

                case Telecontrol.DataItems.BrowseNames.EuLo:
                {
                    if (createOrReplace)
                    {
                        if (EuLo == null)
                        {
                            if (replacement == null)
                            {
                                EuLo = new PropertyState<double>(this);
                            }
                            else
                            {
                                EuLo = (PropertyState<double>)replacement;
                            }
                        }
                    }

                    instance = EuLo;
                    break;
                }

                case Telecontrol.DataItems.BrowseNames.EuHi:
                {
                    if (createOrReplace)
                    {
                        if (EuHi == null)
                        {
                            if (replacement == null)
                            {
                                EuHi = new PropertyState<double>(this);
                            }
                            else
                            {
                                EuHi = (PropertyState<double>)replacement;
                            }
                        }
                    }

                    instance = EuHi;
                    break;
                }

                case Telecontrol.DataItems.BrowseNames.IrLo:
                {
                    if (createOrReplace)
                    {
                        if (IrLo == null)
                        {
                            if (replacement == null)
                            {
                                IrLo = new PropertyState<double>(this);
                            }
                            else
                            {
                                IrLo = (PropertyState<double>)replacement;
                            }
                        }
                    }

                    instance = IrLo;
                    break;
                }

                case Telecontrol.DataItems.BrowseNames.IrHi:
                {
                    if (createOrReplace)
                    {
                        if (IrHi == null)
                        {
                            if (replacement == null)
                            {
                                IrHi = new PropertyState<double>(this);
                            }
                            else
                            {
                                IrHi = (PropertyState<double>)replacement;
                            }
                        }
                    }

                    instance = IrHi;
                    break;
                }

                case Telecontrol.DataItems.BrowseNames.LimitLo:
                {
                    if (createOrReplace)
                    {
                        if (LimitLo == null)
                        {
                            if (replacement == null)
                            {
                                LimitLo = new PropertyState<double>(this);
                            }
                            else
                            {
                                LimitLo = (PropertyState<double>)replacement;
                            }
                        }
                    }

                    instance = LimitLo;
                    break;
                }

                case Telecontrol.DataItems.BrowseNames.LimitHi:
                {
                    if (createOrReplace)
                    {
                        if (LimitHi == null)
                        {
                            if (replacement == null)
                            {
                                LimitHi = new PropertyState<double>(this);
                            }
                            else
                            {
                                LimitHi = (PropertyState<double>)replacement;
                            }
                        }
                    }

                    instance = LimitHi;
                    break;
                }

                case Telecontrol.DataItems.BrowseNames.LimitLoLo:
                {
                    if (createOrReplace)
                    {
                        if (LimitLoLo == null)
                        {
                            if (replacement == null)
                            {
                                LimitLoLo = new PropertyState<double>(this);
                            }
                            else
                            {
                                LimitLoLo = (PropertyState<double>)replacement;
                            }
                        }
                    }

                    instance = LimitLoLo;
                    break;
                }

                case Telecontrol.DataItems.BrowseNames.LimitHiHi:
                {
                    if (createOrReplace)
                    {
                        if (LimitHiHi == null)
                        {
                            if (replacement == null)
                            {
                                LimitHiHi = new PropertyState<double>(this);
                            }
                            else
                            {
                                LimitHiHi = (PropertyState<double>)replacement;
                            }
                        }
                    }

                    instance = LimitHiHi;
                    break;
                }

                case Telecontrol.DataItems.BrowseNames.EngineeringUnits:
                {
                    if (createOrReplace)
                    {
                        if (EngineeringUnits == null)
                        {
                            if (replacement == null)
                            {
                                EngineeringUnits = new PropertyState<string>(this);
                            }
                            else
                            {
                                EngineeringUnits = (PropertyState<string>)replacement;
                            }
                        }
                    }

                    instance = EngineeringUnits;
                    break;
                }

                case Telecontrol.DataItems.BrowseNames.Aperture:
                {
                    if (createOrReplace)
                    {
                        if (Aperture == null)
                        {
                            if (replacement == null)
                            {
                                Aperture = new PropertyState<double>(this);
                            }
                            else
                            {
                                Aperture = (PropertyState<double>)replacement;
                            }
                        }
                    }

                    instance = Aperture;
                    break;
                }

                case Telecontrol.DataItems.BrowseNames.Deadband:
                {
                    if (createOrReplace)
                    {
                        if (Deadband == null)
                        {
                            if (replacement == null)
                            {
                                Deadband = new PropertyState<double>(this);
                            }
                            else
                            {
                                Deadband = (PropertyState<double>)replacement;
                            }
                        }
                    }

                    instance = Deadband;
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
        private PropertyState<string> m_displayFormat;
        private PropertyState<AnalogConversionDataType> m_conversion;
        private PropertyState<bool> m_clamping;
        private PropertyState<double> m_euLo;
        private PropertyState<double> m_euHi;
        private PropertyState<double> m_irLo;
        private PropertyState<double> m_irHi;
        private PropertyState<double> m_limitLo;
        private PropertyState<double> m_limitHi;
        private PropertyState<double> m_limitLoLo;
        private PropertyState<double> m_limitHiHi;
        private PropertyState<string> m_engineeringUnits;
        private PropertyState<double> m_aperture;
        private PropertyState<double> m_deadband;
        #endregion
    }
    #endif
    #endregion

    #region TsFormatState Class
    #if (!OPCUA_EXCLUDE_TsFormatState)
    /// <summary>
    /// Stores an instance of the TsFormatType ObjectType.
    /// </summary>
    /// <exclude />
    [System.CodeDom.Compiler.GeneratedCodeAttribute("Opc.Ua.ModelCompiler", "1.0.0.0")]
    public partial class TsFormatState : BaseObjectState
    {
        #region Constructors
        /// <summary>
        /// Initializes the type with its default attribute values.
        /// </summary>
        public TsFormatState(NodeState parent) : base(parent)
        {
        }

        /// <summary>
        /// Returns the id of the default type definition node for the instance.
        /// </summary>
        protected override NodeId GetDefaultTypeDefinitionId(NamespaceTable namespaceUris)
        {
            return Opc.Ua.NodeId.Create(Telecontrol.DataItems.ObjectTypes.TsFormatType, Telecontrol.DataItems.Namespaces.TelecontrolDataItems, namespaceUris);
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
           "AwAAACEAAABodHRwOi8vdGVsZWNvbnRyb2wucnUvb3BjdWEvc2NhZGEjAAAAaHR0cDovL3RlbGVjb250" +
           "cm9sLnJ1L29wY3VhL2hpc3RvcnkmAAAAaHR0cDovL3RlbGVjb250cm9sLnJ1L29wY3VhL2RhdGFfaXRl" +
           "bXP/////BGCAAgEAAAADABQAAABUc0Zvcm1hdFR5cGVJbnN0YW5jZQEDUAABA1AAUAAAAAEAAAABARcA" +
           "AQEDGwAEAAAAFWCJCgIAAAADAAkAAABPcGVuTGFiZWwBA1EAAC4ARFEAAAAADP////8BAf////8AAAAA" +
           "FWCJCgIAAAADAAoAAABDbG9zZUxhYmVsAQNSAAAuAERSAAAAAAz/////AQH/////AAAAABVgiQoCAAAA" +
           "AwAJAAAAT3BlbkNvbG9yAQNTAAAuAERTAAAAAAf/////AQH/////AAAAABVgiQoCAAAAAwAKAAAAQ2xv" +
           "c2VDb2xvcgEDVAAALgBEVAAAAAAH/////wEB/////wAAAAA=";
        #endregion
        #endif
        #endregion

        #region Public Properties
        /// <remarks />
        public PropertyState<string> OpenLabel
        {
            get
            {
                return m_openLabel;
            }

            set
            {
                if (!Object.ReferenceEquals(m_openLabel, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_openLabel = value;
            }
        }

        /// <remarks />
        public PropertyState<string> CloseLabel
        {
            get
            {
                return m_closeLabel;
            }

            set
            {
                if (!Object.ReferenceEquals(m_closeLabel, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_closeLabel = value;
            }
        }

        /// <remarks />
        public PropertyState<uint> OpenColor
        {
            get
            {
                return m_openColor;
            }

            set
            {
                if (!Object.ReferenceEquals(m_openColor, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_openColor = value;
            }
        }

        /// <remarks />
        public PropertyState<uint> CloseColor
        {
            get
            {
                return m_closeColor;
            }

            set
            {
                if (!Object.ReferenceEquals(m_closeColor, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_closeColor = value;
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
            if (m_openLabel != null)
            {
                children.Add(m_openLabel);
            }

            if (m_closeLabel != null)
            {
                children.Add(m_closeLabel);
            }

            if (m_openColor != null)
            {
                children.Add(m_openColor);
            }

            if (m_closeColor != null)
            {
                children.Add(m_closeColor);
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
                case Telecontrol.DataItems.BrowseNames.OpenLabel:
                {
                    if (createOrReplace)
                    {
                        if (OpenLabel == null)
                        {
                            if (replacement == null)
                            {
                                OpenLabel = new PropertyState<string>(this);
                            }
                            else
                            {
                                OpenLabel = (PropertyState<string>)replacement;
                            }
                        }
                    }

                    instance = OpenLabel;
                    break;
                }

                case Telecontrol.DataItems.BrowseNames.CloseLabel:
                {
                    if (createOrReplace)
                    {
                        if (CloseLabel == null)
                        {
                            if (replacement == null)
                            {
                                CloseLabel = new PropertyState<string>(this);
                            }
                            else
                            {
                                CloseLabel = (PropertyState<string>)replacement;
                            }
                        }
                    }

                    instance = CloseLabel;
                    break;
                }

                case Telecontrol.DataItems.BrowseNames.OpenColor:
                {
                    if (createOrReplace)
                    {
                        if (OpenColor == null)
                        {
                            if (replacement == null)
                            {
                                OpenColor = new PropertyState<uint>(this);
                            }
                            else
                            {
                                OpenColor = (PropertyState<uint>)replacement;
                            }
                        }
                    }

                    instance = OpenColor;
                    break;
                }

                case Telecontrol.DataItems.BrowseNames.CloseColor:
                {
                    if (createOrReplace)
                    {
                        if (CloseColor == null)
                        {
                            if (replacement == null)
                            {
                                CloseColor = new PropertyState<uint>(this);
                            }
                            else
                            {
                                CloseColor = (PropertyState<uint>)replacement;
                            }
                        }
                    }

                    instance = CloseColor;
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
        private PropertyState<string> m_openLabel;
        private PropertyState<string> m_closeLabel;
        private PropertyState<uint> m_openColor;
        private PropertyState<uint> m_closeColor;
        #endregion
    }
    #endif
    #endregion

    #region SimulationSignalState Class
    #if (!OPCUA_EXCLUDE_SimulationSignalState)
    /// <summary>
    /// Stores an instance of the SimulationSignalType VariableType.
    /// </summary>
    /// <exclude />
    [System.CodeDom.Compiler.GeneratedCodeAttribute("Opc.Ua.ModelCompiler", "1.0.0.0")]
    public partial class SimulationSignalState : BaseVariableState
    {
        #region Constructors
        /// <summary>
        /// Initializes the type with its default attribute values.
        /// </summary>
        public SimulationSignalState(NodeState parent) : base(parent)
        {
        }

        /// <summary>
        /// Returns the id of the default type definition node for the instance.
        /// </summary>
        protected override NodeId GetDefaultTypeDefinitionId(NamespaceTable namespaceUris)
        {
            return Opc.Ua.NodeId.Create(Telecontrol.DataItems.VariableTypes.SimulationSignalType, Telecontrol.DataItems.Namespaces.TelecontrolDataItems, namespaceUris);
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
           "AwAAACEAAABodHRwOi8vdGVsZWNvbnRyb2wucnUvb3BjdWEvc2NhZGEjAAAAaHR0cDovL3RlbGVjb250" +
           "cm9sLnJ1L29wY3VhL2hpc3RvcnkmAAAAaHR0cDovL3RlbGVjb250cm9sLnJ1L29wY3VhL2RhdGFfaXRl" +
           "bXP/////FWCJAgIAAAADABwAAABTaW11bGF0aW9uU2lnbmFsVHlwZUluc3RhbmNlAQNDAAEDQwBDAAAA" +
           "ABj+////AQEBAAAAAQEXAAEBAxwABAAAABVgiQoCAAAAAwAIAAAARnVuY3Rpb24BA7A6AC4ARLA6AAAB" +
           "A646/////wEB/////wAAAAAVYKkKAgAAAAMABgAAAFBlcmlvZAED5gAALgBE5gAAAAsAAAAAAEztQAAH" +
           "/////wEB/////wAAAAAVYIkKAgAAAAMABQAAAFBoYXNlAQPnAAAuAETnAAAAAAf/////AQH/////AAAA" +
           "ABVgqQoCAAAAAwAOAAAAVXBkYXRlSW50ZXJ2YWwBA+gAAC4AROgAAAALAAAAAABAj0AAB/////8BAf//" +
           "//8AAAAA";
        #endregion
        #endif
        #endregion

        #region Public Properties
        /// <remarks />
        public PropertyState<SimulationFunctionDataType> Function
        {
            get
            {
                return m_function;
            }

            set
            {
                if (!Object.ReferenceEquals(m_function, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_function = value;
            }
        }

        /// <remarks />
        public PropertyState<uint> Period
        {
            get
            {
                return m_period;
            }

            set
            {
                if (!Object.ReferenceEquals(m_period, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_period = value;
            }
        }

        /// <remarks />
        public PropertyState<uint> Phase
        {
            get
            {
                return m_phase;
            }

            set
            {
                if (!Object.ReferenceEquals(m_phase, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_phase = value;
            }
        }

        /// <remarks />
        public PropertyState<uint> UpdateInterval
        {
            get
            {
                return m_updateInterval;
            }

            set
            {
                if (!Object.ReferenceEquals(m_updateInterval, value))
                {
                    ChangeMasks |= NodeStateChangeMasks.Children;
                }

                m_updateInterval = value;
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
            if (m_function != null)
            {
                children.Add(m_function);
            }

            if (m_period != null)
            {
                children.Add(m_period);
            }

            if (m_phase != null)
            {
                children.Add(m_phase);
            }

            if (m_updateInterval != null)
            {
                children.Add(m_updateInterval);
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
                case Telecontrol.DataItems.BrowseNames.Function:
                {
                    if (createOrReplace)
                    {
                        if (Function == null)
                        {
                            if (replacement == null)
                            {
                                Function = new PropertyState<SimulationFunctionDataType>(this);
                            }
                            else
                            {
                                Function = (PropertyState<SimulationFunctionDataType>)replacement;
                            }
                        }
                    }

                    instance = Function;
                    break;
                }

                case Telecontrol.DataItems.BrowseNames.Period:
                {
                    if (createOrReplace)
                    {
                        if (Period == null)
                        {
                            if (replacement == null)
                            {
                                Period = new PropertyState<uint>(this);
                            }
                            else
                            {
                                Period = (PropertyState<uint>)replacement;
                            }
                        }
                    }

                    instance = Period;
                    break;
                }

                case Telecontrol.DataItems.BrowseNames.Phase:
                {
                    if (createOrReplace)
                    {
                        if (Phase == null)
                        {
                            if (replacement == null)
                            {
                                Phase = new PropertyState<uint>(this);
                            }
                            else
                            {
                                Phase = (PropertyState<uint>)replacement;
                            }
                        }
                    }

                    instance = Phase;
                    break;
                }

                case Telecontrol.DataItems.BrowseNames.UpdateInterval:
                {
                    if (createOrReplace)
                    {
                        if (UpdateInterval == null)
                        {
                            if (replacement == null)
                            {
                                UpdateInterval = new PropertyState<uint>(this);
                            }
                            else
                            {
                                UpdateInterval = (PropertyState<uint>)replacement;
                            }
                        }
                    }

                    instance = UpdateInterval;
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
        private PropertyState<SimulationFunctionDataType> m_function;
        private PropertyState<uint> m_period;
        private PropertyState<uint> m_phase;
        private PropertyState<uint> m_updateInterval;
        #endregion
    }

    #region SimulationSignalState<T> Class
    /// <summary>
    /// A typed version of the SimulationSignalType variable.
    /// </summary>
    /// <exclude />
    [System.CodeDom.Compiler.GeneratedCodeAttribute("Opc.Ua.ModelCompiler", "1.0.0.0")]
    public class SimulationSignalState<T> : SimulationSignalState
    {
        #region Constructors
        /// <summary>
        /// Initializes the instance with its defalt attribute values.
        /// </summary>
        public SimulationSignalState(NodeState parent) : base(parent)
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

    #region AliasState Class
    #if (!OPCUA_EXCLUDE_AliasState)
    /// <summary>
    /// Stores an instance of the AliasType ObjectType.
    /// </summary>
    /// <exclude />
    [System.CodeDom.Compiler.GeneratedCodeAttribute("Opc.Ua.ModelCompiler", "1.0.0.0")]
    public partial class AliasState : BaseObjectState
    {
        #region Constructors
        /// <summary>
        /// Initializes the type with its default attribute values.
        /// </summary>
        public AliasState(NodeState parent) : base(parent)
        {
        }

        /// <summary>
        /// Returns the id of the default type definition node for the instance.
        /// </summary>
        protected override NodeId GetDefaultTypeDefinitionId(NamespaceTable namespaceUris)
        {
            return Opc.Ua.NodeId.Create(Telecontrol.DataItems.ObjectTypes.AliasType, Telecontrol.DataItems.Namespaces.TelecontrolDataItems, namespaceUris);
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
           "AwAAACEAAABodHRwOi8vdGVsZWNvbnRyb2wucnUvb3BjdWEvc2NhZGEjAAAAaHR0cDovL3RlbGVjb250" +
           "cm9sLnJ1L29wY3VhL2hpc3RvcnkmAAAAaHR0cDovL3RlbGVjb250cm9sLnJ1L29wY3VhL2RhdGFfaXRl" +
           "bXP/////BGCAAgEAAAADABEAAABBbGlhc1R5cGVJbnN0YW5jZQEDrToBA606rToAAP////8AAAAA";
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
}