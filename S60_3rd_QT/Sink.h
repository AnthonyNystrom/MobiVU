#ifndef SINK_H
#define SINK_H

#include <e32base.h>
#include <f32file.h>
#include "mainwindow.h"

#include <CCMRMediaSink.h>


//#define DUMP_VIDEO_FRAMES

class CDemoVideoCallAppUi;
class CAudioStreamEngine;
class CCMRMediaBuffer;
class MCMRMediaSink;
class MainWindow;

class CDummySink : public MCMRMediaSink
{
    public:
        
        CDummySink(MainWindow *iAppUi);
        virtual ~CDummySink();
        
        void WriteBufferL( CCMRMediaBuffer* aBuffer );

        inline TInt SetVideoFrameSize(TSize /*aSize*/){return 0;}

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
        CAudioStreamEngine* iAudioEngine;
        MainWindow *_iAppUi;
        TBuf8<1024> _iAudioBuffer2Send;
    };
    
#endif  // SINK_H

// eof
