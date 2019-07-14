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
    #region AnalogConversionDataType Enumeration
    #if (!OPCUA_EXCLUDE_AnalogConversionDataType)
    /// <summary>
    /// 
    /// </summary>
    /// <exclude />
    [System.CodeDom.Compiler.GeneratedCodeAttribute("Opc.Ua.ModelCompiler", "1.0.0.0")]
    [DataContract(Namespace = Telecontrol.DataItems.Namespaces.TelecontrolDataItems)]
    public enum AnalogConversionDataType
    {
        /// <remarks />
        [EnumMember(Value = "None_0")]
        None = 0,

        /// <remarks />
        [EnumMember(Value = "Linear_1")]
        Linear = 1,
    }

    #region AnalogConversionDataTypeCollection Class
    /// <summary>
    /// A collection of AnalogConversionDataType objects.
    /// </summary>
    /// <exclude />
    [System.CodeDom.Compiler.GeneratedCodeAttribute("Opc.Ua.ModelCompiler", "1.0.0.0")]
    [CollectionDataContract(Name = "ListOfAnalogConversionDataType", Namespace = Telecontrol.DataItems.Namespaces.TelecontrolDataItems, ItemName = "AnalogConversionDataType")]
    #if !NET_STANDARD
    public partial class AnalogConversionDataTypeCollection : List<AnalogConversionDataType>, ICloneable
    #else
    public partial class AnalogConversionDataTypeCollection : List<AnalogConversionDataType>
    #endif
    {
        #region Constructors
        /// <summary>
        /// Initializes the collection with default values.
        /// </summary>
        public AnalogConversionDataTypeCollection() {}

        /// <summary>
        /// Initializes the collection with an initial capacity.
        /// </summary>
        public AnalogConversionDataTypeCollection(int capacity) : base(capacity) {}

        /// <summary>
        /// Initializes the collection with another collection.
        /// </summary>
        public AnalogConversionDataTypeCollection(IEnumerable<AnalogConversionDataType> collection) : base(collection) {}
        #endregion

        #region Static Operators
        /// <summary>
        /// Converts an array to a collection.
        /// </summary>
        public static implicit operator AnalogConversionDataTypeCollection(AnalogConversionDataType[] values)
        {
            if (values != null)
            {
                return new AnalogConversionDataTypeCollection(values);
            }

            return new AnalogConversionDataTypeCollection();
        }

        /// <summary>
        /// Converts a collection to an array.
        /// </summary>
        public static explicit operator AnalogConversionDataType[](AnalogConversionDataTypeCollection values)
        {
            if (values != null)
            {
                return values.ToArray();
            }

            return null;
        }
        #endregion

        #if !NET_STANDARD
        #region ICloneable Methods
        /// <summary>
        /// Creates a deep copy of the collection.
        /// </summary>
        public object Clone()
        {
            return (AnalogConversionDataTypeCollection)this.MemberwiseClone();
        }
        #endregion
        #endif

        /// <summary cref="Object.MemberwiseClone" />
        public new object MemberwiseClone()
        {
            AnalogConversionDataTypeCollection clone = new AnalogConversionDataTypeCollection(this.Count);

            for (int ii = 0; ii < this.Count; ii++)
            {
                clone.Add((AnalogConversionDataType)Utils.Clone(this[ii]));
            }

            return clone;
        }
    }
    #endregion
    #endif
    #endregion

    #region SimulationFunctionDataType Enumeration
    #if (!OPCUA_EXCLUDE_SimulationFunctionDataType)
    /// <summary>
    /// 
    /// </summary>
    /// <exclude />
    [System.CodeDom.Compiler.GeneratedCodeAttribute("Opc.Ua.ModelCompiler", "1.0.0.0")]
    [DataContract(Namespace = Telecontrol.DataItems.Namespaces.TelecontrolDataItems)]
    public enum SimulationFunctionDataType
    {
        /// <remarks />
        [EnumMember(Value = "Random_0")]
        Random = 0,

        /// <remarks />
        [EnumMember(Value = "Ramp_1")]
        Ramp = 1,

        /// <remarks />
        [EnumMember(Value = "Step_2")]
        Step = 2,

        /// <remarks />
        [EnumMember(Value = "Sin_3")]
        Sin = 3,

        /// <remarks />
        [EnumMember(Value = "Cos_4")]
        Cos = 4,
    }

    #region SimulationFunctionDataTypeCollection Class
    /// <summary>
    /// A collection of SimulationFunctionDataType objects.
    /// </summary>
    /// <exclude />
    [System.CodeDom.Compiler.GeneratedCodeAttribute("Opc.Ua.ModelCompiler", "1.0.0.0")]
    [CollectionDataContract(Name = "ListOfSimulationFunctionDataType", Namespace = Telecontrol.DataItems.Namespaces.TelecontrolDataItems, ItemName = "SimulationFunctionDataType")]
    #if !NET_STANDARD
    public partial class SimulationFunctionDataTypeCollection : List<SimulationFunctionDataType>, ICloneable
    #else
    public partial class SimulationFunctionDataTypeCollection : List<SimulationFunctionDataType>
    #endif
    {
        #region Constructors
        /// <summary>
        /// Initializes the collection with default values.
        /// </summary>
        public SimulationFunctionDataTypeCollection() {}

        /// <summary>
        /// Initializes the collection with an initial capacity.
        /// </summary>
        public SimulationFunctionDataTypeCollection(int capacity) : base(capacity) {}

        /// <summary>
        /// Initializes the collection with another collection.
        /// </summary>
        public SimulationFunctionDataTypeCollection(IEnumerable<SimulationFunctionDataType> collection) : base(collection) {}
        #endregion

        #region Static Operators
        /// <summary>
        /// Converts an array to a collection.
        /// </summary>
        public static implicit operator SimulationFunctionDataTypeCollection(SimulationFunctionDataType[] values)
        {
            if (values != null)
            {
                return new SimulationFunctionDataTypeCollection(values);
            }

            return new SimulationFunctionDataTypeCollection();
        }

        /// <summary>
        /// Converts a collection to an array.
        /// </summary>
        public static explicit operator SimulationFunctionDataType[](SimulationFunctionDataTypeCollection values)
        {
            if (values != null)
            {
                return values.ToArray();
            }

            return null;
        }
        #endregion

        #if !NET_STANDARD
        #region ICloneable Methods
        /// <summary>
        /// Creates a deep copy of the collection.
        /// </summary>
        public object Clone()
        {
            return (SimulationFunctionDataTypeCollection)this.MemberwiseClone();
        }
        #endregion
        #endif

        /// <summary cref="Object.MemberwiseClone" />
        public new object MemberwiseClone()
        {
            SimulationFunctionDataTypeCollection clone = new SimulationFunctionDataTypeCollection(this.Count);

            for (int ii = 0; ii < this.Count; ii++)
            {
                clone.Add((SimulationFunctionDataType)Utils.Clone(this[ii]));
            }

            return clone;
        }
    }
    #endregion
    #endif
    #endregion
}