package com.coherentlabs.ui;

import android.webkit.ConsoleMessage;
import android.webkit.JsPromptResult;
import android.webkit.JsResult;
import android.webkit.WebChromeClient;
import android.webkit.WebView;

public class CoherentWebChromeClient extends WebChromeClient {
	
	private static String LOG_TAG = "CoherentWCC";

	@Override
	public boolean onConsoleMessage(ConsoleMessage consoleMessage) {
		// TODO: return true and let the user handle the situation
		return super.onConsoleMessage(consoleMessage);
	}

	@Override
	public boolean onJsAlert(WebView view, String url, String message,
			JsResult result) {
		// TODO: return true and let the user handle the situation
		notifyOnJSAlert(view.getId(), url, message);
		return super.onJsAlert(view, url, message, result);
	}

	@Override
	public boolean onJsConfirm(WebView view, String url, String message,
			JsResult result) {
		// TODO: return true and let the user handle the situation
		notifyOnJSConfirm(view.getId(), url, message);
		return super.onJsConfirm(view, url, message, result);
	}

	@Override
	public boolean onJsPrompt(WebView view, String url, String message,
			String defaultValue, JsPromptResult result) {
		// TODO: return true and let the user handle the situation
		notifyOnJSPrompt(view.getId(), url, message, defaultValue);
		return super.onJsPrompt(view, url, message, defaultValue, result);
	}

	public native void notifyOnJSAlert(int id, String url, String message);
	public native void notifyOnJSConfirm(int id, String url, String message);
	public native void notifyOnJSPrompt(int id, String url, String message, String defaultValue);
}