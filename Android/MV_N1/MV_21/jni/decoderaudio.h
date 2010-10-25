#include <jni.h>

#define BUFFER_DECODE_SIZE_AUDIO 10000
#define SIZE_FRAME 320
#define HALF_FRAME 160
#define SIZE_FRAME_AMR 13
//#define SIZE_FRAME_AMR 32


class CDecoderAudio{
public:
	int Init(JNIEnv* env);
	int Decode(JNIEnv*  env, jbyteArray iData, int iSizeData);
	void Release(JNIEnv* env);
	int GetSizeBufferDecoded(){ return m_iSizeDecoded; }
	jbyteArray m_byDecodedAudio;
	jshortArray m_iDecodedAudio;
private:
	void* _ptrAmr;
	short _outbuffer[BUFFER_DECODE_SIZE_AUDIO/2];
	jbyte *m_pbyDecodedAudio;
	short *m_piDecodedAudio;
	int m_iSizeDecoded;
}; 
