#!/usr/bin/env python

import os
import subprocess
import platform


def build_android(pathToSwig, baseDir, isMac):
    swigCommand = ['mono'] if isMac else []
    swigCommand.extend([
        pathToSwig,
        '-csharp',
        '-outdir', 'Src/DotNet/swig/Android',
        '-namespace', 'Coherent.UI.Mobile',
        '-platform', 'Android',
        '-DCOHERENT_PLATFORM_ANDROID',
        '-dllimport', 'CoherentAndroidNative',
        '-DCOHERENT_UNITY3D_BUILD',
        '-o', 'Src/DotNet/swig/Android/UISystem_wrap.cxx',
        '-c++', 'Src/UISystem.i'])

    subprocess.check_call(swigCommand)

    projectPath = 'Src/DotNet/MobileNet/CoherentUIMobileNetAndroid.sln'
    if isMac:
        subprocess.check_call(['/Applications/Xamarin Studio.app/Contents/MacOS/mdtool',
                         'build', projectPath, '-c:Release'])
    else:
        subprocess.check_call(['msbuild', projectPath, '/p:Configuration=Release'])

    androidSdkHome = os.environ.get('ANDROID_SDK_HOME')
    if androidSdkHome == None:
        sys.exit(
            'Please set the ANDROID_SDK_HOME environment variable first! It should point to the root of the Android SDK')
    androidNdkHome = os.environ.get('ANDROID_NDK_HOME')
    if androidNdkHome == None:
        sys.exit(
            'Please set the ANDROID_SDK_HOME environment variable first! It should point to the root of the Android SDK.')

    subprocess.check_call('javac' + ' -bootclasspath '
                          + ('%s/platforms/android-18/android.jar' %
                             androidSdkHome) + ' '
                          + 'Src/Android/com/coherentlabs/ui/*.java'
                          + ' -target 1.6 -source 1.6 -d Src/Android/', shell=True)

    os.chdir('Src/Android')
    subprocess.check_call(
        'jar cvfM CoherentAndroid.jar com/coherentlabs/ui/*.class', shell=True)
    os.chdir(baseDir)
    subprocess.check_call(androidNdkHome + '/ndk-build'
                          + ' -C Src/Android/jni BUILD_CONFIG=dotnet', shell=True)


def build_ios_native_library(arch, sdk):
    subprocess.check_call(['xcodebuild', 'build',
                           '-project', 'Src/CoherentUIMobile/CoherentUIMobile.xcodeproj',
                           '-target', 'CoherentUIMobile',
                           '-configuration', 'Release Unity',
                           '-arch', arch,
                           '-sdk', sdk,
                           'ONLY_ACTIVE_ARCH=NO',
                           'SYMROOT=' + '../../Build/iOS/Release Unity/' + arch])


def build_ios(pathToSwig):
    subprocess.check_call(['mono',
						   pathToSwig,
						   '-csharp',
						   '-outdir', 'Src/DotNet/swig/iOS',
						   '-namespace', 'Coherent.UI.Mobile',
						   '-platform', 'iOS',
						   '-DCOHERENT_PLATFORM_IOS',
                           '-ITools/DotNETWrapperGenerator/swig-2.0.9/Lib_iOS',
                           '-dllimport', '__Internal',
                           '-DCOHERENT_UNITY3D_BUILD',
						   '-o', 'Src/DotNet/swig/iOS/UISystem_wrap.cxx',
						   '-c++', 'Src/UISystem.i'])

    subprocess.check_call(
        ['python', 'Src/DotNet/MobileNet/ReplaceHandleRefForIOS.py', '--base', '.'])

    projectPath = 'Src/DotNet/MobileNet/CoherentUIMobileNetIOS.sln'
    subprocess.check_call(['/Applications/Xamarin Studio.app/Contents/MacOS/mdtool',
                     'build', projectPath, '-c:Release'])

    build_ios_native_library('x86_64', 'iphonesimulator')
    build_ios_native_library('i386', 'iphonesimulator')
    build_ios_native_library('arm64', 'iphoneos')


if __name__ == '__main__':
    baseDir = os.path.abspath(os.path.dirname(__file__))
    os.chdir(baseDir)

    swig = os.path.join(baseDir, 'Tools/DotNETWrapperGenerator/Swigger.exe')

    isMac = True
    if platform.system() == 'Darwin':
        build_ios(swig)
    else:
        isMac = False
        print('Cannot build iOS library on ' + platform.system())

    build_android(swig, baseDir, isMac)
