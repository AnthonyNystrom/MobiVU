/************************************************************************
 *
 *  yuvrgb24.c, colour space conversion for tmndecode (H.263 decoder)
 *  Copyright (C) 1996  Telenor R&D, Norway
 *        Karl Olav Lillevold <Karl.Lillevold@nta.no>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  Karl Olav Lillevold               <Karl.Lillevold@nta.no>
 *  Telenor Research and Development
 *  P.O.Box 83                        tel.:   +47 63 84 84 00
 *  N-2007 Kjeller, Norway            fax.:   +47 63 81 00 76
 *
 *  Robert Danielsen                  e-mail: Robert.Danielsen@nta.no
 *  Telenor Research and Development  www:    http://www.nta.no/brukere/DVC/
 *  P.O.Box 83                        tel.:   +47 63 84 84 00
 *  N-2007 Kjeller, Norway            fax.:   +47 63 81 00 76
 *  
 ************************************************************************/

#include "DGlobal.h"
#include "mylog.h"
#include "YuvToRgb.h"

/* Data for ConvertYUVtoRGB*/


long int crv_tab[256];
long int cbu_tab[256];
long int cgu_tab[256];

long int cgv_tab[256];
long int tab_76309[256];


/**
*   Init Routine...
*   This must be invoked only once before calling conversion routine
*/

void init_dither_tab()
{
  long int crv,cbu,cgu,cgv;
  int i;   
  
  crv = 104597; cbu = 132201;  /* fra matrise i global.h */
  cgu = 25675;  cgv = 53279;
  
  for (i = 0; i < 256; i++) {
    crv_tab[i] = (i-128) * crv;
    cbu_tab[i] = (i-128) * cbu;
    cgu_tab[i] = (i-128) * cgu;
    cgv_tab[i] = (i-128) * cgv;
    tab_76309[i] = 76309*(i-16);
  }
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
	pixel[3] = 0xFF;

	return pixel32;
}


void ConvertYUVtoRGB(unsigned char *src0,unsigned char *src1,unsigned char *src2, unsigned char *pRGB24Dst,int iWidth, int iHeight){
	int lStrideY = iWidth;
	int lStrideUV = lStrideY / 2;
	unsigned char *pYV12SrcV = src2;
	unsigned char *pYV12SrcU = src1;
	unsigned char *pYV12SrcY = src0;
	unsigned long ulClr = 0;
	for(int y = 0; y < iHeight; y++, pYV12SrcY += lStrideY, pYV12SrcU += (y%2)*lStrideUV, pYV12SrcV += (y%2)*lStrideUV)    
	{
		for(int x = 0; x < iWidth; x++, pRGB24Dst += 3) {
			ulClr = yuv2rgb(pYV12SrcY[x], pYV12SrcU[x/2], pYV12SrcV[x/2]);
			pRGB24Dst[2] = (unsigned char)((ulClr & 0x00FF0000) >> 16);
			pRGB24Dst[1] = (unsigned char)((ulClr & 0x0000FF00) >> 8);
			pRGB24Dst[0] = (unsigned char)(ulClr & 0x000000FF);
		}
	}
}
void ConvertYUVtoRGB32(unsigned char *src0,unsigned char *src1,unsigned char *src2, unsigned int *pRGB24Dst,int iWidth, int iHeight){
	int lStrideY = iWidth;
	int lStrideUV = lStrideY / 2;
	unsigned char *pYV12SrcV = src2;
	unsigned char *pYV12SrcU = src1;
	unsigned char *pYV12SrcY = src0;
	unsigned int ulClr = 0;
	for(int y = 0; y < iHeight; y++, pYV12SrcY += lStrideY, pYV12SrcU += (y%2)*lStrideUV, pYV12SrcV += (y%2)*lStrideUV)    
	{
		for(int x = 0; x < iWidth; x++, pRGB24Dst ++) {
			ulClr = yuv2rgb(pYV12SrcY[x], pYV12SrcU[x/2], pYV12SrcV[x/2]);
			*pRGB24Dst = ulClr;
		}
	}
}


/**********************************************************************
 *
 *	Name:	         ConvertYUVtoRGB	
 *	Description:     Converts YUV image to RGB (packed mode)
 *	
 *	Input:	         pointer to source luma, Cr, Cb, destination,
 *                       image width and height
 *	Returns:       
 *	Side effects:
 *
 *	Date: 951208	Author: Karl.Lillevold@nta.no
 *
 ***********************************************************************/
void ConvertYUVtoRGBOLD(unsigned char *src0,unsigned char *src1,unsigned char *src2,unsigned char *dst_ori,int width,int height)
{ /*      
  extern long int crv_tab[];
  extern long int cbu_tab[];
  extern long int cgu_tab[];

  extern long int cgv_tab[];
  extern long int tab_76309[];
*/
	//myLog.WriteLog((char*)"ConvertYUVtoRGB()",(char*)"Sono nella vecchia 2");
  int y11,y21;
	int y12,y22;
  int y13,y23;
	int y14,y24;
  int u,v; 
  int i,j;
	int c11, c21, c31, c41;
	int c12, c22, c32, c42;
	unsigned int DW;
	unsigned int *id1, *id2;
  unsigned char *py1,*py2,*pu,*pv;
  unsigned char *d1, *d2;
  
  d1 = dst_ori;
  d1 += width*height*3 - width*3;
	d2 = d1 - width*3;
  
  py1 = src0; pu = src1; pv = src2;
	py2 = py1 + width;
 
	id1 = (unsigned int *)d1;
	id2 = (unsigned int *)d2;

  for (j = 0; j < height; j += 2) { 
    /* line j + 0 */
    for (i = 0; i < width; i += 4) {
      u = *pu++;
      v = *pv++;
      c11 = crv_tab[v];
      c21 = cgu_tab[u];
      c31 = cgv_tab[v];
      c41 = cbu_tab[u];
      u = *pu++;
      v = *pv++;
      c12 = crv_tab[v];
      c22 = cgu_tab[u];
      c32 = cgv_tab[v];
      c42 = cbu_tab[u];

      y11 = tab_76309[*py1++]; /* (255/219)*65536 */
      y12 = tab_76309[*py1++];
      y13 = tab_76309[*py1++]; /* (255/219)*65536 */
      y14 = tab_76309[*py1++];

      y21 = tab_76309[*py2++];
      y22 = tab_76309[*py2++];
      y23 = tab_76309[*py2++];
      y24 = tab_76309[*py2++];

      /* RGBR*/
      DW = ((clp[(y11 + c41)>>16])) |
           ((clp[(y11 - c21 - c31)>>16])<<8) |
           ((clp[(y11 + c11)>>16])<<16) |  
           ((clp[(y12 + c41)>>16])<<24);
      *id1++ = DW;

      /* GBRG*/
      DW = ((clp[(y12 - c21 - c31)>>16])) |
           ((clp[(y12 + c11)>>16])<<8) |  
           ((clp[(y13 + c42)>>16])<<16) |
           ((clp[(y13 - c22 - c32)>>16])<<24);
      *id1++ = DW;

      /* BRGB*/
      DW = ((clp[(y13 + c12)>>16])) |  
           ((clp[(y14 + c42)>>16])<<8) |
           ((clp[(y14 - c22 - c32)>>16])<<16) |
           ((clp[(y14 + c12)>>16])<<24);  
      *id1++ = DW;

      /* RGBR*/
      DW = ((clp[(y21 + c41)>>16])) |
           ((clp[(y21 - c21 - c31)>>16])<<8) |
           ((clp[(y21 + c11)>>16])<<16) |  
           ((clp[(y22 + c41)>>16])<<24);
      *id2++ = DW;

      /* GBRG*/
      DW = ((clp[(y22 - c21 - c31)>>16])) |
           ((clp[(y22 + c11)>>16])<<8) |  
           ((clp[(y23 + c42)>>16])<<16) |
           ((clp[(y23 - c22 - c32)>>16])<<24);
      *id2++ = DW;

      /* BRGB*/
      DW = ((clp[(y23 + c12)>>16])) |  
           ((clp[(y24 + c42)>>16])<<8) |
           ((clp[(y24 - c22 - c32)>>16])<<16) |
           ((clp[(y24 + c12)>>16])<<24);  
      *id2++ = DW;
    }
    id1 -= (9 * width)>>2;
    id2 -= (9 * width)>>2;
    py1 += width;
    py2 += width;
  }           
}  



