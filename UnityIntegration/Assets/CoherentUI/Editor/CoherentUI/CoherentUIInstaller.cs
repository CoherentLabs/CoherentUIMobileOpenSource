// After Coherent UI version 1.5.4 the "Install" step is no longer required
using UnityEngine;
using UnityEditor;
using System.IO;

[InitializeOnLoad]
public class CoherentUIInstaller : Editor
{
	static CoherentUIInstaller()
	{
		var uiResources = PlayerPrefs.GetString("CoherentUIResources");
		if (!string.IsNullOrEmpty(uiResources))
		{
			var absolutePath = Path.Combine(Application.dataPath, uiResources);
			if (!Directory.Exists(absolutePath))
			{
				Debug.LogError("The UI Resources directory \"" + absolutePath +"\" does not exist! " +
					"Please reselect the UI Resources folder using the Edit->Project Settings->Coherent UI->Select UI Folder entry.");
			}
		}
		else
		{
			var defaultUIPath = "WebPlayerTemplates/uiresources";
			PlayerPrefs.SetString("CoherentUIResources", defaultUIPath);
			Debug.Log ("Setting default UI resource path to: " + defaultUIPath);
		}
	}
}