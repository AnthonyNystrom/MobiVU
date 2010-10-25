#include <android/log.h>
#include "decodervideo.h"
#include "include/mp4dec_api.h"
#include "include/mp4lib_int.h"

VideoDecControls *_iVideoCtrls;
CDecoderVideo::CDecoderVideo(){
	_pvDec = NULL;
		
    VO_START_CODE1[0] = 0x00;
    VO_START_CODE1[1] = 0x00;
    VO_START_CODE1[2] = 0x01;
    VO_START_CODE1[3] = 0x00;

    VOSH_START_CODE1[0] = 0x00;
    VOSH_START_CODE1[1] = 0x00;
    VOSH_START_CODE1[2] = 0x01;
    VOSH_START_CODE1[3] = 0xB0;

    VOP_START_CODE1[0] = 0x00;
    VOP_START_CODE1[1] = 0x00;
    VOP_START_CODE1[2] = 0x01;
    VOP_START_CODE1[3] = 0xB6;

    H263_START_CODE1[0] = 0x00;
    H263_START_CODE1[1] = 0x00;
    H263_START_CODE1[2] = 0x80;	
}
CDecoderVideo::~CDecoderVideo(){
}

void CDecoderVideo::Release(JNIEnv*  env)
{
	env->ReleaseByteArrayElements(m_byDecoded, m_pbyDecoded, 0);
	env->ReleaseByteArrayElements(m_byRGB, m_pbyRGB, 0);
	env->ReleaseIntArrayElements(m_iRGB, m_piRGB, 0);
	OSCL_DELETE(((ColorConvert32*)_pCC));
	
	PVCleanUpVideoDecoder(_iVideoCtrls);
	//oscl_free(_H263Header);
	if (_pvDec){
		_pvDec->CleanUpVideoDecoder();
		delete _pvDec;
		_pvDec=NULL;
	}
	OSCL_DELETE((VideoDecControls *)_iVideoCtrls);
    _iVideoCtrls = NULL;
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
		uint32 TimeStamp =0xFFFFFFFF;
		uint UseExtTimestamp = 0;
		//bool bRes = _pvDec->DecodeVideoFrame((uint8**)&pbydatain, (uint32*)&TimeStamp, (int32*)&iSize, &UseExtTimestamp, (uint8 *)m_pbyDecoded);		
		bool bRes = DecodeFrame((uint8**)&pbydatain, (uint32*)&TimeStamp, (int32*)&iSize, &UseExtTimestamp, (uint8 *)m_pbyDecoded);		
		if (!bRes)
			__android_log_write(ANDROID_LOG_DEBUG, "CDecoderVideo::Decode", "Failed");
		else{		
			//__android_log_write(ANDROID_LOG_DEBUG, "CDecoderVideo::Decode", "OK");			
			//ConvertYUVtoRGB32(_py, _pu, _pv, (unsigned int *)m_piDecoded, 176, 144);			
		    if (_pCC->Convert((uint8 *)m_pbyDecoded, (uint8 *)m_pbyRGB) == 0)
		    {
				__android_log_write(ANDROID_LOG_DEBUG, "CDecoderVideo::ColorConversion", "Failed");
			}
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
	int min_stride = ((iWidth + 15) & (~15));
	int min_sliceheight = ((iHeight + 15) & (~15));
    int iSizeOut = (min_sliceheight * min_stride * 3) >> 1;
	
	m_byDecoded = env->NewByteArray(iSizeOut);
	m_pbyDecoded = env->GetByteArrayElements(m_byDecoded, 0);
	
	m_iRGB = env->NewIntArray(iWidth*iHeight);
	m_piRGB = env->GetIntArrayElements(m_iRGB, 0);

	_pCC = (ColorConvert32*)ColorConvert32::NewL();
	if (!(_pCC->Init((iWidth + 1)&(~1), (iHeight + 1)&(~1), (iWidth + 1)&(~1), iWidth, (iHeight + 1)&(~1), (iWidth + 1)&(~1), CCROTATE_NONE)))
//	if (!(_pCC->Init(iWidth, iHeight, iWidth, iWidth, iHeight, iWidth, CCROTATE_NONE)))
	{
		// Color converter failed Init
		__android_log_write(ANDROID_LOG_DEBUG, "CDecoderVideo::Init", "Color Conversion Failed");
		return iRet;
	}
	//_pCC->SetYuvFullRange(true);
	_pCC->SetMemHeight((iHeight+1)&(~1));
    _pCC->SetMode(0); // Do not scaling 

    uint32 rgbbufsize = (uint32)(_pCC->GetOutputBufferSize());
	m_byRGB  = env->NewByteArray(rgbbufsize);
	__android_log_print(ANDROID_LOG_DEBUG, "CDecoderVideo::Init", "SizeBuffer Color:%d", rgbbufsize);
	m_pbyRGB = env->GetByteArrayElements(m_byRGB, 0);
	
	_pvDec = PVM4VDecoder::New();
	if (_pvDec!=NULL){
		int32 iSize =0;
		_H263Header = NULL;//(uint8*)oscl_malloc(iSize);
		int iMode = H263_MODE;
		//int iret = GetVideoHeader(1, _H263Header, iSize);
		//__android_log_print(ANDROID_LOG_DEBUG, "CDecoderVideo::Init", "GetVideoHeader:%d", iret);
		//_pvDec->InitVideoDecoder(&_H263Header, &iSize, 1, (int32*)&iWidth, (int32*)&iHeight, &iMode);
		bool bRes = Init2(&_H263Header, &iSize, 1, (int32*)&iWidth, (int32*)&iHeight, &iMode);
		if (!bRes){
			__android_log_write(ANDROID_LOG_DEBUG, "CDecoderVideo::Init", "Failed");
			return false;
		}
		/*_pvDec->SetPostProcType(0);
		_pvDec->SetReferenceYUV((uint8*)m_pbyDecoded);	*/
		
		__android_log_write(ANDROID_LOG_DEBUG, "CDecoderVideo::Init", "Initialized");	
		iRet=1;
	}
	else
		__android_log_write(ANDROID_LOG_DEBUG, "CDecoderVideo::Init", "Can't alloc encoder");
		
	return iRet;
}

bool CDecoderVideo::DecodeFrame(uint8 *bitstream[], uint32 *timestamp, int32 *buffer_size, uint *use_ext_timestamp, uint8 *currYUV){
	return PVDecodeVideoFrame((VideoDecControls *)_iVideoCtrls, (uint8 **) bitstream, (uint32*)timestamp, (int32*)buffer_size, (uint *) use_ext_timestamp, (uint8 *) currYUV) ? true : false;
}
bool CDecoderVideo::Init2(uint8 *volbuf[],
							int32 *volbuf_size,
							int32 nLayers,
							int32* iWidth,
							int32* iHeight,
							int *mode){
	_iVideoCtrls = (VideoDecControls *) new VideoDecControls;
    if (_iVideoCtrls){
        oscl_memset(_iVideoCtrls, 0, sizeof(VideoDecControls));	
		if (PVInitVideoDecoder((VideoDecControls *)_iVideoCtrls, (uint8 **) volbuf, (int32*)volbuf_size, (int32)nLayers, *iWidth, *iHeight, (MP4DecodingMode) *mode))
		{
			GetVideoDimensions(iWidth, iHeight);
			*mode = (int)PVGetDecBitstreamMode((VideoDecControls *)_iVideoCtrls);
			PVSetReferenceYUV(_iVideoCtrls, (uint8*)m_pbyDecoded);
			PVSetPostProcType(_iVideoCtrls, 0);			
			VideoDecData *video = (VideoDecData *) _iVideoCtrls->videoDecoderData;
			/*video->vol[0]->quantPrecision = 4;
			video->vol[0]->errorResDisable = 1;
			video->vol[0]->useReverseVLC = 1;
			video->intra_acdcPredDisable = 0;
			video->vol[0]->scalability = 0;
			video->vol[0]->scalType = 1;*/
			return true;
		}
		else
		{
			return false;
		}
	}
	else
		return false;
}
void CDecoderVideo::GetVideoDimensions(int32 *display_width, int32 *display_height)
{
    PVGetVideoDimensions((VideoDecControls *)_iVideoCtrls, display_width, display_height);
}
int32 CDecoderVideo::GetVideoHeader(int32 aLayer, uint8* aBuf, int32 aMaxSize)
{
    OSCL_UNUSED_ARG(aLayer);

    int32 count = 0;
    char my_sc[4];

    uint8 *tmp_bs = aBuf;

    oscl_memcpy(my_sc, tmp_bs, 4);
    my_sc[3] &= 0xf0;
	
	oscl_memcpy(VOSH_START_CODE1, aBuf, 4);
	return count ;

    if (aMaxSize >= 4)
    {
		/*__android_log_write(ANDROID_LOG_DEBUG, "CDecoderVideo::GetVideoHeader", "passo 1");
        if (oscl_memcmp(my_sc, VOSH_START_CODE1, 4) && oscl_memcmp(my_sc, VO_START_CODE1, 4))
        {__android_log_write(ANDROID_LOG_DEBUG, "CDecoderVideo::GetVideoHeader", "passo 2");
            count = 0;
        }
        else
        {__android_log_write(ANDROID_LOG_DEBUG, "CDecoderVideo::GetVideoHeader", "passo 3");*/
            count = 0;
			bool bShortHeader=false;
            while (oscl_memcmp(tmp_bs + count, VOP_START_CODE1, 4))
            {__android_log_write(ANDROID_LOG_DEBUG, "CDecoderVideo::GetVideoHeader", "passo 4");
                count++;
                if (count > 1000)
                {__android_log_write(ANDROID_LOG_DEBUG, "CDecoderVideo::GetVideoHeader", "passo 5");
                    bShortHeader = true;
                    break;
                }
            }
            if (bShortHeader == true)
            {__android_log_write(ANDROID_LOG_DEBUG, "CDecoderVideo::GetVideoHeader", "passo 6");
                count = 0;
                while (oscl_memcmp(tmp_bs + count, H263_START_CODE1, 3))
                {__android_log_write(ANDROID_LOG_DEBUG, "CDecoderVideo::GetVideoHeader", "passo 7");
                    count++;
                }
            }
        //}
    }
    return count;
}
