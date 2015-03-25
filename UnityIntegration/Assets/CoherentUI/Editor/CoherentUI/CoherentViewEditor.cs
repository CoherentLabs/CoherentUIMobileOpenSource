using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using UnityEditor;

[CustomEditor(typeof(CoherentUIView))]
public class CoherentViewEditor : Editor {
	
	private CoherentUIView m_Target;
	private CoherentFoldout[] m_Fields;
	
	public void OnEnable() {
		m_Target = target as CoherentUIView;
		m_Fields = CoherentExposeProperties.GetProperties(m_Target);
	}
	
	public override void OnInspectorGUI() {
		if(m_Target == null)
			return;
		this.DrawDefaultInspector();
		CoherentExposeProperties.Expose(m_Fields);
	}
}

[InitializeOnLoad]
class CoherentUIHierarchy
{
	static Texture m_Texture;
	
	static CoherentUIHierarchy() {
		EditorApplication.hierarchyWindowItemOnGUI += HierarchyWindowListElementOnGUI;
	}
	
	static void HierarchyWindowListElementOnGUI(int instanceID, Rect selectionRect)
    {
		var go = EditorUtility.InstanceIDToObject(instanceID) as GameObject;
		if(go) {
			var view = go.GetComponent(typeof(CoherentUIView));
			var sys = go.GetComponent(typeof(CoherentUISystem));
			if(view || sys)
			{
				if(!m_Texture)
				{
					m_Texture = EditorGUIUtility.FindTexture("CoherentUIView_icon");
				}
				
				if(m_Texture)
				{
					var newRect = selectionRect;
					newRect.x = selectionRect.xMax - selectionRect.height;
					newRect.width = selectionRect.height;
					newRect.height = selectionRect.height;
					GUI.DrawTexture(newRect, m_Texture);
				}
			}
		}
    }
}

