/*
 * Copyright © 2008 Nokia Corporation.
 */

#include <mda\common\audio.h>
#include <mmf\common\mmfutilities.h>
#include <MdaAudioInputStream.h>    // audio input stream
#include <MdaAudioOutputStream.h>   // audio output stream
#include <s32file.h>                // RFileWriteStream and RFileReadStream
#include <QApplication>

#include "AudioStreamEngine.h"
#include "audiostream.pan"

// Audio data buffer size.
// In both 3rd Edition and 2nd Edition the total buffer (iStreamBuffer) size is 
// KFrameSizePCM * KFrameCountPCM = 40960 bytes. This will contain 2560 ms 
// of 16-bit audio data. 
// In 3rd Edition the KFrameSizePCM is 4096 bytes, because CMdaAudioInputStream::ReadL() 
// returns audio data in 4096-byte chunks. In 2nd Edition, ReadL() returns data in 320-byte
// chunks.
const TInt KFrameSizePCM = 4096;
const TInt KFrameCountPCM = 100;

// Audio data buffer size for AMR encoding. For AMR, the buffer size is the same in
// both 2nd and 3rd Edition devices (20 ms per frame, a total of 2560 ms in 128 frames).
const TInt KFrameSizeAMR = 14;
const TInt KFrameCountAMR = 128;
// Header data for an AMR-encoded audio file
const TInt KAMRHeaderLength=6;
const TUint8 KAMRNBHeader[KAMRHeaderLength] = { 0x23, 0x21, 0x41, 0x4d, 0x52, 0x0a };

// Files to store the sample audio clips
_LIT(KAudioFilePCM, "sample.aud");
_LIT(KAudioFileAMR, "sample.amr");

#ifdef __SERIES60_3X__
    #define KFullDuplexAudioPrefOutput            0x05220001
    #define KFullDuplexAudioPrefInput             0x05210001
#else // S60 2nd Edition FP2 + FP3

    #define KFullDuplexAudioPrefOutput            0x01350001
    #define KFullDuplexAudioPrefInput             0x01360001

#endif

#define KFullDuplexAudioPriority                      99


CAudioStreamEngine* CAudioStreamEngine::NewL(MainWindow* aAppUi)
    {
    CAudioStreamEngine* self = CAudioStreamEngine::NewLC(aAppUi);
    CleanupStack::Pop(self);
    return self;
    }

CAudioStreamEngine* CAudioStreamEngine::NewLC(MainWindow* aAppUi)
    {
    CAudioStreamEngine* self = new (ELeave) CAudioStreamEngine(aAppUi);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// Standard EPOC 2nd phase constructor
void CAudioStreamEngine::ConstructL()
    {
    _bAccessBuffer     = ETrue;
    _bFreeAccessBuffer = ETrue;
    _bCanPlay          = EFalse;
    _bCanWrite         = ETrue;
    _bWaitShortPlay    = false;
    _bCanRecord        = false;

    // Construct streams. We need to construct these here, so that at least the input stream
    // exists if SetEncodingL() is called before any recording has taken place 
    iInputStream = CMdaAudioInputStream::NewL(*this);
    iOutputStream = CMdaAudioOutputStream::NewL(*this);

    // Get a handle to the RFs session to be used (owned by CEikonEnv, NOT to be closed
    // when this application exits!)        
    iFs = CEikonEnv::Static()->FsSession();

    // Save the default encoding for later reference (the encoding is the same for
    // both input and output streams).
    iDefaultEncoding = iInputStream->DataType();
    // At first we are using the default encoding.
    iCurrentEncoding = iDefaultEncoding;

    // Stream buffer allocation (by default for PCM)
    iStreamBuffer = HBufC8::NewMaxL(iFrameSize * iFrameCount);
    iStreamStart=0;
    iStreamEnd=iFrameCount - 1;
    
    // The sample.aud/amr can be found in \private\<UID3>\ folder
    User::LeaveIfError( iFs.CreatePrivatePath( EDriveC ) );
    User::LeaveIfError( iFs.SetSessionToPrivate( EDriveC ) );
    
    iStop = CIdle::NewL( CActive::EPriorityIdle );
    }

// ----------------------------------------------------------------------------
// CAudioStreamEngine::CAudioStreamEngine(
//     CAudioStreamAppUi* aAppUi)
//
// onstructor
// ----------------------------------------------------------------------------
CAudioStreamEngine::CAudioStreamEngine(MainWindow* aAppUi)
: iAppUi(aAppUi), iUseAMR(EFalse), iAudioFile(KAudioFilePCM), iFrameSize(KFrameSizePCM), 
iFrameCount(KFrameCountPCM), iStreamBuffer(0), iFramePtr(0,0), iBufferOK(EFalse)
    {
    // By default we use PCM and initialise the instance variables accordingly above.
        
    // Initial audio stream properties for input and output, 8KHz mono. 
    // These settings could also be set/changed using method SetAudioPropertiesL() of
    // the input and output streams.
    iStreamSettings.iChannels=TMdaAudioDataSettings::EChannelsMono;
    iStreamSettings.iSampleRate=TMdaAudioDataSettings::ESampleRate8000Hz;
    }

// ----------------------------------------------------------------------------
// CAudioStreamEngine::~CAudioStreamEngine()
//
// destructor
// ----------------------------------------------------------------------------
CAudioStreamEngine::~CAudioStreamEngine()
    { 
    if (iInputStream)
        {
        if (iInputStatus!=ENotReady) 
            {
            iInputStream->Stop();
            }
        delete iInputStream;
        }

    if (iOutputStream)
        {
        if (iOutputStatus!=ENotReady) 
            {
            iOutputStream->Stop();
            }
        delete iOutputStream;
        }

    if (iStreamBuffer)
        {
        delete iStreamBuffer;
        }
    if (iStop)
        {
        iStop->Cancel();
        }
    delete iStop;
    }


// ----------------------------------------------------------------------------
// CAudioStreamEngine::Play()
//
// plays the audio data contained in the buffer
// ----------------------------------------------------------------------------
void CAudioStreamEngine::OpenForPlay()
    {
    if (iInputStatus == EOpening)
    	return;
        
    iAudioBuffer.Zero();
    _bCanPlay=EFalse;
    //iAppUi->LogInfo("CAudioStreamEngine::OpenForPlay()");
    // if either stream is active, return
    if (iInputStatus!=ENotReady || iOutputStatus!=ENotReady) 
        {
        iAppUi->LogInfo("CAudioStreamEngine::OpenForPlay() Stream in use, cannot play audio.");
        return;
        }
    iInputStatus = EOpening;

    /*if(!iBufferOK)
        {
        iAppUi->LogInfo("CAudioStreamEngine::Play() Nothing to play - record or load \na file first.");
        return;
        }*/
        
    // Open output stream.
    // Upon completion will receive callback in 
    // MMdaAudioOutputStreamCallback::MaoscOpenComplete().
    
    iOutputStream->Open(&iStreamSettings);

    while(!_bCanRecord)
        MyWait(100);
    Record();
    }

TBool CAudioStreamEngine::CanPlay(){
	return _bCanPlay;
}

// ----------------------------------------------------------------------------
// CAudioStreamEngine::Record()
//
// records audio data into the buffer
// ----------------------------------------------------------------------------
void CAudioStreamEngine::Record()
    {
    iAppUi->LogInfo("CAudioStreamEngine::Record()");
    // If either stream is active, return
    /*if (iInputStatus!=ENotReady || iOutputStatus!=ENotReady)
    {
        iAppUi->LogInfo("CAudioStreamEngine::Record() Stream in use, \ncannot record audio.");
        return;
    }*/
    if (!_bCanRecord){
        iAppUi->LogInfo("CAudioStreamEngine::Record() I Can't record, wating ending Output init");
        return;
    }


    // Open input stream.
    // Upon completion will receive callback in 
    // MMdaAudioInputStreamCallback::MaiscOpenComplete().
    iInputStream->Open(&iStreamSettings);
    }

// ----------------------------------------------------------------------------
// CAudioStreamEngine::Stop()
//
// stops playing/recording
// ----------------------------------------------------------------------------
void CAudioStreamEngine::Stop()
    {
    _bCanPlay=EFalse;
    // if input or output streams are active, close them
    if (iInputStatus!=ENotReady) 
        {
        iInputStream->Stop();
        iAppUi->LogInfo("CAudioStreamEngine::Stop() Recording stopped!");
        iBufferOK = ETrue;
        iInputStatus = ENotReady;
        }       
    if (iOutputStatus!=ENotReady) 
        {
        iOutputStream->Stop();
        iOutputStatus = ENotReady;
        iAppUi->LogInfo("CAudioStreamEngine::Stop() Playback stopped!");
        }
    }


// ----------------------------------------------------------------------------
// CAudioStreamEngine::LoadAudioFileL()
//
// loads the audio data from a file into the buffer
// ----------------------------------------------------------------------------
void CAudioStreamEngine::LoadAudioFileL()
    {
    RFileReadStream audiofile;

    // open file
    TFileName fileName;
    fileName.Copy(iAudioFilePath);
    fileName.Append(iAudioFile);

    TInt err = audiofile.Open(iFs, fileName, EFileRead|EFileStream);
    iStreamBuffer->Des().FillZ(iFrameCount * iFrameSize);  // Empty the stream buffer
    if (err==KErrNone) 
        {
        // file opened ok, proceed reading
        if (iUseAMR)
            {
            // Read the AMR header (the first 6 bytes). We don't need to save/use the header,
            // since while playback we already know it's an AMR-NB encoded stream.
            TBuf8<KAMRHeaderLength> temp;
            audiofile.ReadL(temp, KAMRHeaderLength);
            }

        TUint idx=0;
        while (idx < iFrameCount)
            {
            TRAPD(fstatus, audiofile.ReadL(GetFrame(idx), iFrameSize));
            if (fstatus!=KErrNone)
                break;
            idx++;
            }
        iStreamStart=0;
        iStreamEnd=idx-1;
        iAppUi->LogInfo("CAudioStreamEngine::LoadAudioFileL() Loading complete!");
        iBufferOK = ETrue;  
        }   
    else 
        {
        // failed to open file
        iAppUi->LogInfo("CAudioStreamEngine::LoadAudioFileL() Error loading audio sample!");
        iBufferOK = EFalse;
        }
    audiofile.Close();
    }


// ----------------------------------------------------------------------------
// CAudioStreamEngine::SaveAudioFileL()
//
// saves the audio data in the buffer into a file
// ----------------------------------------------------------------------------
void CAudioStreamEngine::SaveAudioFileL()
    {
    if (!iBufferOK)
    {
        // In case the encoding was changed between recording and trying to save the file
        iAppUi->LogInfo("CAudioStreamEngine::SaveAudioFileL() Recorded buffer does not match current encoding.");
        iAppUi->LogInfo("CAudioStreamEngine::SaveAudioFileL() Please re-record and try again.");
        return;
    }
    RFileWriteStream audiofile;

    // Check for free space for saving the sample
    TVolumeInfo volinfo;
    TInt err=iFs.Volume(volinfo,EDriveC);
    if ( volinfo.iFree<(iFrameCount*iFrameSize))
        {
        // Not enough free space on drive for saving, report and exit
        iAppUi->LogInfo("CAudioStreamEngine::SaveAudioFileL() Cannot save file:\nnot enough space!");
        return;
        }

    TFileName fileName;
    fileName.Copy(iAudioFilePath);
    fileName.Append(iAudioFile);
    err = audiofile.Replace(iFs, fileName, EFileWrite|EFileStream);
    if (err==KErrNone) 
        {
        if (iUseAMR)
            {
                // Write the six-byte AMR header, so that the file can be used by other
                // applications as well.
                for (int i = 0; i < KAMRHeaderLength; i++)
                    audiofile.WriteUint8L(KAMRNBHeader[i]);
            }
            
        // File opened ok, proceed writing.
        // Write audio data directly from iStreamBuffer
        for (TUint idx=iStreamStart; idx<=iStreamEnd; idx++)//iFrameCount; idx++)
            {
            audiofile.WriteL(GetFrame(idx));
            }
        iAppUi->LogInfo("CAudioStreamEngine::SaveAudioFileL() Saving complete!");
        }   
    else 
        {
        // failed to open file
        iAppUi->LogInfo("CAudioStreamEngine::SaveAudioFileL() Error saving audio sample!");
        }
    audiofile.Close();
    }

// ----------------------------------------------------------------------------
// CAudioStreamEngine::SetEncodingL(TBool aAmr)
//
// If argument is ETrue, AMR-NB encoding will be used in audio input/output.
// If EFalse, the default PCM is used. If the platform does not support AMR-NB,
// PCM will be used no matter what the argument's value is.
// ----------------------------------------------------------------------------
void CAudioStreamEngine::SetEncodingL(TBool aAmr)
    {
    // Act only if the new encoding differs from the current one
    if (iUseAMR != aAmr)
        {
        iUseAMR = aAmr;
        if (iUseAMR)
            {
            // Try to set AMR-NB encoding, this will indicate whether it is supported
            // by the platform or not.
            TRAPD(err, iInputStream->SetDataTypeL(KMMFFourCCCodeAMR));
            if (err != KErrNone)
                {
                iAppUi->LogInfo("CAudioStreamEngine::SetEncodingL AMR-NB not supported,\nusing PCM.");
                iCurrentEncoding = iDefaultEncoding;
                iUseAMR = EFalse;
                // We do not need to invalidate the buffer or change buffer settings, 
                // since the encoding was not changed -> just return.
                return;  
                }
            else
                {
                iCurrentEncoding = KMMFFourCCCodeAMR;
                iAudioFile.Zero();  // Empty the audio file name                
                iAudioFile.Append(KAudioFileAMR);
                iFrameCount = KFrameCountAMR;
                iFrameSize = KFrameSizeAMR;
                iAppUi->LogInfo("CAudioStreamEngine::SetEncodingL Encoding set to AMR-NB.");
                }
            }
        else
            {
            // If we get here, the encoding has previously been changed to AMR. Switch back to
            // PCM.
            iCurrentEncoding = iDefaultEncoding;
            iAudioFile.Zero();  // Empty the audio file name                
            iAudioFile.Append(KAudioFilePCM);
            iFrameCount = KFrameCountPCM;
            iFrameSize = KFrameSizePCM;
            iAppUi->LogInfo("CAudioStreamEngine::SetEncodingL Encoding set to PCM.");
            }

        // Make sure the user re-records or reloads the audio file, so that we do not 
        // accidentally try to play PCM data using AMR or vice versa.
        iBufferOK = EFalse; 
        if (iStreamBuffer) delete iStreamBuffer;
        iStreamBuffer = NULL; // In case the following NewL leaves
        iStreamBuffer = HBufC8::NewMaxL(iFrameSize * iFrameCount);
        iStreamStart=0;
        iStreamEnd=iFrameCount - 1;
        }   
    }

// ----------------------------------------------------------------------------
// CAudioStreamEngine::ShowMessage(
//     const TDesC& aMsg, TBool aReset=EFalse)
//
// displays text referenced by aMsg in the label, will append the aMsg in the 
// existing text in label if aReset is EFalse, otherwise will reset the label 
// text.
// ----------------------------------------------------------------------------
void CAudioStreamEngine::ShowMessage(const TDesC& aMsg, TBool aReset=EFalse)
    {
    return;
    /*
    if (aReset)     // if ETrue, clear the message on the label prior to output
        iMsg.Zero();
    iMsg.Append(aMsg);    
    TRAPD(error, iAppUi->LogInfo(aMsg));
    PanicIfError(error);*/
    }

// ----------------------------------------------------------------------------
// TPtr8& CAudioStreamEngine::GetFrame(TUint aFrameIdx)
//
// Returns a modifiable pointer to a single frame inside the audio buffer 
// ----------------------------------------------------------------------------
TPtr8& CAudioStreamEngine::GetFrame(TUint aFrameIdx)
    {
      __ASSERT_ALWAYS(aFrameIdx < iFrameCount, 
                                    User::Panic(_L("AudioStreamEx"), 1));
                                
      iFramePtr.Set((TUint8*)(iStreamBuffer->Ptr() + (aFrameIdx * iFrameSize)),
                                 iFrameSize,
                                 iFrameSize);
      return iFramePtr;
    }

// ----------------------------------------------------------------------------
// TPtr8& CAudioStreamEngine::GetPlaybackFrames(TUint aLastFrame)
//
// Returns a modifiable pointer to the requested frames inside the audio buffer
// (from the first frame to aLastFrame). 
// ----------------------------------------------------------------------------
TPtr8& CAudioStreamEngine::GetPlaybackFrames(TUint aLastFrame)
    {
    __ASSERT_ALWAYS(aLastFrame < iFrameCount, 
                                User::Panic(_L("AudioStreamEx"), 2));
                                
    iFramePtr.Set((TUint8*)(iStreamBuffer->Ptr()),
                                 (aLastFrame + 1) * iFrameSize,
                                 (aLastFrame + 1) * iFrameSize);
    return iFramePtr;
    }


//
// MMdaAudioInputStream callbacks (MMdaAudioInputStreamCallback)
//
// ----------------------------------------------------------------------------
// CAudioStreamEngine::MaiscOpenComplete(
//     TInt aError)
//
// called upon completion of CMdaAudioInputStream::Open(),
// if the stream was opened succesfully (aError==KErrNone), it's ready for use.
// upon succesful open, the first audio data block will be read from the input
// stream.
// ----------------------------------------------------------------------------
void CAudioStreamEngine::MaiscOpenComplete(TInt aError)
    {
    if (aError==KErrNone) 
        {
        // Input stream opened succesfully, set status
        iInputStatus = EOpen;
        // Set the data type (encoding)
        TRAPD(error, iInputStream->SetDataTypeL(iCurrentEncoding));
        PanicIfError(error);

        // set stream input gain to maximum
        iInputStream->SetGain(iInputStream->MaxGain()); 
        // set stream priority to normal and time sensitive
        iInputStream->SetPriority(EPriorityNormal, EMdaPriorityPreferenceTime);
        //iInputStream->SetPriority(KFullDuplexAudioPriority, (TMdaPriorityPreference)KFullDuplexAudioPrefInput);
        iAppUi->LogInfo("CAudioStreamEngine::MaiscOpenComplete Recording...");
        iAudioBuffer.Zero();
        
        // Emtpy the buffer and issue ReadL() to read the first audio data block, 
        // subsequent calls to ReadL() will be issued 
        // in MMdaAudioInputStreamCallback::MaiscBufferCopied()
        iStreamBuffer->Des().FillZ(iFrameCount * iFrameSize);
        iStreamIdx=0;
        TRAPD(error2, iInputStream->ReadL(GetFrame(iStreamIdx)));
        PanicIfError(error2);
        } 
    else 
        {
        // input stream open failed
        iInputStatus = ENotReady;
        iAppUi->LogInfo("CAudioStreamEngine::MaiscOpenComplete Recording failed!");
        }
    }

// ----------------------------------------------------------------------------
// CAudioStreamEngine::MaiscBufferCopied(
//     TInt aError, const TDesC8& aBuffer)
//
// called when a block of audio data has been read and is available at the 
// buffer reference *aBuffer.  calls to ReadL() will be issued until all blocks
// in the audio data buffer (iStreamBuffer) are filled.
// ----------------------------------------------------------------------------
void CAudioStreamEngine::MaiscBufferCopied(TInt aError, const TDesC8& /*aBuffer*/)
    {
    if (aError!=KErrNone)
        {
        _LIT(KMessage,"CAudioStreamEngine::MaiscBufferCopied Recording error: %d");
        HBufC16* message = HBufC16::NewLC(KMessage().Length()+10);
        message->Des().AppendFormat(KMessage,aError);
        ShowMessage(*message, ETrue);
        CleanupStack::PopAndDestroy(); // message
        message = NULL;
        }
    
    if (aError==KErrNone) 
        {
        // stop recording if at the end of the buffer
        /*iStreamIdx++;
        if (iStreamIdx == iFrameCount)
            {
            iAppUi->LogInfo("CAudioStreamEngine::MaiscBufferCopied Recording complete!");
            iStreamEnd = iStreamIdx - 1;
            iBufferOK = ETrue;

            // Playback is complete:
            // Start the active object that will stop the stream
            iStop->Start( TCallBack(BackgroundStop, this) );            
            return;
            }*/       
        //while(!_bFreeAccessBuffer);
        
        _bAccessBuffer=EFalse;
        // issue ReadL() for next frame             
        iAudioBuffer2Send.Append(iAudioBuffer);
        if (iAudioBuffer2Send.Length()>=350){    //350 = 500 msec / 20 * 14. It's half second of buffering
            iAppUi->SendAudioFrame(iAudioBuffer2Send);
            iAudioBuffer2Send.Zero();
        }
        TRAPD(error, iInputStream->ReadL(iAudioBuffer));
        _bAccessBuffer=ETrue;
        PanicIfError(error);
        }
    else if (aError==KErrAbort) 
        {
        // Recording was aborted, due to call to CMdaAudioInputStream::Stop()
        // This KErrAbort will occur each time the Stop() method in this class is executed.
        // Also, MaiscRecordComplete() will be called after exiting this method.
        iStreamEnd = iStreamIdx - 1;
        iBufferOK = ETrue;
        iInputStatus = ENotReady;
        }
    else 
        {
        iAppUi->LogInfo("CAudioStreamEngine::MaiscBufferCopied Error reading data from input");
        iInputStatus = ENotReady;
        }
    }

void CAudioStreamEngine::WaitAccessBuffer(){
	while(!_bAccessBuffer);
	_bFreeAccessBuffer = EFalse;
}
void CAudioStreamEngine::FreeAccessBuffer(){
	_bFreeAccessBuffer = ETrue;
}


TInt CAudioStreamEngine::BackgroundStop( TAny *aStream )  // static member function
    {
    ((CAudioStreamEngine*)aStream)->Stop();
    return EFalse;
    }


// ----------------------------------------------------------------------------
// CAudioStreamEngine::MaiscRecordComplete(
//     TInt aError)
//
// called when input stream is closed by CMdaAudioInputStream::Stop()
// ----------------------------------------------------------------------------
void CAudioStreamEngine::MaiscRecordComplete(TInt aError)
    {   
    iInputStatus = ENotReady;
    if (aError==KErrNone) 
        {
        // normal stream closure
        }
    else 
        {
        // completed with error(s)
        }
    }

void CAudioStreamEngine::Play(TDesC8& aBuffer)
{    
    if (_bCanPlay && _bCanWrite){
		
		if (aBuffer.Length()+iBuff1.Length()>iBuff1.MaxLength()){
            iAppUi->LogInfo("Lose1 %d packets audio", iBuff1.Length());
			iBuff1.Zero();
		}
		if (aBuffer.Length()+iBuff1.Length()<iBuff1.MaxLength())
			iBuff1.Append(aBuffer);
		else
            iAppUi->LogInfo("Lose2 %d packets audio", aBuffer.Length());
		if (iBuff2.Length()<=0 && iBuff1.Length()>0){
			iBuff2.Copy(iBuff1);
			iBuff1.Zero();
            iAppUi->LogInfo("CAudioStreamEngine::Play WriteL");
			TRAPD(error, iOutputStream->WriteL(iBuff2));
            if (error!=KErrNone)
                iAppUi->LogInfo("CAudioStreamEngine::Play WriteL error:%d", error);
			PanicIfError(error);            
		}
		else{
			/*int iBuf1 = iBuff1.Length();
			int iBuf2 = iBuff2.Length();
                        iAppUi->LogInfo("Lose3 %d %d", iBuf1, iBuf2);*/
		}
	}
    else
        iAppUi->LogInfo("I can't play %d %d", _bCanPlay, _bCanWrite);
}

// MMdaAudioOutputStream callbacks (MMdaAudioOutputStreamCallback)

// ----------------------------------------------------------------------------
// CAudioStreamEngine::MaoscOpenComplete(
//     TInt aError)
//
// called upon completion of CMdaAudioOutputStream::Open(),
// if the stream was opened succesfully (aError==KErrNone), it's ready for use.
// upon succesful open, the first audio data block will be written to the 
// output stream.
// ----------------------------------------------------------------------------
void CAudioStreamEngine::MaoscOpenComplete(TInt aError)
    {
    if (aError==KErrNone) 
        {
        // output stream opened succesfully, set status
        iOutputStatus = EOpen;
        // Set the data type (encoding). Should not fail, since we already
        // have tested support for this encoding in SetEncodingL with the 
        // corresponding input stream!
        TRAPD(error, iOutputStream->SetDataTypeL(iCurrentEncoding));
        PanicIfError(error);
        
        // set volume to 1/4th of stream max volume
        iOutputStream->SetVolume(iOutputStream->MaxVolume()/2);
        // set stream priority to normal and time sensitive
        iOutputStream->SetPriority(EPriorityNormal, EMdaPriorityPreferenceTime);
        //iOutputStream->SetPriority(KFullDuplexAudioPriority, (TMdaPriorityPreference)KFullDuplexAudioPrefOutput);
        iAppUi->LogInfo("CAudioStreamEngine::MaoscOpenCompleted Playing ");
        _bCanPlay=ETrue;        
		iStreamIdx = 0;
        _bWaitShortPlay=true;
        _bCanRecord = false;
        TBuf8<28> shortplay;
        unsigned char b[28];
        memset(b, 0, 28);
        shortplay.Copy(b);
        TRAP(error, iOutputStream->WriteL(shortplay));
        if (error!=KErrNone)
            iAppUi->LogInfo("CAudioStreamEngine::Play short error:%d", error);
        for(int i=0; i<10 && !_bCanRecord; i++)
            MyWait(100);
        /*if (_bCanRecord)
            Record();*/
        }    
    else 
        {
        // output stream open failed
        iOutputStatus = ENotReady;
        iAppUi->LogInfo("CAudioStreamEngine::MaoscOpenComplete Playback failed!");
        }       
    }

void CAudioStreamEngine::MyWait(int iMsec)
{
    QTime myTimer;
    myTimer.start();
    int iT1 = myTimer.elapsed();
    while (myTimer.elapsed()-iT1<iMsec)
        QApplication::processEvents();

}

// ----------------------------------------------------------------------------
// CAudioStreamEngine::MaoscBufferCopied(
//     TInt aError, const TDesC8& aBuffer)
//
// called when a block of audio data has been written to MMF. calls to WriteL() 
// will be issued until all blocks in the audio data buffer (iStreamBuffer) are 
// written.
// ----------------------------------------------------------------------------
void CAudioStreamEngine::MaoscBufferCopied(TInt aError, const TDesC8& /*aBuffer*/)
    {   
    if (aError==KErrNone) 
        {
        if (_bWaitShortPlay){
            _bWaitShortPlay=false;            
            _bCanRecord    =true;
        }
        iBuff2.Zero();
        if (iBuff1.Length()>0){
			iBuff2.Copy(iBuff1);
			iBuff1.Zero();
			TRAPD(error, iOutputStream->WriteL(iBuff2));
			PanicIfError(error);
        }
        
        /*if (iStreamIdx==iStreamEnd)
            {
            iAppUi->LogInfo("CAudioStreamEngine::MaoscBufferCopied Playback complete!"), EFalse);
            // Playback is complete:
            // Start the active object that will stop the stream
            iStop->Start( TCallBack(BackgroundStop, this) );            
            }
        else 
            {
            iStreamIdx++;
            TRAPD(error, iOutputStream->WriteL(GetFrame(iStreamIdx)));  
            PanicIfError(error);
            }*/
        }
    else if (aError==KErrAbort) 
        {
        iBuff2.Zero();
        iAppUi->LogInfo("CAudioStreamEngine::MaoscBufferCopied KErrAbort");
        // Playing was aborted, due to call to CMdaAudioOutputStream::Stop().
        // MaoscRecordComplete() will be called after exiting this method.
        iOutputStatus = ENotReady;
        }
    else 
        {
        iBuff2.Zero();
        iAppUi->LogInfo("CAudioStreamEngine::MaoscBufferCopied Error writing data to output:%d", aError);
        iOutputStatus = ENotReady;
        }
    }


// ----------------------------------------------------------------------------
// CAudioStreamEngine::MaoscPlayComplete(
//     TInt aError)
//
// called when output stream is closed by CMdaAudioOutputStream::Stop() or if 
// end of audio data has been reached, in this case KErrUnderflow will be 
// returned.
// ----------------------------------------------------------------------------
void CAudioStreamEngine::MaoscPlayComplete(TInt aError)
    {
    
    //iOutputStatus = ENotReady;
    if (aError==KErrNone) 
        {
        // normal stream closure
        iAppUi->LogInfo("CAudioStreamEngine::MaoscPlayComplete");
        }   
    else if (aError==KErrUnderflow) 
        {
        // end of audio data stream was reached because of stream underflow,
        iAppUi->LogInfo("CAudioStreamEngine::MaoscPlayComplete KErrUnderflow");
        }
    else 
        {
        // completed with error(s)
        iAppUi->LogInfo("CAudioStreamEngine::MaoscPlayComplete error:%d", aError);
        }   
    }

// END OF FILE

