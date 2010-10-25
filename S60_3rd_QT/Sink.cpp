#include "Sink.h"
#include <eikenv.h>				// CEikonEnv

CDummySink::CDummySink(MainWindow *iAppUi)
{
    _iAppUi             = iAppUi;
    iVideoBufferCounter = 0;
    iVideoDataLength    = 0;
    iAudioBufferCounter = 0;
    iAudioDataLength    = 0;
    iDumpVideoFrames    = EFalse;
    iDumpAudioFrames    = EFalse;

 #ifdef DUMP_VIDEO_FRAMES

  if(iFs.Connect() == KErrNone)
    {
    if( iFile.Replace(iFs, _L("c:\\Data\\test.h263"), EFileWrite | EFileStream ) == KErrNone )
      iDumpVideoFrames = ETrue;
    if( iFileAudio.Replace(iFs, _L("c:\\Data\\test.amr"), EFileWrite | EFileStream ) == KErrNone )
        iDumpAudioFrames = ETrue;
    
    }
#endif  
}
CDummySink::~CDummySink()
{
#ifdef DUMP_VIDEO_FRAMES
    iFile.Close();
    iFs.Close();
#endif
}
TInt CDummySink::SetAverageVideoBitRate(TInt aBitRate)
{
    iAvgVideoBitRate = aBitRate;
    return 0;
}


TInt CDummySink::SetMaxVideoBitRate(TInt aBitRate)
{
    iMaxVideoBitRate = aBitRate;
    return 0;
}


TInt CDummySink::SetAverageAudioBitRate(TInt aBitRate)
{
    iAvgAudioBitRate = aBitRate;
    return 0;
}


void CDummySink::WriteBufferL( CCMRMediaBuffer* aBuffer )
{
    TPtrC8 pData = aBuffer->Data();
    if ( aBuffer->Type() == iVideoCodec )
    {
        /*TBuf8<8000> tmp;
        tmp.Copy(pData);
        unsigned char* p=(unsigned char*)tmp.Ptr();        

        //_iAppUi->LogInfo("CDummySink::WriteBufferL:%d", aBuffer->BufferSize());
        */        
        unsigned char *p = (unsigned char*)pData.Ptr();        
        _iAppUi->SendVideoFrame(p, aBuffer->BufferSize());

    }
    else if ( aBuffer->Type() == iAudioCodec )
    {        
        _iAudioBuffer2Send.Append(pData);
        if (_iAudioBuffer2Send.Length()>=350){    //350 = 500 msec / 20 * 14. It's half second of buffering
            iAppUi->SendAudioFrame(_iAudioBuffer2Send);
            _iAudioBuffer2Send.Zero();
        }        
    }
}
    
// eof
