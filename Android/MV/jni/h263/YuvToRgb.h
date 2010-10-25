#ifndef _YUV2RGB
#define _YUV2RGB
void ConvertYUVtoRGB(unsigned char *src0,unsigned char *src1,unsigned char *src2, unsigned char *pRGB24Dst,int iWidth, int iHeight);
void ConvertYUVtoRGBOLD(unsigned char *src0,unsigned char *src1,unsigned char *src2, unsigned char *pRGB24Dst,int iWidth, int iHeight);
void ConvertYUVtoRGB32(unsigned char *src0,unsigned char *src1,unsigned char *src2, unsigned int *pRGB24Dst,int iWidth, int iHeight);
void init_dither_tab();
#endif
