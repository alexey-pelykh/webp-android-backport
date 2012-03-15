package android.backport.webp;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;

/**
 * Factory to encode and decode WebP images into Android Bitmap
 * @author Alexey Pelykh
 */
public final class WebPFactory {
	
	// Load library at class loading
	static {
		System.loadLibrary("webpbackport");
	};

	/**
	 * Decodes byte array to bitmap 
	 * @param data Byte array with WebP bitmap data
	 * @param opts Options to control decoding. Accepts null
	 * @return Decoded bitmap
	 */
	public static native Bitmap nativeDecodeByteArray(byte[] data, BitmapFactory.Options options);
	
	/**
	 * Encodes bitmap into byte array
	 * @param bitmap Bitmap
	 * @param quality Quality, should be between 0 and 100
	 * @return Encoded byte array
	 */
	public static native byte[] nativeEncodeBitmap(Bitmap bitmap, int quality);
	
}
