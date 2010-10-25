#ifndef SINK_H
#define SINK_H

#include <e32base.h>
#include <f32file.h>

#include "ccmrmediasink.h"
#include "AudioStreamEngine.h"

//#define DUMP_VIDEO_FRAMES

class CDemoVideoCallAppUi;
class CAudioStreamEngine;

class CDummySink : public MCMRMediaSink
{
    public:
        
        CDummySink();
        ~CDummySink(){ iFile.Close(); iFs.Close(); };
        
        void WriteBufferL( CCMRMediaBuffer* aBuffer );

        inline TInt SetVideoFrameSize(TSize /*aSize*/)
            {
            return 0;
            }

        TInt SetAverageVideoBitRate( TInt aBitRate );
        TInt SetMaxVideoBitRate( TInt aBitRate );
        TInt SetAverageAudioBitRate( TInt aBitRate );

    public:
        TInt iVideoBufferCounter;
        TInt iVideoDataLength;
        TInt iAudioBufferCounter;
        TInt iAudioDataLength;
        TTimeIntervalMicroSeconds iLatestTimeStamp;
                
        TInt iAvgVideoBitRate;
        TInt iMaxVideoBitRate;
        TInt iAvgAudioBitRate;
        
        CCMRMediaBuffer::TBufferType iVideoCodec;
        CCMRMediaBuffer::TBufferType iAudioCodec;
        
        RFs     iFs;
        RFile   iFile;
        RFile   iFileAudio;
	    TBool   iDumpVideoFrames;
	    TBool   iDumpAudioFrames;
	    TBool bFirstHeaderSentVideo;
	    CDemoVideoCallAppUi* iAppUi;	    
	    CAudioStreamEngine* iAudioEngine;
    };
    
#endif  // SINK_H

// eof
