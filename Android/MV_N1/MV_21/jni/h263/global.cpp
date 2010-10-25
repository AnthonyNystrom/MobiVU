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
//    Name    : cpp
//
//
/////////////////////////////////////////////////////////////////////////////

#include "global.h"

// Global variable 
WriteByte WriteByteFunction;

CGlobal Global;

CGlobal::CGlobal(){
	advanced=0;
	cpels=0;
//FILE * debugf=0;
	headerlength=0;
	lines=0;
	long_vectors=0;
	mbc=0;
	mbr=0;
	mv_file=0;
	mv_outside_frame=0;
	pb_frames=0;
	pels=0;
	search_p_frames=0;
	sizeof_frame=0;
	target_framerate=0;
	tf=0;
   trace=0;
   uskip=0;
   vskip=0;
   pic=NULL;}





