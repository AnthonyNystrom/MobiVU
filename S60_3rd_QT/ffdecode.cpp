#include "ffdecode.h"
#include "opt.h"
#include <string.h>
#include <avformat.h>


//#define WIDTH_THUMB  240
//#define HEIGHT_THUMB 196
#define WIDTH_THUMB  124
#define HEIGHT_THUMB 91

#define FORMAT_COLOR PIX_FMT_RGB565
#define MAX_FRAMES_BUFFER 40

//void ffmpeg_avcodec_log(void *, int val, const char * msg, va_list ap);

CFFDecode::CFFDecode()
	: m_codec(NULL)
	, m_c(NULL)
{
    buffer      = NULL;
    _bStop      = false;
}


CFFDecode::~CFFDecode()
{
    _bStop = true;
    msleep(300);

    // Free the RGB image
    if (buffer)
        delete [] buffer;
    av_free(pFrameRGB);

    if (m_c != NULL) {
        avcodec_close(m_c);
        av_free(m_c);
        m_c = NULL;
    }
    if (_img_convert_ctx != NULL)
        av_free(_img_convert_ctx);


    ClearFramesBuffer();
}

void CFFDecode::ClearFramesBuffer()
{
    iAppUi->LogInfo("ClearFramesBuffer:%d", _Frames.count());
    Frm *pf;
    for (int i=0; i<_Frames.count(); i++){
        pf = _Frames[i];
        delete pf;
    }
    _Frames.clear();
}

static void ffmpeg_avcodec_log(void *ptr, int val, const char * msg, va_list ap)
{
    AVClass* avc= ptr ? *(AVClass**)ptr : NULL;

    char sLog[255];
    sprintf(sLog, "[%s @ %p] ", avc->item_name(ptr), avc);
    iAppUi->LogInfo(sLog);
    sprintf(sLog, msg, ap);
    iAppUi->LogInfo(sLog);

    //iAppUi->LogInfo(_L("ffmpeg_avcodec_log [%s @ %p]"), avc->item_name(ptr), avc);
    //iAppUi->LogInfo(_L(msg), ap);
}


TBool CFFDecode::Init(int codec_id, int width, int height, int iwidthscale, int iheightscale, int fps, int iSizeExtraData, unsigned char *pExtraData)
{
    _iWS=iwidthscale;
    _iHS=iheightscale;

    avcodec_init();
    register_avcodec(&h263_decoder);
    register_avcodec(&mpeg4_decoder);
    avcodec_register_all();

    if ((width & 1) == 1) return EFalse;
    if ((height & 1) == 1) return EFalse;

    av_log_set_callback( ffmpeg_avcodec_log );

    m_codec = avcodec_find_decoder((CodecID)codec_id);
    if (m_codec == NULL) {iAppUi->LogInfo("avcodec_find_decoder fail");return EFalse;}

    m_c = avcodec_alloc_context();
    if (m_c == NULL) {iAppUi->LogInfo("avcodec_alloc_context error");return EFalse;}

    //avcodec_get_context_defaults2(m_c, CODEC_TYPE_VIDEO);
    avcodec_get_context_defaults(m_c);
    avcodec_get_frame_defaults(&m_picture);

    m_c->codec_id = (CodecID) codec_id;
    m_c->codec_type = CODEC_TYPE_VIDEO;
    m_c->strict_std_compliance = 0;

    // put sample parameters
    //m_c->bit_rate = bitrate;
    // resolution must be a multiple of two
    m_c->width = width;
    m_c->height = height;
    m_c->time_base.den = fps;
    m_c->time_base.num = 1;
    m_c->gop_size = 12; // emit one intra frame every twelve frames at most
    m_c->pix_fmt = PIX_FMT_YUV420P;
    if (m_c->codec_id == CODEC_ID_MPEG2VIDEO) {
        // just for testing, we also add B frames
        m_c->max_b_frames = 2;
    }
    if (m_c->codec_id == CODEC_ID_MPEG1VIDEO){
        // needed to avoid using macroblocks in which some coeffs overflow
        // this doesnt happen with normal video, it just happens here as the
        // motion of the chroma plane doesnt match the luma plane
        m_c->mb_decision=2;
    }
    if (codec_id==CODEC_ID_WMV3){
        iAppUi->LogInfo("FFINIT CODEC_ID_WMV3 Extradata Size:%d\n",iSizeExtraData);
        m_c->extradata_size = iSizeExtraData;
        m_c->extradata = (uint8_t*) av_mallocz(m_c->extradata_size);
        memcpy(m_c->extradata, pExtraData, m_c->extradata_size);
        iAppUi->LogInfo("m_c->extradata:");
        for (int i=0;i<m_c->extradata_size*8; i++)
        {
            if ((*(m_c->extradata)>>i) & 0x1)
                iAppUi->LogInfo("1");
            else
                iAppUi->LogInfo("0");
        }
        iAppUi->LogInfo("\n");
    }

    int iRetOpen = avcodec_open(m_c, m_codec);
    if ( iRetOpen < 0) {
        iAppUi->LogInfo("avcodec_open error:%d",iRetOpen);
        av_log(m_c, AV_LOG_DEBUG, "Fail avcodec_open\n");
        return EFalse;
    }
	
	// Allocate an AVFrame structure
    pFrameRGB=avcodec_alloc_frame();
    if(pFrameRGB==NULL)
        return EFalse;

    // Determine required buffer size and allocate buffer
    int numBytes=avpicture_get_size(FORMAT_COLOR,  _iWS, _iHS);
    buffer=new uint8_t[numBytes];

    // Assign appropriate parts of buffer to image planes in pFrameRGB
    avpicture_fill((AVPicture *)pFrameRGB, buffer, FORMAT_COLOR, _iWS, _iHS);

    if(_iWS==_VIDEO_SCALE_FULL_W){
        _img_convert_ctx = sws_getContext(m_c->width, m_c->height, PIX_FMT_YUV420P, _iWS, _iHS, FORMAT_COLOR, SWS_FAST_BILINEAR,NULL,NULL,NULL);
        iAppUi->LogInfo("sw_scale use %dx%d", _iWS, _iHS);
    }
    else{
        iAppUi->LogInfo("img_convert use %dx%d", _iWS, _iHS);
        _img_convert_ctx = NULL;
    }

    iAppUi->LogInfo("FFInit Decode completed");
    start();

    return ETrue;
}

/* prepare a dummy image */
void CFFDecode::fill_yuv_image(AVFrame *pict, int frame_index, int width, int height)
{
    int x, y, i;

    i = frame_index;

    /* Y */
    for(y=0;y<height;y++) {
        for(x=0;x<width;x++) {
            pict->data[0][y * pict->linesize[0] + x] = x + y + i * 3;
        }
    }

    /* Cb and Cr */
    for(y=0;y<height/2;y++) {
        for(x=0;x<width/2;x++) {
            pict->data[1][y * pict->linesize[1] + x] = 128 + y + i * 2;
            pict->data[2][y * pict->linesize[2] + x] = 64 + x + i * 5;
        }
    }
}
AVFrame *CFFDecode::alloc_picture(int pix_fmt, int width, int height)
{
    AVFrame *picture;
    uint8_t *picture_buf;
    int size;

    picture = avcodec_alloc_frame();
    if (!picture)
        return NULL;
    size = avpicture_get_size(pix_fmt, width, height);
    picture_buf = (uint8_t *)av_malloc(size);
    if (!picture_buf) {
        av_free(picture);
        return NULL;
    }
    avpicture_fill((AVPicture *)picture, picture_buf,
                   pix_fmt, width, height);
    return picture;
}

static const char* context_to_name(void* ptr) {
	AVCodecContext *avc= (AVCodecContext*)ptr;

	if(avc && avc->codec && avc->codec->name)
            return avc->codec->name;
	else
            return "NULL";
}

void CFFDecode::run(){
    int got_picture;
    Frm *pFrm;
    while(!_bStop){
        for (int i=0; i<_Frames.count(); i++){
            pFrm = _Frames[i];
            got_picture = 0;
            try {

                avcodec_decode_video(m_c, &m_picture, &got_picture, pFrm->_pData, pFrm->_lsize);
            } catch (...) {
                got_picture = 0;
            }
            if (got_picture != 0) {

                if (_img_convert_ctx)
                    sws_scale(_img_convert_ctx, m_picture.data,  m_picture.linesize,0, m_c->height, pFrameRGB->data, pFrameRGB->linesize);
                else
                    img_convert((AVPicture *)pFrameRGB, FORMAT_COLOR, (AVPicture*)&m_picture, m_c->pix_fmt, m_c->width, m_c->height);
            }            
            //iAppUi->LogInfo("Refreshh decoder");
            emit Refresh((unsigned char*)pFrameRGB->data[0]);
        }
        if(_Frames.count()>0)
            ClearFramesBuffer();
        else{            
            msleep(10);
        }
    }
}


// size pDataOut for YUV (width*height*3)/2
TBool CFFDecode::DecodePic(unsigned char* pData, long lsize)
{
    if (_Frames.count()>MAX_FRAMES_BUFFER)
        ClearFramesBuffer();

    unsigned char *p = new unsigned char[lsize];
    memcpy(p, pData, lsize);
    Frm *pFrm = new Frm(p, lsize);
    _Frames.append(pFrm);
    return ETrue;
}


