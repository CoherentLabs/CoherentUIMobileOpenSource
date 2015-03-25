using UnityEngine;
using System;
using System.Runtime.InteropServices;

/// <summary>
/// Component containing a Coherent UI view for surfaces on mobile platforms.
/// </summary>
[AddComponentMenu("Coherent UI/Mobile Surface UI View")]
public class MobileSurfaceView : CoherentUIView
{
	#if (UNITY_IPHONE || UNITY_ANDROID) && !UNITY_EDITOR
		private Texture2D m_Texture;

		private Color32[] m_Data;
		private GCHandle m_DataPin;

		public void Start()
		{
			Listener.BitmapReady += this.UpdateTexture;

			m_Texture = new Texture2D(Width, Height, TextureFormat.ARGB32,
				false);

			var mesh = GetComponent<MeshRenderer>();
			mesh.material.mainTexture = m_Texture;
			m_Data = new Color32[Width * Height];
		}

		private void UpdateTexture(IntPtr buffer, int size)
		{
			m_DataPin.Free();
			m_Texture.SetPixels32(m_Data);
			m_Texture.Apply();
		}

		/// <summary>
		/// Update the view. This method triggers the view to be rendered to the
		/// underlaying texture.
		/// </summary>
		/// <returns>true if the view was actually updated</returns>
		public bool UpdateView()
		{
			if (View != null)
			{
				m_DataPin = GCHandle.Alloc(m_Data,
					System.Runtime.InteropServices.GCHandleType.Pinned);

				return View.GetAsBitmap(m_DataPin.AddrOfPinnedObject(),
					Width * Height * 4);
			}
			return false;
		}
#else
	/// <summary>
	/// Update the view. This method triggers the view to be rendered to the
	/// underlaying texture.
	/// </summary>
	/// <returns>true if the view was actually updated</returns>
	public bool UpdateView()
	{
		return true;
	}
#endif

	public override bool ReceivesInput
	{
		get { return false; }
		set { }
	}

	public override bool IsSurfaceView
	{
		get { return true; }
	}
}
