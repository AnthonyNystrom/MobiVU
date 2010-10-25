#include <jni.h>

class CEncoderAudio{
public:
	int Init(JNIEnv* env);
	int Encode(JNIEnv*  env, jbyteArray iData, int iSizeData);
	void Release(JNIEnv* env);
	int GetSizeBufferEncoded();
	jbyteArray m_byEncodedAudio;
private:
	void* _encCtx;
	void* _pidSyncCtx;
	jbyte *m_pbyEncodedAudio;
	int m_iSizeEncoded;
}; 
