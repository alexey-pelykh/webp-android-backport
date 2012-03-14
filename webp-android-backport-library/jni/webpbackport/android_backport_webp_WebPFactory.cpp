#include "android_backport_webp_WebPFactory.h"

/*
 * Class:     android_backport_webp_WebPFactory
 * Method:    nativeDecodeByteArray
 * Signature: ([BLandroid/graphics/BitmapFactory/Options;)Landroid/graphics/Bitmap;
 */
JNIEXPORT jobject JNICALL Java_android_backport_webp_WebPFactory_nativeDecodeByteArray
  (JNIEnv *jniEnv, jclass, jbyteArray byteArray, jobject options)
{
	return 0;
}

/*
 * Class:     android_backport_webp_WebPFactory
 * Method:    nativeEncodeBitmap
 * Signature: (Landroid/graphics/Bitmap;)[B
 */
JNIEXPORT jbyteArray JNICALL Java_android_backport_webp_WebPFactory_nativeEncodeBitmap
  (JNIEnv * jniEnv, jclass, jobject bitmap)
{
	return 0;
}