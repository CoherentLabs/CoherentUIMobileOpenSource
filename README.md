# Coherent UI Mobile Open Source

Welcome to the Coherent UI Mobile Open Source!

*Coherent UI* Mobile is a modern user interface middleware solution that
allows you to integrate HTML pages built with CSS and JavaScript in your
game with Unity3D. 

Currently Unity 5.0 is not supported due to
this bug http://fogbugz.unity3d.com/default.asp?679875_o1li58umnfjk9jhr

Unity 4.6.3 is supported.
Please note, that there is a bug in Unity 4.6.3 that gives you the following
error when exporting iOS games:

*"You are using Unity iPhone Basic. You are not allowed to remove the Unity splash screen from your game."*

You can workaround it by changing the value of the *Launch Screen** key to NO in the Info.plist of the exported game.

## Gettting Started

For building run the Build.py script.

Built binaries are compatible with Coherent UI 1.x Mobile

### Requirements 

* The Unity3D plugin supports Unity3D 4.6.3 on Windows and Mac OS X
* Supports Android target version 4.0 or later
* Supports iOS target version 5.0 latear

## License

Copyright (c) 2012-2015, Coherent Labs AD
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of Coherent UI nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
