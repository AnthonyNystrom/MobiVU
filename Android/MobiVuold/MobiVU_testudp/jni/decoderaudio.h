#include <jni.h>

class CDecoderAudio{
public:
	int Init(JNIEnv* env);
	int Decode(JNIEnv*  env, jbyteArray iData, int iSizeData);
	void Release(JNIEnv* env);
	int GetSizeBufferDecoded(){ return m_iSizeDecoded; }
	jbyteArray m_byDecodedAudio;
private:
	void* _ptrAmr;
	short _outbuffer[160];
	jbyte *m_pbyDecodedAudio;
	int m_iSizeDecoded;
}; 
