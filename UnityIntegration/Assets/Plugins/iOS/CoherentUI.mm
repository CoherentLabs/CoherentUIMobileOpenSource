extern "C" int CoherentDummyUnityCall();

const int CoherentDummyValue = CoherentDummyUnityCall();

#if defined(__cplusplus) && defined(UNITY_4_5_0)
extern "C"
#else
extern
#endif
UIViewController* UnityGetGLViewController();


#ifdef __cplusplus
extern "C"
#endif
UIViewController* CoherentGetUnityGLViewController()
{
	return UnityGetGLViewController();
}

