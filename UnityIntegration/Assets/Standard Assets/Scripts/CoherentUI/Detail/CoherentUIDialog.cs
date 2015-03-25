#if UNITY_STANDALONE || UNITY_STANDALONE_WIN || UNITY_STANDALONE_OSX
#define COHERENT_UNITY_STANDALONE
#endif

#if UNITY_NACL || UNITY_WEBPLAYER
#define COHERENT_UNITY_UNSUPPORTED_PLATFORM
#endif

using UnityEngine;
using System.Collections;
#if COHERENT_UNITY_STANDALONE || COHERENT_UNITY_UNSUPPORTED_PLATFORM
using Coherent.UI;
#elif UNITY_IPHONE || UNITY_ANDROID
using Coherent.UI.Mobile;
#endif

class CoherentUIDialog : MonoBehaviour {
	public enum DialogType
	{
		Alert,
		Confirm,
		Prompt,
		Authentication
	}
	
	// Statics
	const int SPACING = 5;
	const int CONTROL_HEIGHT = 20;
	const int WINDOW_WIDTH = 420;
	const int LETTERS_PER_LINE = 56;

	static int s_InternalWindowId = 0;
	
	// Common member variables
	private int m_InternalWindowId = 0;
	private Rect m_WindowRect = new Rect(0, 0, 0, 0);
	internal DialogType m_Type = DialogType.Alert;
	internal UnityViewListener m_ViewListener { get; set; }

	// Alert specific
	internal string AlertMessage { get; set; }
	
	// Confirm specific
	internal string ConfirmMessage { get; set; }
	
	// Prompt specific
	internal string PromptMessage { get; set; }
	internal string PromptReply { get; set; }
	
	// Authentication specific
	internal string AuthenticationMessage { get; set; }
	internal string AuthenticationUsername { get; set; }
	internal string AuthenticationPassword { get; set; }
		
	void Start () {
		m_InternalWindowId = ++s_InternalWindowId;
		
		if (PromptReply == null)
		{
			PromptReply = string.Empty;
		}
		AuthenticationUsername = string.Empty;
		AuthenticationPassword = string.Empty;
		
		switch (m_Type)
		{
		case DialogType.Alert:
			{
				// height = window title + spacing + alert text + spacing + button + spacing
				int height = CONTROL_HEIGHT + SPACING +
						GetStringPixelHeight(AlertMessage) + SPACING +
						CONTROL_HEIGHT + SPACING;
				
				m_WindowRect = new Rect((Screen.width - WINDOW_WIDTH) / 2, (Screen.height - height) / 2, WINDOW_WIDTH, height);
			}
			break;
		case DialogType.Authentication:
			{
				// height = window title + spacing + auth + spacing + text field(user) + spacing + text field(pass) + spacing + buttons + spacing
				int height = CONTROL_HEIGHT + SPACING +
						GetStringPixelHeight(AuthenticationMessage) + SPACING +
						CONTROL_HEIGHT + SPACING +
						CONTROL_HEIGHT + SPACING +
						CONTROL_HEIGHT + SPACING;

				m_WindowRect = new Rect((Screen.width - WINDOW_WIDTH) / 2, (Screen.height - height) / 2, WINDOW_WIDTH, height);
			}
			break;
		case DialogType.Confirm:
			{
				// height = window title + spacing + alert text + spacing + buttons + spacing
				int height = CONTROL_HEIGHT + SPACING +
						GetStringPixelHeight(ConfirmMessage) + SPACING +
						CONTROL_HEIGHT + SPACING;
				
				m_WindowRect = new Rect((Screen.width - WINDOW_WIDTH) / 2, (Screen.height - height) / 2, WINDOW_WIDTH, height);
			}
			break;
		case DialogType.Prompt:
			{
				// height = window title + spacing + prompt + spacing + text field + spacing + buttons + spacing
				int height = CONTROL_HEIGHT + SPACING +
						GetStringPixelHeight(PromptMessage) + SPACING +
						CONTROL_HEIGHT + SPACING +
						CONTROL_HEIGHT + SPACING;
				
				m_WindowRect = new Rect((Screen.width - WINDOW_WIDTH) / 2, (Screen.height - height) / 2, WINDOW_WIDTH, height);
			}
			break;
		}
	}

	int GetStringPixelHeight(string str)
	{
		if (string.IsNullOrEmpty(str))
		{
			return 0;
		}

		const int TEXT_HEIGHT = 16;
		const int TEXT_ADJUSTMENT = 6;
		return (((str.Length - 1) / LETTERS_PER_LINE) + 1) * TEXT_HEIGHT + TEXT_ADJUSTMENT;
	}

	void OnGUI()
	{
		switch (m_Type)
		{
		case DialogType.Alert:
			m_WindowRect = GUI.Window(m_InternalWindowId, m_WindowRect, AlertWindowFunction, "Alert");
			break;
		case DialogType.Authentication:
			m_WindowRect = GUI.Window(m_InternalWindowId, m_WindowRect, AuthenticationWindowFunction, "Authentication required");
			break;
		case DialogType.Confirm:
			m_WindowRect = GUI.Window(m_InternalWindowId, m_WindowRect, ConfirmWindowFunction, "Confirm");
			break;
		case DialogType.Prompt:
			m_WindowRect = GUI.Window(m_InternalWindowId, m_WindowRect, PromptWindowFunction, "Prompt");
			break;
		}
	}
	
	void OnDestroy()
	{
		m_ViewListener.HasModalDialogOpen = false;
	}

	void AlertWindowFunction(int id)
	{
		int yOffset = 20;
		int xOffset = 10;
		int width = WINDOW_WIDTH - 2 * xOffset;
		int height = GetStringPixelHeight(AlertMessage);

		GUI.Label(new Rect(xOffset, yOffset, width, height), AlertMessage);

		yOffset += height + SPACING;
		height = CONTROL_HEIGHT;
		width = 80;
		xOffset = WINDOW_WIDTH - (width + 10);
		if (GUI.Button(new Rect(xOffset, yOffset, width, height), "OK"))
		{
			Destroy(this.gameObject);
			ReplyToJavaScriptMessage(true, string.Empty);
		}

		GUI.DragWindow(new Rect(0, 0, 10000, 20));
	}
	
	void ConfirmWindowFunction(int id)
	{
		int yOffset = 20;
		int xOffset = 10;
		int width = WINDOW_WIDTH - 2 * xOffset;
		int height = GetStringPixelHeight(ConfirmMessage);

		GUI.Label(new Rect(xOffset, yOffset, width, height), ConfirmMessage);

		yOffset += height + SPACING;
		height = CONTROL_HEIGHT;
		width = 80;
		xOffset = WINDOW_WIDTH - 2 * (width + 10);
		if (GUI.Button(new Rect(xOffset, yOffset, width, height), "OK"))
		{
			Destroy(this.gameObject);
			ReplyToJavaScriptMessage(true, string.Empty);
		}

		xOffset += width + 10;
		if (GUI.Button(new Rect(xOffset, yOffset, width, height), "Cancel"))
		{
			Destroy(this.gameObject);
			ReplyToJavaScriptMessage(false, string.Empty);
		}		

		GUI.DragWindow(new Rect(0, 0, 10000, 20));
	}

	void PromptWindowFunction(int id)
	{
		int yOffset = 20;
		int xOffset = 10;
		int width = WINDOW_WIDTH - 2 * xOffset;
		int height = GetStringPixelHeight(PromptMessage);

		GUI.Label(new Rect(xOffset, yOffset, width, height), PromptMessage);

		yOffset += height + SPACING;
		height = CONTROL_HEIGHT;
		PromptReply = GUI.TextField(new Rect(xOffset, yOffset, width, height), PromptReply);

		yOffset += height + SPACING;
		width = 80;
		xOffset = WINDOW_WIDTH - 2 * (width + 10);
		if (GUI.Button(new Rect(xOffset, yOffset, width, height), "OK"))
		{
			Destroy(this.gameObject);
			ReplyToJavaScriptMessage(true, PromptReply);
		}

		xOffset += width + 10;
		if (GUI.Button(new Rect(xOffset, yOffset, width, height), "Cancel"))
		{
			Destroy(this.gameObject);
			ReplyToJavaScriptMessage(false, PromptReply);
		}

		GUI.DragWindow(new Rect(0, 0, 10000, 20));
	}

	void AuthenticationWindowFunction(int id)
	{
		int yOffset = 20;
		int xOffset = 10;
		int width = WINDOW_WIDTH - 2 * xOffset;
		int height = GetStringPixelHeight(AuthenticationMessage);

		GUI.Label(new Rect(xOffset, yOffset, width, height), AuthenticationMessage);

		yOffset += height + SPACING;
		height = CONTROL_HEIGHT;
		width = 60;
		GUI.Label(new Rect(xOffset, yOffset, width, height), "Username: ");
		xOffset += width + SPACING;
		width = WINDOW_WIDTH - 10 - xOffset;
		AuthenticationUsername = GUI.TextField(new Rect(xOffset, yOffset, width, height), AuthenticationUsername);

		xOffset = 10;
		yOffset += height + SPACING;
		height = CONTROL_HEIGHT;
		width = 60;
		GUI.Label(new Rect(xOffset, yOffset, width, height), "Password: ");
		xOffset += width + SPACING;
		width = WINDOW_WIDTH - 10 - xOffset;
		AuthenticationPassword = GUI.PasswordField(new Rect(xOffset, yOffset, width, height), AuthenticationPassword, (char)0x25CF);

		yOffset += height + SPACING;
		width = 80;
		xOffset = WINDOW_WIDTH - 2 * (width + 10);
		if (GUI.Button(new Rect(xOffset, yOffset, width, height), "OK"))
		{
			Destroy(this.gameObject);
			AuthCredentialsResponse(AuthenticationUsername, AuthenticationPassword, true);
		}

		xOffset += width + 10;
		if (GUI.Button(new Rect(xOffset, yOffset, width, height), "Cancel"))
		{
			Destroy(this.gameObject);
			AuthCredentialsResponse(string.Empty, string.Empty, false);
		}

		GUI.DragWindow(new Rect(0, 0, 10000, 20));
	}
	
	private void ReplyToJavaScriptMessage(bool flag, string rep)
	{
		#if COHERENT_UNITY_STANDALONE
		m_ViewListener.View.ReplyToJavaScriptMessage(flag, rep);
		#endif
	}
	
	private void AuthCredentialsResponse(string user, string pass, bool ok)
	{
		#if COHERENT_UNITY_STANDALONE
		m_ViewListener.View.AuthCredentialsResponse(user, pass, ok);
		#endif
	}
}
