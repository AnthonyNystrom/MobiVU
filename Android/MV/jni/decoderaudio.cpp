#include <android/log.h>
#include "decoderaudio.h"
#include "include/gsmamr_dec.h"

int CDecoderAudio::Init(JNIEnv* env){
	m_iDecodedAudio  = env->NewShortArray(BUFFER_DECODE_SIZE_AUDIO);
	m_piDecodedAudio = env->GetShortArrayElements(m_iDecodedAudio, 0);
	m_byDecodedAudio = env->NewByteArray(BUFFER_DECODE_SIZE_AUDIO);
	m_pbyDecodedAudio = env->GetByteArrayElements(m_byDecodedAudio, 0);
	_ptrAmr = NULL;
	GSMInitDecode(&_ptrAmr, (int8*)"Decoder");
	m_iSizeDecoded = SIZE_FRAME;
	__android_log_write(ANDROID_LOG_DEBUG, "CDecoderAudio::Init", "Initialized");		
	return 1;
}

int CDecoderAudio::Decode(JNIEnv* env, jbyteArray iData, int iSizeData){
	jbyte *pbydatain  = env->GetByteArrayElements(iData, 0);
	/*
	unsigned char type;
	int iNFrm = iSizeData/SIZE_FRAME_AMR;
	UWord8* pin = (UWord8*)pbydatain;
	for (int i=0;i<iNFrm;i++,pin+=SIZE_FRAME_AMR){
		type = (*(pin) >> 3) & 0x0f;		
		AMRDecode(_ptrAmr, (enum Frame_Type_3GPP) type, pin+1, &_outbuffer[HALF_FRAME*i], MIME_IETF);
	}
	
	// Convert to little endian
	uint8_t* ptr = (uint8_t*)m_pbyDecodedAudio;	
	for (int i = 0; i < HALF_FRAME*iNFrm; i++) {
		*ptr++ = (_outbuffer[i] >> 0) & 0xff;
		*ptr++ = (_outbuffer[i] >> 8) & 0xff;
	}*/
	/*FILE *fpyuv;
	fpyuv=fopen("/sdcard/test.pcm","ab");
	if (fpyuv!=NULL){
		fwrite(m_pbyDecodedAudio,1,iNFrm*SIZE_FRAME, fpyuv);
		fclose(fpyuv);
	}*/	
	
	unsigned char type;
	int iNFrm = (iSizeData-3)/SIZE_FRAME_AMR;
	UWord8* pin = (UWord8*)pbydatain+3;
	for (int i=0;i<iNFrm;i++,pin+=SIZE_FRAME_AMR){
		type = (*(pin) >> 3) & 0x0f;		
		AMRDecode(_ptrAmr, (enum Frame_Type_3GPP) type, pin+1, &_outbuffer[HALF_FRAME*i], MIME_IETF);
		//AMRDecode(_ptrAmr, (enum Frame_Type_3GPP) type, pin+1, &m_piDecodedAudio[HALF_FRAME*i], MIME_IETF);
	}
	
	// Convert to little endian
	uint8_t* ptr = (uint8_t*)m_pbyDecodedAudio;	
	for (int i = 0; i < HALF_FRAME*iNFrm; i++) {
		*ptr++ = (_outbuffer[i] >> 0) & 0xff;
		*ptr++ = (_outbuffer[i] >> 8) & 0xff;
	}

	env->ReleaseByteArrayElements(iData, pbydatain, 0);
	return 1;
}


void CDecoderAudio::Release(JNIEnv* env){
	GSMDecodeFrameExit(&_ptrAmr);
	env->ReleaseByteArrayElements(m_byDecodedAudio, m_pbyDecodedAudio, 0);
	env->ReleaseShortArrayElements(m_iDecodedAudio, m_piDecodedAudio, 0);
	
	__android_log_write(ANDROID_LOG_DEBUG, "CDecoderAudio", "Released");
}


