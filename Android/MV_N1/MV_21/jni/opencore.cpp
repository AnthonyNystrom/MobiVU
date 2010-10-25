#include <jni.h>
#include <android/log.h>
#include "encodervideo.h"
#include "decodervideo.h"
#include "encoderaudio.h"
#include "decoderaudio.h"
#include "include/oscl_mem.h"
#include "include/oscl_mem_audit.h"
#include "include/oscl_error.h"
#include "include/oscl_scheduler.h"
#include "include/oscl_init.h"
CEncoderVideo encVideo;
CDecoderVideo decVideo;
CDecoderAudio decAudio;
CEncoderAudio encAudio;

extern "C" {
	jint Java_com_mobivu_Codecs_InitOpencore(JNIEnv*  env, jobject  thiz){
		//init all Oscl layers except Oscl scheduler.
        OsclSelect select;
		int error;
        select.iOsclScheduler = false;
        OsclInit::Init(error, &select);
        if (error)
        {
            __android_log_print(ANDROID_LOG_DEBUG, "InitOpencore", "Initialized failed error %d", error);
            return 0;
        }
		OsclBase::Init();
		OsclErrorTrap::Init();
		OsclMem::Init();
//		OsclScheduler::Init("7touch_core");
		__android_log_write(ANDROID_LOG_DEBUG, "InitOpencore", "Initialized");
  		return 1;
	}
	void Java_com_mobivu_Codecs_ReleaseOpencore(JNIEnv*  env, jobject  thiz){
		//Cleanup		
		OsclMem::Cleanup();
		OsclErrorTrap::Cleanup();
		OsclBase::Cleanup();
		//OsclScheduler::Cleanup();
		OsclSelect select;
        select.iOsclScheduler = false;
		int error;
        OsclInit::Cleanup(error, &select);
		__android_log_write(ANDROID_LOG_DEBUG, "ReleaseOpencore", "Released");
	}
	
// VIDEO ENCODER ******************************
	jint Java_com_mobivu_Codecs_InitEncoderVideo(JNIEnv*  env, jobject  thiz, jint iWidth, jint iHeight, jint iQ)
	{		return encVideo.Init(env, (int)iWidth, (int)iHeight, (int)iQ);
	}
	JNIEXPORT jbyteArray Java_com_mobivu_Codecs_EncodeVideo(JNIEnv*  env, jobject thiz, jbyteArray iData, jint iTimeStamp)
	{
		encVideo.EncodeVideo(env, iData, iTimeStamp);
		return encVideo.m_byEncodedVideo;
	}
	void Java_com_mobivu_Codecs_ReleaseEncoderVideo(JNIEnv*  env, jobject  thiz)
	{
		encVideo.Release(env);
	}	jint Java_com_mobivu_Codecs_GetSizeBufferEncodedVideo(){
		return encVideo.GetSizeBufferEncoded();
	}

// VIDEO DECODER ******************************
	jint Java_com_mobivu_Codecs_InitDecoderVideo(JNIEnv*  env, jobject  thiz, jint iWidthSrc, jint iHeightSrc, jint iWidthDst, jint iHeightDst)
	{		
		return decVideo.Init(env, (int)iWidthSrc, (int)iHeightSrc, (int)iWidthDst, (int)iHeightDst);
	}
	JNIEXPORT jintArray Java_com_mobivu_Codecs_DecodeVideo(JNIEnv*  env, jobject thiz, jbyteArray iData, jint iSize)
	{
		decVideo.Decode(env, iData, iSize);
		return decVideo.m_iRGB;
	}
	void Java_com_mobivu_Codecs_ReleaseDecoderVideo(JNIEnv*  env, jobject  thiz)
	{
		decVideo.Release(env);
	}
	jint Java_com_mobivu_codecs_GetSizeBufferDecodedVideo(){
		return decVideo.GetSizeBufferDecoded();
	}

// AUDIO ENCODER ******************************
	jint Java_com_mobivu_Codecs_InitEncoderAudio(JNIEnv*  env, jobject  thiz)
	{
		return encAudio.Init(env);
	}
	JNIEXPORT jbyteArray Java_com_mobivu_Codecs_EncodeAudio(JNIEnv*  env, jobject thiz, jbyteArray iData, int iSizeData)
	{
		encAudio.Encode(env, iData, iSizeData);
		return encAudio.m_byEncodedAudio;
	}
	void Java_com_mobivu_Codecs_ReleaseEncoderAudio(JNIEnv*  env, jobject  thiz)
	{
		encAudio.Release(env);
	}
	jint Java_com_mobivu_Codecs_GetSizeBufferEncodedAudio(){
		return encAudio.GetSizeBufferEncoded();
	}
// AUDIO DECODER ******************************
	jint Java_com_mobivu_Codecs_InitDecoderAudio(JNIEnv*  env, jobject  thiz)
	{
		return decAudio.Init(env);
	}
	void Java_com_mobivu_Codecs_ReleaseDecoderAudio(JNIEnv*  env, jobject  thiz)
	{
		decAudio.Release(env);
	}
	JNIEXPORT jbyteArray Java_com_mobivu_Codecs_DecodeAudio(JNIEnv*  env, jobject thiz, jbyteArray iData, int iSizeData)
	{
		decAudio.Decode(env, iData, iSizeData);
		//return decAudio.m_iDecodedAudio;
		return decAudio.m_byDecodedAudio;
	}
	jint Java_com_mobivu_Codecs_GetSizeBufferDecodedAudio(){
		return decAudio.GetSizeBufferDecoded();
	}
}




