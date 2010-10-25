#include <android/log.h>
#include "encodervideo.h"
#include "include/mp4enc_api.h"

#define DEFAULT_BITRATE 72000
#define DEFAULT_FRAME_RATE 15
#define DEFAULT_INTRA_PERIOD 10
#define DEFAULT_OMX_MP4ENC_QPI 12
#define DEFAULT_OMX_MP4ENC_QPP 10
#define DEFAULT_OMX_MP4ENC_QPB 10


CEncoderVideo::CEncoderVideo(){
	m_pvEncVid=NULL;
}
CEncoderVideo::~CEncoderVideo(){

}

void CEncoderVideo::Release(JNIEnv*  env)
{	
//	fclose(_fplog);
	if (m_pvEncVid!=NULL)
	{	delete m_pvEncVid;
		m_pvEncVid=NULL;
	}
	env->ReleaseByteArrayElements(m_byEncodedVideo, m_pbyEncodedVideo, 0);
	env->ReleaseByteArrayElements(m_byYUV420, m_pbyYUV420, 0);
	OSCL_DELETE(((CCYUV420SEMItoYUV420*)_pCC));
	__android_log_write(ANDROID_LOG_DEBUG, "CEncoderVideo", "Released");
}

int CEncoderVideo::Init(JNIEnv*  env, int iWidth, int iHeight){
	m_iSizeEncoded = 0;

	int iRet=0;

	m_byEncodedVideo = env->NewByteArray(BUFFER_ENCODE_SIZE_VIDEO);
	m_pbyEncodedVideo = env->GetByteArrayElements(m_byEncodedVideo, 0);

	_pCC = (CCYUV420SEMItoYUV420*)CCYUV420SEMItoYUV420::New();
	if (!(_pCC->Init(iWidth, iHeight, iWidth, iWidth, iHeight, iWidth, CCROTATE_NONE)))
	{
		// Color converter failed Init
		__android_log_write(ANDROID_LOG_DEBUG, "CDecoderVideo::Init", "Color Conversion Failed");
		return iRet;
	}
	uint32 iBufSizeYUV420 = _pCC->GetOutputBufferSize();
	__android_log_print(ANDROID_LOG_DEBUG, "CDecoderVideo::Init", "Size Buffer YUV420:%d", iBufSizeYUV420);
	m_byYUV420  = env->NewByteArray(iBufSizeYUV420);
	m_pbyYUV420 = env->GetByteArrayElements(m_byYUV420, 0);
	_pCC->SetMemHeight(iHeight);
    _pCC->SetMode(0); // no scaling

	m_pvEncVid = CPVM4VEncoder::New(-1);
	
	if (m_pvEncVid!=NULL){
	
		iRet=1;
		m_inForm.iFrameWidth  = iWidth;
		m_inForm.iFrameHeight = iHeight;
		m_inForm.iFrameRate   = DEFAULT_FRAME_RATE;
		m_inForm.iFrameOrientation =-1;
		m_inForm.iVideoFormat = ECVEI_YUV420;
		
		oscl_memset(&m_enParam, 0, sizeof(m_enParam));

		m_enParam.iEncodeID = 0;
		m_enParam.iNumLayer = 1;
		m_enParam.iFrameWidth[0]  = iWidth;
		m_enParam.iFrameHeight[0] = iHeight;
		m_enParam.iBitRate[0] = DEFAULT_BITRATE;		
		m_enParam.iFrameRate[0] = (float)DEFAULT_FRAME_RATE;
		m_enParam.iFrameQuality = 1;
		m_enParam.iEnableFrameQuality = false;
		m_enParam.iIFrameInterval = -1;//DEFAULT_INTRA_PERIOD;
		m_enParam.iNumIntraMBRefresh = 1;
		m_enParam.iBufferDelay	= -1;
		m_enParam.iContentType = ECVEI_H263;//ECVEI_STREAMING;//ECVEI_H263;
		m_enParam.iRateControlType = ECBR_1;
		m_enParam.iRDOptimal = false;
		m_enParam.iIquant[0] = DEFAULT_OMX_MP4ENC_QPI;
		m_enParam.iPquant[0] = DEFAULT_OMX_MP4ENC_QPP;
		m_enParam.iBquant[0] = DEFAULT_OMX_MP4ENC_QPB;
		m_enParam.iSearchRange = 16;
		m_enParam.iMV8x8 = false;
		m_enParam.iMVHalfPel = true;
		m_enParam.iSceneDetection = false;
		m_enParam.iPacketSize = 0;
		m_enParam.iNoCurrentSkip = false;
		m_enParam.iNoFrameSkip = false;
		m_enParam.iClipDuration = 0;
		m_enParam.iProfileLevel = ECVEI_CORE_LEVEL2;
		
		m_inData.iTimeStamp=0;
		if (m_pvEncVid->Initialize(&m_inForm, &m_enParam)==ECVEI_SUCCESS){
			iRet=1;			__android_log_write(ANDROID_LOG_DEBUG, "CEncoderVideo::Init", "Video Encoder Initialized");		
	//		_fplog = fopen("/sdcard/encoder.log", "w");
		}
		else
			__android_log_write(ANDROID_LOG_DEBUG, "CEncoderVideo::Init", "Video Encoder Failed");		
	}
	else{
		__android_log_write(ANDROID_LOG_DEBUG, "CEncoderVideo::Init", "Video Encoder Can't alloc encoder");
	}

	return iRet;
}


int CEncoderVideo::EncodeVideo(JNIEnv*  env, jbyteArray iData, int iTimeStamp)
{
	jbyte *pbydatain  = env->GetByteArrayElements(iData, 0);
	if (_pCC->Convert((uint8 *)pbydatain, (uint8 *)m_pbyYUV420) == 0)
	{	__android_log_write(ANDROID_LOG_DEBUG, "CEncoderVideo::ColorConversion", "Failed");
		return 0;
	}

	m_iSizeEncoded = 0;
	if (m_pvEncVid){
		m_inData.iSource = (uint8*)m_pbyYUV420;
		int iDiff = iTimeStamp-m_inData.iTimeStamp;
//	    m_inData.iTimeStamp=iTimeStamp;
		m_inData.iTimeStamp+=66;
		int iRemain;		
		m_outData.iBitStream = (uint8*)m_pbyEncodedVideo;
		m_outData.iBitStreamSize = BUFFER_ENCODE_SIZE_VIDEO;
		TCVEI_RETVAL ret = m_pvEncVid->EncodeFrame(&m_inData, &m_outData, &iRemain);
		/*char sstr[128];
		sstr[0]=NULL;*/
		switch(ret){
			case ECVEI_SUCCESS:
				//__android_log_print(ANDROID_LOG_DEBUG, "CEncoderVideo::Encode", "ECVEI_SUCCESS:%d", m_outData.iBitStreamSize);
				//sprintf(sstr, "Encoded:%d times:%d\n", m_outData.iBitStreamSize, iDiff);
				m_iSizeEncoded = m_outData.iBitStreamSize;
				break;
			case ECVEI_FAIL:
//				strcat(sstr, "Encoded failed\n");
				__android_log_write(ANDROID_LOG_DEBUG, "CEncoderVideo::Encode", "ECVEI_FAIL");
				break;
			case ECVEI_MORE_OUTPUT:
//				strcat(sstr, "Encoded more output\n");
				__android_log_print(ANDROID_LOG_DEBUG, "CEncoderVideo::Encode", "ECVEI_MORE_OUTPUT:%d", iRemain);
				m_iSizeEncoded = m_outData.iBitStreamSize;
				break;
		}
//		fwrite(sstr, 1, strlen(sstr), _fplog);

		
	}
	env->ReleaseByteArrayElements(iData, pbydatain, 0);

	return m_iSizeEncoded;
}


