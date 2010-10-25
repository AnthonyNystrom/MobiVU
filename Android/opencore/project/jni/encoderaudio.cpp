#include <android/log.h>
#include "encoderaudio.h"
#include "include/gsmamr_enc.h"
#define BUFFER_ENCODE_SIZE_AUDIO 1024

int CEncoderAudio::Init(JNIEnv* env){
	m_byEncodedAudio = env->NewByteArray(BUFFER_ENCODE_SIZE_AUDIO);
	m_pbyEncodedAudio = env->GetByteArrayElements(m_byEncodedAudio, 0);
	AMREncodeInit(&_encCtx, &_pidSyncCtx, 0);
	__android_log_write(ANDROID_LOG_DEBUG, "CEncoderAudio::Init", "Initialized");		
	return 1;
}

int CEncoderAudio::Encode(JNIEnv* env, jbyteArray iData, int iSizeData){
	jbyte *pbydatain  = env->GetByteArrayElements(iData, 0);

	enum Mode mode = MR475;
	enum Frame_Type_3GPP frame_type = (enum Frame_Type_3GPP) mode;
	m_iSizeEncoded = 0;
	int ret = AMREncode(_encCtx, _pidSyncCtx, mode, (Word16*) pbydatain, (unsigned char*)m_pbyEncodedAudio, &frame_type, AMR_TX_IETF);
	
	if (ret>0){
		m_pbyEncodedAudio[0] |= 0x04;
		m_iSizeEncoded = ret;
	}
	else
		__android_log_print(ANDROID_LOG_DEBUG, "CEncoderAudio::Encode", "failed:%d", ret);


	env->ReleaseByteArrayElements(iData, pbydatain, 0);
	return 1;
}

void CEncoderAudio::Release(JNIEnv* env){
	env->ReleaseByteArrayElements(m_byEncodedAudio, m_pbyEncodedAudio, 0);
	AMREncodeExit(&_encCtx, &_pidSyncCtx);
}


