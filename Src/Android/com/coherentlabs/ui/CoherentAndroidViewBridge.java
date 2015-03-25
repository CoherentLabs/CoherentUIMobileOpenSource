package com.coherentlabs.ui;

import java.nio.ByteBuffer;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.os.SystemClock;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.ViewGroup.LayoutParams;
import android.view.ViewTreeObserver.OnGlobalLayoutListener;
import android.view.ViewParent;
import android.view.inputmethod.InputMethodManager;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.widget.ImageView;
import android.widget.RelativeLayout;


public class CoherentAndroidViewBridge {
	private static final String LOG_TAG = "CoherentJavaView";
	private static boolean mGlobalLayoutListenerAdded = false;

	private Activity mActivity;
	private WebView mWebView;
	private boolean mIsTransparent;

	public CoherentAndroidViewBridge(Activity activity) {
		if (activity == null) {
			Log.wtf(LOG_TAG, "Trying to create Coherent UI View with null activity!");
		}

		mActivity = activity;
	}

	public boolean isViewNonNull() {
		return mWebView != null;
	}

	public void dispatchKeyEvent(final int state, final int character) {
		if (mWebView == null) {
			Log.wtf(LOG_TAG, "DispatchKeyEvent called on a null Coherent View!");
		}

		mActivity.runOnUiThread(new Runnable() {

			public void run() {
				// COHERENT BEGIN: Special handling for Unity3D
				if (state == 100)
				{
					// Show the software keyboard so it can be hidden by the Unity3D API
					InputMethodManager mgr = (InputMethodManager) mActivity.getSystemService(Context.INPUT_METHOD_SERVICE);
					mgr.showSoftInput(mWebView, InputMethodManager.SHOW_FORCED);

					return;
				}
				else if (state == 101)
				{
					KeyEvent k = new KeyEvent(KeyEvent.ACTION_DOWN, KeyEvent.KEYCODE_BACK);
					mActivity.dispatchKeyEvent(k);
					return;
				}
				// COHERENT END: Special handling for Unity3D

				int action = (state == 1 ? KeyEvent.ACTION_DOWN : KeyEvent.ACTION_UP);

				KeyEvent event = null;

				if (character == 0x08) {
					// Backspace
					event = new KeyEvent(action, KeyEvent.KEYCODE_DEL);
				} else {
					event = new KeyEvent(SystemClock.uptimeMillis(), new String(new char[] { (char)character }), 0, 0);
				}

				if (event != null) {
					mWebView.dispatchKeyEvent(event);
				}
			}
		});
	}

	private void addGlobalLayoutListener() {
		if (mGlobalLayoutListenerAdded) {
			return;
		}

		final View rootView = mActivity.getWindow().getDecorView()
								.getRootView();
		rootView.getViewTreeObserver().addOnGlobalLayoutListener(
				new OnGlobalLayoutListener() {

					public void onGlobalLayout() {
						if (mActivity.isFinishing()) {
							Log.v(LOG_TAG, "Activity is finishing, onGlobalLayout handler canceled!");
							return;
						}
						android.graphics.Rect rect = new android.graphics.Rect();
						rootView.getWindowVisibleDisplayFrame(rect); // get the visible rect of the view
						android.view.Display display = mActivity.getWindowManager().getDefaultDisplay();
						@SuppressWarnings("deprecation")
						int height = display.getHeight();
						int heightDiff = rootView.getRootView().getHeight() - (rect.bottom - rect.top);

						// assume that this means that the keyboard is on
						final boolean isSoftKeyboardVisible = (heightDiff > height / 4);
						mActivity.runOnUiThread(new Runnable() {
							public void run() {
								notifySoftKeyboardVisible(isSoftKeyboardVisible);
							}
						});
					}
				});

		mGlobalLayoutListenerAdded = true;
	}

	@SuppressLint("SetJavaScriptEnabled")
	public void createView(
            final int width,
            final int height,
			final int id,
			final boolean showImmediately,
			final boolean isTransparent,
			final boolean usesDefaultFileHandler,
			final boolean isSurfaceView,
			final boolean forceSoftwareRendering,
			final boolean ignoreAndroidDeviceDensity,
			final boolean enableViewportMetaTag,
			final String initialUrl) {
		if (mWebView != null) {
			Log.wtf(LOG_TAG, "Coherent View is already created!");
		}

		mIsTransparent = isTransparent;

		mActivity.runOnUiThread(new Runnable() {

			public void run() {
				Log.v(LOG_TAG, "Creating Coherent Java View for URL " + initialUrl + " ...");

				addGlobalLayoutListener();

				mWebView = new WebView(mActivity);
				mWebView.setId(id);

				setShow(showImmediately && !isSurfaceView);

				if (forceSoftwareRendering) {
					Log.v(LOG_TAG, "Forcing software rendering for view " +
						id);
					mWebView.setLayerType(View.LAYER_TYPE_SOFTWARE, null);
				}
				
				if (ignoreAndroidDeviceDensity) {
					Log.v(LOG_TAG, "View " + id + " ignores display density.");
					mWebView.setInitialScale(100);
				}

				// WebViewClient must be set BEFORE calling loadUrl!
				mWebView.setWebViewClient(new CoherentWebViewClient(mActivity, usesDefaultFileHandler));
				mWebView.setWebChromeClient(new CoherentWebChromeClient());

				mWebView.addJavascriptInterface(new CoherentJavaScriptInterface(mWebView), CoherentJavaScriptInterface.INTERFACE_NAME);

				if (width != 0 && height != 0) {
					mActivity.addContentView(mWebView,
						new LayoutParams(width, height));
				} else {
					Log.w(LOG_TAG, "View size has zero area (" + width + " x "
							+ height + ")! " +
							"Creating a view matching parent dimensions!");
					mActivity.addContentView(mWebView,
							new LayoutParams(LayoutParams.MATCH_PARENT,
								LayoutParams.MATCH_PARENT));
				}

				WebSettings webViewSettings = mWebView.getSettings();
				// Enable JS
				webViewSettings.setJavaScriptEnabled(true);

				// Enable local storage
				webViewSettings.setDatabaseEnabled(true);
				java.io.File databaseEntry = mActivity.getDir("databases",
						Context.MODE_PRIVATE);
				if (databaseEntry != null) {
					String databasePath = databaseEntry.getPath();
					webViewSettings.setDatabasePath(databasePath);
					webViewSettings.setDomStorageEnabled(true);	
				} else {
					Log.w(LOG_TAG, "Unable to obtain local storage directory!"
							+ "Local storage will be disabled.");
				}

				if (enableViewportMetaTag) {
					webViewSettings.setUseWideViewPort(true);
					webViewSettings.setLoadWithOverviewMode(true);
				}

				mWebView.requestFocus();
				mWebView.loadUrl(initialUrl);

				// If the view has transparent elements, the background should be transparent
				if (mIsTransparent) {
					mWebView.setBackgroundColor(Color.TRANSPARENT);
				}
			}
		});
	}

	public void getAsBitmap(final ByteBuffer buffer,
			final int width,
			final int height) {
		mActivity.runOnUiThread(new Runnable() {
			public void run() {
				Bitmap bitmap = Bitmap.createBitmap(width, height,
					Bitmap.Config.ARGB_8888);
				final Canvas canvas = new Canvas(bitmap);

				mWebView.draw(canvas);
				bitmap.copyPixelsToBuffer(buffer);
				// we clear the buffer so it is ready to be used next time
				// the native bytes will remain unchanged
				buffer.clear();

				notifyOnBitmapReady(mWebView.getId(), buffer,
					4 * width * height);
			}
		});
	}

	public void destroyView() {
		Log.v(LOG_TAG, "DestroyView!");
		if (mWebView == null) {
			Log.wtf(LOG_TAG, "DestroyView called on a null Coherent View!");
			return;
		}
		
		if (mActivity.isFinishing()) {
			Log.v(LOG_TAG, "Activity is finishing, destroy view canceled!");
			return;
		}

		mActivity.runOnUiThread(new Runnable() {
			public void run() {
				ViewParent parent = mWebView.getParent();
				if (!(parent instanceof ViewGroup)) {
					Log.wtf(LOG_TAG, "Unknown type of Coherent View parent element! Unable to remove from layout!");
				}
				((ViewGroup)parent).removeView(mWebView);
				mWebView.loadUrl("about:blank"); // Reset the internal view state
				mWebView.destroy();
				mWebView = null;
			}
		});
	}

	public void setShow(final boolean show) {
		if (mWebView == null) {
			Log.wtf(LOG_TAG, "SetShow called on a null Coherent View!");
		}

		mActivity.runOnUiThread(new Runnable() {
			public void run() {
				int visibility = (show ? View.VISIBLE : View.INVISIBLE);
				mWebView.setVisibility(visibility);
			}
		});
	}

	public void load(final String path) {
		if (mWebView == null) {
			Log.wtf(LOG_TAG, "Load called on a null Coherent View!");
		}

		mActivity.runOnUiThread(new Runnable() {
			public void run() {
				mWebView.loadUrl(path);

				// If the view has transparent elements, the background should be transparent
				if (mIsTransparent) {
					mWebView.setBackgroundColor(Color.TRANSPARENT);
				}
			}
		});
	}

	public void reload() {
		if (mWebView == null) {
			Log.wtf(LOG_TAG, "Reload called on a null Coherent View!");
		}

		mActivity.runOnUiThread(new Runnable() {
			public void run() {
				mWebView.reload();
			}
		});
	}

	public void executeScript(final String script) {
		if (mWebView == null) {
			Log.wtf(LOG_TAG, "ExecuteScript called on a null Coherent View!");
		}

		mActivity.runOnUiThread(new Runnable() {
			public void run() {
				mWebView.loadUrl("javascript:" + script);
			}
		});
	}

	public void goBack() {
		if (mWebView == null) {
			Log.wtf(LOG_TAG, "GoBack called on a null Coherent View!");
		}

		mActivity.runOnUiThread(new Runnable() {
			public void run() {
				mWebView.goBack();
			}
		});
	}

	public void goForward() {
		if (mWebView == null) {
			Log.wtf(LOG_TAG, "GoForward called on a null Coherent View!");
		}

		mActivity.runOnUiThread(new Runnable() {
			public void run() {
				mWebView.goForward();
			}
		});
	}

	public void moveView(final int left, final int top) {
		if (mWebView == null) {
			Log.wtf(LOG_TAG, "MoveView called on a null Coherent View!");
		}

		mActivity.runOnUiThread(new Runnable() {
			public void run() {
				mWebView.setTranslationX(left);
				mWebView.setTranslationY(top);
			}
		});
	}

	public void resizeView(final int width, final int height) {
		if (mWebView == null) {
			Log.wtf(LOG_TAG, "ResizeView called on a null Coherent View!");
		}

		mActivity.runOnUiThread(new Runnable() {
			public void run() {
				LayoutParams params = mWebView.getLayoutParams();
				params.width = width;
				params.height = height;
				mWebView.setLayoutParams(params);
			}
		});
	}

	private native void notifySoftKeyboardVisible(boolean visible);
	private native void notifyOnBitmapReady(int viewId, ByteBuffer buffer,
			int size);
}

