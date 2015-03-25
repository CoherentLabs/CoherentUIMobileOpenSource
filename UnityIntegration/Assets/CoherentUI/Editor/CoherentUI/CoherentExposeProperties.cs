using UnityEditor;
using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Reflection;
 
public static class CoherentExposeProperties
{
	static Texture m_LogoTexture;
	static Texture m_SupportTexture;
	static Texture m_DocsTexture;
	
	public static void Expose( CoherentFoldout[] foldouts )
	{
		GUILayoutOption[] emptyOptions = new GUILayoutOption[0];
		
		if(!m_LogoTexture)
		{
			m_LogoTexture = EditorGUIUtility.FindTexture("Coherent_UI_inspector");
		}
		if(!m_SupportTexture)
		{
			m_SupportTexture = EditorGUIUtility.FindTexture("Coherent_UI_support");
		}
		if(!m_DocsTexture)
		{
			m_DocsTexture = EditorGUIUtility.FindTexture("Coherent_UI_docs");
		}
		EditorGUILayout.BeginVertical( GUILayout.Height(32) );
		EditorGUILayout.BeginHorizontal( emptyOptions );
		var labelStyle = new GUIStyle();
		labelStyle.fixedHeight = 32;
		labelStyle.fixedWidth = 163;
		if(m_LogoTexture) {
			EditorGUILayout.LabelField(new GUIContent("", m_LogoTexture), labelStyle, emptyOptions);
		}
		
		if(m_DocsTexture && GUILayout.Button(m_DocsTexture)){
			Application.OpenURL("http://coherent-labs.com/Documentation/unity/");
		}
		if(m_SupportTexture && GUILayout.Button(m_SupportTexture)){
			Application.OpenURL("http://coherent-labs.com/developer/");
		}
		
		EditorGUILayout.EndHorizontal();
		EditorGUILayout.EndVertical();
		
		EditorGUILayout.BeginVertical( emptyOptions );				
		foreach (CoherentFoldout fold in foldouts)
		{
			if(fold == null)
				continue;
			bool hasPropertiesToShow = false;
			foreach ( CoherentPropertyField field in fold.Fields )
			{
				if(!Application.isPlaying || !field.IsStatic) {
					hasPropertiesToShow = true;
					break;
				}
			}
			if(!hasPropertiesToShow)
				continue;
						
			fold.Show = EditorGUILayout.Foldout(fold.Show, new GUIContent(fold.Name, fold.Tooltip));

			if(fold.Show) {
				foreach ( CoherentPropertyField field in fold.Fields )
				{
					if(Application.isPlaying && field.IsStatic) {
						continue;						
					}
					
					var realType = field.RealType;
						
					EditorGUILayout.BeginHorizontal( emptyOptions );
					GUIContent content = new GUIContent(field.Name);
					if(field.Tooltip.Length > 0)
					{
						content.tooltip = field.Tooltip;
					}
					
					switch ( field.Type )
					{
					case SerializedPropertyType.Integer:
							field.SetValue( EditorGUILayout.IntField( content, (int)field.GetValue(), emptyOptions ) ); 
						break;
					 
					case SerializedPropertyType.Float:
							field.SetValue( EditorGUILayout.FloatField( content, (float)field.GetValue(), emptyOptions ) );
						break;
		 
					case SerializedPropertyType.Boolean:
							field.SetValue( EditorGUILayout.Toggle( content, (bool)field.GetValue(), emptyOptions ) );
						break;
		 
					case SerializedPropertyType.String:
							field.SetValue( EditorGUILayout.TextField( content, (String)field.GetValue(), emptyOptions ) );
						break;
		 
					case SerializedPropertyType.Vector2:
							field.SetValue( EditorGUILayout.Vector2Field( field.Name, (Vector2)field.GetValue(), emptyOptions ) );
						break;
		 
					case SerializedPropertyType.Vector3:
							field.SetValue( EditorGUILayout.Vector3Field( field.Name, (Vector3)field.GetValue(), emptyOptions ) );
						break;
		  
					case SerializedPropertyType.Enum:
							field.SetValue(EditorGUILayout.EnumPopup( content, (Enum)field.GetValue(), emptyOptions));
						break;
					case SerializedPropertyType.ObjectReference:
							field.SetValue(EditorGUILayout.ObjectField( content.text, (UnityEngine.Object)field.GetValue(), realType, true, emptyOptions));
						break;
		 
					default:
		 
						break;
		 
					}

					EditorGUILayout.EndHorizontal();	 
				}
			}
		}
		EditorGUILayout.EndVertical();
	}
	
	public static CoherentFoldout[] GetProperties( System.Object obj )
	{
		CoherentFoldout[] foldouts = new CoherentFoldout[(int)CoherentExposePropertyInfo.FoldoutType.Count];
		 
		PropertyInfo[] infos = obj.GetType().GetProperties( BindingFlags.Public | BindingFlags.Instance );
 
		foreach ( PropertyInfo info in infos )
		{
 
			if ( ! (info.CanRead && info.CanWrite) )
				continue;
 
			object[] attributes = info.GetCustomAttributes( true );
 
			bool isExposed = false;
			object infoAttribute = null;
			
			foreach( object o in attributes )
			{
				var t = o.GetType();
				if ( t == typeof( CoherentExposePropertyAttribute )
					#if UNITY_STANDALONE
					|| t == typeof(CoherentExposePropertyStandaloneAttribute)
					#elif UNITY_IPHONE || UNITY_ANDROID
					|| t == typeof(CoherentExposePropertyiOSAttribute)
					#endif
					)
				{
					infoAttribute = o;
					isExposed = true;
					break;
				}
			}
 
			if ( !isExposed )
				continue;
 
			SerializedPropertyType type = SerializedPropertyType.Integer;
 
			if( CoherentPropertyField.GetPropertyType( info, out type ) )
			{
				CoherentPropertyField field = new CoherentPropertyField( obj, info, type, infoAttribute );
				
				var category = CoherentExposePropertyInfo.FoldoutType.General;
				var attr = infoAttribute as CoherentExposePropertyInfo;
				if(attr != null)
				{
					category = attr.Category;
				}
											
				if(foldouts[(int)category] == null)
				{
					foldouts[(int)category] = new CoherentFoldout(category);
				}
				foldouts[(int)category].AddField(field);
			}
 
		}
 
		return foldouts;
	}
 
}
 
public class CoherentPropertyField
{
	System.Object m_Instance;
	System.Object m_InfoAttribute;
	PropertyInfo m_Info;
	SerializedPropertyType m_Type;
 
	MethodInfo m_Getter;
	MethodInfo m_Setter;
 
	public System.Type RealType
	{
		get
		{
			return m_Info.PropertyType;
		}
	}
	
	public SerializedPropertyType Type
	{
		get
		{
			return m_Type;	
		}
	}
 
	public String Name
	{	
		get
		{
			string name;
			
			var ia = m_InfoAttribute as CoherentExposePropertyInfo;
			if(ia != null && ia.PrettyName != null && ia.PrettyName.Length > 0)
			{
				name = ia.PrettyName;
			}
			else
			{
				name = ObjectNames.NicifyVariableName( m_Info.Name );
			}
			
			return name;
		}
	}
	
	public String Tooltip
	{
		get
		{
			string tip = "";
			
			var ia = m_InfoAttribute as CoherentExposePropertyInfo;
			if(ia != null && ia.Tooltip != null)
			{
				tip = ia.Tooltip;
			}
			
			return tip;
		}
	}
	
	public bool IsStatic
	{
		get
		{
			var ia = m_InfoAttribute as CoherentExposePropertyInfo;
			if(ia != null)
			{
				return ia.IsStatic;
			}
			else
			{
				return false;
			}
		}
	}
 
	public CoherentPropertyField( System.Object instance, PropertyInfo info, SerializedPropertyType type, System.Object infoAttribute )
	{	
		m_Instance = instance;
		m_Info = info;
		m_Type = type;
		m_InfoAttribute = infoAttribute;
 
		m_Getter = m_Info.GetGetMethod();
		m_Setter = m_Info.GetSetMethod();
	}
 
	public System.Object GetValue() 
	{
		return m_Getter.Invoke( m_Instance, null );
	}
 
	public void SetValue( System.Object value )
	{
#pragma warning disable 618
		if (!Equal(value))
		{
			Undo.RegisterUndo((UnityEngine.Object)m_Instance, this.Name);
			m_Setter.Invoke( m_Instance, new System.Object[] { value } );
			EditorUtility.SetDirty((UnityEngine.Object)m_Instance);
		}
#pragma warning restore 618
	}
 
	public static bool GetPropertyType( PropertyInfo info, out SerializedPropertyType propertyType )
	{
		propertyType = SerializedPropertyType.Generic;
 
		Type type = info.PropertyType;
 
		if ( type == typeof( int ) )
		{
			propertyType = SerializedPropertyType.Integer;
			return true;
		}
 
		if ( type == typeof( float ) )
		{
			propertyType = SerializedPropertyType.Float;
			return true;
		}
 
		if ( type == typeof( bool ) )
		{
			propertyType = SerializedPropertyType.Boolean;
			return true;
		}
 
		if ( type == typeof( string ) )
		{
			propertyType = SerializedPropertyType.String;
			return true;
		}	
 
		if ( type == typeof( Vector2 ) )
		{
			propertyType = SerializedPropertyType.Vector2;
			return true;
		}
 
		if ( type == typeof( Vector3 ) )
		{
			propertyType = SerializedPropertyType.Vector3;
			return true;
		}
 
		if ( type.IsEnum )
		{
			propertyType = SerializedPropertyType.Enum;
			return true;
		}
		
		if ( type == typeof(GameObject) 
			|| type == typeof(UnityEngine.Object)
			|| type == typeof(Texture2D))
		{
			propertyType = SerializedPropertyType.ObjectReference;
			return true;
		}
 
		return false;
	}
	
	private bool Equal(System.Object other)
	{
		switch ( m_Type )
		{
		case SerializedPropertyType.Integer:
				return (int)GetValue() == (int)other; 
		 
		case SerializedPropertyType.Float:
				return (float)GetValue() == (float)other;

		case SerializedPropertyType.Boolean:
				return (bool)GetValue() == (bool)other;

		case SerializedPropertyType.String:
				return (string)GetValue() == (string)other;

		case SerializedPropertyType.Vector2:
				return (Vector2)GetValue() == (Vector2)other;

		case SerializedPropertyType.Vector3:
				return (Vector3)GetValue() == (Vector3)other;

		case SerializedPropertyType.Enum:
				return (Enum)GetValue() == (Enum)other;
			
		case SerializedPropertyType.ObjectReference:
				return (UnityEngine.Object)GetValue() == (UnityEngine.Object)other;
			
		default:

			break;

		}
		return false;
	}
}

public class CoherentFoldout
{
	public CoherentFoldout(CoherentExposePropertyInfo.FoldoutType type)
	{
		m_Fields = new List<CoherentPropertyField>();
		
		Type = type;
		
		switch(Type)
		{
		case CoherentExposePropertyInfo.FoldoutType.General:
			Name = "General";
			Tooltip = "Shows the most common UI properties";
			Show = true;
			break;
		case CoherentExposePropertyInfo.FoldoutType.Rendering:
			Name = "Rendering";
			Tooltip = "Shows rendering-related properties";
			break;
		case CoherentExposePropertyInfo.FoldoutType.AdvancedRendering:
			Name = "Advanced rendering";
			Tooltip = "Shows advanced rendering properties";
			break;
		case CoherentExposePropertyInfo.FoldoutType.Input:
			Name = "Input";
			Tooltip = "Shows UI input-related properties";
			break;
		case CoherentExposePropertyInfo.FoldoutType.Scripting:
			Name = "Scripting";
			Tooltip = "Shows UI scripting-related properties";
			break;
		}
	}
	
	public String Name
	{
		get; private set;
	}
					
	public String Tooltip
	{
		get; private set;
	}
	
	public CoherentExposePropertyInfo.FoldoutType Type
	{
		get; private set;
	}
	
	public bool Show
	{
		get { return m_Show; }
		set { m_Show = value; }
	}
		
	public List<CoherentPropertyField> Fields
	{
		get { return m_Fields; }
	}
	
	public void AddField(CoherentPropertyField f)
	{
		m_Fields.Add(f);
	}
	
	List<CoherentPropertyField> m_Fields;
	bool m_Show = false;
}
