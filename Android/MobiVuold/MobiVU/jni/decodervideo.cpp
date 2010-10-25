#include <android/log.h>
#include "decodervideo.h"
#include "include/mp4dec_api.h"

CDecoderVideo::CDecoderVideo(){
	_pvDec = NULL;
}
CDecoderVideo::~CDecoderVideo(){
}

void CDecoderVideo::Release(JNIEnv*  env)
{
	env->ReleaseByteArrayElements(m_byDecoded, m_pbyDecoded, 0);
	env->ReleaseByteArrayElements(m_byRGB, m_pbyRGB, 0);
	env->ReleaseIntArrayElements(m_iRGB, m_piRGB, 0);
	OSCL_DELETE(((ColorConvert32*)_pCC));
	if (_pvDec){
		_pvDec->CleanUpVideoDecoder();
		delete _pvDec;
		_pvDec=NULL;
	}
	__android_log_write(ANDROID_LOG_DEBUG, "CDecoderVideo", "Released");
}
// return 0 if not decoded
int CDecoderVideo::Decode(JNIEnv*  env, jbyteArray iData, int iSize){
	jbyte *pbydatain  = env->GetByteArrayElements(iData, 0);
	/*FILE *_fp;
	char sfile[130];
	sprintf(sfile, "/sdcard/video.h263", _icnt);
	_icnt++;
	_fp = fopen(sfile, "a");
	fwrite(pbydatain, sizeof(uint8), iSize, _fp);
	fclose(_fp);*/
	
	if (_pvDec){
		uint32 TimeStamp = 0xFFFFFFFF;
		uint UseExtTimestamp = 0;
		bool bRes = _pvDec->DecodeVideoFrame((uint8**)&pbydatain, (uint32*)&TimeStamp, (int32*)&iSize, &UseExtTimestamp, (uint8 *)m_pbyDecoded);
		if (!bRes)
			__android_log_write(ANDROID_LOG_DEBUG, "CDecoderVideo::Decode", "Failed");
		else{		
			//__android_log_write(ANDROID_LOG_DEBUG, "CDecoderVideo::Decode", "OK");			
			//ConvertYUVtoRGB32(_py, _pu, _pv, (unsigned int *)m_piDecoded, 176, 144);			
		    if (_pCC->Convert((uint8 *)m_pbyDecoded, (uint8 *)m_pbyRGB) == 0)
		    {
				__android_log_write(ANDROID_LOG_DEBUG, "CDecoderVideo::ColorConversion", "Failed");
			}
/*
			int i, ii;
			int iSize=176 * 144;
			for(i=0, ii=0;i<iSize;i++, ii+=3){
				m_piRGB[i] = 0x0000FF00;
				m_piRGB[i] |= m_pbyRGB[ii+2];   m_piRGB[i]<<=8;
				m_piRGB[i] |= m_pbyRGB[ii+1]; m_piRGB[i]<<=8;
				m_piRGB[i] |= m_pbyRGB[ii];
			}*/
			int is = 176 * 144;
			int *p = (int*)m_pbyRGB;
			int i;
			for(i=0;i<is;i++){
				m_piRGB[i] = *p;
				m_piRGB[i]|= 0xFF000000;
				p++;
			}
		}
	}
	else
		__android_log_write(ANDROID_LOG_DEBUG, "CDecoderVideo::Decode", "Decoder doesn't init");
	
	env->ReleaseByteArrayElements(iData, pbydatain, 0);
	
	return 0;
}

int CDecoderVideo::Init(JNIEnv*  env, int iWidth, int iHeight){
	int iRet=0;
	_icnt=0;
	m_byDecoded = env->NewByteArray(BUFFER_DECODE_SIZE);
	m_pbyDecoded = env->GetByteArrayElements(m_byDecoded, 0);
	
	m_iRGB = env->NewIntArray(iWidth*iHeight);
	m_piRGB = env->GetIntArrayElements(m_iRGB, 0);

	_pCC = (ColorConvert32*)ColorConvert32::NewL();
//	if (!(_pCC->Init((iWidth + 1)&(~1), (iHeight + 1)&(~1), (iWidth + 1)&(~1), iWidth, (iHeight + 1)&(~1), (iWidth + 1)&(~1), CCROTATE_NONE)))
	if (!(_pCC->Init(iWidth, iHeight, iWidth, iWidth, iHeight, iWidth, CCROTATE_NONE)))
	{
		// Color converter failed Init
		__android_log_write(ANDROID_LOG_DEBUG, "CDecoderVideo::Init", "Color Conversion Failed");
		return iRet;
	}
	//_pCC->SetYuvFullRange(true);
	_pCC->SetMemHeight(iHeight);
    _pCC->SetMode(0); // Do not scaling 

    uint32 rgbbufsize = (uint32)(_pCC->GetOutputBufferSize());
	m_byRGB  = env->NewByteArray(rgbbufsize);
	__android_log_print(ANDROID_LOG_DEBUG, "CDecoderVideo::Init", "SizeBuffer Color:%d", rgbbufsize);
	m_pbyRGB = env->GetByteArrayElements(m_byRGB, 0);
				
	_pvDec = PVM4VDecoder::New();
	if (_pvDec!=NULL){
		uint8 * buff = NULL;
		int32 iSize = 0;
		int iMode = H263_MODE;
		bool bRes = _pvDec->InitVideoDecoder(&buff, &iSize, 1, (int32*)&iWidth, (int32*)&iHeight, &iMode);
		if (!bRes){
			__android_log_write(ANDROID_LOG_DEBUG, "CDecoderVideo::Init", "Failed");
			return false;
		}
		_pvDec->SetPostProcType(0);
		_pvDec->SetReferenceYUV((uint8*)m_pbyDecoded);	
		
		__android_log_write(ANDROID_LOG_DEBUG, "CDecoderVideo::Init", "Initialized");	
		iRet=1;
	}
	else
		__android_log_write(ANDROID_LOG_DEBUG, "CDecoderVideo::Init", "Can't alloc encoder");
	return iRet;
}
