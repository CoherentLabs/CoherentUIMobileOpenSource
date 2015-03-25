LOCAL_PATH := $(call my-dir)

ifeq ($(TARGET_ARCH),arm)
	LOCAL_TARGET_ABI := armeabi
else ifeq ($(TARGET_ARCH),mips)
	LOCAL_TARGET_ABI := mips
else ifeq ($(TARGET_ARCH),x86)
	LOCAL_TARGET_ABI := x86
endif

# Definitions for CoherentAndroidNative

include $(CLEAR_VARS)

LOCAL_CPPFLAGS   += -std=c++0x -frtti -fexceptions -Wno-format-security
LOCAL_C_INCLUDES += ../../ ../../../ThirdParty
LOCAL_MODULE     := CoherentAndroidNative
LOCAL_LDLIBS     := -llog
LOCAL_STATIC_LIBRARIES := CoherentMemoryManagement-prebuilt CoherentLogging-prebuilt CoherentPlatform-prebuilt
LOCAL_SRC_FILES  := JNI_OnLoad.cpp ResourceResponseImpl.cpp ViewImpl.cpp UISystem.cpp StringConversions.cpp CriticalSection.cpp
# Binding files
LOCAL_SRC_FILES  +=  ScriptEventWriter.cpp BinderAndroid.cpp ../../Binding/Blob/Blob.cpp ../../Binding/Blob/BlobStream.cpp ../../Binding/Blob/FromBlob.cpp ../../Binding/Blob/ToBlob.cpp Binding/BlobFromJS.cpp Binding/EventArguments.cpp Binding/JStoNativeBinder.cpp Binding/ViewBinder.cpp ../../Binding/Binding.cpp ../../Binding/EventHandler.cpp ../../Binding/Property.cpp ../../Logging/ILogHandler.cpp ../../Logging/Logger.cpp

ifeq ($(TARGET_ARCH),arm)
	LOCAL_CPPFLAGS += -mno-unaligned-access
endif

ifdef BUILD_CONFIG
ifeq ($(BUILD_CONFIG), $(filter $(BUILD_CONFIG), dotnet unity3d_basic unity3d_standard unity3d_pro))
	LOCAL_CPPFLAGS += -DCOHERENT_DOT_NET
	LOCAL_SRC_FILES += ManagedReferences.cpp ../../DotNet/swig/Android/UISystem_wrap.cxx UnityLogHandlerDecorator.cpp
endif
endif

include $(BUILD_SHARED_LIBRARY)