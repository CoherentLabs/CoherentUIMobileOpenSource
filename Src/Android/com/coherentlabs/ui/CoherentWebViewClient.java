package com.coherentlabs.ui;


import java.io.IOException;
import java.io.InputStream;
import java.net.URLConnection;
import java.net.URL;

import android.app.Activity;
import android.util.Log;
import android.webkit.WebResourceResponse;
import android.webkit.WebView;
import android.webkit.WebViewClient;

public class CoherentWebViewClient extends WebViewClient {
	
	private Activity mActivity;
	private boolean mUseDefaultFileHandler;
	private static String LOG_TAG = "CoherentWVC";
	private static String CUSTOM_PROTOCOL = "coui://";
	
	public CoherentWebViewClient(Activity activity, boolean useDefaultFileHandler) {
		mActivity = activity;
		mUseDefaultFileHandler = useDefaultFileHandler;
	}
	
	@Override
	public boolean shouldOverrideUrlLoading(WebView view, String url) {
		if (url != null && !shouldStartLoad(view.getId(), url)) {
			Log.v(LOG_TAG, "Overriding URL loading of \"" + url + "\" for view " + view.getId());
			return true;
		} else {
			return false;
		}
	}

	@Override
	public void onLoadResource(WebView view, String url) {
		if (url != null && url.startsWith("coherent-js")) {
			triggerNativeCode(view.getId(), url);
		}
	}
	
	private String trimLeadingSlashes(String input) {
		if (input == null) {
			Log.v(LOG_TAG, "Trimming slashes from null string!");
			return null;
		}
		
		int index = 0;
		while (index < input.length() && input.charAt(index) == '/') {
			++index;
		}
		if (index != 0) {
			return input.substring(index);
		} else {
			return input;
		}
	}
	
	// Intercept requests to check for custom protocol
	@Override
	public WebResourceResponse shouldInterceptRequest(WebView view,
			String url) {
		if (url != null && url.toLowerCase().startsWith(CUSTOM_PROTOCOL)) {
			Log.v(LOG_TAG, "Getting response for \"" + url + "\" using custom protocol on view " + view.getId() + "...");
			String resourceUrl = trimLeadingSlashes(
				url.substring(CUSTOM_PROTOCOL.length()));
			WebResourceResponse response = getCouiResource(resourceUrl);
			Log.v(LOG_TAG, "Getting response for \"" + url + "\" " + (response != null ? "succeeded" : "failed") + ".");
			if (response != null) {
				return response;
			}
		}
		
		return super.shouldInterceptRequest(view, url);
	}
	
	// Return WebResourceResponse from an asset (e.g. "assets/style.css"). 
	private WebResourceResponse getCouiResource(String url) {
		try {
			URL javaUrl;
			try {
				javaUrl = new URL("http://" + url); // Add a dummy known protocol to be able to construct the URL object
				url = javaUrl.getHost() + javaUrl.getPath(); // Exclude query parameters
			} catch (java.net.MalformedURLException mex) {
				Log.v(LOG_TAG, "Unable to construct URL object for \"" + url + "\", using it as-is.");
			}
			url = java.net.URLDecoder.decode(url, "UTF-8");

			return getUtf8EncodedWebResourceResponse(url);
		} catch (IOException e) {
			Log.e(LOG_TAG, e.getMessage());
			return null;
		}
	}
	
	private WebResourceResponse getUtf8EncodedWebResourceResponse(String url) {
		String mime = null;
		InputStream responseInputStream = null;
		
		if (mUseDefaultFileHandler) {
			try {
				responseInputStream = mActivity.getAssets().open(url);
				mime = URLConnection.guessContentTypeFromName(url);
				if (mime == null) {
					mime = URLConnection
							.guessContentTypeFromStream(responseInputStream);
				}
			} catch (IOException e) {
				Log.e(LOG_TAG, e.getMessage());
				return null;
			}
		} else {
			responseInputStream = new CoherentInputStream(CUSTOM_PROTOCOL, url);
		}
		
		if (mime == null) {
			mime = URLConnection.guessContentTypeFromName("android_asset/"
					+ url);
			if (mime == null) {
				mime = "text/plain";
			}
		}
		Log.v(LOG_TAG, "Assumed content type for \"" + url + "\": " + mime);

		return new WebResourceResponse(mime, "UTF-8", responseInputStream);
	}
	
	@Override
	public void onReceivedError(WebView view, int errorCode,
			String description, String failingUrl) {
		if (failingUrl.startsWith(CUSTOM_PROTOCOL)
			&& errorCode == ERROR_UNSUPPORTED_SCHEME) {
			// This branch is used when a coui:// page URL fails to load.
			// Note that this does not occur for resources loaded by the page,
			// e.g. js or css files.
			notifyOnPageError(view.getId(), failingUrl, ERROR_FILE_NOT_FOUND,
					"The requested local page cannot be read.");
		} else {
			notifyOnPageError(view.getId(), failingUrl, errorCode, description);
		}
	}

	@Override
	public void onPageStarted(WebView view, String url,
			android.graphics.Bitmap favicon) {
		notifyOnPageStarted(view.getId(), url);
	}

	@Override
	public void onPageFinished(WebView view, String url)
	{
		if (view.getProgress() == 100) {
			notifyOnPageFinished(view.getId(), url);
		}
	}
	
	public native void notifyOnPageStarted(int id, String url);
	public native void notifyOnPageError(int id, String url, int errorCode, String description);
	public native void notifyOnPageFinished(int id, String url);
	public native boolean shouldStartLoad(int id, String url);
	public native void triggerNativeCode(int id, String url);
}