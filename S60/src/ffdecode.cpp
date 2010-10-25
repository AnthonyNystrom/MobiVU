#include "ffdecode.h"
#include "opt.h"
#include <string.h>
#include <avformat.h>

//#define WIDTH_THUMB  240
//#define HEIGHT_THUMB 196
#define WIDTH_THUMB  124
#define HEIGHT_THUMB 91

//void ffmpeg_avcodec_log(void *, int val, const char * msg, va_list ap);

CFFDecode::CFFDecode()
	: m_codec(NULL)
	, m_c(NULL)
{
	iAppUi = static_cast<CDemoVideoCallAppUi*> (CEikonEnv::Static()->EikAppUi());
	buffer=NULL;
	tmp_picture=NULL;
}


CFFDecode::~CFFDecode()
{
	// Free the RGB image
	if (buffer)
		delete [] buffer;	
	av_free(pFrameRGB);	
	
	if (m_c != NULL) {
		avcodec_close(m_c);
		av_free(m_c);
		m_c = NULL;
	}	
	if(tmp_picture)
		av_free(tmp_picture);	
}

void CFFDecode::NewThumb()
{
	if(tmp_picture){
		av_free(tmp_picture);
		tmp_picture=NULL;
	}
}
static void ffmpeg_avcodec_log(void *ptr, int val, const char * msg, va_list ap)
{
	AVClass* avc= ptr ? *(AVClass**)ptr : NULL;
	CDemoVideoCallAppUi *iAppUi = static_cast<CDemoVideoCallAppUi*> (CEikonEnv::Static()->EikAppUi());

	char sLog[255];
	sprintf(sLog, "[%s @ %p] ", avc->item_name(ptr), avc);
	iAppUi->LogInfo(sLog);
	sprintf(sLog, msg, ap);
	iAppUi->LogInfo(sLog);
	
	//iAppUi->LogInfo(_L("ffmpeg_avcodec_log [%s @ %p]"), avc->item_name(ptr), avc);
	//iAppUi->LogInfo(_L(msg), ap);
}


TBool CFFDecode::Init(int codec_id, int width, int height, int fps, char *pFileNameThumbBmp, int iSizeExtraData, unsigned char *pExtraData)
{
	avcodec_init();
	register_avcodec(&h263_decoder);
	register_avcodec(&mpeg4_decoder);
	avcodec_register_all();

	if ((width & 1) == 1) return EFalse;
	if ((height & 1) == 1) return EFalse;

	m_iCntFrame=0;

	av_log_set_callback( ffmpeg_avcodec_log );
	
	if (pFileNameThumbBmp)
		strcpy(m_fileNamePic, pFileNameThumbBmp);
	m_codec = avcodec_find_decoder((CodecID)codec_id);
	if (m_codec == NULL) {iAppUi->LogInfo(_L("avcodec_find_decoder fail"));return EFalse;}

	m_c = avcodec_alloc_context(); 
	if (m_c == NULL) {iAppUi->LogInfo(_L("avcodec_alloc_context error"));return EFalse;}

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
		iAppUi->LogInfo(_L("FFINIT CODEC_ID_WMV3 Extradata Size:%d\n"),iSizeExtraData);
		m_c->extradata_size = iSizeExtraData;
		m_c->extradata = (uint8_t*) av_mallocz(m_c->extradata_size); 
		memcpy(m_c->extradata, pExtraData, m_c->extradata_size);
		iAppUi->LogInfo(_L("m_c->extradata:"));
		for (int i=0;i<m_c->extradata_size*8; i++)
		{
			if ((*(m_c->extradata)>>i) & 0x1)
				iAppUi->LogInfo(_L("1"));
			else	
				iAppUi->LogInfo(_L("0"));
		}
		iAppUi->LogInfo(_L("\n"));
	}

	int iRetOpen = avcodec_open(m_c, m_codec);
	if ( iRetOpen < 0) {
		iAppUi->LogInfo(_L("avcodec_open error:"),iRetOpen);
		av_log(m_c, AV_LOG_DEBUG, "Fail avcodec_open\n");
		return EFalse;
	}
	
	// Allocate an AVFrame structure
    pFrameRGB=avcodec_alloc_frame();
    if(pFrameRGB==NULL)
        return EFalse;

    // Determine required buffer size and allocate buffer
    int numBytes=avpicture_get_size(PIX_FMT_RGB24,  m_c->width, m_c->height);
    buffer=new uint8_t[numBytes];

    // Assign appropriate parts of buffer to image planes in pFrameRGB
    avpicture_fill((AVPicture *)pFrameRGB, buffer, PIX_FMT_RGB24, m_c->width, m_c->height);

	iAppUi->LogInfo(_L("FFInit Decode completed"));
	return ETrue;
}

// size pDataOut for YUV (width*height*3)/2
TBool CFFDecode::DecodePic(unsigned char* pData, long lsize, long pts, AVFrame **ppFrame, bool *pInterlaced)
{
	if (ppFrame == NULL) return EFalse;
	if (m_c->pix_fmt != PIX_FMT_YUV420P) {iAppUi->LogInfo(_L("m_c->pix_fmt != PIX_FMT_YUV420P"));return EFalse;}
	*ppFrame = NULL;

	int got_picture = 0;
	int ret = 0;

	try {
		
		ret = avcodec_decode_video(m_c, &m_picture, &got_picture, pData, lsize);
		//iAppUi->LogInfo(_L("ret decode:%d"),ret);
		//iAppUi->LogInfo(_L("lsize decode:%d"),lsize);
	} catch (...) {
		ret = 0;
		got_picture = 0;
	}
	//iAppUi->LogInfo(_L("got_picture %d"),got_picture);
	if (got_picture == 0) {
		return EFalse;
	}
	m_picture.quality = 1;
	*ppFrame = &m_picture;
	if (pInterlaced != NULL) 
		*pInterlaced = (bool) m_picture.interlaced_frame;
	
	img_convert((AVPicture *)pFrameRGB, PIX_FMT_RGB24, (AVPicture*)&m_picture, m_c->pix_fmt, m_c->width, m_c->height);


	//iAppUi->LogInfo(_L("interlacced %d"),m_picture.interlaced_frame);

	/*if (!tmp_picture && m_picture.pict_type== FF_I_TYPE){ // one time jpg conversion
	//if (!tmp_picture){ // one time jpg conversion
		img_convert_ctx = sws_getContext(m_c->width, m_c->height, PIX_FMT_YUV420P,WIDTH_THUMB, HEIGHT_THUMB,PIX_FMT_RGB32, SWS_FAST_BILINEAR,NULL,NULL,NULL);
		if (img_convert_ctx){
			tmp_picture = alloc_picture(PIX_FMT_RGB32, WIDTH_THUMB, HEIGHT_THUMB);
			//fill_yuv_image(tmp_picture, 0, m_c->width, m_c->height);
			int ires=sws_scale(img_convert_ctx, m_picture.data,  m_picture.linesize,0, m_c->height, tmp_picture->data, tmp_picture->linesize);
			SaveFrame(tmp_picture, WIDTH_THUMB, HEIGHT_THUMB);
		}
	}*/
	

	return ETrue;
}

inline unsigned int yuv2rgb(int y, int u, int v)  {
	unsigned int pixel32;
	unsigned char *pixel = (unsigned char *)&pixel32;
	int r, g, b;
	r = (int)(y + (1.370705 * (v-128)));
	g = (int)(y - (0.698001 * (v-128)) - (0.337633 * (u-128)));
	b = (int)(y + (1.732446 * (u-128)));
	if (r > 255)
		r = 255;
	if (g > 255)
		g = 255;
	if (b > 255)
		b = 255;
	if (r < 0)
		r = 0;
	if (g < 0)
		g = 0;
	if (b < 0)
		b = 0;

	pixel[0] = r * 220 / 256;
	pixel[1] = g * 220 / 256;
	pixel[2] = b * 220 / 256;
	pixel[3] = 0;

	return pixel32;
}
void CFFDecode::yuv420Torgb(int iWidth, int iHeight, unsigned char* pYV12SrcY, unsigned char *pRGB24Dst)
{
	int lStrideY = iWidth;
	int lStrideUV = lStrideY / 2;
	unsigned char *pYV12SrcV = pYV12SrcY + lStrideY * iHeight;
	unsigned char *pYV12SrcU = pYV12SrcV + lStrideUV* iHeight / 2;
	unsigned long ulClr = 0;
	for(int y = 0; y < iHeight; y++, pYV12SrcY += lStrideY, pYV12SrcU += (y%2)*lStrideUV, pYV12SrcV += (y%2)*lStrideUV)    
	{
		for(int x = 0; x < iWidth; x++, pRGB24Dst += 3) {
			ulClr = yuv2rgb(pYV12SrcY[x], pYV12SrcU[x/2], pYV12SrcV[x/2]);
			pRGB24Dst[0] = (unsigned char)((ulClr & 0x00FF0000) >> 16);
			pRGB24Dst[1] = (unsigned char)((ulClr & 0x0000FF00) >> 8);
			pRGB24Dst[2] = (unsigned char)(ulClr & 0x000000FF);
		}
	}
}

void CFFDecode::SaveFrame(AVFrame *img, int width, int height)
{/*
	// Bitmap file header 
	BITMAPFILEHEADER *m_header = (BITMAPFILEHEADER *) malloc(sizeof(BITMAPFILEHEADER));
	m_header->bfType = 'MB'; 
	m_header->bfSize = 0;
	m_header->bfReserved1 = 0;
	m_header->bfReserved2 = 0;
	m_header->bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	// Bitmap infoheader 
	BITMAPINFOHEADER *m_DIB = (BITMAPINFOHEADER *) malloc(sizeof(BITMAPINFOHEADER));
	m_DIB->biSize = sizeof (BITMAPINFOHEADER);
	m_DIB->biWidth = width;
	m_DIB->biHeight = height;
	m_DIB->biPlanes = 1; 
	m_DIB->biBitCount = 32;
	m_DIB->biCompression = BI_RGB; 
	m_DIB->biSizeImage = 0;
	m_DIB->biXPelsPerMeter = 0;
	m_DIB->biYPelsPerMeter = 0;
	m_DIB->biClrUsed = 0;
	m_DIB->biClrImportant = 0;

	unsigned char *pByteImg = img->data[0];
	int isizesample=4;
	if (m_DIB->biBitCount==24)
		isizesample--;
	int iSizeBmp=width*height*isizesample;

	// Open the file 
	HANDLE hFile = CreateFileA(m_fileNamePic, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	return;

	// Write BITMAPFILEHEADER
	DWORD dwBytesWritten;
	WriteFile(hFile, m_header, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);

	// Write BITMAPINFOHEADER
	WriteFile(hFile, m_DIB, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);

	//WriteFile(hFile, pByteImg, iSizeBmp, &dwBytesWritten, NULL);

	// Write bitmap data
	unsigned char *p=pByteImg;
	int iLevelsGray=0;
	for (int ii=0; ii<height; ii++){
		p=(pByteImg+iSizeBmp-(width*isizesample))-(ii*width*isizesample);
		for (int i=0; i<width; i++){
			unsigned char cR=(unsigned char)*(p);
			unsigned char cG=(unsigned char)*(p+1);
			unsigned char cB=(unsigned char)*(p+2);
			if ((cR>=0x80-5 && cR<=0x80+5) && (cG>=0x80-5 && cG<=0x80+5) && (cB>=0x80-5 && cB<=0x81+5) )
				iLevelsGray++;
			WriteFile(hFile, (LPCVOID)p, 1, &dwBytesWritten, NULL);p++;
			WriteFile(hFile, (LPCVOID)p, 1, &dwBytesWritten, NULL);p++;
			WriteFile(hFile, (LPCVOID)p, 1, &dwBytesWritten, NULL);p++;
			if (m_DIB->biBitCount==32){
				WriteFile(hFile, (LPCVOID)p, 1, &dwBytesWritten, NULL);p++;}
		}
	}
	int iPercGray=(iLevelsGray*100)/(width*height);

	// Close the file 
	CloseHandle(hFile);

	DbgOutInt("iPercGray:",iPercGray);
	DbgOutInt("iLevelsGray:",iLevelsGray);
	if (iPercGray>80 && m_c->codec_id!=CODEC_ID_WMV3)
	{	DeleteFileA(m_fileNamePic);
		DbgOut("Delete file");
		DbgOut(m_fileNamePic);
		NewThumb();
	}
	else
		DbgOut("frame Thumb saved");*/
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

