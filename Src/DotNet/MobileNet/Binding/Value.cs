/*
Copyright (c) 2012-2015, Coherent Labs AD
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of Coherent UI nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
using System.Collections.Generic;

using Coherent.UI.Binding;

namespace Coherent.UI.Mobile.Binding
{
	/// <summary>
	/// Specifies the type of Coherent.UI.Binding.Value
	/// </summary>
	public enum ValueType
	{
		/// <summary> empty value </summary>
		Null,

		/// <summary> boolean value </summary>
		Boolean,

		/// <summary> integer value </summary>
		Integer,

		/// <summary> unsigned integer value </summary>
		UInteger,

		/// <summary> double value </summary>
		Number,

		/// <summary> string value </summary>
		String,

		/// <summary> array value </summary>
		Array,

		/// <summary> object value </summary>
		Object,

		/// <summary> signed byte value </summary>
		SByte = 10,

		/// <summary> unsigned byte value </summary>
		Byte,
	}

	/// <summary>
	/// Thrown when casting a Value to an incompatible type
	/// </summary>
	public class InvalidValueCastException : System.InvalidCastException
	{
		private static string FormatMessage(ValueType expected, ValueType actual)
		{
			return string.Format("Cannot cast from {0} to {1}", actual, expected);
		}

		internal InvalidValueCastException(ValueType expected, ValueType actual)
			: base(FormatMessage(expected, actual))
		{
		}
	}

	/// <summary>
	/// Type for representing generic JavaScript values
	/// </summary>
	public class Value
	{
		/// <summary>
		/// The type of the data stored in this Value instance
		/// </summary>
		public ValueType Type;

		private object ActualValue;

		/// <summary>
		/// Create a null value
		/// </summary>
		public Value()
		{
			Type = ValueType.Null;
		}

		/// <summary>
		/// Create a boolean value
		/// </summary>
		/// <param name="value">the integer value of the Value object</param>
		public Value(bool value)
		{
			Type = ValueType.Boolean;
			ActualValue = value;
		}

		/// <summary>
		/// Create an integer value
		/// </summary>
		/// <param name="value">the integer value of the Value object</param>
		public Value(int value)
		{
			Type = ValueType.Integer;
			ActualValue = value;
		}

		/// <summary>
		/// Create an unsigned integer value
		/// </summary>
		/// <param name="value">the unsigned integer value of the Value object</param>
		public Value(uint value)
		{
			Type = ValueType.UInteger;
			ActualValue = value;
		}

		/// <summary>
		/// Create a signed byte value
		/// </summary>
		/// <param name="value">the signed byte value of the Value object</param>
		public Value(sbyte value)
		{
			Type = ValueType.SByte;
			ActualValue = value;
		}

		/// <summary>
		/// Create a byte value
		/// </summary>
		/// <param name="value">the byte value of the Value object</param>
		public Value(byte value)
		{
			Type = ValueType.Byte;
			ActualValue = value;
		}

		/// <summary>
		/// Create a float value
		/// </summary>
		/// <param name="value">the float value of the Value object</param>
		public Value(float value)
		{
			Type = ValueType.Number;
			ActualValue = (double)value;
		}

		/// <summary>
		/// Create a double value
		/// </summary>
		/// <param name="value">the double value of the Value object</param>
		public Value(double value)
		{
			Type = ValueType.Number;
			ActualValue = value;
		}

		/// <summary>
		/// Create a string value
		/// </summary>
		/// <param name="value">the string value of the Value object</param>
		public Value(string value)
		{
			Type = ValueType.String;
			ActualValue = value;
		}

		/// <summary>
		/// Create an array value
		/// </summary>
		/// <param name="value">the array value of the Value object</param>
		public Value(Value[] value)
		{
			Type = ValueType.Array;
			ActualValue = value;
		}

		/// <summary>
		/// Create an object value
		/// </summary>
		/// <param name="value">dictionary of properties of the Value object</param>
		public Value(Dictionary<string, Value> value)
		{
			Type = ValueType.Object;
			ActualValue = new ValueObject(value);
		}

		/// <summary>
		/// Create an object value
		/// </summary>
		/// <param name="value">dictionary of properties of the Value object</param>
		public Value(ValueObject value)
		{
			Type = ValueType.Object;
			ActualValue = value;
		}

		/// <summary>
		/// cast a value to a boolean
		/// </summary>
		/// <param name="value">value to be casted</param>
		/// <returns>the boolean stored in the value</returns>
		public static implicit operator bool(Value value)
		{
			if (value.Type == ValueType.Boolean)
			{
				return (bool)value.ActualValue;
			}
			throw new InvalidValueCastException(ValueType.Boolean, value.Type);
		}

		/// <summary>
		/// cast a value to an int
		/// </summary>
		/// <param name="value">value to be casted</param>
		/// <returns>the int stored in the value</returns>
		public static implicit operator int(Value value)
		{
			switch (value.Type)
			{
				case ValueType.Integer:
					return (int)value.ActualValue;
				case ValueType.UInteger:
					return (int)(uint)value.ActualValue;
				case ValueType.Number:
					return (int)(double)value.ActualValue;
				case ValueType.Byte:
					return (int)(byte)value.ActualValue;
				case ValueType.SByte:
					return (int)(sbyte)value.ActualValue;
			}
			throw new InvalidValueCastException(ValueType.Integer, value.Type);
		}

		/// <summary>
		/// cast a value to an uint
		/// </summary>
		/// <param name="value">value to be casted</param>
		/// <returns>the unsigned int stored in the value</returns>
		public static implicit operator uint(Value value)
		{
			switch (value.Type)
			{
				case ValueType.Integer:
					return (uint)(int)value.ActualValue;
				case ValueType.UInteger:
					return (uint)value.ActualValue;
				case ValueType.Number:
					return (uint)(double)value.ActualValue;
				case ValueType.Byte:
					return (uint)(byte)value.ActualValue;
				case ValueType.SByte:
					return (uint)(sbyte)value.ActualValue;
			}
			throw new InvalidValueCastException(ValueType.UInteger, value.Type);
		}

		/// <summary>
		/// cast a value to a signed byte
		/// </summary>
		/// <param name="value">value to be cast</param>
		/// <returns>the signed byte stored in the value</returns>
		public static implicit operator sbyte(Value value)
		{
			switch (value.Type)
			{
				case ValueType.Integer:
					return (sbyte)(int)value.ActualValue;
				case ValueType.UInteger:
					return (sbyte)(uint)value.ActualValue;
				case ValueType.Number:
					return (sbyte)(double)value.ActualValue;
				case ValueType.Byte:
					return (sbyte)(byte)value.ActualValue;
				case ValueType.SByte:
					return (sbyte)value.ActualValue;
			}
			throw new InvalidValueCastException(ValueType.SByte, value.Type);
		}

		/// <summary>
		/// cast a value to a byte
		/// </summary>
		/// <param name="value">value to be cast</param>
		/// <returns>the byte stored in the value</returns>
		public static implicit operator byte(Value value)
		{
			switch (value.Type)
			{
				case ValueType.Integer:
					return (byte)(int)value.ActualValue;
				case ValueType.UInteger:
					return (byte)(uint)value.ActualValue;
				case ValueType.Number:
					return (byte)(double)value.ActualValue;
				case ValueType.Byte:
					return (byte)value.ActualValue;
				case ValueType.SByte:
					return (byte)(sbyte)value.ActualValue;
			}
			throw new InvalidValueCastException(ValueType.Byte, value.Type);
		}

		/// <summary>
		/// cast a value to a float
		/// </summary>
		/// <param name="value">value to be casted</param>
		/// <returns>the float stored in the value</returns>
		public static implicit operator float(Value value)
		{
			return (float)(double)value;
		}

		/// <summary>
		/// cast a value to a double
		/// </summary>
		/// <param name="value">value to be casted</param>
		/// <returns>the double stored in the value</returns>
		public static implicit operator double(Value value)
		{
			switch (value.Type)
			{
				case ValueType.Integer:
					return (double)(int)value.ActualValue;
				case ValueType.UInteger:
					return (double)(uint)value.ActualValue;
				case ValueType.Number:
					return (double)value.ActualValue;
				case ValueType.Byte:
					return (double)(byte)value.ActualValue;
				case ValueType.SByte:
					return (double)(sbyte)value.ActualValue;
			}
			throw new InvalidValueCastException(ValueType.Number, value.Type);
		}

		/// <summary>
		/// cast a value to a string
		/// </summary>
		/// <param name="value">value to be casted</param>
		/// <returns>the string stored in the value</returns>
		public static implicit operator string(Value value)
		{
			if (value.Type == ValueType.String)
			{
				return (string)value.ActualValue;
			}
			throw new InvalidValueCastException(ValueType.String, value.Type);
		}

		/// <summary>
		/// cast a value to an array
		/// </summary>
		/// <param name="value">value to be casted</param>
		/// <returns>the array of Value objects stored in the value</returns>
		public static implicit operator Value[](Value value)
		{
			if (value.Type == ValueType.Array)
			{
				return (Value[])value.ActualValue;
			}
			throw new InvalidValueCastException(ValueType.Array, value.Type);
		}

		/// <summary>
		/// cast a value to a dictionary of properties
		/// </summary>
		/// <param name="value">value to be casted</param>
		/// <returns>the dictionary of properties of the value</returns>
		public static implicit operator Dictionary<string, Value>(Value value)
		{
			if (value.Type == ValueType.Object)
			{
				return (value.ActualValue as ValueObject).Properties;
			}
			throw new InvalidValueCastException(ValueType.Object, value.Type);
		}

		internal void Export(Coherent.UI.Binding.Exporter exporter)
		{
			switch (Type)
			{
				case ValueType.Null: exporter.Export(this.ActualValue);
					break;
				case ValueType.Boolean: exporter.Export((bool)this.ActualValue);
					break;
				case ValueType.Integer: exporter.Export((int)this.ActualValue);
					break;
				case ValueType.UInteger: exporter.Export((uint)this.ActualValue);
					break;
                case ValueType.SByte: exporter.Export((sbyte)this.ActualValue);
                    break;
                case ValueType.Byte: exporter.Export((byte)this.ActualValue);
                    break;
				case ValueType.Number: exporter.Export((double)this.ActualValue);
					break;
				case ValueType.String: exporter.Export((string)this.ActualValue);
					break;
				case ValueType.Array: exporter.Export((Value[])this.ActualValue);
					break;
				case ValueType.Object: exporter.Export(((ValueObject)this.ActualValue).Properties);
					break;
			}
		}

		internal static Value Import(Coherent.UI.Binding.Importer importer)
		{
			var type = importer.Peek();
			switch (type)
			{
				case ValueType.Null:
					{
						importer.SkipValue();
						return new Value();
					}
				case ValueType.Boolean: return new Value((bool)Coherent.UI.Binding.Importer.ReadBool(importer));

				case ValueType.Integer: return new Value((int)Coherent.UI.Binding.Importer.ReadInt(importer));

				case ValueType.UInteger: return new Value((uint)Coherent.UI.Binding.Importer.ReadUInt(importer));

				case ValueType.SByte: return new Value((sbyte)Coherent.UI.Binding.Importer.ReadSByte(importer));

				case ValueType.Byte: return new Value((byte)Coherent.UI.Binding.Importer.ReadByte(importer));

				case ValueType.Number: return new Value((double)Coherent.UI.Binding.Importer.ReadDouble(importer));

				case ValueType.String: return new Value((string)Coherent.UI.Binding.Importer.ReadString(importer));

				case ValueType.Array: return new Value(importer.Read<Value[]>());

				case ValueType.Object: return new Value(importer.Read<Dictionary<string, Value>>());
			}
			return null;
		}
		
		/// <summary>
		/// Compares two Value objects
		/// </summary>
		/// <param name="lhs">object to be compared</param>
		/// <param name="rhs">object to be compared</param>
		/// <returns>true if the objects are from the same type and have the same values</returns>
		public static bool operator==(Value lhs, Value rhs)
		{
			return (lhs.Type == rhs.Type) && lhs.ActualValue == rhs.ActualValue;
		}

		/// <summary>
		/// Compares two Value objects
		/// </summary>
		/// <param name="lhs">object to be compared</param>
		/// <param name="rhs">object to be compared</param>
		/// <returns>false if the objects are from the same type and have the same values</returns>
		public static bool operator!=(Value lhs, Value rhs)
		{
			return !(lhs == rhs);
		}

		/// <summary>
		/// Compare to another object
		/// </summary>
		/// <param name="obj"></param>
		/// <returns>true if obj is a Value instance equal to this one</returns>
		public override bool Equals(object obj)
		{
			var other = obj as Value;
			return other != null && this == other;
		}

		/// <summary>
		/// Hash code for Value
		/// </summary>
		/// <returns>hash code for the current instance</returns>
		public override int GetHashCode()
		{
			return base.GetHashCode();
		}
	}

	/// <summary>
	/// Class for compound JavaScript objects, behaves like a Dictionary<string, Value>
	/// </summary>
	public class ValueObject : System.Collections.Generic.IDictionary<string, Value>, System.Collections.IDictionary
	{
		internal System.Collections.Generic.Dictionary<string, Value> Properties;

		public ValueObject()
		{
			Properties = new System.Collections.Generic.Dictionary<string, Value>();
		}

		public ValueObject(System.Collections.Generic.Dictionary<string, Value> properties)
		{
			Properties = properties;
		}

		public void Add(string key, Value value)
		{
			Properties.Add(key, value);
		}

		public bool ContainsKey(string key)
		{
			return Properties.ContainsKey(key);
		}

		public System.Collections.Generic.ICollection<string> Keys
		{
			get { return Properties.Keys; }
		}

		public bool Remove(string key)
		{
			return Properties.Remove(key);
		}

		public bool TryGetValue(string key, out Value value)
		{
			return Properties.TryGetValue(key, out value);
		}

		public System.Collections.Generic.ICollection<Value> Values
		{
			get { return Properties.Values; }
		}

		public Value this[string key]
		{
			get
			{
				return Properties[key];
			}
			set
			{
				Properties[key] = value;
			}
		}

		public void Add(System.Collections.Generic.KeyValuePair<string, Value> item)
		{
			(Properties as ICollection<KeyValuePair<string, Value>>).Add(item);
		}

		public void Clear()
		{
			Properties.Clear();
		}

		public bool Contains(System.Collections.Generic.KeyValuePair<string, Value> item)
		{
			return Properties[item.Key] == item.Value;
		}

		void ICollection<KeyValuePair<string, Value>>.CopyTo(System.Collections.Generic.KeyValuePair<string, Value>[] array, int arrayIndex)
		{
			(Properties as ICollection<KeyValuePair<string, Value>>).CopyTo(array, arrayIndex);
		}

		public int Count
		{
			get { return Properties.Count; }
		}

		public bool IsReadOnly
		{
			get { return false; }
		}

		public bool Remove(System.Collections.Generic.KeyValuePair<string, Value> item)
		{
			return (Properties as ICollection<KeyValuePair<string, Value>>).Remove(item);
		}

		public System.Collections.Generic.IEnumerator<System.Collections.Generic.KeyValuePair<string, Value>> GetEnumerator()
		{
			return Properties.GetEnumerator();
		}

		System.Collections.IEnumerator System.Collections.IEnumerable.GetEnumerator()
		{
			return Properties.GetEnumerator();
		}

		#region IDictionary implementation
		public void Add(object key, object value)
		{
			((System.Collections.IDictionary)Properties).Add(key, value);
		}

		public bool Contains(object key)
		{
			return ((System.Collections.IDictionary)Properties).Contains(key);
		}

		System.Collections.IDictionaryEnumerator System.Collections.IDictionary.GetEnumerator()
		{
			return ((System.Collections.IDictionary)Properties).GetEnumerator();
		}

		public void Remove(object key)
		{
			((System.Collections.IDictionary)Properties).Remove(key);
		}

		public bool IsFixedSize {
			get {
				return ((System.Collections.IDictionary)Properties).IsFixedSize;
			}
		}

		public object this[object key] {
			get {
				return ((System.Collections.IDictionary)Properties)[key];
			}
			set {
				((System.Collections.IDictionary)Properties)[key] = value;
			}
		}

		System.Collections.ICollection System.Collections.IDictionary.Keys {
			get {
				return ((System.Collections.IDictionary)Properties).Keys;
			}
		}

		System.Collections.ICollection System.Collections.IDictionary.Values {
			get {
				return ((System.Collections.IDictionary)Properties).Values;
			}
		}
		#endregion



		#region ICollection implementation
		public void CopyTo(System.Array array, int index)
		{
			((System.Collections.IDictionary)Properties).CopyTo(array, index);
		}

		public bool IsSynchronized {
			get {
				return ((System.Collections.IDictionary)Properties).IsSynchronized;
			}
		}

		public object SyncRoot {
			get {
				return ((System.Collections.IDictionary)Properties).SyncRoot;
			}
		}
		#endregion
	}
}
