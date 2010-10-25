#include <jni.h>
#include "call_encoder.h"
#include "call_decoder.h"
#include "call_mylog.h"
#include "Tmndec.h"
#include "mylog.h"


// h263 variable
jintArray iRGB;
jint *piRGB;
jbyteArray byH263;
jbyte *pbyH263;
#define FRAME_SIZE_RGB 101376 //176*144*4

// amr variabile
void *destate;
jbyteArray byPCM;
jbyte *pbyPCM;
#define FRAME_SIZE_PCM 320
	
extern "C" {
	#include "interf_dec.h"
	
	
	jint
	Java_com_seventouch_mobivu_MobiVu_InitEncoder( JNIEnv*  env, jobject  thiz)
	{
		// alloc for rgb
		byH263 = env->NewByteArray(BUFFER_ENCODE_SIZE);
		pbyH263 = env->GetByteArrayElements(byH263, 0);
		return InitEncoder();
	}
	void
	Java_com_seventouch_mobivu_MobiVu_ReleaseEncoder( JNIEnv*  env, jobject  thiz)
	{
		env->ReleaseByteArrayElements(byH263, pbyH263, 0);
		ReleaseEncoder();
	}
	int
	Java_com_seventouch_mobivu_MobiVu_InitDecoder( JNIEnv*  env, jobject  thiz)
	{
		// alloc for rgb
		iRGB = env->NewIntArray(FRAME_SIZE_RGB);
		piRGB = env->GetIntArrayElements(iRGB, 0);
		return InitDecoder();
	}
	void
	Java_com_seventouch_mobivu_MobiVu_ReleaseDecoder( JNIEnv*  env, jobject  thiz)
	{
		env->ReleaseIntArrayElements(iRGB, piRGB, 0);
		ReleaseDecoder();
	}
	void
	Java_com_seventouch_mobivu_MobiVu_InitLog( JNIEnv*  env, jobject  thiz)
	{
		InitLog();
	}
	void
	Java_com_seventouch_mobivu_MobiVu_ReleaseLog( JNIEnv*  env, jobject  thiz)
	{
		ReleaseLog();
	}
	JNIEXPORT jintArray
	Java_com_seventouch_mobivu_MobiVu_Decode( JNIEnv*  env, jobject  thiz, jbyteArray byData, jint iDataSize)
	{
		jbyte *pbydatain  = env->GetByteArrayElements(byData, 0);
		int iEsito = DecompressFrame((unsigned char*)pbydatain, iDataSize, (unsigned int*)piRGB, FRAME_SIZE_RGB);
		env->ReleaseByteArrayElements(byData, pbydatain, 0);
		return iRGB;
	}
	JNIEXPORT jbyteArray
	Java_com_seventouch_mobivu_MobiVu_Encode( JNIEnv*  env, jobject  thiz, jbyteArray byData, jint iDataSize)
	{
		jbyte *pbydatain  = env->GetByteArrayElements(byData, 0);
		Encode((unsigned char*)pbydatain);
		env->ReleaseByteArrayElements(byData, pbydatain, 0);
		return byH263;
	}
	int
	Java_com_seventouch_mobivu_MobiVu_GetSizeEncoded( JNIEnv*  env, jobject  thiz)
	{
		return iSizeBufferH263;
	}

//--------------------------------------- AMR-NB -------------------------------------------	
	
	void
	Java_com_seventouch_mobivu_MobiVu_InitDecoderAmrNb(JNIEnv*  env, jobject  thiz){
		// alloc for rgb
		byPCM  = env->NewByteArray(FRAME_SIZE_PCM);
		pbyPCM = env->GetByteArrayElements(byPCM, 0);
		//destate = Decoder_Interface_init();
	}
	void
	Java_com_seventouch_mobivu_MobiVu_ReleaseDecoderAmrNb(JNIEnv*  env, jobject  thiz){
		env->ReleaseByteArrayElements(byPCM, pbyPCM, 0);
		//Decoder_Interface_exit(destate);
	}
	JNIEXPORT jbyteArray
	Java_com_seventouch_mobivu_MobiVu_DecodeAmrNb( JNIEnv*  env, jobject  thiz, jbyteArray byData)
	{
		jbyte *pbydatain  = env->GetByteArrayElements(byData, 0);
//		Decoder_Interface_Decode(destate, (unsigned char*)pbydatain, (short*)pbyPCM, 0);
		env->ReleaseByteArrayElements(byData, pbydatain, 0);
		return byPCM;
	}
}
