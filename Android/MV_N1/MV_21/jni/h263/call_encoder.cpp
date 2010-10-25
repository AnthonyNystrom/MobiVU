#include "libr263.h"
#include "call_encoder.h"
#include "convert.h"
#include <jni.h>

CParam cparams;
int InitCodec();
void OwnWriteFunction(int byte);

extern jbyte *pbyH263;
int iSizeBufferH263;

int InitEncoder(){
	iSizeBufferH263=0;
	cparams.format = CPARAM_QCIF;
	InitLookupTable();
	WriteByteFunction = OwnWriteFunction;
	cparams.format=CPARAM_QCIF;
	cparams.inter = CPARAM_INTRA;  
	cparams.Q_intra = 8;
	return InitH263Encoder(&cparams);	
}

void ReleaseEncoder(){
	ExitH263Encoder(&cparams);
}

int Encode(unsigned char *pIn){
	Bits bits;		// Various count about compression
	iSizeBufferH263=0;
	cparams.data=(unsigned int*)pIn;		//  Data in YUV format...*************VERIFICARE CAST NON VA BENE ORA
	CompressFrame(&cparams, &bits);
	return iSizeBufferH263;
}

// Callback function gets invoked during compression...
// It returns the compressed data byte by byte...
void OwnWriteFunction(int byte)
{
	if(iSizeBufferH263<BUFFER_ENCODE_SIZE)
		pbyH263[iSizeBufferH263]=(unsigned char)byte;
	
	iSizeBufferH263++;
}


