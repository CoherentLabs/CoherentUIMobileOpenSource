#if UNITY_STANDALONE || UNITY_STANDALONE_WIN || UNITY_STANDALONE_OSX
#define COHERENT_UNITY_STANDALONE
#endif

#if UNITY_NACL || UNITY_WEBPLAYER
#define COHERENT_UNITY_UNSUPPORTED_PLATFORM
#endif

#if COHERENT_UNITY_STANDALONE || COHERENT_UNITY_UNSUPPORTED_PLATFORM
namespace Coherent.UI
#elif UNITY_IPHONE || UNITY_ANDROID
namespace Coherent.UI.Mobile
#endif
{
	public class SystemListener : EventListener
	{
		public SystemListener(System.Action onSystemReady)
		{
			m_OnSystemReady = onSystemReady;
		}
		
		public override void SystemReady()
		{
			IsReady = true;
			m_OnSystemReady();
		}
	
		public override void OnError(SystemError arg0)
		{
			UnityEngine.Debug.Log(string.Format("An error occured! {0} (#{1})", arg0.Error, arg0.ErrorCode));
		}
	
		public bool IsReady
		{
			get;
			private set;
		}

#if COHERENT_UNITY_STANDALONE
		public override bool OnDownloadStarted (Download downloadItem)
		{
			UnityEngine.Debug.Log("Downloads are disabled when using the default Unity3D SystemListener!");
			return false;
		}
#endif

#if !UNITY_EDITOR && (UNITY_ANDROID)
		public static string GetTouchScreenKbdInitialText()
		{
			return new string('-', 64);
		}

		public override void OnSoftwareKeyboardVisibilityChanged(bool visible)
		{
			UnityEngine.TouchScreenKeyboard kbd = null;

			bool isHidden = (!UnityEngine.TouchScreenKeyboard.visible)
				|| (CoherentUISystem.Instance.TouchscreenKeyboard == null);

			if (visible)
			{
				if (!isHidden)
				{
					return;
				}

				UnityEngine.TouchScreenKeyboard.hideInput = true;
				string initialString = GetTouchScreenKbdInitialText();
				kbd = UnityEngine.TouchScreenKeyboard.Open(initialString);

				if (kbd != null && kbd.text.Length != initialString.Length)
				{
					kbd.text = initialString;
				}
			}
			CoherentUISystem.Instance.TouchscreenKeyboard = kbd;
		}
#endif

		private System.Action m_OnSystemReady;
	}
}
