////////////////////////////////////////////////////////////////////////////
//
//
//    Project     : VideoNet version 1.1.
//    Description : Peer to Peer Video Conferencing over the LAN.
//	  Author      :	Nagareshwar Y Talekar ( nsry2002@yahoo.co.in)
//    Date        : 15-6-2004.
//
//    This is the modified version of tmndecode (H.263 decoder) 
//    written by Karl & Robert.It was in ANSI C. I have converted into C++
//    so that it can be integrated into any windows application. I have 
//    removed some of the files which had display and file storing 
//    functions.I have removed the unnecessary code and also added some
//    new files..
//	  Original library dealt with files. Input & Output , both were files.
//    I have done some major changes so that it can be used for real time 
//    decoding process. Now one can use this library for decoding H263 frames. 
//
//
//    File description : 
//    Name    : Tmndec.cpp
//    Details : Main entry point for decoder.
//
//    Usage : 
//			1) InitH263Decoder
//			2) Invoke DecompressFrame with parameters
//			      compressed data and output data pointers.
//			  Note > Output data must be large enough to hold RGB data.
//                 > Change the DecompressFrame function if you want 
//					 output in YUV420 format only. Just comment out conversion routine
//          3) Repeat the step 2 for each compressed frame , for exit do 4
//			4) ExitH263Decoder  .
//
/////////////////////////////////////////////////////////////////////////////



/************************************************************************
 *
 *  tmndec.c, main(), initialization, options for tmndecode (H.263 decoder)
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

/*
 * based on mpeg2decode, (C) 1994, MPEG Software Simulation Group
 * and mpeg2play, (C) 1994 Stefan Eckart
 *                         <stefan@lis.e-technik.tu-muenchen.de>
 *
 */

#include "Tmndec.h"
#include <android/log.h>
#include <errno.h>
#include "mylog.h"
#include "YuvToRgb.h"

int InitH263Decoder()
{
	
/*	if(dlog==NULL)
	{
	trace=0;
	quiet=1;
	verbose=0;
	__android_log_print(ANDROID_LOG_ERROR, "InitH263Decoder()","Unable to open log file: %s", strerror(errno) );
	return -1;
	}*/

   framerate=0;  // not used...
   outtype=0;    // YUV 
	temp_ref = 0;
   prev_temp_ref= -1; 

   ld = &base; 
     
	initbits();    // Setup source buffer pointers
	InitConvertTable();

	myLog.WriteLog((char*)"InitH263Decoder()",(char*)"Init OK");
	return 0;
 	 
	// For debugging....	 
	// trace=1;		// trace output
	// quiet=0;       // don't keep quiet print mesgs
	 
}



/**
*    Decompress the frame.....
*    > After decompression 
*    > yp ,up and vp pointers will point to respective Y , U , V frames...
*  
*  Input 
*      cdata    compressed data
*      size     size of the compressed frame
*      outdata  pointer to rgb buffer (size must be enough to store data)  
*  
*  Output
*      return 1  success  , outdata contains rgbdata
*	   return 0  operation failed...
*/

int DecompressFrame(unsigned char *cdata,int size,unsigned char **outdata,int outsize)
{
static int first=1,framenum=0;
		
		//Initialize output pointers...
		yp=NULL;
		up=NULL;
		vp=NULL;

		// Copy to other buffer can be done...
		// So that source buffer will be free
		 
		cindex=0;       // index into the cframe....
		csize=size;     // size of compressed frame 
		cframe=cdata;   // pointer to compressed frme
		//memcpy(cframe,cdata,size);


		// Reset the pointers...
		initbits();

		// Get the frame header
		getheader();

	
		// If first time...then call initdecoder....
		// This has to be done after calling getheader()
		// because getheader() setup some variables using the information
		// from header...

		if (first) 
		{
		   initdecoder();
		   first = 0;
		}

    

		// Decompress the frame and get the picture frame in YUV format
		getpicture(&framenum);
    
		framenum++;

		if(yp==NULL || up==NULL || vp==NULL)
		{
			myLog.WriteLog((char*)"DecompressFrame",(char*)"decompression failed...");
			return  0;
		}
		*outdata = yp;
/*
        if(yp!=NULL && up!=NULL && vp!=NULL){
			FILE *fpyuv;
			if (framenum==1)
				fpyuv=fopen("/sdcard/video2.yuv","wb");
			else
				fpyuv=fopen("/sdcard/video2.yuv","ab");
			if (fpyuv!=NULL){
				char sMsgLog[128];
				int iUVSize = (horizontal_size/2) * (vertical_size/2);
				fwrite(yp,1,horizontal_size * vertical_size+(iUVSize*2), fpyuv);
				fclose(fpyuv);
			}
		}*/
		return 1;
	


   		// convert YUV to RGB
		// *******  TODO *********
		// check if outdata is enough to store rgb data
		//
		int totalsize=horizontal_size * vertical_size *4;
		if(outsize< totalsize || outdata==NULL)
		{
			myLog.WriteLog((char*)"DecompressFrame",(char*)"Output buffer is not sufficient");			return 0;			
		}
		
//		ConvertYUVtoRGB32(yp,up,vp,outdata,horizontal_size,vertical_size);
		/*FILE *fpyuv;
		char sFileName[128];
		sprintf(sFileName, "/sdcard/video_%d.rgba", framenum);		
		fpyuv=fopen(sFileName,"wb");
		
		if (fpyuv!=NULL){
			fwrite(outdata,1,horizontal_size * vertical_size*4, fpyuv);
			fclose(fpyuv);
		}*/
		
		//myLog.WriteLog((char*)"DecompressFrame()",(char*)"Decode SUCCESSFUL :-)");
 
return 1;
}

            

/**
*    Do the clean up here
*
*/
void ExitH263Decoder()
{
	int i;
		 
	for(i=0;i<3;i++)
	{

		if(refframe[i])
		free(refframe[i]);

		if(oldrefframe[i])
		free(oldrefframe[i]);

		if(bframe[i])
		free(bframe[i]);

		if(edgeframeorig[i])
		free(edgeframeorig[i]);
			  	  

	}	
}



/**
*    Initialize decoder
*
*
*/
void initdecoder()
{
  int i, cc, size;


  /* clip table */
  if (!(clp=(unsigned char *)malloc(1024)))
    error((char*)"malloc failed\n");

  clp += 384;

  for (i=-384; i<640; i++)
    clp[i] = (i<0) ? 0 : ((i>255) ? 255 : i);

  /* MPEG-1 = TMN parameters */
  matrix_coefficients = 5;


  switch (source_format) 
  {
  
	case (SF_SQCIF):
      horizontal_size = 128;
      vertical_size = 96;
      break;
    case (SF_QCIF):
      horizontal_size = 176;
      vertical_size = 144;
      break;
    case (SF_CIF):
      horizontal_size = 352;
      vertical_size = 288;
      break;
    case (SF_4CIF):
      horizontal_size = 704;
      vertical_size = 576;
      break;
    case (SF_16CIF):
      horizontal_size = 1408;
      vertical_size = 1152;
      break;
    default:
		myLog.WriteLog((char*)"initdecoder",(char*)"ERROR: Illegal input format");
      return;
      break;
  }

	char slog[128];
	sprintf(slog, "H.Size:%d V.Size:%d", horizontal_size, vertical_size);
	myLog.WriteLog((char*)"initdecoder",slog);


  mb_width = horizontal_size/16;
  mb_height = vertical_size/16;
  coded_picture_width = horizontal_size;
  coded_picture_height = vertical_size;
  chrom_width =  coded_picture_width>>1;
  chrom_height = coded_picture_height>>1;
  blk_cnt = 6;
  
  int iSize1, iSize2;
  size = coded_picture_width*coded_picture_height;
  iSize1 = size;
  iSize2 = chrom_width*chrom_height; 
  size += (iSize2*2);

  if (!(refframe[0] = (unsigned char *)malloc(size)))
     error((char*)"malloc failed\n");
  else{
	 unsigned char *p = refframe[0];
	 refframe[1] = p+iSize1;
	 refframe[2] = p+iSize1+iSize2;
  }

  if (!(oldrefframe[0] = (unsigned char *)malloc(size)))
    error((char*)"malloc failed\n");
  else{
  	 unsigned char *p = oldrefframe[0];
	 oldrefframe[1] = p+iSize1;
	 oldrefframe[2] = p+iSize1+iSize2;	
  }

  if (!(bframe[0] = (unsigned char *)malloc(size)))
    error((char*)"malloc failed\n");
  else{
  	 unsigned char *p = bframe[0];
	 bframe[1] = p+iSize1;
	 bframe[2] = p+iSize1+iSize2;	
  }
/*  for (cc=0; cc<3; cc++) 
  
  {
  
	// Find out size...
	 if (cc==0)
      size = coded_picture_width*coded_picture_height;
    else
      size = chrom_width*chrom_height;


	// Allocate memory
    if (!(refframe[cc] = (unsigned char *)malloc(size)))
      error((char*)"malloc failed\n");


    if (!(oldrefframe[cc] = (unsigned char *)malloc(size)))
      error((char*)"malloc failed\n");

    if (!(bframe[cc] = (unsigned char *)malloc(size)))
      error((char*)"malloc failed\n");
  }
*/

  for (cc=0; cc<3; cc++) 
  {
  
	  if (cc==0) 
	  {
		  size = (coded_picture_width+64)*(coded_picture_height+64);
      
		  if (!(edgeframeorig[cc] = (unsigned char *)malloc(size)))
		  error((char*)"malloc failed\n");
      
		  
		  edgeframe[cc] = edgeframeorig[cc] + (coded_picture_width+64) * 32 + 32;
	  }
      else 
	  {
      size = (chrom_width+32)*(chrom_height+32);
      
	  if (!(edgeframeorig[cc] = (unsigned char *)malloc(size)))
      error((char*)"malloc failed\n");
      
	  edgeframe[cc] = edgeframeorig[cc] + (chrom_width+32) * 16 + 16;
	  }
    
  }

  
  /* IDCT */
  if (refidct)
    init_idctref();
  else
    init_idct();
}


// Error routine......
void error(char *text)
{
	myLog.WriteLog((char*)"error", (char*)text);}


/* trace output */
void printbits(int code,int bits,int len)
{	myLog.WriteLog((char*)"printbits", (char*)"remove comment..!!!");

  // for (i=0; i<len; i++)
  // printf("%d",(code>>(bits-1-i))&1);
}






