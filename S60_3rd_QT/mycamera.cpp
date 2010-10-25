#include "mycamera.h"
#include <QMessageBox>


// H.263 with profiles & levels. Exact level can be added after this string has been copied to destination
_LIT8(KCMRMimeTypeH263,                "video/H263-2000");
// Baseline Profile
_LIT8(KCMRMimeTypeH263BaselineProfile, "video/H263-2000; profile=0");
// Profile 3; Version 2 Interactive and Streaming Wireless Profile
_LIT8(KCMRMimeTypeH263Profile3,        "video/H263-2000; profile=3");
// MPEG-4 profile & level ID added after ; e.g. "video/mp4v-es; profile-level-id=8"
_LIT8(KCMRMimeTypeH263Profile0L10,        "video/H263-2000; profile=0; level=10");
_LIT8(KCMRMimeTypeH263Profile0L20,        "video/H263-2000; profile=0; level=20");
_LIT8(KCMRMimeTypeH263Profile0L30,        "video/H263-2000; profile=0; level=30");
_LIT8(KCMRMimeTypeH263Profile0L45,        "video/H263-2000; profile=0; level=45");
_LIT8(KCMRMimeTypeH263Profile0L50,        "video/H263-2000; profile=0; level=50");
_LIT8(KCMRMimeTypeH263Profile0L40,        "video/H263-2000; profile=0; level=40");

_LIT8(KCMRMimeTypeMPEG4V,              "video/mp4v-es");
_LIT8(KCMRMimeTypeMPEG4VSP,            "video/mp4v-es; profile-level-id=");
_LIT8(KCMRMimeTypeMPEG4VSPL0,          "video/mp4v-es; profile-level-id=8");
_LIT8(KCMRMimeTypeMPEG4VSPL0B,         "video/mp4v-es; profile-level-id=9");
_LIT8(KCMRMimeTypeMPEG4VSPL1,          "video/mp4v-es; profile-level-id=1");
_LIT8(KCMRMimeTypeMPEG4VSPL2,          "video/mp4v-es; profile-level-id=2");
_LIT8(KCMRMimeTypeMPEG4VSPL3,          "video/mp4v-es; profile-level-id=3");
_LIT8(KCMRMimeTypeMPEG4VSPL4,          "video/mp4v-es; profile-level-id=4");
_LIT8(KCMRMimeTypeMPEG4VSPL8,          "video/mp4v-es; profile-level-id=8");

#define BITRATE 200000

MyCamera::MyCamera(MainWindow *iApp)
{
    _iBR = BITRATE;
    _size.iHeight = VIDEO_H;
    _size.iWidth  = VIDEO_W;
    _iAppUi = iApp;
    _iIdxCam = 0;    
    _Camera = NULL;
    _iRecorder = NULL;
    _iSink     = new (ELeave) CDummySink(_iAppUi);
    _iAudioEngine = CAudioStreamEngine::NewL( _iAppUi );
    _iAudioEngine->SetEncodingL(ETrue);
    _iSink->iAudioEngine = _iAudioEngine;    
    _iAudioEngine->OpenForPlay();    
}


// From MCameraObserver
void MyCamera::ReserveComplete(TInt aError)
{
    if (aError == KErrNone)
    {   _iAppUi->LogInfo("ReserveComplete::ReserveComplete:no error");
        // Once the camera has been successfully reserved, the
        // application will power up the camera.        
        _Camera->PowerOn();
    }
    else
        _iAppUi->LogInfo("ReserveComplete error:%d", aError);
}
void MyCamera::PowerOnComplete(TInt aError)
{
    if(aError == KErrNone)
    {
        _iAppUi->LogInfo("PowerOnComplete:no error");
        // Start the viewfinder.
        _iAppUi->LogInfo("_size %dx%d", _size.iWidth, _size.iHeight);
        TSize sz(_size);
        //_Camera->StartViewFinderBitmapsL(_size);
        _iAppUi->LogInfo("StartViewFinderBitmapsL %dx%d", _size.iWidth, _size.iHeight);
        _size = sz;        
        _iRecorder = CCMRMediaRecorder::NewL();

        TInt errore;
        TMMFMessageDestinationPckg pckg;
        TMMFUidPckg uidPckg( KUidMmfAudioInput );
        _iAudioSource = MDataSource::NewSourceL( uidPckg(), KNullDesC8 );

        if (!_iIdxCam){
            TRAP(errore, _iRecorder->OpenL(this, _iAudioSource, _iSink, _Camera->Handle(), KCMRMimeTypeH263Profile0L30, TFourCC(' ','A','M','R') ));
            _iSink->iVideoCodec = CCMRMediaBuffer::EVideoH263;
        }
        else{
            TRAP(errore, _iRecorder->OpenL(this, _iAudioSource, _iSink, _Camera->Handle(), KCMRMimeTypeH263Profile0L30, TFourCC(' ','A','M','R') ));
            _iSink->iVideoCodec = CCMRMediaBuffer::EVideoH263;
        }

        int iIdxCodecSupported=-1;
        if(errore!=KErrNone)
        {
            QMessageBox msgBox;
            msgBox.setText("Camera Codec doesn't' Supported");
            msgBox.exec();
            _iAppUi->LogInfo("PowerOnComplete:Camera Codec Not Supported");
        }
        else
        {   iIdxCodecSupported=10; // index for KCMRMimeTypeMPEG4VSPL2
            _iRecorder->SetAudioEnabledL( EFalse );
            TInt iGain=_iRecorder->MaxGainL();
            _iRecorder->SetGainL(iGain);
            TMMFPrioritySettings priority;
            priority.iPriority = EPriorityNormal;
            priority.iPref = EMdaPriorityPreferenceTime;
            _iRecorder->SetAudioPriorityL( priority );
            _iSink->iAudioCodec = CCMRMediaBuffer::EAudioAMRNB;

            _iAppUi->LogInfo("Setting video capture");

            int iVideoSize = GetVideoMode(_size);
            _iAppUi->LogInfo("Index Video Size Selected%d", iVideoSize);
            TCameraInfo iInfo;
            _Camera->CameraInfo(iInfo);
            // check for supported size
            TInt errore=KErrNone;
            do{
                errore=KErrNone;
                TRAP(errore, _iRecorder->SetVideoFrameSizeL(_size));
                if(errore && iVideoSize<(iInfo.iNumVideoFrameSizesSupported-1))
                {   _iAppUi->LogInfo("%dx%d doesn't supported", _size.iWidth, _size.iHeight);
                    iVideoSize++;
                    _Camera->EnumerateVideoFrameSizes(_size, iVideoSize, CCamera::EFormatYUV420Planar);
                }
                else if(iVideoSize>iInfo.iNumVideoFrameSizesSupported-1)
                    _iAppUi->LogInfo("Video Capture not supported");
                else if(errore!=KErrNone)
                    _iAppUi->LogInfo("SetVideoSetting:SetVideoFrameSizeL failed error");
            }while(errore!=KErrNone && iVideoSize<iInfo.iNumVideoFrameSizesSupported-1);
            _iAppUi->LogInfo("Video Size W:%d H:%d Index:%d", _size.iWidth, _size.iHeight, iVideoSize);
            if(errore==KErrNone){
                TRAP(errore, _iRecorder->SetVideoBitRateL( _iBR ));
                if(errore!=KErrNone)
                    _iAppUi->LogInfo("Error to set bitrate");
                TRAP(errore, _iRecorder->SetVideoFrameRateL(TReal( 15 )));
                TCCMRVideoCodingOptions options;
                options.iMinRandomAccessPeriodInSeconds = 10;
                options.iSyncIntervalInPicture          = 1;
                options.iExpectedBitErrorRate           = 0.001;
                TRAP(errore, _iRecorder->SetVideoCodingOptionsL( options ));
                _iRecorder->PrepareL();
                _iAppUi->LogInfo("Starting video capture");
            }
            else
                _iAppUi->LogInfo("Error to test video frame size");
        }
    }
    else
        _iAppUi->LogInfo("PowerOn Failed");    
}
void MyCamera::ViewFinderFrameReady(CFbsBitmap& aFrame)
{
}
void MyCamera::ImageReady(CFbsBitmap* aBitmap, HBufC8* aData, TInt aError)
{
}

void MyCamera::FrameBufferReady(MFrameBuffer* aFrameBuffer, TInt aError) {
}
void MyCamera::MmroPrepareComplete(TInt aError)
{
    // handle errors in RunL
    if ( aError != KErrNone ){
        _iAppUi->LogInfo("MyCamera::MmroPrepareComplete error");
        return;
    }
    _iRecorder->RecordL();
    _iAppUi->LogInfo("MmroPrepareComplete");
}


void MyCamera::MmroStateChange(TInt aState, TInt aError)
{
    _iAppUi->LogInfo("MyCamera::MmroStateChange");
}


void MyCamera::MmroTemporaryError(TInt aError)
{
    _iAppUi->LogInfo("MyCamera::MmroTemporaryError");
}


void MyCamera::MmroFatalError(TInt aError)
{
    _iAppUi->LogInfo("MyCamera::MmroFatalError");
}



TInt MyCamera::GetVideoMode(TSize size)
{    
    int iVideoSize=0;
    TSize sizeRet;    
    _Camera->EnumerateVideoFrameSizes(sizeRet, iVideoSize, CCamera::EFormatYUV420Planar);
    _iAppUi->LogInfo("GetVideoMode W1:%d H1:%d W0:%d H0:%d", sizeRet.iWidth, sizeRet.iHeight, size.iWidth, size.iHeight);
    if (sizeRet!=size){
        iVideoSize++;
        _Camera->EnumerateVideoFrameSizes(sizeRet, iVideoSize, CCamera::EFormatYUV420Planar);
        _iAppUi->LogInfo("GetVideoMode W1:%d H1:%d W0:%d H0:%d", sizeRet.iWidth, sizeRet.iHeight, size.iWidth, size.iHeight);
        if (sizeRet!=size){
            iVideoSize++;
            _Camera->EnumerateVideoFrameSizes(sizeRet, iVideoSize, CCamera::EFormatYUV420Planar);
            _iAppUi->LogInfo("GetVideoMode W1:%d H1:%d W0:%d H0:%d", sizeRet.iWidth, sizeRet.iHeight, size.iWidth, size.iHeight);
            if (sizeRet!=size){
                    iVideoSize=-1;
            }
        }
    }
    if (iVideoSize<0){
        iVideoSize=0;
    }
    _iAppUi->LogInfo("GetVideoMode VideoSizeIndex:%d", iVideoSize);

    CDesC8ArrayFlat *iCodecSupported = new (ELeave) CDesC8ArrayFlat(50);
    _iRecorder->GetSupportedVideoCodecsL(*iCodecSupported);

    _iAppUi->LogInfo("----CODEC SUPPORTED----");
    for (int i=0;i<iCodecSupported->MdcaCount(); i++)
    {
        TPtrC8 pCod = iCodecSupported->MdcaPoint(i);
        TBuf8<128> tmp;
        tmp.Copy(pCod);
        _iAppUi->LogInfo((char *)tmp.Ptr());

    }
    _iAppUi->LogInfo("----END LIST CODEC----");
    delete iCodecSupported;
    return iVideoSize;
}

void MyCamera::StopCamera()
{
    _iAudioEngine->Stop();
    _iRecorder->StopL();
    delete _iRecorder;
    _iRecorder=NULL;
    _Camera->Release();
     delete _Camera;
     _Camera=NULL;
     delete _iAudioEngine;
     _iAudioEngine = NULL;
}

void MyCamera::StartCamera()
{
    if (_Camera){
        _Camera->Release();
        delete _Camera;
     }
    _Camera  = CCamera::NewL(*this, _iIdxCam);
    _Camera->Reserve();
}

void MyCamera::SetSize(int iW, int iH)
{
    _size.iHeight = iH;
    _size.iWidth  = iW;
}


void MyCamera::SetBitrate(int iBitrate){
    _iBR = iBitrate;
    if (_iRecorder!=NULL)
        _iRecorder->SetVideoBitRateL( _iBR );
}

void MyCamera::SetCamera(bool bFront){
    if(bFront)
        _iIdxCam=1;
    else
        _iIdxCam=0;
}
