#ifndef AUDIOSTREAM_H_
#define AUDIOSTREAM_H_

#include <MdaAudioOutputStream.h>   // audio output stream
#include <mda\common\audio.h>
#include "DemoVideoCallAppUi.h"


class CDemoVideoCallAppUi;
class AudioStream : public CBase, MMdaAudioOutputStreamCallback
{
public:
	static AudioStream* NewL(CDemoVideoCallAppUi* aAppUi);
	static AudioStream* NewLC(CDemoVideoCallAppUi* aAppUi);
	AudioStream(CDemoVideoCallAppUi* aAppUi);
	virtual ~AudioStream();
	void Play(TPtrC8 BufferAudio);
	
private:
	void ConstructL(CDemoVideoCallAppUi *aAppUI);
	
	virtual void MaoscOpenComplete(TInt aError);
	virtual void MaoscBufferCopied(TInt aError, const TDesC8& aBuffer);
	virtual void MaoscPlayComplete(TInt aError);
	
	void Stop();

    // enumeration of input/output stream status
    enum TStatus
        {
        ENotReady,
        EOpen
        };
    // audio output stream object reference
    CMdaAudioOutputStream* iOutputStream;
    // The default encoding used by the platform
    TFourCC iCurrentEncoding;
    // Audio data stream settings for input and output streams
    TMdaAudioDataSettings iStreamSettings;
    CDemoVideoCallAppUi* iAppUi;
	
    // Status enumeration of output stream  
    TStatus iOutputStatus;
    TBuf8<1024> _BufferStream;
};

#endif /*AUDIOSTREAM_H_*/

