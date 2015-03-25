using UnityEngine;
using UnityEditor;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System;
using System.Text.RegularExpressions;
using System.Security.Cryptography;

public static class IOSPostProcessor {
	
	struct FrameworkRef
	{
		public string Name;
		public string Id;
		public string FileRefId;
		public string Path;
		
		public FrameworkRef(string name, string id, string fileRefId)
		{
			Name = name;
			Id = id;
			FileRefId = fileRefId;
			
			// Set a default path for all frameworks
			Path = "System/Library/Frameworks";
		}
	}
	
	private static RNGCryptoServiceProvider cryptoRng = new RNGCryptoServiceProvider();
	
	private static string GenerateNewFrameworkId()
	{
        byte[] bytes = new byte[12];
        cryptoRng.GetNonZeroBytes(bytes);
        return BitConverter.ToString(bytes).Replace("-", "");
	}
	
	public static void PostProcess(string pathToBuiltProject)
	{
		List<FrameworkRef> frameworks = new List<FrameworkRef>() {
			new FrameworkRef("Security.framework", "0174E86F175698E3008CCAF7", "0174E86E175698E3008CCAF7"),
			new FrameworkRef("MobileCoreServices.framework", "0174E8741756A4A9008CCAF7", "0174E8731756A4A9008CCAF7")
		};
		
		string xCodeProjPath = Path.Combine(pathToBuiltProject, "Unity-iPhone.xcodeproj");
		AddFrameworksToXCodeProj(xCodeProjPath, frameworks);
	}
	
	private static void AddFrameworksToXCodeProj(string xCodeProjPath, List<FrameworkRef> frameworks)
	{
		string pbxProj = Path.Combine(xCodeProjPath, "project.pbxproj");
		string[] lines = File.ReadAllLines(pbxProj);

		Regex idRegex = new Regex(@"^\s+(?<id>[0-9A-F]{24})\s+/\*.*?\*/ = {isa = PBXBuildFile; fileRef = (?<fileref>[0-9A-F]{24})", RegexOptions.ExplicitCapture);
		HashSet<string> usedIds = new HashSet<string>();
		
		// Remove frameworks that are already in the pbxproj;
		// Save used framework IDs
		foreach (string line in lines)
		{
			for (int i = frameworks.Count - 1; i >= 0; --i)
			{
				if (line.Contains(frameworks[i].Name))
				{
					frameworks.RemoveAt(i);
				}
				
				Match m = idRegex.Match(line);
				if (m.Success)
				{
					usedIds.Add(m.Groups["id"].Value);
					usedIds.Add(m.Groups["fileref"].Value);
				}
			}	
		}
		
		// Change the IDs of the new frameworks if necessary
		for (int i = 0; i < frameworks.Count; ++i)
		{
			FrameworkRef fr = frameworks[i];
			
			while (usedIds.Contains(fr.Id))
			{
				fr.Id = GenerateNewFrameworkId();
			}
			usedIds.Add(fr.Id);
			
			while (usedIds.Contains(fr.FileRefId))
			{
				fr.FileRefId = GenerateNewFrameworkId();
			}
			usedIds.Add(fr.FileRefId);
		}
		
		// Iterate over the lines again, this time adding the remaining framework refs
		string currentSection = string.Empty;
		bool addedToBuildFile = false;
		bool addedToFileReferences = false;
		bool addedToFrameworksBuildPhase = false;
		bool addedToGroup = false;
		
		List<string> output = new List<string>(lines.Length);
		
		for (int i = 0; i < lines.Length; ++i)
		{
			string line = lines[i];
			
			// Copy the line to output
			output.Add(line);
			
			// Search for a section start, e.g. "/* Begin PBXBuildFile section */"
			bool sectionStart = false;
			if (line.StartsWith("/* Begin"))
			{
				currentSection = line.Split(' ')[2];
				sectionStart = true;
			}
			else if (line.StartsWith("/* End"))
			{
				currentSection = string.Empty;
			}
			
			if (sectionStart && currentSection == "PBXBuildFile")
			{
				foreach (var framework in frameworks)
				{
					string projline = GetBuildFileLine(framework);
					output.Add(projline);
				}
				addedToBuildFile = true;
			}
			
			if (sectionStart && currentSection == "PBXFileReference")
			{
				foreach (var framework in frameworks)
				{
					string projline = GetFileReferenceLine(framework);	
					output.Add(projline);
				}
				addedToFileReferences = true;
			}
			
			if (currentSection == "PBXFrameworksBuildPhase" &&
				line.Contains("files = ("))
			{
				foreach (var framework in frameworks)
				{
					string projline = GetFrameworksBuildPhaseLine(framework);
					output.Add(projline);
				}
				addedToFrameworksBuildPhase = true;
			}
			
			if (currentSection == "PBXGroup" &&
				line.Contains("children = (") &&
				i >= 2 &&
				lines[i - 2].Contains("/* CustomTemplate */"))
			{
				foreach (var framework in frameworks)
				{
					string projline = GetGroupLine(framework);
					output.Add(projline);
				}
				addedToGroup = true;
			}
		}
		
		LogErrors(addedToBuildFile, addedToFileReferences, addedToFrameworksBuildPhase, addedToGroup);
		
		File.WriteAllLines(pbxProj, output.ToArray());
	}

	private static void LogErrors (bool addedToBuildFile, bool addedToFileReferences, bool addedToFrameworksBuildPhase, bool addedToGroup)
	{
		if (!addedToBuildFile)
		{
			Debug.LogError("Unable find PBXBuildFile section in pbxproj file while post-processing for iOS!");
		}
		
		if (!addedToFileReferences)
		{
			Debug.LogError("Unable find PBXFileReference section in pbxproj file while post-processing for iOS!");
		}
		
		if (!addedToFrameworksBuildPhase)
		{
			Debug.LogError("Unable find PBXFrameworksBuildPhase section in pbxproj file while post-processing for iOS!");
		}
		
		if (!addedToGroup)
		{
			Debug.LogError("Unable find PBXGroup section in pbxproj file while post-processing for iOS!");
		}
	}

	private static string GetBuildFileLine (FrameworkRef framework)
	{
		return String.Format("\t\t{0} /* {1} in Frameworks */ = {{isa = PBXBuildFile; fileRef = {2} /* {1} */; }};",
			framework.Id, framework.Name, framework.FileRefId);
	}

	private static string GetFileReferenceLine (FrameworkRef framework)
	{
		return String.Format("\t\t{0} /* {1} */ = {{isa = PBXFileReference; lastKnownFileType = wrapper.framework; name = {1}; path = {2}/{1}; sourceTree = SDKROOT; }};",
			framework.FileRefId, framework.Name, framework.Path);
	}

	private static string GetFrameworksBuildPhaseLine (FrameworkRef framework)
	{
		return String.Format("\t\t\t\t{0} /* {1} in Frameworks */,",
			framework.Id, framework.Name);
	}

	private static string GetGroupLine (FrameworkRef framework)
	{
		return String.Format("\t\t\t\t{0} /* {1} */,",
			framework.FileRefId, framework.Name);
	}
}
