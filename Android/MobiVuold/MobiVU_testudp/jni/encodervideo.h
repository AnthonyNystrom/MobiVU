#ifndef _ENCODER
#define _ENCODER
#include <jni.h>
#include <include/pvm4vencoder.h>
#include "include/cvei.h"
#include "include/ccyuv420semitoyuv420.h"

#define BUFFER_ENCODE_SIZE_VIDEO 100000

class CEncoderVideo : MPVCVEIObserver {
public:	
	CEncoderVideo();
	~CEncoderVideo();
	int Init(JNIEnv*  env, int iWidth, int iHeight);
	int GetSizeBufferEncoded(){ return m_iSizeEncoded; }
	int EncodeVideo(JNIEnv*  env, jbyteArray iData, int iTimeStamp);
	void Release(JNIEnv*  env);
	jbyteArray m_byEncodedVideo;
	jbyteArray m_byYUV420;
private:	
	void HandlePVCVEIEvent(uint32 aId, uint32 aEvent, uint32 aParam1 = 0){}
	jbyte *m_pbyEncodedVideo;
	jbyte *m_pbyYUV420;
	int m_iSizeEncoded;
	CPVM4VEncoder *m_pvEncVid;
	TPVVideoInputFormat m_inForm;
	TPVVideoEncodeParam m_enParam;
	TPVVideoInputData m_inData;
	TPVVideoOutputData m_outData;
	CCYUV420SEMItoYUV420 *_pCC;
	FILE* _fplog;
};

#endif
