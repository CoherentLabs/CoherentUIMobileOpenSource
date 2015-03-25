#if UNITY_2_6 || UNITY_2_6_1 || UNITY_3_0 || UNITY_3_0_0 || UNITY_3_1 || UNITY_3_2 || UNITY_3_3 || UNITY_3_4 || UNITY_3_5 || UNITY_4_0 || UNITY_4_0_1 || UNITY_4_1
// Coherent UI supports Linux for Unity3D 4.2+
#define COHERENT_UNITY_PRE_4_2
#endif

using UnityEngine;
using System.Collections;
using System;
using System.IO;
using System.Text;
using System.Reflection;
using UnityEditor;
using UnityEditor.Callbacks;

public partial class CoherentPostProcessor {

	public static void DirectoryCopy(string sourceDirName, string destDirName, bool copySubDirs)
	{
		DirectoryInfo dir = new DirectoryInfo(sourceDirName);
		DirectoryInfo[] dirs = dir.GetDirectories();

		if (!dir.Exists)
		{
			throw new DirectoryNotFoundException(
				"Source directory does not exist or could not be found: "
				+ sourceDirName);
		}

		if (!Directory.Exists(destDirName))
		{
			Directory.CreateDirectory(destDirName);
		}

		FileInfo[] files = dir.GetFiles();
		foreach (FileInfo file in files)
		{
			string temppath = Path.Combine(destDirName, file.Name);
			file.CopyTo(temppath, true);
		}

		if (copySubDirs)
		{
			foreach (DirectoryInfo subdir in dirs)
			{
				string temppath = Path.Combine(destDirName, subdir.Name);
				DirectoryCopy(subdir.FullName, temppath, copySubDirs);
			}
		}
	}
	
	private static void DeleteFileIfExists(string path)
	{
		if (File.Exists(path))
		{
			File.Delete(path);
		}
	}
	
	private static void CleanUpForWindows(BuildTarget target, string dataDirectory)
	{
		string sourceDll64Native =  dataDirectory + "StreamingAssets/CoherentUI64_Native.dll";
		string sourceDll64Managed = dataDirectory + "StreamingAssets/CoherentUINet.dll64";
		
		string unneededManagedDllInPlugins = dataDirectory + "Plugins/CoherentUINet.dll";
		DeleteFileIfExists(unneededManagedDllInPlugins);

		if (target == BuildTarget.StandaloneWindows64)
		{
			string targetDll64Native = dataDirectory + "Plugins/CoherentUI64_Native.dll";
			string targetDllManaged = dataDirectory + "Managed/CoherentUINet.dll";
			string unneededNativeDllInPlugins = dataDirectory + "Plugins/CoherentUI_Native.dll";
			
			DeleteFileIfExists(unneededNativeDllInPlugins);
			DeleteFileIfExists(targetDll64Native);
			DeleteFileIfExists(targetDllManaged);
			
			if (!File.Exists(sourceDll64Native) || !File.Exists(sourceDll64Managed))
			{
				Debug.LogError("Unable to copy essential files for Coherent UI x64 when postprocessing build!");
				return;
			}
			File.Move(sourceDll64Native, targetDll64Native);
			File.Move(sourceDll64Managed, targetDllManaged);
		}
		else
		{
			// Delete the unneeded CoherentUI x64 DLLs
			DeleteFileIfExists(sourceDll64Native);
			DeleteFileIfExists(sourceDll64Managed);
		}
		
		if (Directory.Exists(dataDirectory + "StreamingAssets/CoherentUI_Host/macosx"))
		{
			Directory.Delete(dataDirectory + "StreamingAssets/CoherentUI_Host/macosx", true);
		}
		
		if (Directory.Exists(dataDirectory + "StreamingAssets/CoherentUI_Host/linux"))
		{
			Directory.Delete(dataDirectory + "StreamingAssets/CoherentUI_Host/linux", true);
		}		
	}
	
	private static void CleanUpForMacOSX(string bundle)
	{
		var dataDirectory = bundle + "Contents/Data/StreamingAssets/";
		var pluginsDirectory = bundle + "Contents/Plugins/";
		string[] windowsDlls = {
			dataDirectory + "CoherentUI64_Native.dll",
			dataDirectory + "CoherentUINet.dll64",
			pluginsDirectory + "CoherentUI_Native.dll",
			pluginsDirectory + "CoherentUINet.dll",
		};
		
		foreach (var file in windowsDlls)
		{
			DeleteFileIfExists(file);
		}
		
		if (Directory.Exists(dataDirectory + "CoherentUI_Host/windows"))
		{
			Directory.Delete(dataDirectory + "CoherentUI_Host/windows", true);
		}
		
		if (Directory.Exists(dataDirectory + "CoherentUI_Host/linux"))
		{
			Directory.Delete(dataDirectory + "CoherentUI_Host/linux", true);
		}
	}
	
	private static void CleanUpForLinux(string dataDirectory)
	{
		string[] windowsDlls = {
			dataDirectory + "StreamingAssets/CoherentUI64_Native.dll",
			dataDirectory + "StreamingAssets/CoherentUINet.dll64",
		};
		
		foreach (var file in windowsDlls)
		{
			DeleteFileIfExists(file);
		}
		
		if (Directory.Exists(dataDirectory + "StreamingAssets/CoherentUI_Host/windows"))
		{
			Directory.Delete(dataDirectory + "StreamingAssets/CoherentUI_Host/windows", true);
		}
		
		if (Directory.Exists(dataDirectory + "StreamingAssets/CoherentUI_Host/macosx"))
		{
			Directory.Delete(dataDirectory + "StreamingAssets/CoherentUI_Host/macosx", true);
		}
	}
	
	private static void CleanUpForMobile(string dataFolder)
	{
		string[] dlls = {
			dataFolder + "CoherentUINet.dll64",
			dataFolder + "CoherentUI64_Native.dll",
		};
		
		foreach (var file in dlls)
		{
			DeleteFileIfExists(file);
		}
		
		string hostDir = dataFolder + "CoherentUI_Host";
		if(Directory.Exists(hostDir))
		{
			Directory.Delete(hostDir, true);
		}
	}
	
	private static bool IsTargetPlatformSupported(BuildTarget target)
	{
		return target == BuildTarget.Android ||
			target == BuildTarget.iPhone ||
#if !COHERENT_UNITY_PRE_4_2
			target == BuildTarget.StandaloneLinux64 ||
			target == BuildTarget.StandaloneOSXIntel64 ||
			target == BuildTarget.StandaloneOSXUniversal ||
#endif
			target == BuildTarget.StandaloneOSXIntel ||
			target == BuildTarget.StandaloneWindows ||
			target == BuildTarget.StandaloneWindows64;
	}
	
	[PostProcessBuild]
	public static void OnPostprocessBuild(BuildTarget target, string pathToBuiltProject) {
		if (!IsTargetPlatformSupported(target))
		{
			if (Directory.Exists(Path.Combine(pathToBuiltProject, "StreamingAssets/CoherentUI_Host")))
			{
				Directory.Delete(Path.Combine(pathToBuiltProject, "StreamingAssets/CoherentUI_Host"), true);
			}	
			
			Debug.Log("Coherent UI package installed in a project targeting unsupported platfrom (" + target + ")!");
			return;
		}
		
		var outDir = Path.GetDirectoryName(pathToBuiltProject);

		var projName = "";

		if (target == BuildTarget.iPhone)
		{
			var slashDelim = pathToBuiltProject.LastIndexOf('/');

			if(slashDelim != -1)
			{
				projName = pathToBuiltProject.Substring(slashDelim+1) ;
			}
			else
			{
				Debug.Log("Ivalid path to build project passed!");
				return;
			}
		}
		else
		{
			projName = Path.GetFileNameWithoutExtension(pathToBuiltProject);
		}

		var resourcesFolder = PlayerPrefs.GetString("CoherentUIResources");
		
#if !COHERENT_UNITY_PRE_4_2
		if (target == BuildTarget.Android)
		{
			outDir = pathToBuiltProject;
			projName = PlayerSettings.productName;
		}
#endif

		// check for per-project override
		if(string.IsNullOrEmpty(resourcesFolder))
		{
			FieldInfo projectUIResourcesStr = typeof(CoherentPostProcessor).GetField("ProjectUIResources", BindingFlags.Public | BindingFlags.Static);
			if(projectUIResourcesStr != null) 
			{
				string projectResFolder = (string)projectUIResourcesStr.GetValue(null);
				Debug.Log(String.Format("[Coherent UI]: Found project resources folder: {0}", projectResFolder));
				resourcesFolder = projectResFolder;
			}
		}
		
		bool buildingAndroidApk = false;
		string androidUnpackDir = Path.Combine(Application.dataPath, "../Temp/CouiApkRepack");
		if (target == BuildTarget.Android && pathToBuiltProject.EndsWith(".apk"))
		{
			buildingAndroidApk = true;
		}
		
		if (buildingAndroidApk)
		{
			AndroidPostProcessor.FindAndCopySdkAaptAndZipalign();
			AndroidPostProcessor.UnpackAPK(pathToBuiltProject, androidUnpackDir);
		}

		// copy the UI resources
		if(!string.IsNullOrEmpty(resourcesFolder))
		{
			var lastDelim = resourcesFolder.LastIndexOf('/');
			string folderName = lastDelim != -1 ? resourcesFolder.Substring(lastDelim+1) : resourcesFolder;
		
			StringBuilder outputDir = new StringBuilder(outDir);
			string uiResourcesFormat = null;
			switch(target)
			{
#if !COHERENT_UNITY_PRE_4_2
			case BuildTarget.StandaloneOSXIntel64:
			case BuildTarget.StandaloneOSXUniversal:
#endif
			case BuildTarget.StandaloneOSXIntel:
				uiResourcesFormat = "/{0}.app/Contents/{1}";
				break;
			case BuildTarget.StandaloneWindows:
			case BuildTarget.StandaloneWindows64:
#if !COHERENT_UNITY_PRE_4_2
			case BuildTarget.StandaloneLinux64:
#endif
				uiResourcesFormat = "/{0}_Data/{1}";
				break;
			case BuildTarget.iPhone:
				uiResourcesFormat = "/{0}/Data/{1}";
				break;
			case BuildTarget.Android:
				uiResourcesFormat = "/{0}/assets/{1}"; // Format for exported Eclipse project
				break;
			default:
				new System.ApplicationException("Unsupported by Coherent UI build target");
				break;
			}

            var inDir = Path.Combine(Application.dataPath, resourcesFolder);

            if (!Directory.Exists(inDir))
            {
                resourcesFolder = Path.Combine("..", resourcesFolder);
                inDir = Path.Combine(Application.dataPath, resourcesFolder);
            }
			
			if (buildingAndroidApk)
			{
				outputDir = new StringBuilder(Path.Combine(androidUnpackDir, "assets/" + folderName));
			}
			else
			{
				outputDir.AppendFormat(uiResourcesFormat, projName, folderName);
			}
			
			DirectoryCopy(inDir.ToString(), outputDir.ToString(), true);
		}
		
		switch (target)
		{
		case BuildTarget.StandaloneWindows:
		case BuildTarget.StandaloneWindows64:
			CleanUpForWindows(target, string.Format("{0}/{1}_Data/", outDir, projName));
			break;
#if !COHERENT_UNITY_PRE_4_2
		case BuildTarget.StandaloneLinux64:
			CleanUpForLinux(string.Format("{0}/{1}_Data/", outDir, projName));
			break;
		case BuildTarget.StandaloneOSXIntel64:
		case BuildTarget.StandaloneOSXUniversal:
#endif
		case BuildTarget.StandaloneOSXIntel:
			CleanUpForMacOSX(string.Format("{0}/{1}.app/", outDir, projName));
			break;
		case BuildTarget.iPhone:
			var outputFolder = string.Format("{0}/{1}/Data/Raw/", outDir, projName);
			IOSPostProcessor.PostProcess(pathToBuiltProject);
			CleanUpForMobile(outputFolder);
			break;
		case BuildTarget.Android:
			if (buildingAndroidApk)
			{
				AndroidPostProcessor.RepackAPK(pathToBuiltProject, androidUnpackDir);
			}
			else
			{
				bool apiLevel11OrGreater = (PlayerSettings.Android.minSdkVersion >= AndroidSdkVersions.AndroidApiLevel11);
				AndroidPostProcessor.ModifyManifestFile(string.Format("{0}/{1}/AndroidManifest.xml", outDir, projName), apiLevel11OrGreater);
				CleanUpForMobile(string.Format("{0}/{1}/assets/", outDir, projName));
				AndroidPostProcessor.CleanUpForAndroid(string.Format("{0}/{1}/Plugins", outDir, projName));

				AndroidPostProcessor.CopyMobileNetDll(
					string.Format("{0}/{1}/assets/bin/Data/Managed",
									outDir, projName),
					Path.Combine(Application.dataPath, "Plugins"));
			}
			break;
		default:
			new System.ApplicationException("Unsupported by Coherent UI build target");
			break;
		}
	}
}
