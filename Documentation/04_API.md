#Programmer's API

The Unity API is an extension of the normal .Net API, and it is
contained in the main API Reference documentation.


##CoherentUISystem properties for Mobile

- **URL cache** - Decides if the device URL cache will be used.
- **Memory cache size** - The maximum size of the in-memory cache.
- **Disk cache size** - The maximum size of the disk cache.


##CoherentUIView properties for Mobile

- **URL** - Indicates the URL that will be initially loaded.
- **Width** - The width of the Coherent UI View in pixels.
- **Height** - The height of the Coherent UI View in pixels.
- **X Position** - The X of the Coherent UI View position relative to the upper-left corner of the device screen.
- **Y Position** - The Y of the Coherent UI View position relative to the upper-left corner of the device screen.
- **Retina rescale** - If set the content will be scaled to fit the View.
- **Enable WebGL Support** - This property is ignored due to vendor API
  limitations
- **Software only rendering** - **Android only** The View will be rendered
  without hardware acceleration. Some devices might render pages incorrectly
  because of hardware capabilities in which case software rendering helps.
  Note that advanced transformations and effects are not available with
  software rendering.
- **Input behavior** - Controls how the view handles user input (touches and gestures). The behavior only influences events that happen in the bounds of the View. If the user
touches outside a view the event goes normally to the game.
	- Transparent - the user controls which input events to be taken by the UI and which to pass to the game. This is explained in the Mobile input management section [here](05_Points.md)
	- Take all - the View takes all the input - nothing is passed to the game
	- Take none - the View takes no input - everything goes to the game
- **Transparent** - Defines if the View supports transparency.
- **Smart input** - *Available for transparent Views*. Enables transparent input.
- **Auto UI messages** - When enabled, any event triggered in
*JavaScript* on the  `engine` object is forwarded to the game object
containing the View.
- **Enable [CoherentMethodAttribute]** - Enables the usage of the Coherent Method
attribute. See the "Coherent Method attribute section" [here](05_Points.md).

