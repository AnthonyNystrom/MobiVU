#ifndef _DECODER
#define _DECODER
#include <jni.h>
#include "include/pvm4vdecoder.h"
#include "include/cczoomrotation16.h"
#include "include/cczoomrotation32.h"
#include "include/cczoomrotation24.h"

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
	jbyteArray m_byRGB;
	jintArray m_iRGB;
private:	
	jbyte *m_pbyDecoded;
	jbyte *m_pbyRGB;
	jint  *m_piRGB;
	PVM4VDecoder *_pvDec;
	ColorConvert32 *_pCC;
int _icnt	;
};

#endif
