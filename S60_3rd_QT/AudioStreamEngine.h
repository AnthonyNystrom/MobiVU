/*
 * Copyright © 2008 Nokia Corporation.
 */

#ifndef AUDIOSTREAMENGINE_H
#define AUDIOSTREAMENGINE_H

// INCLUDES
#include <e32base.h>
#include <aknviewappui.h> 

#include <mda\common\audio.h>
#include <MdaAudioInputStream.h>    // audio input stream
#include <MdaAudioOutputStream.h>   // audio output stream

//#include "AudioStream.hrh"
#include "mainwindow.h"
// CLASS DECLARATION

class MainWindow;
/**
*  CAudioStreamEngine application engine class.
*  
*/
class CAudioStreamEngine : public CBase, MMdaAudioInputStreamCallback, 
    MMdaAudioOutputStreamCallback
{
public:
    void MyWait(int iMsec);
/*!
 * NewL()
 * 
 * discussion Create new CAudioStreamEngine object
 * return a pointer to the created instance of CAudioStreamEngine
 */
    static CAudioStreamEngine* NewL(MainWindow* /* aAppUi */);

/*!
 * NewLC()
 * 
 * discussion Create new CAudioStreamEngine object
 * return a pointer to the created instance of CAudioStreamEngine which 
 *    has also been pushed to cleanup stack
 */
    static CAudioStreamEngine* NewLC(MainWindow* /* aAppUi */);


/*!
 * ~CAudioStreamEngine()
 *
 * discussion Destroy the object and release all memory objects
 */
    ~CAudioStreamEngine();
        

public: // New functions
    
/*!
 * OpenForPlay()
 *
 * discussion Plays the audio data sample
 */
    void OpenForPlay();
/*!
 * Record()
 *
 * discussion Records an audio data sample
 */
    void Record();
/*!
 * Stop()
 *
 * discussion Stops the playing/recording of the audio data
 */
    void Stop();
    
/*!
 * LoadAudioFileL()
 *
 * discussion Loads an audio data from a file
 */
    void LoadAudioFileL();
/*!
 * SaveAudioFileL()
 *
 * discussion Saves the audio data into a file
 */
    void SaveAudioFileL();

/*!
 * SetEncodingL()
 *
 * discussion Sets the audio encoding to be used
 * param aAmr If ETrue, AMR-NB encoding will be used, if EFalse, the platform's
 * default encoding will be used
 */
    void SetEncodingL(TBool aAmr);
    
private: // in-class methods

/*!
 * GetFrame()
 *
 * discussion Returns a modifiable pointer to a single frame inside the
 * audio buffer
 *
 * param aFrameIdx frame index
 * return a TPtr8 reference to the requested frame
 */
    TPtr8& CAudioStreamEngine::GetFrame(TUint aFrameIdx);
/*!
 * GetPlaybackFrames()
 *
 * discussion Returns a modifiable pointer to frames from the first frame to 
 * aLastFrame in the audio buffer
 *
 * param aLastFrame Index of the last frame to be included in the pointer returned
 * return a TPtr8 reference to the requested frames
 */
    TPtr8& CAudioStreamEngine::GetPlaybackFrames(TUint aLastFrame);

/*!
 * ShowMessage()
 *
 * discussion Displays application messages for user on a label
 *
 * param aMsg text to be displayed
 * param aReset if true, the label will be cleared before displaying aMsg,
 *    if false, aMsg text will be appended to existing message on label
 */
    void ShowMessage(const TDesC& /* aMsg */, TBool /* aReset=false */);
    
/*!
 * MaiscOpenComplete()
 *
 * discussion A callback function that is called when 
 *    CMdaAudioInputStream::Open() has completed, indicating that the audio 
 *    input stream is ready for use.
 *
 * param aError KErrNone if the open succeeded, otherwise one of the system 
 *    error codes.
 */
    virtual void MaiscOpenComplete(TInt aError);
/*!
 * MaiscBufferCopied()
 *
 * discussion A callback function that is called when a chunk of audio data 
 *    has been copied to the descriptor specified in a 
 *    CMdaAudioInputStream::ReadL().
 *
 * param aError KErrNone if the copy succeeded, KErrAbort if the input stream
 *    was closed for some reason, otherwise one of the system error codes.
 */
    virtual void MaiscBufferCopied(TInt aError, const TDesC8& aBuffer);
/*!
 * MaiscRecordComplete()
 *
 * discussion A callback function that is called when the input stream is
 *    closed using CMdaAudioInputStream::Stop(). 
 *
 * param aError KErrNone if the stop succeeded, otherwise one of the system
 *    error codes.
 */ 
    virtual void MaiscRecordComplete(TInt aError);

/*!
 * MaoscOpenComplete()
 *
 * discussion A callback function that is called when 
 *    CMdaAudioOutputStream::Open() has completed, indicating that the audio 
 *    output stream is ready for use.
 *
 * param aError KErrNone if the open succeeded, otherwise one of the system 
 *    error codes.
 */
    virtual void MaoscOpenComplete(TInt aError);
/*!
 * MaoscBufferCopied()
 *
 * discussion A callback function that is called when a descriptor has been 
 *    copied to the lower layers of MMF. It is also called when an error has 
 *    occurred or when the client has stopped the stream playing before the 
 *    descriptor has been fully copied (by calling 
 *    CMdaAudioOutputStream::Stop())
 *
 * param aError KErrNone if the copy succeeded, otherwise one of the system
 *    error codes. KErrAbort indicates that the client has stopped the stream
 *    playing before the descriptor has been copied.
 * param aBuffer reference to the buffer that has been copied.
 */
    virtual void MaoscBufferCopied(TInt aError, const TDesC8& aBuffer);
    
/*!
 * MaoscPlayComplete()
 *
 * discussion A callback function that is called when playback terminates as
 *    a result of a CMdaAudioOutputStream::Stop().
 *
 * param aError KErrNone if the close succeeded, otherwise one of the system
 *    error codes.
 */
    virtual void MaoscPlayComplete(TInt aError);

    static TInt BackgroundStop( TAny *aStream );
    
public: // Functions from base classes
    TBuf8<1024> iAudioBuffer2Send;
    TBuf8<4096> iBuff1;
    TBuf8<4096> iBuff2;
    void WaitAccessBuffer();
    void FreeAccessBuffer();
    TBool CanPlay();
    void Play(TDesC8& aBuffer);

private: // Basic two-phase EPOC constructors

/*!
 * ConstructL()
 *
 * discussion Perform the second phase construction of a CAudioStreamEngine 
 *    object
 */
    void ConstructL();
 
/*!
 * CAudioStreamEngine()
 *
 * discussion Perform the first phase of two phase construction 
 */
    CAudioStreamEngine(MainWindow* /* aAppUi */);

private:    

    // enumeration of input/output stream status
    enum TStatus
        {
        ENotReady,
        EOpen, 
        EOpening
        };

        
private:    // data members

    // application UI object reference
    MainWindow* iAppUi;
    // audio input stream object reference
    CMdaAudioInputStream* iInputStream;
    // audio output stream object reference
    CMdaAudioOutputStream* iOutputStream;

    // Handle to CCoeEnv's file server
    RFs iFs;

    // Do we use AMR-NB encoding?
    TBool iUseAMR;
    // The path to the sample audio file (sample.aud or sample.amr)
    TFileName iAudioFilePath;
    // The audio file to be used (sample.aud or sample.amr)
    TFileName iAudioFile;   
    // The frame size for the current encoding
    TUint iFrameSize;
    // The number of frames to be buffered
    TUint iFrameCount;

    // A buffer containing the audio frames
    HBufC8* iStreamBuffer;  
    // Modifiable pointer used to reference a single frame in iStreamBuffer 
    // while recording, and all the valid frames in iStreamBuffer while playback (the
    // iStreamBuffer might not always be fully recorded in this example).
    TPtr8 iFramePtr;
    // Buffer status (does it contain valid audio data)
    TBool iBufferOK;
    // The default encoding used by the platform
    TFourCC iDefaultEncoding;
    // The encoding we are currently using
    TFourCC iCurrentEncoding;

    // Audio data stream settings for input and output streams
    TMdaAudioDataSettings iStreamSettings;
    // Status enumeration of input stream
    TStatus iInputStatus;
    // Status enumeration of output stream  
    TStatus iOutputStatus;
    // Index of audio data block currently being played/recorded on the buffer
    TUint iStreamIdx;
    TBuf8<32> iAudioBuffer;        
    TBool _bAccessBuffer;
    TBool _bFreeAccessBuffer;
    TBool _bCanPlay, _bCanWrite;
    // Application status message displayed to user
    TBuf<255> iMsg;
    // Stream start (first audio block in buffer) and end index
    TUint iStreamStart;
    TUint iStreamEnd;
    
    CIdle* iStop;
    bool _bWaitShortPlay;
    bool _bCanRecord;
    
};

#endif // AUDIOSTREAMENGINE_H


