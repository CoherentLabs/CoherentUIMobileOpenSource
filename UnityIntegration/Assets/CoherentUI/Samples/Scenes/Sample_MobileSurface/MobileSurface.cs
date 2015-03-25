using UnityEngine;

public class MobileSurface : MonoBehaviour {
	private MobileSurfaceView m_View;

	[SerializeField]
	public Color32[] m_Colors;

	private int m_CurrentColor = 0;
	private float m_NextChange = 0;

	void Start () {
		m_View = GetComponent<MobileSurfaceView>();
	}

	public void Update()
	{
		if (m_View.View != null && Time.time >= m_NextChange)
		{
			m_NextChange = Time.time + 4;

			var color = string.Format("setColor('rgba({0},{1},{2},{3})');",
				m_Colors[m_CurrentColor].r.ToString(),
				m_Colors[m_CurrentColor].g.ToString(),
				m_Colors[m_CurrentColor].b.ToString(),
				(m_Colors[m_CurrentColor].a / 255.0).ToString());
			if (++m_CurrentColor >= m_Colors.Length)
			{
				m_CurrentColor = 0;
			}
			m_View.View.ExecuteScript(color);

			m_View.UpdateView();
		}

		transform.parent.transform.Rotate(new Vector3(0, 0, 1),
				10.0f * Time.deltaTime, Space.World);
	}
}
