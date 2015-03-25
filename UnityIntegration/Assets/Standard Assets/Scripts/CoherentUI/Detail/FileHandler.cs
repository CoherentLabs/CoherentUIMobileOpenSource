#if UNITY_STANDALONE || UNITY_STANDALONE_WIN || UNITY_STANDALONE_OSX
#define COHERENT_UNITY_STANDALONE
#endif

#if UNITY_NACL || UNITY_WEBPLAYER
#define COHERENT_UNITY_UNSUPPORTED_PLATFORM
#endif

using UnityEngine;
using System;
using System.IO;
using System.Runtime.InteropServices;

#if COHERENT_UNITY_STANDALONE || COHERENT_UNITY_UNSUPPORTED_PLATFORM
namespace Coherent.UI
#elif UNITY_IPHONE || UNITY_ANDROID
namespace Coherent.UI.Mobile
#endif
{
	class UnityFileHandler : FileHandler
	{
		private string GetFilepath(string url)
		{
			var asUri = new Uri(url);
			string cleanUrl;
			if(asUri.Scheme != "file") {
#if UNITY_EDITOR
			cleanUrl = asUri.GetComponents(UriComponents.Path, UriFormat.Unescaped);
			// Read resources from the project folder
			var uiResources = PlayerPrefs.GetString("CoherentUIResources");
			if (uiResources == string.Empty)
			{
				Debug.LogError("Missing path for Coherent UI resources. Please select path to your resources via Edit -> Project Settings -> Coherent UI -> Select UI Folder");
				// Try to fall back to the default location
				uiResources = Path.Combine(Path.Combine(Application.dataPath, "WebPlayerTemplates"), "uiresources");
				Debug.LogWarning("Falling back to the default location of the UI Resources in the Unity Editor: " + uiResources);
                PlayerPrefs.SetString("CoherentUIResources", "WebPlayerTemplates/uiresources");
			} else {
				uiResources = Path.Combine(Application.dataPath, uiResources);
			}
			cleanUrl = cleanUrl.Insert(0, uiResources + '/');
#else
			cleanUrl = asUri.GetComponents(UriComponents.Host | UriComponents.Path, UriFormat.Unescaped);
			// Read resources from the <executable>_Data folder
			cleanUrl = Application.dataPath + '/' + cleanUrl;
#endif
			}
			else {
				cleanUrl = asUri.GetComponents(UriComponents.Path, UriFormat.Unescaped);
			}
			return cleanUrl;
		}
		
		public override void ReadFile(string url, ResourceResponse response)
		{
		#if COHERENT_UNITY_UNSUPPORTED_PLATFORM
			throw new ApplicationException("Coherent UI doesn't support the target platform!");
		#else
			string cleanUrl = GetFilepath(url);
			
			if (!File.Exists(cleanUrl))
			{
				response.SignalFailure();
				return;
			}

			byte[] bytes = File.ReadAllBytes(cleanUrl);

			IntPtr buffer = response.GetBuffer((uint)bytes.Length);
			if (buffer == IntPtr.Zero)
			{
				response.SignalFailure();
				return;
			}

			Marshal.Copy(bytes, 0, buffer, bytes.Length);

			response.SignalSuccess();
		#endif
		}
		
		#if COHERENT_UNITY_STANDALONE
		public override void WriteFile(string url, ResourceData resource)
		{
		#if COHERENT_UNITY_UNSUPPORTED_PLATFORM
			throw new ApplicationException("Coherent UI doesn't support the target platform!");
		#else
			IntPtr buffer = resource.GetBuffer();
			if (buffer == IntPtr.Zero)
			{
				resource.SignalFailure();
				return;
			}

			byte[] bytes = new byte[resource.GetSize()];
			Marshal.Copy(buffer, bytes, 0, bytes.Length);

			string cleanUrl = GetFilepath(url);
			
			try
			{
				File.WriteAllBytes(cleanUrl, bytes);
			}
			catch (IOException ex)
			{
				Console.Error.WriteLine(ex.Message);
				resource.SignalFailure();
				return;
			}

			resource.SignalSuccess();
		#endif
		}
		#endif
	}
}
