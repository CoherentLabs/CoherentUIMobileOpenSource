#if UNITY_2_6 || UNITY_2_6_1 || UNITY_3_0 || UNITY_3_0_0 || UNITY_3_1 || UNITY_3_2 || UNITY_3_3 || UNITY_3_4 || UNITY_3_5 || UNITY_4_0 || UNITY_4_0_1 || UNITY_4_1
// Coherent UI supports Linux for Unity3D 4.2+
#define COHERENT_UNITY_PRE_4_2
#endif

using UnityEngine;
using System;
using System.IO;
using UnityEditor;
using UnityEditor.Callbacks;

public class AndroidProjectFixer {
#if !COHERENT_UNITY_PRE_4_2
	[PostProcessBuild]
	public static void OnPostprocessBuild(BuildTarget target, string pathToBuiltProject) {
		if (target != BuildTarget.Android || pathToBuiltProject.EndsWith(".apk"))
		{
			return;
		}
		
		// Delete the source files that Unity 4.2 exported so you don't get a
		// "Multiple dex files define Lcom/unity3d/player/UnityPlayerActivity;" error
		
		var outDir = pathToBuiltProject;
		var projName = PlayerSettings.productName;
		
		var unneededFilesRoot = Path.Combine(Path.Combine(outDir, projName), "src/com/unity3d/player");
		if (Directory.Exists(unneededFilesRoot))
		{
			string[] filesToDelete = Directory.GetFiles(unneededFilesRoot, "Unity*Activity*.java");
			foreach (string item in filesToDelete) {
				File.Delete(item);
			}
		}
	}
#endif
}
