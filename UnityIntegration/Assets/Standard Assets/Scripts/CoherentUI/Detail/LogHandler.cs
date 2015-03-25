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
	class UnityLogHandler : ILogHandler
	{
		public override void WriteLog(Severity severity, string message, uint length)
		{
			UnityEngine.Debug.Log(string.Format("[Coherent UI] ({0}) {1}", severity, message));
		}
	
		public override void Assert(string message)
		{
			// Do nothing; The log will be written by WriteLog with severity level AssertFailure
		}
	}
}
