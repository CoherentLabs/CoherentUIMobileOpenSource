package com.coherentlabs.ui;

import java.io.IOException;
import java.io.InputStream;

public class CoherentInputStream extends InputStream {
	
	private int mPosition;
	private byte[] mData;
	private String mUrl;
	private String mProtocol;
	
	public CoherentInputStream(String protocol, String url) {
		mPosition = 0;
		mProtocol = protocol;
		mUrl = url;
	}

	@Override
	public int read() throws IOException {
		if (mData == null) {
			// Blocks until the user has read the resource
			mData = readAssetInMemory(mProtocol + mUrl);

			if (mData == null) {
				return -1; // Asset not available
			}
		}

		if (mPosition == mData.length) {
			return -1;
		} else {
			int result = (int)mData[mPosition] & 0xFF;
			++mPosition;
			return result;
		}
	}

	@Override
	public int read(byte[] buffer) throws IOException {
		if (mData == null) {
			mData = readAssetInMemory(mProtocol + mUrl); // Blocks until the user has read the resource

			if (mData == null) {
				return -1; // Asset not available
			}
		}
		
		if (buffer.length == 0) {
			return 0;
		}
		
		if (mPosition == mData.length) {
			return -1;
		}
		
		int bytesToCopy = buffer.length;
		if (bytesToCopy > mData.length - mPosition) {
			bytesToCopy = mData.length - mPosition;
		}
		System.arraycopy(mData, mPosition, buffer, 0, bytesToCopy);
		mPosition += bytesToCopy;
		return bytesToCopy;
	}

	private native byte[] readAssetInMemory(String path);
}
