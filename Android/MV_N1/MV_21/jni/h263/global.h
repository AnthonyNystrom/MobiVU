////////////////////////////////////////////////////////////////////////////
//
//
//    Project     : VideoNet version 1.1.
//    Description : Peer to Peer Video Conferencing over the LAN.
//	  Author      :	Nagareshwar Y Talekar ( nsry2002@yahoo.co.in)
//    Date        : 15-6-2004.
//
//    I have converted origional fast h.263 encoder library from C to C++ 
//	  so that it can be integrated into any windows application easily.
//	  I have removed some of unnecessary codes/files from the
//	  fast h263 library.Also moved definitions and declarations
//	  in their proper .h and .cpp files.
//
//    File description : 
//    Name    : Global.h
//
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLOBAL_H__8D36A381_5D0B_11D6_889B_000B2B0F84B6__INCLUDED_)
#define AFX_GLOBAL_H__8D36A381_5D0B_11D6_889B_000B2B0F84B6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "def.h"
#include "struct.h"


typedef void (*WriteByte) (int);

// Global variable 
extern WriteByte WriteByteFunction;


class CGlobal  
{
public:
	CGlobal();
	
  int headerlength; /* Global variables */
  int pels;
  int cpels;
  int uskip;
  int vskip;
  size_t sizeof_frame;
  int lines;
  int trace;
  int advanced;
  int pb_frames;
  int mv_outside_frame;
  int long_vectors;
  int mbr;
  int mbc;

  float target_framerate;
  FILE *tf;

//#ifdef PRINT_DEBUG
//  FILE *debugf;
//#endif


  int search_p_frames;		/* internal variable for exh/log search */
  FILE *mv_file;
	Pict *pic;
};

extern CGlobal Global;


#endif // !defined(AFX_GLOBAL_H__8D36A381_5D0B_11D6_889B_000B2B0F84B6__INCLUDED_)
