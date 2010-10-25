#include "DemoVideoCallAppUi.h"
#include "sink.h"
#include <eikenv.h>				// CEikonEnv

CDummySink::CDummySink()
: iVideoBufferCounter(0), 
  iVideoDataLength(0),         
  iAudioBufferCounter(0), 
  iAudioDataLength(0),
  iDumpVideoFrames(EFalse),
  iDumpAudioFrames(EFalse),
  bFirstHeaderSentVideo(EFalse)
  {
 #ifdef DUMP_VIDEO_FRAMES

  if(iFs.Connect() == KErrNone)
    {
    if( iFile.Replace(iFs, _L("c:\\Data\\test.h263"), EFileWrite | EFileStream ) == KErrNone )
      iDumpVideoFrames = ETrue;
    if( iFileAudio.Replace(iFs, _L("c:\\Data\\test.amr"), EFileWrite | EFileStream ) == KErrNone )
        iDumpAudioFrames = ETrue;
    
    }
#endif
  iAppUi = static_cast<CDemoVideoCallAppUi*> (CEikonEnv::Static()->EikAppUi());
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
	if ( aBuffer->Type() == iVideoCodec )
    {   
		TPtrC8 pData = aBuffer->Data();
		iAppUi->SendVideoFrame(pData);
    }
	else if ( aBuffer->Type() == iAudioCodec )
    {
    }
}
    
// eof
