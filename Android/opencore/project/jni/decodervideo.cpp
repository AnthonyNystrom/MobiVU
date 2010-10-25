#include <android/log.h>
#include "decodervideo.h"
#include "include/mp4dec_api.h"

CDecoderVideo::CDecoderVideo(){
	_pvDec = NULL;
}
CDecoderVideo::~CDecoderVideo(){
}

void CDecoderVideo::Release(JNIEnv*  env)
{
	env->ReleaseByteArrayElements(m_byDecoded, m_pbyDecoded, 0);
	if (_pvDec){
		_pvDec->CleanUpVideoDecoder();
		delete _pvDec;
		_pvDec=NULL;
	}
	__android_log_write(ANDROID_LOG_DEBUG, "CDecoderVideo::Release", "");
}
// return 0 if not decoded
int CDecoderVideo::Decode(JNIEnv*  env, jbyteArray iData, int iSize){
	jbyte *pbydatain  = env->GetByteArrayElements(iData, 0);
	__android_log_print(ANDROID_LOG_DEBUG, "CDecoderVideo::Decode", "Decoder get encoded pointer:%d", iSize);
	
	if (_pvDec){
		uint32 TimeStamp = 0xFFFFFFFF;
		uint UseExtTimestamp = 0;
		__android_log_write(ANDROID_LOG_DEBUG, "CDecoderVideo::Decode", "start decoder");
		bool bRes = _pvDec->DecodeVideoFrame((uint8**)&pbydatain, &TimeStamp, (int32*)&iSize, &UseExtTimestamp, (uint8 *)m_pbyDecoded);
		__android_log_write(ANDROID_LOG_DEBUG, "CDecoderVideo::Decode", "end decoder");
		if (!bRes)
			__android_log_write(ANDROID_LOG_DEBUG, "CDecoderVideo::Decode", "Failed");			
		else{		
			__android_log_write(ANDROID_LOG_DEBUG, "CDecoderVideo::Decode", "OK");
			
		}
	}
	else
		__android_log_write(ANDROID_LOG_DEBUG, "CDecoderVideo::Decode", "Decoder doesn't init");
	
	env->ReleaseByteArrayElements(iData, pbydatain, 0);
	
	return 0;
}

int CDecoderVideo::Init(JNIEnv*  env, int iWidth, int iHeight){
	int iRet=0;
	m_byDecoded = env->NewByteArray(BUFFER_DECODE_SIZE);
	m_pbyDecoded = env->GetByteArrayElements(m_byDecoded, 0);
	__android_log_write(ANDROID_LOG_DEBUG, "CDecoderVideo::Init", "Buffer initialized");
				
	_pvDec = PVM4VDecoder::New();
	if (_pvDec!=NULL){
		uint8 * buff = NULL;
		int32 iSize = 0;
		int iMode = H263_MODE;
		bool bRes = _pvDec->InitVideoDecoder(&buff, &iSize, 1, (int32*)&iWidth, (int32*)&iHeight, &iMode);
		if (!bRes){
			__android_log_write(ANDROID_LOG_DEBUG, "CDecoderVideo::Init", "Failed");
			return false;
		}
		_pvDec->SetPostProcType(0);
		_pvDec->SetReferenceYUV((uint8*)m_pbyDecoded);	
		
		__android_log_print(ANDROID_LOG_DEBUG, "CDecoderVideo::Init", "OK %dx%d", iWidth, iHeight);	
		iRet=1;
	}
	else
		__android_log_write(ANDROID_LOG_DEBUG, "CDecoderVideo::Init", "Can't alloc encoder");
	return iRet;
}
