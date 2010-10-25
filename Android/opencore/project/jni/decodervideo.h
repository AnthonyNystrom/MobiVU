#ifndef _DECODER
#define _DECODER
#include <jni.h>
#include "include/pvm4vdecoder.h"

#define BUFFER_DECODE_SIZE 38016
class CDecoderVideo{
public:
	CDecoderVideo();
	~CDecoderVideo();
	void Release(JNIEnv*  env);
	int Init(JNIEnv*  env, int iWidth, int iHeight);
	int GetSizeBufferDecoded() {return 1;}
	int Decode(JNIEnv*  env, jbyteArray iData, int iSize);
	jbyteArray m_byDecoded;
private:	
	jbyte *m_pbyDecoded;
	PVM4VDecoder *_pvDec;
};

#endif
