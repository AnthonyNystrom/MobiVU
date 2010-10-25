/*
 * Author Stefano Falasca, 7TouchGroup
 *
 */

#ifndef __FFDECODE_H__
#define __FFDECODE_H__

#include <avcodec.h>
#include <QThread>
#include "mainwindow.h"
#include "ffdecode.h"
#include <swscale.h>

#define _VIDEO_SCALE_FULL_W 240
#define _VIDEO_SCALE_FULL_H 200
/*#define _VIDEO_SCALE_FULL_W 288
#define _VIDEO_SCALE_FULL_H 240*/
#define _VIDEO_SCALE_HALF_W 176
#define _VIDEO_SCALE_HALF_H 144

class Frm
{
public:
    Frm(unsigned char* pData, long lsize){
        _pData = pData;
        _lsize = lsize;
    }
    ~Frm(){
        delete []_pData;
    }

    unsigned char* _pData;
    long _lsize;
};

class CDemoVideoCallAppUi;
class CFFDecode: public QThread
{
    Q_OBJECT
public:
    AVFrame *pFrameRGB;

    CFFDecode();
    ~CFFDecode();
    void run();

    TBool Init(int codec_id, int width, int height, int iwidthscale, int iheightscale, int fps, int iSizeExtraData, unsigned char *pExtraData);
    TBool DecodePic(unsigned char* pData, long lsize);    
signals:
    void Refresh(unsigned char*);

protected:
    void fill_yuv_image(AVFrame *pict, int frame_index, int width, int height);
    AVFrame *alloc_picture(int pix_fmt, int width, int height);    
    void avcodec_get_context_defaults2(AVCodecContext *s, enum CodecType codec_type);
    void ClearFramesBuffer();

    uint8_t  *buffer;
    AVCodec *m_codec;
    AVCodecContext *m_c;
    SwsContext * _img_convert_ctx;
    AVFrame m_picture;    
    bool _bStop;
    int _iWS, _iHS;

    QList<Frm*> _Frames;
};

#endif
