#include <android/log.h>
#include "encodervideo.h"
#include "include/mp4enc_api.h"

#define DEFAULT_BITRATE 120000
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
	//OSCL_DELETE(((CCYUV420SEMItoYUV420*)_pCC));
	__android_log_write(ANDROID_LOG_DEBUG, "CEncoderVideo", "Released");
}

int CEncoderVideo::Init(JNIEnv*  env, int iWidth, int iHeight){
	m_iSizeEncoded = 0;

	int iRet=0;

	m_byEncodedVideo = env->NewByteArray(BUFFER_ENCODE_SIZE_VIDEO);
	m_pbyEncodedVideo = env->GetByteArrayElements(m_byEncodedVideo, 0);

	
	/*_pCC = (CCYUV420SEMItoYUV420*)CCYUV420SEMItoYUV420::New();
	if (!(_pCC->Init(iWidth, iHeight, iWidth, iWidth, iHeight, iWidth, CCROTATE_NONE)))
	{
		// Color converter failed Init
		__android_log_write(ANDROID_LOG_DEBUG, "CEncoderVideo::Init", "Color Conversion Failed");
		return iRet;
	}
	uint32 iBufSizeYUV420 = _pCC->GetOutputBufferSize();*/
	
	_iW = iWidth;
	_iH = iHeight;
	_ioutYsize = (_iW*_iH);
	_iSizeYUV = _ioutYsize+((_iW/2) * (_iH/2));

	int iSizeBuff = ((_iW * _iH*3) >> 1);
	m_byYUV420  = env->NewByteArray(iSizeBuff);
	m_pbyYUV420 = env->GetByteArrayElements(m_byYUV420, 0);
	
	/*_pCC->SetMemHeight(iHeight);
    _pCC->SetMode(0); // no scaling
	_pCC->SetYuvFullRange(true);
	*/
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
		m_enParam.iEnableFrameQuality = true;
		m_enParam.iIFrameInterval = DEFAULT_INTRA_PERIOD;
		m_enParam.iNumIntraMBRefresh = 10;
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

void CEncoderVideo::Semi2Planar(uint32 *inyuv_4){

	uint32 temp, tempU, tempV;
	int i, j;
	_outy  = (uint32*) m_pbyYUV420;
	_outcb = (uint16*) (m_pbyYUV420 + _ioutYsize);
	_outcr = (uint16*) (m_pbyYUV420 + _ioutYsize + (_ioutYsize >> 2));

	for (i = _iH; i > 0; i--)
	{	for (j = _iW >> 2; j > 0; j--)
		{
			temp = *inyuv_4++;
			*_outy++ = temp;
		}
	}	
	for (i = _iH >> 1; i > 0; i--)
	{	for ( j = _iW >> 2; j > 0; j--)
		{	temp = *inyuv_4++; /* V1U1V0U0 */
			tempU = temp & 0xFF; // U0
			tempU = tempU | ((temp >> 8) & 0xFF00); //U1U0

			tempV = (temp >> 8) & 0xFF; // V0
			tempV = tempV | ((temp >> 16) & 0xFF00); //V1V0

			*_outcb++ = tempV;
			*_outcr++ = tempU;
		}
	}
}

int CEncoderVideo::EncodeVideo(JNIEnv*  env, jbyteArray iData, int iTimeStamp)
{
	m_iSizeEncoded = 0;	
	jbyte *pbydatain  = env->GetByteArrayElements(iData, 0);
	
			/*FILE *fpyuv;
			fpyuv=fopen("/sdcard/video1.yuv","ab");
			if (fpyuv!=NULL){
				int iUVSize = (176/2) * (144/2);
				fwrite(pbydatain,1,176 * 144+(iUVSize*2), fpyuv);
				fclose(fpyuv);
			}*/	
			
/*	
	if (_pCC->Convert((uint8 *)pbydatain, (uint8 *)m_pbyYUV420) == 0)
	{	__android_log_write(ANDROID_LOG_DEBUG, "CEncoderVideo::ColorConversion", "Failed");
		return 0;
	}*/
	
	Semi2Planar((uint32*)pbydatain);
			
	m_inData.iSource = (uint8*)m_pbyYUV420;
	m_inData.iTimeStamp+=66;
	m_outData.iBitStream = (uint8*)m_pbyEncodedVideo+3;
	m_outData.iBitStreamSize = BUFFER_ENCODE_SIZE_VIDEO-3;
	
	int iRemain;		
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

	env->ReleaseByteArrayElements(iData, pbydatain, 0);

	return m_iSizeEncoded;
}


