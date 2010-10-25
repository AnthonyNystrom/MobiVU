#include <android/log.h>
#include "decoderaudio.h"
#include "include/gsmamr_dec.h"
#define BUFFER_DECODE_SIZE_AUDIO 1024

int CDecoderAudio::Init(JNIEnv* env){
	m_byDecodedAudio = env->NewByteArray(BUFFER_DECODE_SIZE_AUDIO);
	m_pbyDecodedAudio = env->GetByteArrayElements(m_byDecodedAudio, 0);
	_ptrAmr = NULL;
	GSMInitDecode(&_ptrAmr, (int8*)"Decoder");
	m_iSizeDecoded = 320;
	__android_log_write(ANDROID_LOG_DEBUG, "CDecoderAudio::Init", "Initialized");		
	return 1;
}

int CDecoderAudio::Decode(JNIEnv* env, jbyteArray iData, int iSizeData){
	jbyte *pbydatain  = env->GetByteArrayElements(iData, 0);

	unsigned char type = (pbydatain[0] >> 3) & 0x0f;
	pbydatain++;
	
	AMRDecode(_ptrAmr, (enum Frame_Type_3GPP) type, (UWord8*) pbydatain, _outbuffer, MIME_IETF);

	// Convert to little endian
	uint8_t* ptr = (uint8_t*)m_pbyDecodedAudio;
	for (int i = 0; i < 160; i++) {
		*ptr++ = (_outbuffer[i] >> 0) & 0xff;
		*ptr++ = (_outbuffer[i] >> 8) & 0xff;
	}

	env->ReleaseByteArrayElements(iData, pbydatain, 0);
	return 1;
}

void CDecoderAudio::Release(JNIEnv* env){
	GSMDecodeFrameExit(&_ptrAmr);
	env->ReleaseByteArrayElements(m_byDecodedAudio, m_pbyDecodedAudio, 0);
	__android_log_write(ANDROID_LOG_DEBUG, "CDecoderAudio", "Released");
}


