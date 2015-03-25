using UnityEngine;
using System.Collections;
using System.Runtime.InteropServices;

public class CoherentUIViewRenderer : MonoBehaviour
{
	CoherentUIViewRenderer()
	{
		ViewId = -1;
		DrawAfterPostEffects = false;
		IsActive = true;
	}

#if UNITY_STANDALONE_WIN
	[DllImport ("CoherentUI_Native", EntryPoint="DummyUnityCall")]
	private static extern void DummyUnityCall32();

	[DllImport("CoherentUI64_Native", EntryPoint = "DummyUnityCall")]
	private static extern void DummyUnityCall64();

	void Start () {
		if (System.IntPtr.Size == 4) {
			DummyUnityCall32();
		} else {
			DummyUnityCall64();
		}
		if (DrawAfterPostEffects && m_DrawAtEndOfFrame == null)
		{
			m_DrawAtEndOfFrame = StartCoroutine("DrawAtEndOfFrame");
		}
	}

	public static void WakeRenderer() {
		if (System.IntPtr.Size == 4) {
			DummyUnityCall32();
		} else {
			DummyUnityCall64();
		}

		GL.IssuePluginEvent(MakeEvent(CoherentUISystem.CoherentRenderingEventType.WakeRenderer, CoherentUISystem.CoherentRenderingFlags.None, 0, 0));
	}
#else
	[DllImport("CoherentUI_Native")]
	private static extern void DummyUnityCall();

	void Start()
	{
		DummyUnityCall();

		if (DrawAfterPostEffects && m_DrawAtEndOfFrame == null)
		{
			m_DrawAtEndOfFrame = StartCoroutine("DrawAtEndOfFrame");
		}
	}

	public static void WakeRenderer()
	{
		DummyUnityCall();

		GL.IssuePluginEvent(MakeEvent(CoherentUISystem.CoherentRenderingEventType.WakeRenderer, CoherentUISystem.CoherentRenderingFlags.None, 0, 0));
	}
#endif

private static int MakeEvent(CoherentUISystem.CoherentRenderingEventType evType, CoherentUISystem.CoherentRenderingFlags renderingFlags, CoherentUISystem.CoherentFilteringModes filteringMode, short viewId)
 	{
 		int eventId = CoherentUISystem.COHERENT_PREFIX << 24;
		
 		eventId |= ((int)renderingFlags) << 20;
 		eventId |= ((int)filteringMode) << 18;
		eventId |= ((int)evType) << 16;
 		eventId |= viewId;
		
		return eventId;
	}

	void OnPostRender()
	{
		if (!DrawAfterPostEffects)
		{
			Draw();
		}
	}
	private IEnumerator DrawAtEndOfFrame()
	{
		while (true)
		{
			yield return new WaitForEndOfFrame();
			Draw();
		}
	}

	private Coroutine m_DrawAtEndOfFrame;

	public void OnEnable()
	{
		if (DrawAfterPostEffects)
		{
			m_DrawAtEndOfFrame = StartCoroutine("DrawAtEndOfFrame");
		}
	}

	public void OnDisable()
	{
		if (DrawAfterPostEffects)
		{
			StopCoroutine("DrawAtEndOfFrame");
			m_DrawAtEndOfFrame = null;
		}
	}


	private void Draw()
	{
		if(!IsActive) return;

		CoherentUISystem.CoherentRenderingFlags renderingFlags = CoherentUISystem.CoherentRenderingFlags.None;
		
		if (FlipY)
		{
			renderingFlags |= CoherentUISystem.CoherentRenderingFlags.FlipY;
		}
		
		if(ShouldCorrectGamma)
		{
			renderingFlags |= CoherentUISystem.CoherentRenderingFlags.CorrectGamma;
		}

		GL.IssuePluginEvent(MakeEvent(CoherentUISystem.CoherentRenderingEventType.DrawView, renderingFlags, FilteringMode, ViewId));
	}

	internal short ViewId
	{
		get;
		set;
	}

	internal bool DrawAfterPostEffects
	{
		get;
		set;
	}

	internal bool IsActive
	{
		get;
		set;
	}

	internal bool FlipY;
	
	internal CoherentUISystem.CoherentFilteringModes FilteringMode
	{
		get;
		set;
	}
	internal bool ShouldCorrectGamma
    {
        get;
        set;
    }
}
