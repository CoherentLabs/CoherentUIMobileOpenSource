#if UNITY_2_6 || UNITY_2_6_1 || UNITY_3_0 || UNITY_3_0_0 || UNITY_3_1 || UNITY_3_2 || UNITY_3_3 || UNITY_3_4 || UNITY_3_5 
#define COHERENT_UNITY_PRE_4
#endif

using System.Collections.Generic;

using UnityEngine;
using UnityEditor;
using UnityEditor.Callbacks;

public class CoherentIOSPostProcess
{
#if COHERENT_UNITY_PRE_4
	[PostProcessBuild]
	public static void FixSimulatorProject(BuildTarget target, string pathToProject)
	{
		if (target == BuildTarget.iPhone)
		{
			Debug.Log(string.Format("Fixing native plugins project {0}", pathToProject));
			string registerModules = pathToProject + "/Libraries/RegisterMonoModules.cpp";
			string[] lines = System.IO.File.ReadAllLines(registerModules);

			int n = lines.Length;
			List<string> result = new List<string>(n + 1);

			string notSimulatorBegin = "#if !(TARGET_IPHONE_SIMULATOR)";
			string notSimulatorEnd = "#endif // !(TARGET_IPHONE_SIMULATOR)";

			bool firstSection = false;

			for (int i = 0; i < n; ++i)
			{
				string line = lines[i];
				if (line == notSimulatorBegin)
				{
					if (!firstSection)
					{
						result.Add("	void				mono_dl_register_symbol (const char* name, void *addr);");
						result.Add(line);
						firstSection = true;
					}
					else
					{
						while (i < n && !lines[i].StartsWith("	mono_dl_register_symbol("))
						{
							result.Add(lines[i]);
							++i;
						}
						result.Add(notSimulatorEnd);
						while (i < n && lines[i] != notSimulatorEnd)
						{
							result.Add(lines[i]);
							++i;
						}
						// skip the #endif
					}
				}
				else
				{
					result.Add(line);
				}
			}

			System.IO.File.WriteAllLines(registerModules, result.ToArray());
		}
	}
#endif
}
