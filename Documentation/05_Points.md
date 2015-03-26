#Important points

##Binding

Binding *C#*, *UnityScript* and *Boo* handlers to JavaScript callbacks
is the same as binding for the .Net platform. You have to register
handlers when the `UnityViewListener`'s `ReadyForBindings` event is
fired. You can do that by using either `Coherent.UI.View.BindCall`
(for single-cast delegates) and `Coherent.UI.View.RegisterForEvent`
(when you have multiple subscribers for the event).
For more details see the general reference documentation,
chapter **Binding for .Net**.

~~~~~{.cs}
private void RegisterBindings(int frame, string url, bool isMain)
{
	if (isMain)
	{
		var view = ViewComponent.View;
		if (view != null)
		{
			// When engine.call('NewGame') is executed in JavaScript,
			// the this.NewGame method will be called as well
			view.BindCall("NewGame", (System.Action)this.NewGame);
		}
	}
}
~~~~~

The `Coherent.UI.View` can be obtained using the `View` property
of the `CoherentUIView` component.

To take advantage of the *Unity3D* component and message system each
CoherentUIView has the `InterceptAllEvents` property. If intercepting of
all events is enabled, any event triggered in *JavaScript* on the
`engine` object is forwarded to the game object containing the view.
This is done by using `SendMessage` with `Coherent.UI.Binding.Value[]`
containing all the event arguments.

~~~~~{.js}
engine.trigger('Event', 42);
// will execute SendMessage('Event', [42])
~~~~~

**Note:** Using this method of handling triggered events has some
additional overhead over the direct .Net `Coherent.UI.View.RegisterForEvent`
method.

##CoherentMethod attribute for .NET scripts

Instead of handling the `ReadyForBindings` event and doing `BindCall` or
`RegisterForEvent` by yourself, you can use the `CoherentMethod` attribute
to decorate methods in components.

**Warning:** This attribute only works if the `CoherentUIView`'s
`Enable Binding Attribute` is set to *true*. By default it is **false**.

The decorated methods will be
automatically bound to the `View` owned by the `CoherentUIView`
component in the Game Object. If the Game Object has no `CoherentUIView`
component, the attribute has no effect.
The `CoherentMethod` attribute has a string property for the JavaScript
event name, and an optional boolean parameter that specifies whether
the method is a *call* or *event* handler (*calls* can have only a single
handler, while *events* may have many). Here's an example component
using the attribute:

~~~~~{.cs}
public class BindingComponent : MonoBehaviour {

	[Coherent.UI.CoherentMethod("NewGame")]
	void MyCallHandler()
	{
		Debug.Log("MyCallHandler called in response to engine.call('NewGame')");
	}

	[Coherent.UI.CoherentMethod("EnemySpotted", true)]
	void MyEventHandler()
	{
		Debug.Log("MyEventHandler called in response to engine.trigger('EnemySpotted')");
	}
}
~~~~~

**Warning:** Binding methods using the `CoherentMethod` attribute is easier
than doing it manually in `ReadyForBindings`, but presents possible
performance penalties during game startup. When the `CoherentUIView`
component is created, it searches all the other components in the host
Game Object for methods marked with `CoherentMethod` using reflection.
This can be a costly operation and to prevent undesirable slowdowns
during startup the `Enable Binding Attribute` property for each
`CoherentUIView` is set to **false** by default.

**Warning:** The `CoherentMethod` attribute currently does **NOT** support
dynamically added components. Methods decorated with the attribute are
only bound when the `CoherentUIView` component is created, which is
usually when the Game Object it is part of is created.

##JavaScript and Unity3D

Briefly, Unity3D can call JavaScript using *events*; JavaScript can
call Unity3D using *events* or *calls*.

- Events

Events can be called by JavaScript
~~~~~{.js}
engine.trigger("MyEvent", args);
~~~~~
or Unity3D
~~~~~{.cs}
coherentUIView.View.TriggerEvent("MyEvent", args);
~~~~~

The "MyEvent" will be handled by any registered method in JavaScript
~~~~~{.js}
engine.on("MyEvent", function() {...});
~~~~~
or Unity3D
~~~~~{.cs}
coherentUIView.View.RegisterForEvent("MyEvent", handlerMethod);
~~~~~

- Calls

Calls, unlike events, can have only one handler.
The can also return values.
To execute a *call* from JavaScript use
~~~~~{.js}
engine.call("MyCall", args);
~~~~~
It will be handled by a method registered using
~~~~~{.cs}
coherentUIView.View.BindCall("MyCall", handlerMethod);
~~~~~

##Namespaces

Coherent UI classes are placed in the `Coherent.UI.Mobile` namespace for the Mobile version.
You can check the Coherent UI files - for instance CoherentUIView and take a
look at the beginning of the file at how the namespaces are imported
depending on the Unity platform targeted. Exceptions
to that rule are classes that cannot be in a namespace because Unity
doesn't allow it, such as components that derive from `MonoBehaviour`.

##Subclassing CoherentUIView and UnityViewListener

The default CoherentUIView component and the UnityViewListener provide
the most common functionality and are usually enough for normal usage of
*Coherent UI*. If you need custom behavior, you need to subclass them.

The class you derive from UnityViewListener would usually subscribe to
various events that aren't handled by default. It is recommended not to
override the OnViewCreated callbacks since the UnityViewListener class
contains important logic that you would have to implement yourself
otherwise.

The class you derive from CoherentUIView would only need to create an
instance of your custom View Listener. This can be done by copying the
Update method of CoherentUIView and editing it appropriately.

Note that when subclassing CoherentUIView you will no longer be able to
view or edit the properties in the Inspector. That's because we're using
C# properties in our component instead of fields and they are not
automatically shown. To show the properties of a given C# script we need
to make a new Editor script (inside the Editor folder of your Assets)
that shows the properties for a specific type. We've already done that
for `CoherentUIView`, but you'll have to do it yourself for derived
classes. The script contents should be the following:


	using UnityEngine;
	using System.Collections;
	using UnityEditor;

	[CustomEditor(typeof(<YourType>))]
	public class <YourType>ViewEditor : Editor {

		private <YourType> m_Target;
		private CoherentPropertyField[] m_Fields;

		public void OnEnable() {
			m_Target = target as <YourType>;
			m_Fields = CoherentExposeProperties.GetProperties(m_Target);
		}

		public override void OnInspectorGUI() {
			if(m_Target == null)
				return;
			this.DrawDefaultInspector();
			CoherentExposeProperties.Expose(m_Fields);
		}
	}

Just replace *<YourType>* with the actual name of your class.

##Coherent UI system lifetime

Since initialization of the CoherentUISystem component is a costly
operation, it is designed to be done once in the first scene of your
game. The component itself has the same lifetime as the application.
Since Unity tears down the state of the game when you load a new scene,
the component is marked not be destroyed using the DontDestroyOnLoad()
function. This makes it persist through scenes and is available using
the Object.FindObjectOfType function. Getting the system can be done
with the following line of code:

	var uiSystem = Object.FindObjectOfType(typeof(CoherentUISystem)) as CoherentUISystem;

##Customizing initialization of the Coherent UI System

When using only `CoherentUIView` components, the Coherent UI System will
be automatically initialized using the default parameters.
These parameters define global system settings such as whether cookies
are enabled, local storage and cache paths, debugger port and others.
Check the `CoherentUISystem` component for a full list.

The Coherent UI System can be initialized with parameters other than the
defaults in the following ways. Either drag the `CoherentUISystem`
component to any object and edit the properties in the Inspector window,
or edit the *CoherentUISystem.cs* script located in
*Standard Assets/Scripts/CoherentUI* to fit your needs.

The `CoherentUISystem` component is designed to be only one in the whole
game. Adding more than one `CoherentUISystem`s to your level will
result in undefined behavior.

##Update cycle

In a standard C++ or .NET application you need to poll Coherent UI for
surfaces each frame using the Update and FetchSurfaces API calls. In our
Unity integration, this is all hidden from you and you don't have to
worry about it. The CoherentUIViewRenderer component issues rendering
events which are handled by our library. All that's left for you is
to drag a CoherentUIView component on an object!

##Input forwarding

Due to differences in the input management for iOS/Android, Coherent UI provides a helper class for getting the list of touches for the current frame.
The Coherent touch structure is `InputManager.CoherentTouch` - very much the same as Unity3D's `Touch` structure. The equivalent methods for `Input.touchCount`
and `Input.GetTouch(index)` are `InputManager.TouchesCount` and `InputManger.GetTouch(index)`, respectively.

For **iOS** there's practically no difference between the Coherent and Unity3D touches. For **Android**, however, Coherent touches contain only touches that reached the game
(i.e. the touch wasn't filtered using the techniques described in the next paragraph).
The Unity3D supplied touches on Android aren't filtered and you get all of them when using the `Input.touches` array, regardless of whether they are on the UI or not.
When using Coherent UI for mobile, it is recommended to use Coherent touches instead of Unity3D touches to avoid platform differences between iOS and Android.

On iOS/Android all *Coherent UI* Views are composited in 2D on-top of your
Unity3D application content. When the user start a touch event or performs a
gesture there is a mechanism that decides if that event should be handled
by the UI or the application.
It works like this: in the properties of the View the user can select
one of three input modes for a every View - "Take All", "Take None", "Transparent". Keep in mind that all those modifiers are in effect only for events that happend are in the bounds of the View.
If the user touches outside a particular View the event is always handled by the game.

 - "Take All" specifies that all events are handled by the View and nothing goes to the game. It is usable if you want to have for instance an in-game browser. All touches on it should be
 handled by itself and not influence the game.
 - "Take None" specifies that the View passes all input to the game. This is usable if you need to just show some non-interactive views or disable their input completely in some situation.
 - "Transparent" specifies that the input is either handled by the View or the game - usable for HUDs, Menus etc.
For Views with "Transparent" input mode the user is the one in charge of deciding if an element on the page is interactive - hence should receive input or is 'input transparent'.
**Note:** **iOS**: Input "Transparent" views work correctly **ONLY** if you have included *coherent.js* in your HTML page.
**Note:** **Android**: Input on any view works correctly **ONLY** if you have included *coherent.js* in your HTML page.

Upon touch within a View, *Coherent UI Mobile* inspects the touched element:

 - if the element has the CSS class *coui-noinput* it passes input to the game. The element does not accept input.
 - if the element has the CSS class *coui-inputcallback*, a method called *couiInputCallback(x, y)* is called on the element with the
 coordinates of the event. It should return "true" if the user wants the element to handle the input and "false" if the game should handle it. This allows for custom fine-grained control in JavaScript
 on which elements are interactive.
 - if the class *coui-noinput* and *coui-inputcallback* are missing from the element, it is assumed that it is interactable and takes the input.

To summarize:
If a View has an Input State set to `Transparent`, all elements are by default interactive and take input. You can mark elements with the CSS class *coui-noinput* to make them transparent to input.
 If you need more advanced logic when deciding if an element is interactive or not you can decorate it with *coui-inputcallback* and implement a method *couiInputCallback(x, y)* on it.

##Custom file handler

The `CoherentUISystem` component makes use of a static factory function
object (`FileHandlerFactoryFunc`) to create the `FileHandler` object that is used
reading URLs with the *coui* scheme. The default function returns a handler that
reads resources from the path set by *Edit &rarr; Project Settings &rarr; Coherent UI &rarr; Select UI folder*
for the Editor and in the Data folder for built games.

The factory function object is public and can be customized. The `FileHandler`
it returns is passed to the UI initialization routine in the `Start` method of the
`CoherentUISystem` component. That means the user should set the factory function
prior to the invocation of the `Start` method of the components - e.g. in the `Awake` method.

**Note:** You can check the execution order of event function in Unity3D
[on this page](http://http://docs.unity3d.com/Documentation/Manual/ExecutionOrder.html)

This is an example usage of a custom file handler:

~~~~~{.cs}
public class CustomFileHandlerScript : MonoBehaviour {

	class CustomFileHandler : Coherent.UI.FileHandler
	{
		public override void ReadFile (string url, Coherent.UI.ResourceResponse response)
		{
			// Implementation here
		}

		public override void WriteFile (string url, Coherent.UI.ResourceData resource)
		{
			// Implementation here
		}
	}

	//----------------------------------------------------------------------------------

	void Awake()
	{
		CoherentUISystem.FileHandlerFactoryFunc = () => { return new CustomFileHandler(); };
	}
}
~~~~~

##UI Resources

Files for Coherent UI are by default selected from the folder set by *Edit &rarr; Project Settings &rarr; Coherent UI &rarr; Select UI folder*. Resources found there will be used by the editor and will automatically be copied in your game upon build.

The selected UI resources folder is per-user so that different developers working on the game can have their UI folders wherever they want on their machine. You can also set a per-project folder for the UI resources. This is done by extending the CoherentPostProcessor class by setting a static const setting named ProjectUIResources:
~~~~~{.cs}
public partial class CoherentPostProcessor {
	public static string ProjectUIResources = "relative/path/to/ui/resources";
}
~~~~~

The per-project path must be relative to the folder of the project and the extension class should live under the 'Editor' folder in Unity. This feature is very handy also if you build your game on machines that can't start Unity and you use the command line. The per-user setting overrides the per-project one so that developers can still put their resources wherever they want.

**Warning:**  When using the "Build & run" option for Android, the required
resources will **NOT** be packaged because this is done as a post-build step.
Unity3D pushes the non-repacked APK first, and then executes the post-build
step which results in an APK without UI resources and including unneeded
files. To work around that, you should either use the "Build" option, which
produces an APK on your computer that can be installed on a device using
the ADB tool in the Android SDK, or generate an Eclipse project and run it
from the IDE.

##Mobile Surface Views

Displaying CoherentUI view on a surface is straightforward for Desktop
platforms. Unfortunately the performance and API restrictions of the current
platforms do not allow us to fully support views on surfaces for mobile. *Mobile
Surface Views* allow displaying of HTML5 content on a surface in mobile games
with the following limitations:

* the view must be explicitly updated using the `MobileSurfaceView.UpdateView`
  method.
* there is no input for the view. Input could be simulated using JavaScript.

To create a *Mobile Surface View*, simply add the component to a object with a
renderer component. The `MobileSurfaceView` component will create a new texture
for the renderer and replace the main texture of the material with the view.

See also the Mobile Surface Sample in the [samples walk-through](03_Samples.md).

##Logging

Coherent UI logs are automatically redirected to the Unity console
(or game log for built games) using the Debug.Log method.
You can control the minimum severity of the Coherent UI logs when
initializing the Coherent UI System.


