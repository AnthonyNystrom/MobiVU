#include <android/log.h>
#include "encoderaudio.h"
#include "include/gsmamr_enc.h"
#include "h263/mylog.h"
#define BUFFER_ENCODE_SIZE_AUDIO 1024

int CEncoderAudio::Init(JNIEnv* env){
	m_byEncodedAudio = env->NewByteArray(BUFFER_ENCODE_SIZE_AUDIO);
	m_pbyEncodedAudio = env->GetByteArrayElements(m_byEncodedAudio, 0);	
	AMREncodeInit(&_encCtx, &_pidSyncCtx, 0);
	__android_log_write(ANDROID_LOG_DEBUG, "CEncoderAudio::Init", "Initialized");
	m_iSizeEncoded=0;
	//myLog.EnableLog();
	return 1;
}

int CEncoderAudio::Encode(JNIEnv* env, jbyteArray iData, int iSizeData){
	jbyte *pbydatain  = env->GetByteArrayElements(iData, 0);
	/*
	FILE *fp;
	fp=fopen("/sdcard/audio.pcm","ab");
	if (fp!=NULL){
		fwrite(pbydatain,1, iSizeData, fp);
		fclose(fp);
	}*/

	enum Mode mode = MR475;
	enum Frame_Type_3GPP frame_type = (enum Frame_Type_3GPP) mode;
	if (m_iSizeEncoded+16>=BUFFER_ENCODE_SIZE_AUDIO)
		m_iSizeEncoded=0;
	
	int iNFrm = iSizeData/320;
	int ret, ii;
	short dat=0;
	//__android_log_print(ANDROID_LOG_DEBUG, "CEncoderAudio::Encode", "iNFrm:%d", iNFrm);
	int iMax=0;
	int iSoglia = 2000;
	for (int i=0; i<iNFrm; i++, pbydatain+=320)
	{	
		//check if encode
		for (ii=0;ii<160;ii+=2){
			dat = (short) (pbydatain[ii+1] & 0x00FF);
			dat<<=8;
			dat |= (short)(pbydatain[ii] & 0x00FF);	
			if (dat>iMax)
				iMax = dat;
		}
		//__android_log_print(ANDROID_LOG_DEBUG, "CEncoderAudio::Encode", "max:%d", iMax);
		if (iMax>iSoglia){				
			ret = AMREncode(_encCtx, _pidSyncCtx, mode, (Word16*) pbydatain, (unsigned char*)&m_pbyEncodedAudio[3+m_iSizeEncoded], &frame_type, AMR_TX_IETF);
			if (ret>0){
				m_pbyEncodedAudio[3+m_iSizeEncoded] |= 0x04;
				m_iSizeEncoded += ret;
			}
			else
				__android_log_print(ANDROID_LOG_DEBUG, "CEncoderAudio::Encode", "failed:%d", ret);
		}
	}
/*	
	enum Mode mode = MR475;
	enum Frame_Type_3GPP frame_type = (enum Frame_Type_3GPP) mode;
	if (m_iSizeEncoded+16>=BUFFER_ENCODE_SIZE_AUDIO)
		m_iSizeEncoded=0;
	int ret = AMREncode(_encCtx, _pidSyncCtx, mode, (Word16*) pbydatain, (unsigned char*)&m_pbyEncodedAudio[3+m_iSizeEncoded], &frame_type, AMR_TX_IETF);
	
	if (ret>0){
		m_pbyEncodedAudio[3+m_iSizeEncoded] |= 0x04;
		m_iSizeEncoded += ret;
	}
	else
		__android_log_print(ANDROID_LOG_DEBUG, "CEncoderAudio::Encode", "failed:%d", ret);
*/

	env->ReleaseByteArrayElements(iData, pbydatain, 0);
	return 1;
}

int CEncoderAudio::GetSizeBufferEncoded()
{	int iRet = m_iSizeEncoded;
	m_iSizeEncoded=0;
	/*FILE *fp;
	fp=fopen("/sdcard/audio.amr","ab");
	if (fp!=NULL){
		fwrite(&m_pbyEncodedAudio[3],1, iRet, fp);
		fclose(fp);
	}*/
	return iRet; 
}

void CEncoderAudio::Release(JNIEnv* env){
	//myLog.DisableLog();
	env->ReleaseByteArrayElements(m_byEncodedAudio, m_pbyEncodedAudio, 0);
	AMREncodeExit(&_encCtx, &_pidSyncCtx);
	__android_log_write(ANDROID_LOG_DEBUG, "CEncoderAudio", "Released");
}


