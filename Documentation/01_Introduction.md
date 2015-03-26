#Introduction


This guide describes the features of the Coherent UI integration in
Unity3D. A basic understanding of the Unity3D engine, as well as
HTML/JavaScript is assumed. Having basic knowledge of the C++ API and
design of Coherent UI may be advantageous. To familiarize yourself with
Coherent UI, please read the main documentation file or visit
[Coherent UI](http://coherent-labs.com/) website.

#Brief overview of Coherent UI

*Coherent UI* is a modern user interface middleware that allows
you to integrate HTML pages built with CSS and JavaScript in your game. The
communication between your game and the HTML engine is done through the
UI System component. Each HTML page is called a *View*. The *View*
component allows you to perform operations on the page, such as
resizing, navigating to a different URL, sending input events, executing
custom JavaScript code and so on. You can create a view through the
CreateView method of the UI System component. It requires you to supply
some initialization parameters, such as width, height, initial URL, etc.
It also requires an instance of a *View Listener*. *Coherent UI* is
highly asynchronous by design, meaning that when you change the URL of a
View, for example, the function will return immediately and you will
receive a notification when the URL has actually changed. The
`ViewListener` is the class that receives such notifications for a
specific view - when the URL was changed, the page you're trying
to open requires authentication details, etc.
