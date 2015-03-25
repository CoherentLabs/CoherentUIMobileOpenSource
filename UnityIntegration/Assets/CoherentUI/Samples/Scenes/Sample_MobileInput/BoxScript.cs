#if UNITY_STANDALONE || UNITY_STANDALONE_WIN || UNITY_STANDALONE_OSX
#define COHERENT_UNITY_STANDALONE
#endif
using UnityEngine;
using System.Collections;
#if COHERENT_UNITY_STANDALONE
using Coherent.UI.Binding;
using Coherent.UI;
#elif UNITY_IPHONE || UNITY_ANDROID
using Coherent.UI.Mobile.Binding;
using Coherent.UI.Mobile;
#endif

public class BoxScript : MonoBehaviour
{
	public Camera m_MainCamera;
	private CoherentUIView m_CameraView;

	// Use this for initialization
	void Start()
	{
#if COHERENT_UNITY_STANDALONE
		Debug.LogWarning("The mobile input emulation in the Editor " +
			"works only when you switch to Android/iOS!");
#endif

		m_CameraView = m_MainCamera.gameObject.GetComponent<CoherentUIView>();
		if (m_CameraView && !m_CameraView.ClickToFocus)
		{
			m_CameraView.ReceivesInput = true;
		}
		if (m_CameraView)
		{
			m_CameraView.Listener.ReadyForBindings += HandleReadyForBindings;
		}
		ToggleMouseLook(); // Disable mouse look initially
	}
	
	void HandleReadyForBindings (int frameId, string path, bool isMainFrame)
	{
		if (isMainFrame)
		{
			m_CameraView.View.BindCall("BumpBox", (System.Action<float>)this.BumpBox);
			m_CameraView.View.BindCall("ToggleMouseLook", (System.Action)this.ToggleMouseLook);
		}
	}

	public void ToggleMouseLook()
	{
		foreach (MouseLook mouseLook in GameObject.Find("First Person Controller").GetComponentsInChildren<MouseLook>())
		{
			mouseLook.enabled = !mouseLook.enabled;
		}
	}
	
	public void BumpBox(float force)
	{
		if (this.rigidbody)
		{
			this.rigidbody.AddForce(new Vector3(0, force, 0));
		}
	}

	private void ShootRayFromCamera(Vector3 inputDevicePosition)
	{
		RaycastHit hitInfo;
		if (Physics.Raycast(m_MainCamera.ScreenPointToRay(inputDevicePosition), out hitInfo))
		{
			if (hitInfo.collider.name == "DynamicCube")
			{
				if (m_MainCamera != null)
				{
					var body = hitInfo.collider.rigidbody;
					var force = (body.position - m_MainCamera.transform.position).normalized * 566;
					body.AddForce(force);
				}
			}
		}
	}

	// Update is called once per frame
	void Update()
	{
		for (int i = 0; i < InputManager.TouchesCount; ++i)
		{
			var touch = InputManager.GetTouch(i);
			if (touch.phase == TouchPhase.Began)
			{
				ShootRayFromCamera(touch.position);
			}
		}
	}
}
