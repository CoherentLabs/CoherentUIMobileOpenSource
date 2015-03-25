#if UNITY_STANDALONE || UNITY_STANDALONE_WIN || UNITY_STANDALONE_OSX
#define COHERENT_UNITY_STANDALONE
#endif

#if UNITY_NACL || UNITY_WEBPLAYER
#define COHERENT_UNITY_UNSUPPORTED_PLATFORM
#endif

//#if UNITY_EDITOR && (UNITY_IPHONE || UNITY_ANDROID)
//#define COHERENT_SIMULATE_MOBILE_IN_EDITOR
//#endif

using UnityEngine;
using System.Collections;
using Coherent.UI;

#if COHERENT_UNITY_STANDALONE || COHERENT_UNITY_UNSUPPORTED_PLATFORM
namespace Coherent.UI
#elif UNITY_IPHONE || UNITY_ANDROID
namespace Coherent.UI.Mobile
#endif
{
	public static class InputManager
	{
		const float UNITY_WHEEL_TICK_FACTOR = 1.0f / 3.0f;
		static int[] s_KeyCodeMapping;
	
		#region Keycode mapping initialization
		static InputManager ()
		{
			//KeyCode[] enumValues = (KeyCode[])System.Enum.GetValues(typeof(KeyCode));
			//int arraySize = (int)enumValues[enumValues.Length - 1];
			//System.Diagnostics.Debug.Assert(arraySize == (int)KeyCode.Joystick4Button19);
		
			s_KeyCodeMapping = new int[(int)KeyCode.Joystick4Button19 + 1];
		
			s_KeyCodeMapping [(int)KeyCode.None] = 0;
			s_KeyCodeMapping [(int)KeyCode.Backspace] = 0x08;
			s_KeyCodeMapping [(int)KeyCode.Tab] = 0x09;
			s_KeyCodeMapping [(int)KeyCode.Clear] = 0x0C;
			s_KeyCodeMapping [(int)KeyCode.Return] = 0x0D;
			s_KeyCodeMapping [(int)KeyCode.Pause] = 0x13;
			s_KeyCodeMapping [(int)KeyCode.Escape] = 0x1B;
			s_KeyCodeMapping [(int)KeyCode.Space] = 0x20;
			s_KeyCodeMapping [(int)KeyCode.Exclaim] = 0x31;
			s_KeyCodeMapping [(int)KeyCode.DoubleQuote] = 0xDE;
			s_KeyCodeMapping [(int)KeyCode.Hash] = 0x33;
			s_KeyCodeMapping [(int)KeyCode.Dollar] = 0x34;
			s_KeyCodeMapping [(int)KeyCode.Ampersand] = 0x37;
			s_KeyCodeMapping [(int)KeyCode.Quote] = 0xDE;
			s_KeyCodeMapping [(int)KeyCode.LeftParen] = 0x39;
			s_KeyCodeMapping [(int)KeyCode.RightParen] = 0x30;
			s_KeyCodeMapping [(int)KeyCode.Asterisk] = 0x38;
			s_KeyCodeMapping [(int)KeyCode.Plus] = 0xBB;
			s_KeyCodeMapping [(int)KeyCode.Comma] = 0xBC;
			s_KeyCodeMapping [(int)KeyCode.Minus] = 0xBD;
			s_KeyCodeMapping [(int)KeyCode.Period] = 0xBE;
			s_KeyCodeMapping [(int)KeyCode.Slash] = 0xBF;
			s_KeyCodeMapping [(int)KeyCode.Alpha0] = 0x30;
			s_KeyCodeMapping [(int)KeyCode.Alpha1] = 0x31;
			s_KeyCodeMapping [(int)KeyCode.Alpha2] = 0x32;
			s_KeyCodeMapping [(int)KeyCode.Alpha3] = 0x33;
			s_KeyCodeMapping [(int)KeyCode.Alpha4] = 0x34;
			s_KeyCodeMapping [(int)KeyCode.Alpha5] = 0x35;
			s_KeyCodeMapping [(int)KeyCode.Alpha6] = 0x36;
			s_KeyCodeMapping [(int)KeyCode.Alpha7] = 0x37;
			s_KeyCodeMapping [(int)KeyCode.Alpha8] = 0x38;
			s_KeyCodeMapping [(int)KeyCode.Alpha9] = 0x39;
			s_KeyCodeMapping [(int)KeyCode.Colon] = 0xBA;
			s_KeyCodeMapping [(int)KeyCode.Semicolon] = 0xBA;
			s_KeyCodeMapping [(int)KeyCode.Less] = 0xBC;
			s_KeyCodeMapping [(int)KeyCode.Equals] = 0xBB;
			s_KeyCodeMapping [(int)KeyCode.Greater] = 0xBE;
			s_KeyCodeMapping [(int)KeyCode.Question] = 0xBF;
			s_KeyCodeMapping [(int)KeyCode.At] = 0x32;
			s_KeyCodeMapping [(int)KeyCode.LeftBracket] = 0xDB;
			s_KeyCodeMapping [(int)KeyCode.Backslash] = 0xDC;
			s_KeyCodeMapping [(int)KeyCode.RightBracket] = 0xDD;
			s_KeyCodeMapping [(int)KeyCode.Caret] = 0x36;
			s_KeyCodeMapping [(int)KeyCode.Underscore] = 0xBD;
			s_KeyCodeMapping [(int)KeyCode.BackQuote] = 0xC0;
			s_KeyCodeMapping [(int)KeyCode.A] = 65;
			s_KeyCodeMapping [(int)KeyCode.B] = 66;
			s_KeyCodeMapping [(int)KeyCode.C] = 67;
			s_KeyCodeMapping [(int)KeyCode.D] = 68;
			s_KeyCodeMapping [(int)KeyCode.E] = 69;
			s_KeyCodeMapping [(int)KeyCode.F] = 70;
			s_KeyCodeMapping [(int)KeyCode.G] = 71;
			s_KeyCodeMapping [(int)KeyCode.H] = 72;
			s_KeyCodeMapping [(int)KeyCode.I] = 73;
			s_KeyCodeMapping [(int)KeyCode.J] = 74;
			s_KeyCodeMapping [(int)KeyCode.K] = 75;
			s_KeyCodeMapping [(int)KeyCode.L] = 76;
			s_KeyCodeMapping [(int)KeyCode.M] = 77;
			s_KeyCodeMapping [(int)KeyCode.N] = 78;
			s_KeyCodeMapping [(int)KeyCode.O] = 79;
			s_KeyCodeMapping [(int)KeyCode.P] = 80;
			s_KeyCodeMapping [(int)KeyCode.Q] = 81;
			s_KeyCodeMapping [(int)KeyCode.R] = 82;
			s_KeyCodeMapping [(int)KeyCode.S] = 83;
			s_KeyCodeMapping [(int)KeyCode.T] = 84;
			s_KeyCodeMapping [(int)KeyCode.U] = 85;
			s_KeyCodeMapping [(int)KeyCode.V] = 86;
			s_KeyCodeMapping [(int)KeyCode.W] = 87;
			s_KeyCodeMapping [(int)KeyCode.X] = 88;
			s_KeyCodeMapping [(int)KeyCode.Y] = 89;
			s_KeyCodeMapping [(int)KeyCode.Z] = 90;
			s_KeyCodeMapping [(int)KeyCode.Delete] = 0x2E;
			s_KeyCodeMapping [(int)KeyCode.Keypad0] = 0x60;
			s_KeyCodeMapping [(int)KeyCode.Keypad1] = 0x61;
			s_KeyCodeMapping [(int)KeyCode.Keypad2] = 0x62;
			s_KeyCodeMapping [(int)KeyCode.Keypad3] = 0x63;
			s_KeyCodeMapping [(int)KeyCode.Keypad4] = 0x64;
			s_KeyCodeMapping [(int)KeyCode.Keypad5] = 0x65;
			s_KeyCodeMapping [(int)KeyCode.Keypad6] = 0x66;
			s_KeyCodeMapping [(int)KeyCode.Keypad7] = 0x67;
			s_KeyCodeMapping [(int)KeyCode.Keypad8] = 0x68;
			s_KeyCodeMapping [(int)KeyCode.Keypad9] = 0x69;
			s_KeyCodeMapping [(int)KeyCode.KeypadPeriod] = 0x6E;
			s_KeyCodeMapping [(int)KeyCode.KeypadDivide] = 0x6F;
			s_KeyCodeMapping [(int)KeyCode.KeypadMultiply] = 0x6A;
			s_KeyCodeMapping [(int)KeyCode.KeypadMinus] = 0x6D;
			s_KeyCodeMapping [(int)KeyCode.KeypadPlus] = 0x6B;
			s_KeyCodeMapping [(int)KeyCode.KeypadEnter] = 0x0D;
			s_KeyCodeMapping [(int)KeyCode.KeypadEquals] = 0;
			s_KeyCodeMapping [(int)KeyCode.UpArrow] = 0x26;
			s_KeyCodeMapping [(int)KeyCode.DownArrow] = 0x28;
			s_KeyCodeMapping [(int)KeyCode.RightArrow] = 0x27;
			s_KeyCodeMapping [(int)KeyCode.LeftArrow] = 0x25;
			s_KeyCodeMapping [(int)KeyCode.Insert] = 0x2D;
			s_KeyCodeMapping [(int)KeyCode.Home] = 0x24;
			s_KeyCodeMapping [(int)KeyCode.End] = 0x23;
			s_KeyCodeMapping [(int)KeyCode.PageUp] = 0x21;
			s_KeyCodeMapping [(int)KeyCode.PageDown] = 0x22;
			s_KeyCodeMapping [(int)KeyCode.F1] = 0x70;
			s_KeyCodeMapping [(int)KeyCode.F2] = 0x71;
			s_KeyCodeMapping [(int)KeyCode.F3] = 0x72;
			s_KeyCodeMapping [(int)KeyCode.F4] = 0x73;
			s_KeyCodeMapping [(int)KeyCode.F5] = 0x74;
			s_KeyCodeMapping [(int)KeyCode.F6] = 0x75;
			s_KeyCodeMapping [(int)KeyCode.F7] = 0x76;
			s_KeyCodeMapping [(int)KeyCode.F8] = 0x77;
			s_KeyCodeMapping [(int)KeyCode.F9] = 0x78;
			s_KeyCodeMapping [(int)KeyCode.F10] = 0x79;
			s_KeyCodeMapping [(int)KeyCode.F11] = 0x7A;
			s_KeyCodeMapping [(int)KeyCode.F12] = 0x7B;
			s_KeyCodeMapping [(int)KeyCode.F13] = 0x7C;
			s_KeyCodeMapping [(int)KeyCode.F14] = 0x7D;
			s_KeyCodeMapping [(int)KeyCode.F15] = 0x7E;
			s_KeyCodeMapping [(int)KeyCode.Numlock] = 0x90;
			s_KeyCodeMapping [(int)KeyCode.CapsLock] = 0x14;
			s_KeyCodeMapping [(int)KeyCode.ScrollLock] = 0x91;
			s_KeyCodeMapping [(int)KeyCode.RightShift] = 0x10;
			s_KeyCodeMapping [(int)KeyCode.LeftShift] = 0x10;
			s_KeyCodeMapping [(int)KeyCode.RightControl] = 0x11;
			s_KeyCodeMapping [(int)KeyCode.LeftControl] = 0x11;
			s_KeyCodeMapping [(int)KeyCode.RightAlt] = 0x12;
			s_KeyCodeMapping [(int)KeyCode.LeftAlt] = 0x12;
			s_KeyCodeMapping [(int)KeyCode.RightApple] = 0x5C;
			s_KeyCodeMapping [(int)KeyCode.LeftApple] = 0x5B;
			s_KeyCodeMapping [(int)KeyCode.LeftWindows] = 0x5C;
			s_KeyCodeMapping [(int)KeyCode.RightWindows] = 0x5B;
			s_KeyCodeMapping [(int)KeyCode.AltGr] = 0x12;
			s_KeyCodeMapping [(int)KeyCode.Help] = 0x2F;
			s_KeyCodeMapping [(int)KeyCode.Print] = 0x2A;
			s_KeyCodeMapping [(int)KeyCode.SysReq] = 0x2C;
			s_KeyCodeMapping [(int)KeyCode.Break] = 0x13;
			s_KeyCodeMapping [(int)KeyCode.Menu] = 0; // Which key is this?
			s_KeyCodeMapping [(int)KeyCode.Mouse0] = 0;
			s_KeyCodeMapping [(int)KeyCode.Mouse1] = 0;
			s_KeyCodeMapping [(int)KeyCode.Mouse2] = 0;
			s_KeyCodeMapping [(int)KeyCode.Mouse3] = 0;
			s_KeyCodeMapping [(int)KeyCode.Mouse4] = 0;
			s_KeyCodeMapping [(int)KeyCode.Mouse5] = 0;
			s_KeyCodeMapping [(int)KeyCode.Mouse6] = 0;
			s_KeyCodeMapping [(int)KeyCode.JoystickButton0] = 0;
			s_KeyCodeMapping [(int)KeyCode.JoystickButton1] = 0;
			s_KeyCodeMapping [(int)KeyCode.JoystickButton2] = 0;
			s_KeyCodeMapping [(int)KeyCode.JoystickButton3] = 0;
			s_KeyCodeMapping [(int)KeyCode.JoystickButton4] = 0;
			s_KeyCodeMapping [(int)KeyCode.JoystickButton5] = 0;
			s_KeyCodeMapping [(int)KeyCode.JoystickButton6] = 0;
			s_KeyCodeMapping [(int)KeyCode.JoystickButton7] = 0;
			s_KeyCodeMapping [(int)KeyCode.JoystickButton8] = 0;
			s_KeyCodeMapping [(int)KeyCode.JoystickButton9] = 0;
			s_KeyCodeMapping [(int)KeyCode.JoystickButton10] = 0;
			s_KeyCodeMapping [(int)KeyCode.JoystickButton11] = 0;
			s_KeyCodeMapping [(int)KeyCode.JoystickButton12] = 0;
			s_KeyCodeMapping [(int)KeyCode.JoystickButton13] = 0;
			s_KeyCodeMapping [(int)KeyCode.JoystickButton14] = 0;
			s_KeyCodeMapping [(int)KeyCode.JoystickButton15] = 0;
			s_KeyCodeMapping [(int)KeyCode.JoystickButton16] = 0;
			s_KeyCodeMapping [(int)KeyCode.JoystickButton17] = 0;
			s_KeyCodeMapping [(int)KeyCode.JoystickButton18] = 0;
			s_KeyCodeMapping [(int)KeyCode.JoystickButton19] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick1Button0] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick1Button1] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick1Button2] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick1Button3] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick1Button4] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick1Button5] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick1Button6] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick1Button7] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick1Button8] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick1Button9] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick1Button10] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick1Button11] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick1Button12] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick1Button13] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick1Button14] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick1Button15] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick1Button16] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick1Button17] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick1Button18] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick1Button19] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick2Button0] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick2Button1] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick2Button2] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick2Button3] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick2Button4] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick2Button5] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick2Button6] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick2Button7] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick2Button8] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick2Button9] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick2Button10] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick2Button11] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick2Button12] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick2Button13] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick2Button14] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick2Button15] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick2Button16] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick2Button17] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick2Button18] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick2Button19] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick3Button0] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick3Button1] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick3Button2] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick3Button3] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick3Button4] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick3Button5] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick3Button6] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick3Button7] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick3Button8] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick3Button9] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick3Button10] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick3Button11] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick3Button12] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick3Button13] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick3Button14] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick3Button15] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick3Button16] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick3Button17] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick3Button18] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick3Button19] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick4Button0] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick4Button1] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick4Button2] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick4Button3] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick4Button4] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick4Button5] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick4Button6] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick4Button7] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick4Button8] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick4Button9] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick4Button10] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick4Button11] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick4Button12] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick4Button13] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick4Button14] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick4Button15] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick4Button16] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick4Button17] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick4Button18] = 0;
			s_KeyCodeMapping [(int)KeyCode.Joystick4Button19] = 0;
		}
		#endregion
		
		#region Touch handling
		public struct CoherentTouch
		{
			public int fingerId;
			public Vector2 position;
			public Vector2 deltaPosition;
			public float deltaTime;
			public int tapCount;
			public TouchPhase phase;
		}

		#if UNITY_ANDROID || COHERENT_SIMULATE_MOBILE_IN_EDITOR || COHERENT_SIMULATE_MOBILE_IN_PLAYER
		private const int MAX_TOUCHES = 8;
		private static CoherentTouch[] m_Touches = new CoherentTouch[MAX_TOUCHES];
		private static CoherentTouch[] m_TouchesForNextFrame = new CoherentTouch[MAX_TOUCHES];
		private static float[] m_LastChangedTime = new float[MAX_TOUCHES];
		private static int m_TouchCount = 0;
		private static int m_TouchCountForNextFrame = 0;
		#endif
		
		public static void ProcessTouchEvent(int id, int phase, int x, int y)
		{
			#if UNITY_ANDROID || COHERENT_SIMULATE_MOBILE_IN_EDITOR || COHERENT_SIMULATE_MOBILE_IN_PLAYER
			// Check if we already have an event with that ID
			int foundIndex = -1;
			for (int i = 0; i < m_TouchCountForNextFrame; ++i)
			{
				if (m_TouchesForNextFrame[i].fingerId == id)
				{
					foundIndex = i;
					break;
				}
			}
			
			CoherentTouch newTouch = new CoherentTouch();
			newTouch.fingerId = id;
			newTouch.phase = (TouchPhase)phase;
			newTouch.position = new Vector2(x, Screen.height - y);
			newTouch.deltaTime = 0;
			newTouch.deltaPosition = new Vector2(0, 0);
			newTouch.tapCount = 1;
			
			if (foundIndex >= 0)
			{
				// Update event
				newTouch.deltaPosition = newTouch.position - m_TouchesForNextFrame[foundIndex].position;
				m_TouchesForNextFrame[foundIndex] = newTouch;
				m_LastChangedTime[foundIndex] = Time.time;
			}
			else
			{
				// Create new event if the touch has just begun; otherwise ignore the event
				if (phase == (int)TouchPhase.Began)
				{
					if (m_TouchCountForNextFrame < MAX_TOUCHES)
					{
						m_TouchesForNextFrame[m_TouchCountForNextFrame] = newTouch;
						m_LastChangedTime[m_TouchCountForNextFrame] = Time.time;
						++m_TouchCountForNextFrame;
					}
					else
					{
						Debug.LogWarning("Cannot process touch event since current touch count exceeds maximum allowed (" + MAX_TOUCHES + ")!");
					}
				}
			}
			#endif
		}
		
		public static int TouchesCount
		{
			get
			{
				#if UNITY_ANDROID || COHERENT_SIMULATE_MOBILE_IN_EDITOR || COHERENT_SIMULATE_MOBILE_IN_PLAYER
				return m_TouchCount;
				#else
				return Input.touchCount;
				#endif
			}
		}
		
		public static CoherentTouch GetTouch(int index)
		{
#if UNITY_ANDROID || COHERENT_SIMULATE_MOBILE_IN_EDITOR || COHERENT_SIMULATE_MOBILE_IN_PLAYER
			if (index >= 0 && index < m_TouchCount)
			{
				return m_Touches[index];
			}
			
			throw new System.IndexOutOfRangeException("Indexing CoherentTouch array with invalid value!");
#else
			CoherentTouch coherentTouch = new CoherentTouch();
			Touch unityTouch = Input.GetTouch(index);
			
			coherentTouch.fingerId = unityTouch.fingerId;
			coherentTouch.deltaPosition = unityTouch.deltaPosition;
			coherentTouch.deltaTime = unityTouch.deltaTime;
			coherentTouch.phase = unityTouch.phase;
			coherentTouch.position = unityTouch.position;
			coherentTouch.tapCount = unityTouch.tapCount;
			
			return coherentTouch;
#endif
		}
		
		public static void PrepareForNextFrame()
		{
#if UNITY_ANDROID || COHERENT_SIMULATE_MOBILE_IN_EDITOR || COHERENT_SIMULATE_MOBILE_IN_PLAYER
			// Copy the 'next' touches in the 'current' array
			for (int i = 0; i < m_TouchCountForNextFrame; ++i)
			{
				m_Touches[i] = m_TouchesForNextFrame[i];
			}
			
			m_TouchCount = m_TouchCountForNextFrame;
			
			// - Cleanup Ended/Canceled touches from last frame;
			// - Check if any of the touches' phase moved to Stationary
			// - Update delta time
			for (int i = 0; i < m_TouchCountForNextFrame; )
			{
				if (m_TouchesForNextFrame[i].phase == TouchPhase.Ended || m_TouchesForNextFrame[i].phase == TouchPhase.Canceled)
				{
					// Swap with last element
					m_LastChangedTime[i] = m_LastChangedTime[m_TouchCountForNextFrame - 1];
					m_TouchesForNextFrame[i] = m_TouchesForNextFrame[m_TouchCountForNextFrame - 1];
					--m_TouchCountForNextFrame;
				}
				else
				{
					if (m_LastChangedTime[i] != Time.time)
					{
						m_TouchesForNextFrame[i].deltaTime = Time.time - m_LastChangedTime[i];
					}
					
					if (m_TouchesForNextFrame[i].phase == TouchPhase.Began ||
						(m_TouchesForNextFrame[i].phase == TouchPhase.Moved && m_TouchesForNextFrame[i].deltaTime > 0))
					{
						m_TouchesForNextFrame[i].phase = TouchPhase.Stationary;
					}
					
					++i;
				}
			}
#endif
		}
		#endregion

#if COHERENT_UNITY_STANDALONE
		private static EventModifiersState GetEventModifiersState (Event evt)
		{
			EventModifiersState state = new EventModifiersState ();
			
			state.IsCtrlDown = evt.control;
			state.IsAltDown = evt.alt && !Input.GetKey(KeyCode.AltGr);
			state.IsShiftDown = evt.shift;
			state.IsCapsOn = evt.capsLock;
			state.IsNumLockOn = false; // Indeterminate
			state.IsMetaDown = evt.command;
			
			return state;
		}

		public static void GenerateMouseMoveEvent (ref MouseEventData mouseMoveEvent)
		{
			mouseMoveEvent.Type = MouseEventData.EventType.MouseMove;
			mouseMoveEvent.Button = MouseEventData.MouseButton.ButtonNone;
		
			mouseMoveEvent.MouseModifiers.IsLeftButtonDown = Input.GetMouseButton (0);
			mouseMoveEvent.MouseModifiers.IsMiddleButtonDown = Input.GetMouseButton (2);
			mouseMoveEvent.MouseModifiers.IsRightButtonDown = Input.GetMouseButton (1);
		
			mouseMoveEvent.Modifiers.IsCtrlDown = Input.GetKey (KeyCode.LeftControl) || Input.GetKey (KeyCode.RightControl);
			mouseMoveEvent.Modifiers.IsAltDown = Input.GetKey (KeyCode.LeftAlt) || Input.GetKey (KeyCode.RightAlt);
			mouseMoveEvent.Modifiers.IsShiftDown = Input.GetKey (KeyCode.LeftShift) || Input.GetKey (KeyCode.RightShift);
			mouseMoveEvent.Modifiers.IsCapsOn = false; // Indeterminate
			mouseMoveEvent.Modifiers.IsNumLockOn = false; // Indeterminate
		
			if (mouseMoveEvent.MouseModifiers.IsLeftButtonDown) {
				mouseMoveEvent.Button = MouseEventData.MouseButton.ButtonLeft;
			} else if (mouseMoveEvent.MouseModifiers.IsMiddleButtonDown) {
				mouseMoveEvent.Button = MouseEventData.MouseButton.ButtonMiddle;
			} else if (mouseMoveEvent.MouseModifiers.IsRightButtonDown) {
				mouseMoveEvent.Button = MouseEventData.MouseButton.ButtonRight;
			}
		
			mouseMoveEvent.X = (int)Input.mousePosition.x;
			mouseMoveEvent.Y = (int)Input.mousePosition.y;
			mouseMoveEvent.WheelX = 0;
			mouseMoveEvent.WheelY = 0;
		}



		public static MouseEventData ProcessMouseEvent (Event evt)
		{
			MouseEventData eventData = new MouseEventData ();

			EventMouseModifiersState mouseMods = new EventMouseModifiersState ();
			mouseMods.IsLeftButtonDown = Input.GetMouseButton (0);
			mouseMods.IsMiddleButtonDown = Input.GetMouseButton (2);
			mouseMods.IsRightButtonDown = Input.GetMouseButton (1);

			eventData.Modifiers = GetEventModifiersState (evt);
			eventData.MouseModifiers = mouseMods;
		
			if (evt.type == EventType.ScrollWheel) {
				eventData.WheelX = evt.delta.x * UNITY_WHEEL_TICK_FACTOR;
				eventData.WheelY = -evt.delta.y * UNITY_WHEEL_TICK_FACTOR;
			}
			eventData.X = (int)evt.mousePosition.x;
			eventData.Y = (int)evt.mousePosition.y;
		
			eventData.Button = MouseEventData.MouseButton.ButtonNone;
			if (evt.button == 0) {
				eventData.Button = MouseEventData.MouseButton.ButtonLeft;
			} else if (evt.button == 2) {
				eventData.Button = MouseEventData.MouseButton.ButtonMiddle;
			} else if (evt.button == 1) {
				eventData.Button = MouseEventData.MouseButton.ButtonRight;
			}
		
			// eventData.Type is left uninitialized
			return eventData;            
		}
	
		public static KeyEventData ProcessKeyEvent (Event evt)
		{
			KeyEventData eventData = new KeyEventData ();
			eventData.Modifiers = GetEventModifiersState (evt);
			eventData.KeyCode = s_KeyCodeMapping [(int)evt.keyCode];
			eventData.IsNumPad = evt.numeric;
			eventData.IsAutoRepeat = false; // Indeterminate
			return eventData;
		}
	
		public static KeyEventData ProcessCharEvent (Event evt)
		{
			KeyEventData eventData = new KeyEventData ();
			eventData.Modifiers = GetEventModifiersState (evt);
			eventData.KeyCode = evt.character;
			eventData.IsNumPad = evt.numeric;
			eventData.IsAutoRepeat = false; // Indeterminate
			eventData.Type = KeyEventData.EventType.Char;
			return eventData;
		}
#endif
	}
}
