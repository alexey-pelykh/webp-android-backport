#include "android_backport_webp_WebPFactory.h"
#include "android_backport_webp.h"

#include <stdio.h>
#include <android/bitmap.h>
#include <android/log.h>
#include <webp/decode.h>
#include <webp/decode_vp8.h>
#include <webp/encode.h>

#ifdef __cplusplus
extern "C" {
#endif

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

#define SK_A32_BITS     8
#define SK_R32_BITS     8
#define SK_G32_BITS     8
#define SK_B32_BITS     8

#define SK_R32_SHIFT    16
#define SK_G32_SHIFT    8
#define SK_B32_SHIFT    0
#define SK_A32_SHIFT    24

#define SK_A32_MASK     ((1 << SK_A32_BITS) - 1)
#define SK_R32_MASK     ((1 << SK_R32_BITS) - 1)
#define SK_G32_MASK     ((1 << SK_G32_BITS) - 1)
#define SK_B32_MASK     ((1 << SK_B32_BITS) - 1)

#define SkGetPackedA32(packed)      ((uint32_t)((packed) << (24 - SK_A32_SHIFT)) >> 24)
#define SkGetPackedR32(packed)      ((uint32_t)((packed) << (24 - SK_R32_SHIFT)) >> 24)
#define SkGetPackedG32(packed)      ((uint32_t)((packed) << (24 - SK_G32_SHIFT)) >> 24)
#define SkGetPackedB32(packed)      ((uint32_t)((packed) << (24 - SK_B32_SHIFT)) >> 24)

#define SK_R16_BITS     5
#define SK_G16_BITS     6
#define SK_B16_BITS     5

#define SK_R16_SHIFT    (SK_B16_BITS + SK_G16_BITS)
#define SK_G16_SHIFT    (SK_B16_BITS)
#define SK_B16_SHIFT    0

#define SK_R16_MASK     ((1 << SK_R16_BITS) - 1)
#define SK_G16_MASK     ((1 << SK_G16_BITS) - 1)
#define SK_B16_MASK     ((1 << SK_B16_BITS) - 1)

#define SkGetPackedR16(color)   (((unsigned)(color) >> SK_R16_SHIFT) & SK_R16_MASK)
#define SkGetPackedG16(color)   (((unsigned)(color) >> SK_G16_SHIFT) & SK_G16_MASK)
#define SkGetPackedB16(color)   (((unsigned)(color) >> SK_B16_SHIFT) & SK_B16_MASK)

static inline unsigned SkR16ToR32(unsigned r) {
	return (r << (8 - SK_R16_BITS)) | (r >> (2 * SK_R16_BITS - 8));
}

static inline unsigned SkG16ToG32(unsigned g) {
	return (g << (8 - SK_G16_BITS)) | (g >> (2 * SK_G16_BITS - 8));
}

static inline unsigned SkB16ToB32(unsigned b) {
	return (b << (8 - SK_B16_BITS)) | (b >> (2 * SK_B16_BITS - 8));
}

#define SkPacked16ToR32(c)      SkR16ToR32(SkGetPackedR16(c))
#define SkPacked16ToG32(c)      SkG16ToG32(SkGetPackedG16(c))
#define SkPacked16ToB32(c)      SkB16ToB32(SkGetPackedB16(c))

typedef void (*ScanlineImporter)(const uint8_t* in, uint8_t* out, int width);

static void ARGB_8888_To_RGBA(const uint8_t* in, uint8_t* rgb, int width)
{
	const uint32_t* src = (const uint32_t*)in;
	for (int i = 0; i < width; ++i)
	{
		const uint32_t c = *src++;
		rgb[0] = SkGetPackedR32(c);
		rgb[1] = SkGetPackedG32(c);
		rgb[2] = SkGetPackedB32(c);
		rgb[3] = SkGetPackedA32(c);
		rgb += 4;
	}
}

static void RGB_565_To_RGB(const uint8_t* in, uint8_t* rgb, int width)
{
	const uint16_t* src = (const uint16_t*)in;
	for (int i = 0; i < width; ++i)
	{
		const uint16_t c = *src++;
		rgb[0] = SkPacked16ToR32(c);
		rgb[1] = SkPacked16ToG32(c);
		rgb[2] = SkPacked16ToB32(c);
		rgb += 3;
	}
}

static ScanlineImporter ChooseImporter(int config)
{
	switch (config) {
	case ANDROID_BITMAP_FORMAT_RGBA_8888:
		return ARGB_8888_To_RGBA;
	case ANDROID_BITMAP_FORMAT_RGB_565:
		return RGB_565_To_RGB;
	default:
		return NULL;
	}
}

static uint32_t GetDestinationScanlineStride(int config) 
{
	switch (config) {
	case ANDROID_BITMAP_FORMAT_RGBA_8888:
		return 4;
	case ANDROID_BITMAP_FORMAT_RGB_565:
		return 3;
	default:
		return 0;
	}
}

/*
 * Class:     android_backport_webp_WebPFactory
 * Method:    nativeEncodeBitmap
 * Signature: (Landroid/graphics/Bitmap;I)[B
 */
JNIEXPORT jbyteArray JNICALL Java_android_backport_webp_WebPFactory_nativeEncodeBitmap
  (JNIEnv * jniEnv, jclass, jobject bitmap, jint quality)
{
	// Check if input is valid
	if(!bitmap)
	{
		jniEnv->ThrowNew(jrefs::java::lang::NullPointerException->jclassRef, "Bitmap can not be null");
		return 0;
	}

	// Get information about bitmap passed
	AndroidBitmapInfo bitmapInfo;
	if(AndroidBitmap_getInfo(jniEnv, bitmap, &bitmapInfo) != ANDROID_BITMAP_RESUT_SUCCESS)
	{
		jniEnv->ThrowNew(jrefs::java::lang::RuntimeException->jclassRef, "Failed to get Bitmap information");
		return 0;
	}

	// Check for format
	if(bitmapInfo.format != ANDROID_BITMAP_FORMAT_RGBA_8888 && bitmapInfo.format != ANDROID_BITMAP_FORMAT_RGB_565)
	{
		jniEnv->ThrowNew(jrefs::java::lang::RuntimeException->jclassRef, "Unsupported Bitmap configuration. Currently only RGBA_8888 and RGB_565 are supported");
		return 0;
	}
	
	// Log what version of WebP is used
	__android_log_print(ANDROID_LOG_INFO, LOG_TAG, "Using WebP Encoder %08x", WebPGetEncoderVersion());

	// Lock pixels
	void* bitmapPixels = 0;
	if(AndroidBitmap_lockPixels(jniEnv, bitmap, &bitmapPixels) != ANDROID_BITMAP_RESUT_SUCCESS)
	{
		jniEnv->ThrowNew(jrefs::java::lang::RuntimeException->jclassRef, "Failed to lock Bitmap pixels");
		return 0;
	}

	// Convert color space
	const uint8_t* src = (uint8_t*)bitmapPixels;
	const uint32_t src_stride = bitmapInfo.stride;
	const uint32_t dst_stride = GetDestinationScanlineStride(bitmapInfo.format) * bitmapInfo.width;
	const ScanlineImporter scanline_import = ChooseImporter(bitmapInfo.format);

	uint8_t* dst = new uint8_t[dst_stride * bitmapInfo.height];
	for (int y = 0; y < bitmapInfo.height; ++y)
		scanline_import(src + y * src_stride, dst + y * dst_stride, bitmapInfo.width);

	// Unlock pixels
	if(AndroidBitmap_unlockPixels(jniEnv, bitmap) != ANDROID_BITMAP_RESUT_SUCCESS)
	{
		jniEnv->ThrowNew(jrefs::java::lang::RuntimeException->jclassRef, "Failed to unlock Bitmap pixels");
		return 0;
	}
	
	// Encode and save
	size_t encodedImageSize = 0;
	uint8_t* encodedImageData = 0;
	if(bitmapInfo.format == ANDROID_BITMAP_FORMAT_RGBA_8888)
		encodedImageSize = WebPEncodeRGB(dst, bitmapInfo.width, bitmapInfo.height, dst_stride, quality, &encodedImageData);
	else if(bitmapInfo.format == ANDROID_BITMAP_FORMAT_RGB_565)
		encodedImageSize = WebPEncodeRGBA(dst, bitmapInfo.width, bitmapInfo.height, dst_stride, quality, &encodedImageData);
	delete[] dst;
	if(encodedImageSize == 0)
	{
		jniEnv->ThrowNew(jrefs::java::lang::RuntimeException->jclassRef, "Failed to encode to WebP");
		return 0;
	}

	// Copy to output buffer
	jbyteArray resultArray = jniEnv->NewByteArray(encodedImageSize);
	jbyte* resultArrayBuffer = jniEnv->GetByteArrayElements(resultArray, NULL);
	memcpy(resultArrayBuffer, encodedImageData, encodedImageSize);
	jniEnv->ReleaseByteArrayElements(resultArray, resultArrayBuffer, 0);

	__android_log_print(ANDROID_LOG_INFO, LOG_TAG, "WebP image size %d bytes", encodedImageSize);

	// Free
	free(encodedImageData);

	return resultArray;
}

#ifdef __cplusplus
}
#endif