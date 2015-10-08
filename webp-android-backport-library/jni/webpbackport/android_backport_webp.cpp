#include "android_backport_webp.h"

#include <jni.h>

#include <assert.h>
#include <android/log.h>

const char* const LOG_TAG = "android.backport.webp:native";

namespace jrefs {

namespace java {
namespace lang {

jclass_NullPointerException::jclass_NullPointerException(JNIEnv* jniEnv)
{
	jclassRef = jniEnv->FindClass("java/lang/NullPointerException");
	assert(jclassRef == 0);
}
jclass_NullPointerException* NullPointerException = 0;

jclass_IllegalArgumentException::jclass_IllegalArgumentException(JNIEnv* jniEnv)
{
	jclassRef = jniEnv->FindClass("java/lang/IllegalArgumentException");
	assert(jclassRef == 0);
}
jclass_IllegalArgumentException* IllegalArgumentException = 0;

jclass_RuntimeException::jclass_RuntimeException(JNIEnv* jniEnv)
{
	jclassRef = jniEnv->FindClass("java/lang/RuntimeException");
	assert(jclassRef == 0);
}
jclass_RuntimeException* RuntimeException = 0;

} // namespace lang
} // namespace java

namespace android {
namespace graphics {

jclass_Bitmap* Bitmap = 0;
jclass_Bitmap::jclass_Bitmap(JNIEnv* jniEnv)
	: Config(jniEnv)
{
	jclassRef = jniEnv->FindClass("android/graphics/Bitmap");
	assert(jclassRef == 0);

	createBitmap = jniEnv->GetStaticMethodID(jclassRef,
		"createBitmap",
		"(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");
	assert(createBitmap);
}
jclass_Bitmap::jclass_Config::jclass_Config(JNIEnv* jniEnv)
{
	jclassRef = jniEnv->FindClass("android/graphics/Bitmap$Config");
	assert(jclassRef == 0);

	ARGB_8888 = jniEnv->GetStaticFieldID(jclassRef,
		"ARGB_8888",
		"Landroid/graphics/Bitmap$Config;");
	assert(ARGB_8888);
}

jclass_BitmapFactory* BitmapFactory = 0;
jclass_BitmapFactory::jclass_BitmapFactory(JNIEnv* jniEnv)
	: Options(jniEnv)
{
}
jclass_BitmapFactory::jclass_Options::jclass_Options(JNIEnv* jniEnv)
{
	jclassRef = jniEnv->FindClass("android/graphics/BitmapFactory$Options");
	assert(jclassRef == 0);
	
	inJustDecodeBounds = jniEnv->GetFieldID(jclassRef,
		"inJustDecodeBounds",
		"Z");
	assert(inJustDecodeBounds);

	outHeight = jniEnv->GetFieldID(jclassRef,
		"outHeight",
		"I");
	assert(outHeight);

	outWidth = jniEnv->GetFieldID(jclassRef,
		"outWidth",
		"I");
	assert(outWidth);
}

} // namespace graphics
} // namespace android
} // namespace jrefs

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jint JNICALL JNI_OnLoad
  (JavaVM *vm, void *reserved)
{
	JNIEnv *jniEnv = 0;
	if(vm->GetEnv((void **)&jniEnv, JNI_VERSION_1_6))
		return JNI_ERR; /* JNI version not supported */
	
	// Load Java classes
	jrefs::java::lang::IllegalArgumentException = new jrefs::java::lang::jclass_IllegalArgumentException(jniEnv);
	jrefs::java::lang::NullPointerException = new jrefs::java::lang::jclass_NullPointerException(jniEnv);
	jrefs::java::lang::RuntimeException = new jrefs::java::lang::jclass_RuntimeException(jniEnv);
	jrefs::android::graphics::Bitmap = new jrefs::android::graphics::jclass_Bitmap(jniEnv);
	jrefs::android::graphics::BitmapFactory = new jrefs::android::graphics::jclass_BitmapFactory(jniEnv);

	__android_log_print(ANDROID_LOG_INFO, LOG_TAG, "JNI_OnLoad completed");

	return JNI_VERSION_1_6;
}

#ifdef __cplusplus
}
#endif