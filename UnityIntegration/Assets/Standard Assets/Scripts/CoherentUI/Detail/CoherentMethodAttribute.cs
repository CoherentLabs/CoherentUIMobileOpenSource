#if UNITY_STANDALONE || UNITY_STANDALONE_WIN || UNITY_STANDALONE_OSX
#define COHERENT_UNITY_STANDALONE
#endif

#if UNITY_NACL || UNITY_WEBPLAYER
#define COHERENT_UNITY_UNSUPPORTED_PLATFORM
#endif

using System;
using System.Reflection;
using System.Linq;
using System.Linq.Expressions;
using System.Collections.Generic;
using UnityEngine;

#if COHERENT_UNITY_STANDALONE || COHERENT_UNITY_UNSUPPORTED_PLATFORM
namespace Coherent.UI
#elif UNITY_IPHONE || UNITY_ANDROID
namespace Coherent.UI.Mobile
#endif
{

	[AttributeUsage(AttributeTargets.Method)]
	public class CoherentMethodAttribute : Attribute
	{
		public string ScriptEventName { get; set; }
		public bool IsEvent { get; set; }

		public CoherentMethodAttribute(string scriptEventName)
			: this(scriptEventName, false)
		{
		}

		public CoherentMethodAttribute(string scriptEventName, bool isEvent)
		{
			ScriptEventName = scriptEventName;
			IsEvent = isEvent;
		}
	}

	public struct CoherentMethodBindingInfo
	{
		public string ScriptEventName { get; set; }
		public MethodInfo Method { get; set; }
		public Delegate BoundFunction { get; set; }
		public bool IsEvent { get; set; }
	}

	public static class CoherentMethodHelper
	{
		static Dictionary<Type, List<CoherentMethodBindingInfo>> s_CoherentMethodsCache;

		static CoherentMethodHelper()
		{
			s_CoherentMethodsCache = new Dictionary<Type, List<CoherentMethodBindingInfo>>();
		}

		private static CoherentMethodBindingInfo BindMethod(CoherentMethodBindingInfo method, Component component)
		{
			return (method.Method != null)
				? new CoherentMethodBindingInfo() {
						ScriptEventName = method.ScriptEventName,
						BoundFunction = ToDelegate(method.Method, component),
						IsEvent = method.IsEvent
					}
				: method;
		}


		private static List<CoherentMethodBindingInfo> BindMethods(List<CoherentMethodBindingInfo> methods, Component component)
		{
			return methods.Select((x) => BindMethod(x, component)).ToList();
		}

		private static List<CoherentMethodBindingInfo> GetCoherentMethodsInComponent(Component component)
		{
			List<CoherentMethodBindingInfo> coherentMethods = new List<CoherentMethodBindingInfo>();

			Type type = component.GetType();

			List<CoherentMethodBindingInfo> cachedCoherentMethods;
			if (s_CoherentMethodsCache.TryGetValue(type, out cachedCoherentMethods))
			{
				return BindMethods(cachedCoherentMethods, component);
			}

			// Iterate methods of each type
			BindingFlags bindingFlags = BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Static | BindingFlags.Instance;
			foreach (MethodInfo methodInfo in type.GetMethods(bindingFlags))
			{
				// Iterate custom attributes
				var attributes = methodInfo.GetCustomAttributes(typeof(CoherentMethodAttribute), true);
				foreach (object customAttribute in attributes)
				{
					CoherentMethodAttribute coherentMethodAttribute = (customAttribute as CoherentMethodAttribute);

					if (methodInfo.IsStatic)
					{
						coherentMethods.Add(new CoherentMethodBindingInfo(){
							ScriptEventName = coherentMethodAttribute.ScriptEventName,
							BoundFunction = ToDelegate(methodInfo, null),
							IsEvent = coherentMethodAttribute.IsEvent
						});
					}
					else
					{
						coherentMethods.Add(new CoherentMethodBindingInfo(){
							ScriptEventName = coherentMethodAttribute.ScriptEventName,
							Method = methodInfo,
							IsEvent = coherentMethodAttribute.IsEvent
						});
					}
				}
			}

			s_CoherentMethodsCache.Add(type, coherentMethods);

			return BindMethods(coherentMethods, component);
		}

		public static List<CoherentMethodBindingInfo> GetCoherentMethodsInGameObject(GameObject gameObject)
		{
			List<CoherentMethodBindingInfo> coherentMethods = new List<CoherentMethodBindingInfo>();

			Component[] components = gameObject.GetComponents(typeof(MonoBehaviour));

			foreach (var item in components)
			{
				MonoBehaviour monoBehaviour = item as MonoBehaviour;
				if (!monoBehaviour.enabled)
				{
					continue;
				}
				coherentMethods.AddRange(GetCoherentMethodsInComponent(item));
			}

			return coherentMethods;
		}

		/// <summary>
		/// Builds a Delegate instance from the supplied MethodInfo object and a target to invoke against.
		/// </summary>
		public static Delegate ToDelegate(MethodInfo methodInfo, object target)
		{
			if (methodInfo == null) throw new ArgumentNullException("Cannot create a delegate instance from null MethodInfo!");

			Type delegateType;

			var typeArgs = methodInfo.GetParameters()
				.Select(p => p.ParameterType)
				.ToList();

			if (methodInfo.ReturnType == typeof(void))
			{
				delegateType = Expression.GetActionType(typeArgs.ToArray());
			}
			else
			{
				typeArgs.Add(methodInfo.ReturnType);
				delegateType = Expression.GetFuncType(typeArgs.ToArray());
			}

			var result = (target == null)
				? Delegate.CreateDelegate(delegateType, methodInfo)
				: Delegate.CreateDelegate(delegateType, target, methodInfo);

			return result;
		}
	}
}
