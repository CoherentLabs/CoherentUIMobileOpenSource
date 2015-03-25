using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using UnityEditor;

[CustomEditor(typeof(CoherentUISystem))]
public class CoherentSystemEditor : Editor {
	
	private CoherentUISystem m_Target;
	private CoherentFoldout[] m_Fields;
	
	public void OnEnable() {
		m_Target = target as CoherentUISystem;
		m_Fields = CoherentExposeProperties.GetProperties(m_Target);
	}
	
	public override void OnInspectorGUI() {
		if(m_Target == null)
			return;
		this.DrawDefaultInspector();
		CoherentExposeProperties.Expose(m_Fields);
	}
	
	internal static string OpenSelectUIFolderDialog()
	{
		string defaultFolder = new System.IO.DirectoryInfo(Application.dataPath).Parent.FullName;
		string folder = EditorUtility.OpenFolderPanel("Select UI resources folder", defaultFolder, "");
		if (string.IsNullOrEmpty(folder)) {
			return string.Empty;
		}
		
		var folderAsUri = new System.Uri(folder);
		var workPathAsUri = new System.Uri(Application.dataPath + "/.");
		
		var relativePath = workPathAsUri.MakeRelativeUri(folderAsUri);
		return relativePath.ToString();
	}
	
	[MenuItem ("Edit/Project Settings/Coherent UI/Select UI folder")]
	static void SelecUIFolder() {
		string relativePath = OpenSelectUIFolderDialog();
		if (string.IsNullOrEmpty(relativePath)) {
			return;
		}
		PlayerPrefs.SetString("CoherentUIResources", relativePath);
	}
}
