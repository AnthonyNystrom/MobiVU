#include <jni.h>
#include "call_decoder.h"

extern "C" {
	int
	Java_com_android_codec_codec_InitDecoder( JNIEnv*  env, jobject  thiz)
	{
		return InitDecoder();
	}
	void
	Java_com_android_codec_codec_ReleaseDecoder( JNIEnv*  env, jobject  thiz)
	{
		ReleaseDecoder();
	}
}
