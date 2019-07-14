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
    #region Iec60870ProtocolDataType Enumeration
    #if (!OPCUA_EXCLUDE_Iec60870ProtocolDataType)
    /// <summary>
    /// 
    /// </summary>
    /// <exclude />
    [System.CodeDom.Compiler.GeneratedCodeAttribute("Opc.Ua.ModelCompiler", "1.0.0.0")]
    [DataContract(Namespace = Telecontrol.Devices.Namespaces.TelecontrolDevices)]
    public enum Iec60870ProtocolDataType
    {
        /// <remarks />
        [EnumMember(Value = "IEC-60870-104_0")]
        IEC-60870-104 = 0,

        /// <remarks />
        [EnumMember(Value = "IEC-60870-101_1")]
        IEC-60870-101 = 1,
    }

    #region Iec60870ProtocolDataTypeCollection Class
    /// <summary>
    /// A collection of Iec60870ProtocolDataType objects.
    /// </summary>
    /// <exclude />
    [System.CodeDom.Compiler.GeneratedCodeAttribute("Opc.Ua.ModelCompiler", "1.0.0.0")]
    [CollectionDataContract(Name = "ListOfIec60870ProtocolDataType", Namespace = Telecontrol.Devices.Namespaces.TelecontrolDevices, ItemName = "Iec60870ProtocolDataType")]
    #if !NET_STANDARD
    public partial class Iec60870ProtocolDataTypeCollection : List<Iec60870ProtocolDataType>, ICloneable
    #else
    public partial class Iec60870ProtocolDataTypeCollection : List<Iec60870ProtocolDataType>
    #endif
    {
        #region Constructors
        /// <summary>
        /// Initializes the collection with default values.
        /// </summary>
        public Iec60870ProtocolDataTypeCollection() {}

        /// <summary>
        /// Initializes the collection with an initial capacity.
        /// </summary>
        public Iec60870ProtocolDataTypeCollection(int capacity) : base(capacity) {}

        /// <summary>
        /// Initializes the collection with another collection.
        /// </summary>
        public Iec60870ProtocolDataTypeCollection(IEnumerable<Iec60870ProtocolDataType> collection) : base(collection) {}
        #endregion

        #region Static Operators
        /// <summary>
        /// Converts an array to a collection.
        /// </summary>
        public static implicit operator Iec60870ProtocolDataTypeCollection(Iec60870ProtocolDataType[] values)
        {
            if (values != null)
            {
                return new Iec60870ProtocolDataTypeCollection(values);
            }

            return new Iec60870ProtocolDataTypeCollection();
        }

        /// <summary>
        /// Converts a collection to an array.
        /// </summary>
        public static explicit operator Iec60870ProtocolDataType[](Iec60870ProtocolDataTypeCollection values)
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
            return (Iec60870ProtocolDataTypeCollection)this.MemberwiseClone();
        }
        #endregion
        #endif

        /// <summary cref="Object.MemberwiseClone" />
        public new object MemberwiseClone()
        {
            Iec60870ProtocolDataTypeCollection clone = new Iec60870ProtocolDataTypeCollection(this.Count);

            for (int ii = 0; ii < this.Count; ii++)
            {
                clone.Add((Iec60870ProtocolDataType)Utils.Clone(this[ii]));
            }

            return clone;
        }
    }
    #endregion
    #endif
    #endregion

    #region Iec60870ModeDataType Enumeration
    #if (!OPCUA_EXCLUDE_Iec60870ModeDataType)
    /// <summary>
    /// 
    /// </summary>
    /// <exclude />
    [System.CodeDom.Compiler.GeneratedCodeAttribute("Opc.Ua.ModelCompiler", "1.0.0.0")]
    [DataContract(Namespace = Telecontrol.Devices.Namespaces.TelecontrolDevices)]
    public enum Iec60870ModeDataType
    {
        /// <remarks />
        [EnumMember(Value = "Client_0")]
        Client = 0,

        /// <remarks />
        [EnumMember(Value = "Server_1")]
        Server = 1,

        /// <remarks />
        [EnumMember(Value = "Passive Client_2")]
        Passive Client = 2,
    }

    #region Iec60870ModeDataTypeCollection Class
    /// <summary>
    /// A collection of Iec60870ModeDataType objects.
    /// </summary>
    /// <exclude />
    [System.CodeDom.Compiler.GeneratedCodeAttribute("Opc.Ua.ModelCompiler", "1.0.0.0")]
    [CollectionDataContract(Name = "ListOfIec60870ModeDataType", Namespace = Telecontrol.Devices.Namespaces.TelecontrolDevices, ItemName = "Iec60870ModeDataType")]
    #if !NET_STANDARD
    public partial class Iec60870ModeDataTypeCollection : List<Iec60870ModeDataType>, ICloneable
    #else
    public partial class Iec60870ModeDataTypeCollection : List<Iec60870ModeDataType>
    #endif
    {
        #region Constructors
        /// <summary>
        /// Initializes the collection with default values.
        /// </summary>
        public Iec60870ModeDataTypeCollection() {}

        /// <summary>
        /// Initializes the collection with an initial capacity.
        /// </summary>
        public Iec60870ModeDataTypeCollection(int capacity) : base(capacity) {}

        /// <summary>
        /// Initializes the collection with another collection.
        /// </summary>
        public Iec60870ModeDataTypeCollection(IEnumerable<Iec60870ModeDataType> collection) : base(collection) {}
        #endregion

        #region Static Operators
        /// <summary>
        /// Converts an array to a collection.
        /// </summary>
        public static implicit operator Iec60870ModeDataTypeCollection(Iec60870ModeDataType[] values)
        {
            if (values != null)
            {
                return new Iec60870ModeDataTypeCollection(values);
            }

            return new Iec60870ModeDataTypeCollection();
        }

        /// <summary>
        /// Converts a collection to an array.
        /// </summary>
        public static explicit operator Iec60870ModeDataType[](Iec60870ModeDataTypeCollection values)
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
            return (Iec60870ModeDataTypeCollection)this.MemberwiseClone();
        }
        #endregion
        #endif

        /// <summary cref="Object.MemberwiseClone" />
        public new object MemberwiseClone()
        {
            Iec60870ModeDataTypeCollection clone = new Iec60870ModeDataTypeCollection(this.Count);

            for (int ii = 0; ii < this.Count; ii++)
            {
                clone.Add((Iec60870ModeDataType)Utils.Clone(this[ii]));
            }

            return clone;
        }
    }
    #endregion
    #endif
    #endregion

    #region ModbusLinkProtocol Enumeration
    #if (!OPCUA_EXCLUDE_ModbusLinkProtocol)
    /// <summary>
    /// 
    /// </summary>
    /// <exclude />
    [System.CodeDom.Compiler.GeneratedCodeAttribute("Opc.Ua.ModelCompiler", "1.0.0.0")]
    [DataContract(Namespace = Telecontrol.Devices.Namespaces.TelecontrolDevices)]
    public enum ModbusLinkProtocol
    {
        /// <remarks />
        [EnumMember(Value = "RTU_0")]
        RTU = 0,

        /// <remarks />
        [EnumMember(Value = "ASCII_1")]
        ASCII = 1,

        /// <remarks />
        [EnumMember(Value = "TCP_2")]
        TCP = 2,
    }

    #region ModbusLinkProtocolCollection Class
    /// <summary>
    /// A collection of ModbusLinkProtocol objects.
    /// </summary>
    /// <exclude />
    [System.CodeDom.Compiler.GeneratedCodeAttribute("Opc.Ua.ModelCompiler", "1.0.0.0")]
    [CollectionDataContract(Name = "ListOfModbusLinkProtocol", Namespace = Telecontrol.Devices.Namespaces.TelecontrolDevices, ItemName = "ModbusLinkProtocol")]
    #if !NET_STANDARD
    public partial class ModbusLinkProtocolCollection : List<ModbusLinkProtocol>, ICloneable
    #else
    public partial class ModbusLinkProtocolCollection : List<ModbusLinkProtocol>
    #endif
    {
        #region Constructors
        /// <summary>
        /// Initializes the collection with default values.
        /// </summary>
        public ModbusLinkProtocolCollection() {}

        /// <summary>
        /// Initializes the collection with an initial capacity.
        /// </summary>
        public ModbusLinkProtocolCollection(int capacity) : base(capacity) {}

        /// <summary>
        /// Initializes the collection with another collection.
        /// </summary>
        public ModbusLinkProtocolCollection(IEnumerable<ModbusLinkProtocol> collection) : base(collection) {}
        #endregion

        #region Static Operators
        /// <summary>
        /// Converts an array to a collection.
        /// </summary>
        public static implicit operator ModbusLinkProtocolCollection(ModbusLinkProtocol[] values)
        {
            if (values != null)
            {
                return new ModbusLinkProtocolCollection(values);
            }

            return new ModbusLinkProtocolCollection();
        }

        /// <summary>
        /// Converts a collection to an array.
        /// </summary>
        public static explicit operator ModbusLinkProtocol[](ModbusLinkProtocolCollection values)
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
            return (ModbusLinkProtocolCollection)this.MemberwiseClone();
        }
        #endregion
        #endif

        /// <summary cref="Object.MemberwiseClone" />
        public new object MemberwiseClone()
        {
            ModbusLinkProtocolCollection clone = new ModbusLinkProtocolCollection(this.Count);

            for (int ii = 0; ii < this.Count; ii++)
            {
                clone.Add((ModbusLinkProtocol)Utils.Clone(this[ii]));
            }

            return clone;
        }
    }
    #endregion
    #endif
    #endregion
}