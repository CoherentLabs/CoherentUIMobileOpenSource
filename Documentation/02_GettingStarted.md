#Getting Started

##Prerequisites

* The plugin supports *Unity3D 4.6.3*
* Both Free and Pro version of Unity is supported
* *Coherent UI Mobile* supports iOS 5.1 and above.
* *Coherent UI Mobile* supports Android 4.0 and above.
* *Java Development Kit (JDK)* is required for exporting to Android.
Referring to the folder you installed JDK in as *<JDK\>*, you need to have
the <JDK>/bin folder in your PATH environment variable and set the
**JAVA_HOME** environment variable to <JDK>.
* *Android Native Development Kit (NDK)* is required for building the shared Android
library. You should set the **ANDROID\_NDK\_HOME** environment variable to refer to
the NDK installation folder
* *Android Development Tools* are required for building the plugin files. Set the **ANDROID\_SDK\_HOME**
to refer to *<adt-installation-folder>/sdk*.


**Note:** Coherent UI SDK has different requirements than the
[requirements for Unity3D](http://unity3d.com/unity/system-requirements). Unity
supports older hardware via fallbacks, which may prevent Coherent UI SDK from
running on older graphics cards. Please make sure that the  requirements of
Coherent UI SDK match your client requirements for the project.


##Building the plugin

Run the Build.py script, located in the base directory of the repository.
Before executing the script, make sure you have set the ANDROID_SDK_HOME
and ANDROID_NDK_HOME environment variables.

On Windows, it will build the plugin files required for Android development:

* CoherentAndroid.jar
* CoherentUIMobileNet.dll (for Android)
* libCoherentAndroidNative.so

On Mac, it will build the plugin files for Android described above
plus those for iOS:

* CoherentUIMobileNet.dll (for iOS)
* libCoherentUIMobile.a

##Setting up your project

No matter if you are using Coherent UI 1.x Mobile or not, you
can still take full advantage of the mobile plugin in your project via
*CoherentUIMobileOpenSource* except for these two limitations:

* Desktop preview is available only to current clients of Coherent UI 1.x.
* iOS and Android are the only available target platforms if you do *NOT* have Coherent UI 1.x

The setup for clients and non-clients is similar, however specific steps will be tagged
with [1.x] for Coherent UI 1.x clients and with [CleanInstall] respectively for users
who don't have Coherent UI 1.x.

After building the libraries for the plugin by executing Build.py,
copy the following files:

###For Android

Rename the **CoherentUIMobileNet.dll**, located in the
*<repo-base\>/Build/Android/Release* directory, to **CoherentUIMobileNet.android**
and place it in the *<YourUnityProject>/Assets/Plugins* folder.

Then, copy the **libs** folder and **CoherentAndroid.jar** from *<repo-base\>Src\Android*
and replace the ones in *<YourUnityProject\>/Assets/Plugins/Android*

**Note:** [1.x] If you're using an older version of Coherent UI 1.x than *1.8.9.2*, replace the
*<YourProject\>Assets/Editor/CoherentUI/CoherentPostProcessor.cs* and
*<YourProject\>Assets/Editor/CoherentUI/AndroidPostProcessor.cs* with
those in *<repo-base\>UnityIntegration/Assets/Editor/CoherentUI/*. This will ensure
that exporting Android game will happen correctly since the plugin structure has been
changed in *Coherent UI 1.8.9.2* in order to support newer versions of Unity with IL2CPP.

###For IOS

Copy the **CoherentUIMobileNet.dll**, located in the
*<repo-base\>/Build/iOS/Release* directory and place it in the
*<YourUnityProject>/Assets/Plugins* folder.

Copy the **libCoherentUIMobile.a** library for the desired architecture
and SDK from *<repo-base\>\Build\iOS\Release Unity* (or you can combine
the already built libraries into archive library with **lipo** tool)
and replace the one in *<YourUnityProject\>/Assets/Plugins/iOS*, then
copy the **CoherentUI.mm** file from *<repo-base\>\UnityIntegration\Assets\Plugins\iOS* and
place it next to the libCoherentUIMobile.a in your project.


[Clean Install] Copy the entire content of the *<repo-base\>\UnityIntegration\Assets\* folder in
the *Assets* folder of you project. This will copy the required scripts for developing
your UI with Coherent UI Mobile and for exporting games.

##How to use the plugin

After placing the binary files in your project, the two
main scripts you'll be using are *Standard
Assets/Scripts/CoherentUI/CoherentUISystem* and *Standard
Assets/Scripts/CoherentUI/CoherentUIView*. The easiest way to use
Coherent UI is to drag the `CoherentUIView` component onto an object
and hitting Play - that's it! Everything will be up and running.
Actually, you can do the same for most of the usage scenarios -
just drag the component and then configure it in the
inspector. Here's a bit more detail about the two scripts.

The first script, `CoherentUISystem`, defines initialization
parameters of the Coherent UI System and should be placed no more than
once in your project. The UI system is meant to be initialized in
the first scene and live throughout the game's lifetime. You need to add
this component to your scene only if you need custom initialization of
the Coherent UI System. For the most cases, using only CoherentUIViews
is enough - they will automatically create an instance of the Coherent
UI System for you with reasonable default parameters.
Check [CoherentUISystem Lifetime](05_Points.md) for details.

The second script, `CoherentUIView`, will represent a single HTML page.
This is the component that renders your CSS and JavaScript animations
and makes your game alive. This component can be placed on any object
that is renderable and serves as its material. When placing it on an
object all the needed components are automatically created, hidden from
you, and the rendered output is bound to the mainTexture of a new
material that is created at runtime. This material is set as the
gameObject's renderer material so that you see the page rendered on your
object.

You can also place views on cameras. When doing so, you get your page
rendered on the whole viewport of the camera. This way you can easily
add a heads-up display for your game. Don't forget to mark your HUD as
transparent and make your HTML page transparent! There's even an
option for drawing the HUD after the post-effects if you like (The
option is available for non-HUDs, too, but it results in a no-op because
it doesn't make sense).
