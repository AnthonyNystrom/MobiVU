/*
 * Author Stefano Falasca, 7TouchGroup
 *
 */

#ifndef __FFDECODE_H__
#define __FFDECODE_H__

#include <avcodec.h>
#include "DemoVideoCallAppUi.h"
#include "ffdecode.h"

class CDemoVideoCallAppUi;
class CFFDecode
{
public:
	AVFrame *pFrameRGB;
	
	CFFDecode();
	~CFFDecode();

	TBool Init(int codec_id, int width, int height, int fps, char *pFileNameThumbBmp, int iSizeExtraData, unsigned char *pExtraData);
	TBool DecodePic(unsigned char* pData, long lsize, long pts, AVFrame **ppFrame, bool *pInterlaced = NULL);
	void NewThumb();
	void yuv420Torgb(int iWidth, int iHeight, unsigned char* pYV12SrcY, unsigned char *pRGB24Dst);
protected:
	void fill_yuv_image(AVFrame *pict, int frame_index, int width, int height);
	AVFrame *alloc_picture(int pix_fmt, int width, int height);
	void SaveFrame(AVFrame *img, int width, int height);
	void avcodec_get_context_defaults2(AVCodecContext *s, enum CodecType codec_type);
	
	
	char m_fileNamePic[1024];
	int m_iCntFrame;
	uint8_t  *buffer;
	AVCodec *m_codec;
	AVCodecContext *m_c;
	AVFrame m_picture;
	//SwsContext *img_convert_ctx;
	AVFrame *tmp_picture;
	CDemoVideoCallAppUi *iAppUi;
};

#endif
