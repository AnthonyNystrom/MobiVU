#ifndef _DECODER
#define _DECODER
#include <jni.h>
#include "include/pvm4vdecoder.h"
#include "include/cczoomrotation16.h"
#include "include/cczoomrotation32.h"
#include "include/cczoomrotation24.h"


#define BUFFER_DECODE_SIZE 38016
class CDecoderVideo{
public:
	CDecoderVideo();
	~CDecoderVideo();
	void Release(JNIEnv*  env);
	int Init(JNIEnv*  env, int iWidth, int iHeight);
	int GetSizeBufferDecoded() {return 1;}
	int Decode(JNIEnv*  env, jbyteArray iData, int iSize);
	jbyteArray m_byDecoded;
	jbyteArray m_byRGB;
	jintArray m_iRGB;
private:	
	int32 GetVideoHeader(int32 aLayer, uint8* aBuf, int32 aMaxSize);
	bool DecodeFrame(uint8 *bitstream[], uint32 *timestamp, int32 *buffer_size, uint *use_ext_timestamp, uint8 *currYUV);
	void GetVideoDimensions(int32 *display_width, int32 *display_height);
	bool Init2(uint8 *volbuf[], int32 *volbuf_size,	int32 nLayers,	int32* iWidth,	int32* iHeight,	int *mode);
	jbyte *m_pbyDecoded;
	jbyte *m_pbyRGB;
	jint  *m_piRGB;
	PVM4VDecoder *_pvDec;
	ColorConvert32 *_pCC;
	int _icnt	;
	uint8 VO_START_CODE1[4];
	uint8 VOSH_START_CODE1[4];
	uint8 VOP_START_CODE1[4];
	uint8 H263_START_CODE1[3];	
	uint8 *_H263Header;	
};

#endif
