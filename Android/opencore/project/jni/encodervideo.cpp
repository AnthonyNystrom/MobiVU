#include <android/log.h>
#include "encodervideo.h"
#include "include/mp4enc_api.h"

#define DEFAULT_BITRATE 32000
#define DEFAULT_FRAME_RATE 15
#define DEFAULT_INTRA_PERIOD 10
#define DEFAULT_OMX_MP4ENC_QPI 15
#define DEFAULT_OMX_MP4ENC_QPP 12
#define DEFAULT_OMX_MP4ENC_QPB 12


CEncoderVideo::CEncoderVideo(){
	m_pvEncVid=NULL;
}
CEncoderVideo::~CEncoderVideo(){

}

void CEncoderVideo::Release(JNIEnv*  env)
{	
	if (m_pvEncVid!=NULL)
	{	delete m_pvEncVid;
		m_pvEncVid=NULL;
	}
	env->ReleaseByteArrayElements(m_byEncodedVideo, m_pbyEncodedVideo, 0);
}

int CEncoderVideo::Init(JNIEnv*  env, int iWidth, int iHeight){
	m_iSizeEncoded = 0;

	int iRet=0;
	m_byEncodedVideo = env->NewByteArray(BUFFER_ENCODE_SIZE_VIDEO);
	m_pbyEncodedVideo = env->GetByteArrayElements(m_byEncodedVideo, 0);
	m_pvEncVid = CPVM4VEncoder::New(-1);
	
	if (m_pvEncVid!=NULL){
	
		iRet=1;
		m_inForm.iFrameWidth  = iWidth;
		m_inForm.iFrameHeight = iHeight;
		m_inForm.iFrameRate   = 15;
		m_inForm.iFrameOrientation =-1;
		m_inForm.iVideoFormat = ECVEI_YUV420;
		
		oscl_memset(&m_enParam, 0, sizeof(m_enParam));

		m_enParam.iEncodeID = 0;
		m_enParam.iNumLayer = 1;
		m_enParam.iFrameWidth[0] = iWidth;
		m_enParam.iFrameHeight[0] = iHeight;
		m_enParam.iBitRate[0] = DEFAULT_BITRATE;		
		m_enParam.iFrameRate[0] = (float)DEFAULT_FRAME_RATE;
		m_enParam.iFrameQuality = 10;
		m_enParam.iIFrameInterval = DEFAULT_INTRA_PERIOD;
		//m_enParam.iNumIntraMBRefresh =
		//m_enParam.iBufferDelay	
		m_enParam.iContentType = ECVEI_H263;//ECVEI_STREAMING;//ECVEI_H263;
		m_enParam.iRateControlType = EVBR_1;
		m_enParam.iRDOptimal = false;
		m_enParam.iIquant[0] = 31; // default to unset
		m_enParam.iPquant[0] = 31;
		m_enParam.iBquant[0] = 31;
		m_enParam.iSearchRange = 16;
		m_enParam.iMV8x8 = false;
		m_enParam.iMVHalfPel = true;
		m_enParam.iSceneDetection = false;
		m_enParam.iPacketSize = 256;
		m_enParam.iNoCurrentSkip = false;
		m_enParam.iNoFrameSkip = false;
		m_enParam.iClipDuration = 0;
		m_enParam.iProfileLevel = ECVEI_CORE_LEVEL2;
		
		m_inData.iTimeStamp=0;
		if (m_pvEncVid->Initialize(&m_inForm, &m_enParam)==ECVEI_SUCCESS){
			iRet=1;			__android_log_write(ANDROID_LOG_DEBUG, "CEncoderVideo::Init", "Video Encoder Initialized");		
		}
		else
			__android_log_write(ANDROID_LOG_DEBUG, "CEncoderVideo::Init", "Video Encoder Failed");		
	}
	else{
		__android_log_write(ANDROID_LOG_DEBUG, "CEncoderVideo::Init", "Video Encoder Can't alloc encoder");
	}

	return iRet;
}


int CEncoderVideo::EncodeVideo(JNIEnv*  env, jbyteArray iData)
{
	jbyte *pbydatain  = env->GetByteArrayElements(iData, 0);
	m_iSizeEncoded = 0;
	if (m_pvEncVid){
		m_inData.iSource = (uint8*)pbydatain;
		m_inData.iTimeStamp+=66;
		__android_log_print(ANDROID_LOG_DEBUG, "CEncoderVideo::Encode", "timestamp:%d", m_inData.iTimeStamp);
		int iRemain;		
		m_outData.iBitStream = (uint8*)m_pbyEncodedVideo;
		m_outData.iBitStreamSize = BUFFER_ENCODE_SIZE_VIDEO;
		TCVEI_RETVAL ret = m_pvEncVid->EncodeFrame(&m_inData, &m_outData, &iRemain);
		switch(ret){
			case ECVEI_SUCCESS:
				__android_log_print(ANDROID_LOG_DEBUG, "CEncoderVideo::Encode", "ECVEI_SUCCESS:%d", m_outData.iBitStreamSize);
				m_iSizeEncoded = m_outData.iBitStreamSize;
				break;
			case ECVEI_FAIL:
				__android_log_write(ANDROID_LOG_DEBUG, "CEncoderVideo::Encode", "ECVEI_FAIL");
				break;
			case ECVEI_MORE_OUTPUT:
				__android_log_print(ANDROID_LOG_DEBUG, "CEncoderVideo::Encode", "ECVEI_MORE_OUTPUT:%d", iRemain);
				m_iSizeEncoded = m_outData.iBitStreamSize;
				break;
		}
		
	}
	env->ReleaseByteArrayElements(iData, pbydatain, 0);

	return m_iSizeEncoded;
}


