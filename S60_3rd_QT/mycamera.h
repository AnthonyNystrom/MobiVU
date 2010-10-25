#ifndef MYCAMERA_H
#define MYCAMERA_H
#include <ecam.h>
#include <ccmrmediarecorderobserver.h>
#include <ccmrmediasink.h>
#include <ccmrmediarecorder.h>
#include <ecam.h>
#include "mainwindow.h"
#include "sink.h"
#include "AudioStreamEngine.h"

#define VIDEO_W 176
#define VIDEO_H 144

class CDummySink;
class CCMRMediaRecorder;
class CAudioStreamEngine;
class MainWindow;

class MyCamera : public MCameraObserver, MCMRMediaRecorderObserver
{
private:
    int _iIdxCam, _iBR;
    CCMRMediaRecorder*  _iRecorder;
    CDummySink*         _iSink;
    MDataSource*        _iAudioSource;    
    CCamera *           _Camera;
    TSize _size;

    TInt GetVideoMode(TSize size);
public:
    MyCamera(MainWindow *iApp);
    void StopCamera();
    void StartCamera();
    void SetSize(int iW, int iH);
    void SetBitrate(int iBitrate);
    void SetCamera(bool bFront);

    void ReserveComplete(TInt aError);
    void PowerOnComplete(TInt aError);
    void ViewFinderFrameReady(CFbsBitmap& aFrame);
    void FrameBufferReady(MFrameBuffer* aFrameBuffer, TInt aError);
    void ImageReady(CFbsBitmap* aBitmap, HBufC8* aData, TInt aError);
    void MmroPrepareComplete(TInt aError);
    void MmroStateChange(TInt aState, TInt aError);
    void MmroTemporaryError(TInt aError);
    void MmroFatalError(TInt aError);

    MainWindow *_iAppUi;
    CAudioStreamEngine *_iAudioEngine;
};

#endif // MYCAMERA_H
