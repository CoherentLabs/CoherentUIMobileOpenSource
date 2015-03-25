using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using UnityEditor;

[CustomEditor(typeof(MobileSurfaceView))]
public class MobileSurfaceViewEditor : Editor
{

    private MobileSurfaceView m_Target;
    private CoherentFoldout[] m_Fields;

    public void OnEnable()
    {
        m_Target = target as MobileSurfaceView;
        var fields = CoherentExposeProperties.GetProperties(m_Target);
        var index = (int)CoherentExposePropertyInfo.FoldoutType.General;
        var renderingIndex =
            (int)CoherentExposePropertyInfo.FoldoutType.Rendering;
        m_Fields = new CoherentFoldout[]
        {
            fields[index],
            fields[renderingIndex]
        };
    }

    public override void OnInspectorGUI()
    {
        if (m_Target == null)
            return;
        this.DrawDefaultInspector();
        CoherentExposeProperties.Expose(m_Fields);
    }
}

