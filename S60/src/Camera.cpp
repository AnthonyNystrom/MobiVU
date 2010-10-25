
#include <eikenv.h>				// CEikonEnv

#include "Camera.h"
#include <bitdev.h>
#include <aknquerydialog.h> 
#include <aknnotewrappers.h>
#include <mmffourcc.h>
#include <MmfBase.h>
#include <mmfcontrollerframework.h> 
#include <mmfcontrollerframeworkbase.h>
#include <mmfdatasource.h>
#include <ECam.h> 

#define MMSYSTEM

const TInt KMRTestDefaultBitrate = 64000;
const TInt32 KCMRFourCCIdAMRNB = KMMFFourCCCodeAMR;
// Four CC for QCELP13 (' ','Q','1','3')
const TInt32 KCMRFourCCIdQCELP13 = 858870048;
// Four CC for MPEG-4 AAC (' ','A','A','C')
const TInt32 KCMRFourCCIdMPEG4AAC = KMMFFourCCCodeAAC;


// Mime types for video

// H.263 with profiles & levels. Exact level can be added after this string has been copied to destination
_LIT8(KCMRMimeTypeH263,                "video/H263-2000");
// Baseline Profile
_LIT8(KCMRMimeTypeH263BaselineProfile, "video/H263-2000; profile=0");
// Profile 3; Version 2 Interactive and Streaming Wireless Profile
_LIT8(KCMRMimeTypeH263Profile3,        "video/H263-2000; profile=3");
// MPEG-4 profile & level ID added after ; e.g. "video/mp4v-es; profile-level-id=8"
_LIT8(KCMRMimeTypeMPEG4V,              "video/mp4v-es");
_LIT8(KCMRMimeTypeMPEG4VSP,            "video/mp4v-es; profile-level-id=");
_LIT8(KCMRMimeTypeMPEG4VSPL0,          "video/mp4v-es; profile-level-id=8");
_LIT8(KCMRMimeTypeMPEG4VSPL0B,         "video/mp4v-es; profile-level-id=9");
_LIT8(KCMRMimeTypeMPEG4VSPL1,          "video/mp4v-es; profile-level-id=1");
_LIT8(KCMRMimeTypeMPEG4VSPL2,          "video/mp4v-es; profile-level-id=2");
_LIT8(KCMRMimeTypeMPEG4VSPL3,          "video/mp4v-es; profile-level-id=3");
_LIT8(KCMRMimeTypeMPEG4VSPL4,          "video/mp4v-es; profile-level-id=4");
_LIT8(KCMRMimeTypeMPEG4VSPL8,          "video/mp4v-es; profile-level-id=8");
										

static const TInt KN2FQualities = 10;
static const TInt KN2FJPEGQuality[] = { 10, 20, 30, 40, 50, 60, 70, 80, 90, 100 };

_LIT(KNoSup, " Not Supported");
_LIT(KHeader, "Next2Friends");
TBuf<50> bufHeader(KHeader);
TPtrC8 ptrHeader((TUint8*)bufHeader.Ptr(), bufHeader.Size());
TInt ShowQueryDialogL(const TDesC& aTitle, TDes& aBuffer);

TInt ShowQueryDialogL(const TDesC& aTitle, TDes& aBuffer)
{
	/*CAknTextQueryDialog* Dialog =
	CAknTextQueryDialog::NewL(aBuffer,CAknQueryDialog::ENoTone);
	Dialog->PrepareLC(R_ASK_NAME_DIALOG);
	Dialog->SetPromptL(aTitle);
	return Dialog->RunLD();*/
	return 0;
}


CCamCom* CCamCom::NewL()
{
	CCamCom* self = NewLC();
	CleanupStack::Pop();
	return self;
}

CCamCom* CCamCom::NewLC()
{
	CCamCom* self = new (ELeave) CCamCom();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CCamCom::CCamCom()
:CActive(CActive::EPriorityStandard),
	iVideoSizeIndex(0),
	iIsPoweredOn(EFalse),
	iIsCapture(EFalse),
	iConnected(EFalse),
	iInfo(),
	iZoomFactor(0),
	iFrameSize(0),
	iPckgdFrameSize(iFrameSize),
	iSentBytesCount(0),
	m_szSizeArea(0,0,0,0),
	
	iFrameBmp(NULL),
	iEncodedFrame2Send(NULL),
	iCrtSendBufferPtr(NULL, 0),
	iFlash(0),iBrightness(0),iContrast(0),iWB(0),iExp(0),iIdxCam(0),iVideoSize(0)
	{
	CActiveScheduler::Add(this);
	}

void CCamCom::ConstructL()
{
	iAppUi = static_cast<CDemoVideoCallAppUi*> (CEikonEnv::Static()->EikAppUi());
	if (CCamera::CamerasAvailable()==1)
		iIdxCam=0;
	else
		iIdxCam=1;	
	if (iIdxCam)
		iAppUi->LogInfo(_L("Front Camera Used"));
	else
		iAppUi->LogInfo(_L("Rear Camera Used"));
	iCamera = CCamera::NewL(*this, iIdxCam);
	iOpened = EFalse;
	iQuality=5;
	iJPEGQuality = KN2FJPEGQuality[iQuality];
	_iBitrate = 100000;
	iIdxCodecSupported = -1;
    _szVideo.iWidth=176;
    _szVideo.iHeight=144;
	
    TRAPD( err, iLight = CHWRMLight::NewL() );
    if( err == KErrNotSupported )
    	iLight=NULL;
    iRecorder = NULL;
    iSink     = new (ELeave) CDummySink;
    iTimer    = CTimeOutTimer::NewL(CActive::EPriorityUserInput, *this);
    iIsPoweredOn = bPowerCameraInProgress = EFalse;
    
	iAudioEngine = CAudioStreamEngine::NewL( iAppUi ); 
	iAudioEngine->SetEncodingL(ETrue);
	iSink->iAudioEngine = iAudioEngine;	
}


CCamCom::~CCamCom()
{
	if(IsActive())
		Cancel();
    delete iTimer;
    iTimer = NULL;
	
	if(iCamera)
	{
		StopCamera();
		delete iCamera;
		iCamera = NULL;
	}
	
	if(iFrameBmp != NULL)
	{
		delete iFrameBmp;
		iFrameBmp = NULL;
	}
	if(iEncodedFrame2Send != NULL)
	{
		iCrtSendBufferPtr.Set(NULL, 0, 0);
		delete iEncodedFrame2Send;
		iEncodedFrame2Send = NULL;
	}
	if (iRecorder) delete iRecorder;
	if (iSink)     delete iSink;
	if (iAudioEngine)
		delete iAudioEngine;
}


void CCamCom::DoCancel()
{
	iTimer->Cancel();
}
void CCamCom::RunL()
{
}



// From MCameraObserver:

void CCamCom::ReserveComplete(TInt aError)
{
	if(aError == KErrNone)
	{
		// Once the camera has been successfully reserved, the
		// application will power up the camera.
		bPowerCameraInProgress=ETrue;
		iCamera->PowerOn();
	}
}
// restart counter timer for standby camera
void CCamCom::RestartTimeout()
{
	return; // disabled standby func. I need to test. doesn't work
	iTimer->Cancel();
	iTimer->After(60000000);
	SetActive();
}
void CCamCom::TimerExpired()
{
	iTimer->Cancel();
	StopCamera();
}
TBool CCamCom::IsStandby()
{
		if (bPowerCameraInProgress)
			return EFalse;
		else
			return !iIsPoweredOn;
}
void CCamCom::PowerOnComplete(TInt aError)
{
	if(aError == KErrNone)
	{
		iAppUi->LogInfo(_L("PowerOnComplete:no error"));

		TSize size=m_szSizeArea.Size();
		switch (iResolution)
		{
		case 0:break;
		case 1:
			size.iHeight=(size.iHeight*3)/4;
			size.iWidth=(size.iWidth*3)/4;
			break;
		case 2:
			size.iHeight=size.iHeight/2;
			size.iWidth=size.iWidth/2;
			break;
		case 3:
			size.iHeight=size.iHeight/4;
			size.iWidth=size.iWidth/4;
			break;
		}

		// Start the viewfinder.
		iAppUi->LogInfo(_L("PowerOnComplete:Start the viewfinder %dx%d"),size.iWidth,size.iHeight);
	    iCamera->StartViewFinderBitmapsL(size);
	    iAppUi->LogInfo(_L("PowerOnComplete:CameraInfo"));
	    iCamera->CameraInfo(iInfo); 
	    iAppUi->LogInfo(_L("PowerOnComplete:SetFeatures"));
	    SetFeatures();
	    iAppUi->LogInfo(_L("PowerOnComplete:iLight"));
		if (iLight)
		{	iLight->ReserveLightL(CHWRMLight::EPrimaryDisplay);
			iLight->LightOnL(CHWRMLight::EPrimaryDisplay);
		}
		
		// INIT VIDEO CAPTURE SYSTEM
	    
		if (iRecorder==NULL){
			iAppUi->LogInfo(_L("PowerOnComplete:Init media recorder"));
			iRecorder = CCMRMediaRecorder::NewL();    
	    	
			TInt errore;
			TMMFMessageDestinationPckg pckg;
			TMMFUidPckg uidPckg( KUidMmfAudioInput );
			// Create the source
			iAudioSource = MDataSource::NewSourceL( uidPckg(), KNullDesC8 );
			
			if (!iIdxCam){
				TRAP(errore, iRecorder->OpenL(this, iAudioSource, iSink, iCamera->Handle(), KCMRMimeTypeMPEG4VSPL2, TFourCC(' ','A','M','R') ));
				iSink->iVideoCodec = CCMRMediaBuffer::EVideoMPEG4;
			}
			else{
				TRAP(errore, iRecorder->OpenL(this, iAudioSource, iSink, iCamera->Handle(), KCMRMimeTypeH263, TFourCC(' ','A','M','R') ));
				iSink->iVideoCodec = CCMRMediaBuffer::EVideoH263;
			}
			
	    	iIdxCodecSupported=-1;
	    	if(errore!=KErrNone)
	    	{	TBuf<50> msg(_L("Camera Codec Not Supported"));
			    ShowQueryDialogL(KHeader, msg);
			    iAppUi->LogInfo(_L("PowerOnComplete:Camera Codec Not Supported"));
	    	}
	    	else
	    	{	iIdxCodecSupported=10; // index for KCMRMimeTypeMPEG4VSPL2

	    		iRecorder->SetAudioEnabledL( EFalse );
	    		TInt iGain=iRecorder->MaxGainL();
	    		/*for audio off
	    			iGain=0;*/
			    iRecorder->SetGainL(iGain);
			    TMMFPrioritySettings priority;
			    priority.iPriority = 93;
			    priority.iPref = EMdaPriorityPreferenceTimeAndQuality;
			    iRecorder->SetAudioPriorityL( priority );
		    	iSink->iAudioCodec = CCMRMediaBuffer::EAudioAMRNB;
		    	//iSink->iAudioCodec = CCMRMediaBuffer::EAudioMPEG4AAC;
		    }
		}
	}
	iAppUi->LogInfo(_L("PowerOnComplete:END"));
	bPowerCameraInProgress=EFalse;
	RestartTimeout();
    iIsPoweredOn=ETrue;

    iAppUi->LogInfo(_L("PowerOnComplete:START VIDEO CAPTURE"));	
    if (GetVideoSize(_iBitrate, _szVideo))
    {	//SetVideoSize(iVideoSize);
    	StartCapture(ETrue);    
    }
}
void CCamCom::ViewFinderFrameReady(CFbsBitmap& aFrame)
{
#ifdef JPG_ENCODE	
	//HandleFrame(aFrame);
#else
	if (iIsPoweredOn){
		iAppUi->DisplayFrame(&aFrame);
		//iAppUi->LogInfo(_L("ViewFinderFrameReady:push frame"));
	}
	else
		iAppUi->DisplayFrame(NULL);
			
	iCounterFrames++;
#endif	
}
void CCamCom::ImageReady(CFbsBitmap* aBitmap, HBufC8* aData, TInt aError)
{
}

void CCamCom::FrameBufferReady(MFrameBuffer* aFrameBuffer, TInt aError) {
}

// from MSmartEncodeObserver:
void CCamCom::ImageEncoded()
{
	if(iFrameBmp != NULL) // finished encoding the primary frame bmp
	{	iFrameBmp->Reset();
		delete iFrameBmp;
		iFrameBmp = NULL;		
		iCrtSendBufferPtr.Set(NULL, 0, 0);
		iEncodedFrame2Send = NULL;
	}
}

TInt CCamCom::GetNearestSizeIndexAvailable(CCamera::TFormat desiredCaptureFormat, TSize desiredCaptureSize)
{
	TCameraInfo camInfo;
	iCamera->CameraInfo(camInfo);
	// check to see whether the desired format is supported by the native camera
	if(!(camInfo.iImageFormatsSupported & desiredCaptureFormat))
	{	iAppUi->LogInfo(_L("GetNearestSizeIndexAvailable:format not supported"));
		return -1;
	}
	TUint32 desiredResolution = desiredCaptureSize.iWidth * desiredCaptureSize.iHeight;
	TInt bestIndex = -1;
	TInt bestDiff = KMaxTInt;
	iAppUi->LogInfo(_L("GetNearestSizeIndexAvailable:enumeration format"));
	for(TInt i = 0; i < camInfo.iNumImageSizesSupported; i++)
	{
		TSize crtCaptureSize;
		iCamera->EnumerateCaptureSizes(crtCaptureSize, i, desiredCaptureFormat);
		iAppUi->LogInfo(_L("GetNearestSizeIndexAvailable:format->%dx%d"), crtCaptureSize.iWidth, crtCaptureSize.iHeight);
		// if we found an exact match, return the corresponding index
		if(crtCaptureSize == desiredCaptureSize)
			return i;
		// otherwise compare the difference with the best match found so far
		TInt crtDiff = Abs(crtCaptureSize.iWidth * crtCaptureSize.iHeight - desiredResolution);
		if(crtDiff < bestDiff)
		{
			bestDiff = crtDiff;
			bestIndex = i;
		}
	}
	// return the best match
	return bestIndex;
}
void CCamCom::SetSizeRect(TRect sz)
{
	m_szSizeArea = sz;
}

TInt CCamCom::SetZoomFactorL(TBool aEnable) 
{   
    TInt bitmapCount = ECameraZoomLimit - ECameraZoom2Uid;  
    //both 0 and 1 indicate that zoom functionality is not supported      
    if ( iInfo.iMaxZoomFactor != 0 && iInfo.iMaxZoomFactor != 1 )
        {
        if ( aEnable && iZoomFactor < iInfo.iMaxZoom )
            {
            iZoomFactor++;
            }
        if ( !aEnable && iZoomFactor > iInfo.iMinZoom )
            {
            iZoomFactor--;
            }
        iCamera->SetZoomFactorL( iZoomFactor );
        //Zoom ind. bitmap offset 
        return ( iInfo.iMaxZoom > bitmapCount )?KErrNotFound:iZoomFactor-1;
        }
    if ( iInfo.iMaxDigitalZoomFactor != 0 && iInfo.iMaxDigitalZoomFactor != 1 )
        {
        if ( aEnable && iZoomFactor < iInfo.iMaxDigitalZoom )
            {
            iZoomFactor++;
            }
        if ( !aEnable && iZoomFactor > 0 )
            {
            iZoomFactor--;
            }
        iCamera->SetDigitalZoomFactorL(iZoomFactor);
        // iCapturePrepared = EFalse;
        //Zoom ind. bitmap offset 
        return (iInfo.iMaxDigitalZoom>bitmapCount)?KErrNotFound:iZoomFactor-1;
        }

    return KErrNotFound; 
}
void CCamCom::StartCamera()
{
	if (bPowerCameraInProgress || iIsPoweredOn){
		iAppUi->LogInfo(_L("CCamCom::StartCamera() I can't power camera"));
		return;
	}		
	iAppUi->LogInfo(_L("CCamCom::StartCamera()"));
	iCamera->Reserve();
}
void CCamCom::StopCamera()
{	
	if (iCamera!=NULL && iIsPoweredOn)
	{	iIsPoweredOn=EFalse;
		if (iRecorder) 
		{	delete iRecorder;
			iRecorder=NULL;
		}
		//iCamera->StopViewFinder();
		//iCamera->PowerOff();
		iCamera->Release();
		if (iLight)
			iLight->ReleaseLight( CHWRMLight::EPrimaryDisplay );
		iAppUi->DisplayFrame(NULL);
	}
	iIsPoweredOn=EFalse;
}
void CCamCom::SetSetting(signed int &iSetFlash, signed int &iSetBrightness, signed int &iSetContrast, signed int &iSetWB, signed int &iSetExp, signed int &iSetIdxCam, signed int &aVideoSize)
{
	iFlash      = iSetFlash;
	iBrightness = iSetBrightness;
	iContrast   = iSetContrast;
	iWB         = iSetWB;
	iExp        = iSetExp;
	iVideoSize  = aVideoSize;
	//iQuality    = iSetQuality;
	
	TInt iNCams = CCamera::CamerasAvailable();
	if (iSetIdxCam+1<=iNCams)
	{	if (iIdxCam != iSetIdxCam)
		{	iIdxCam = iSetIdxCam;
			if(iCamera)
			{	StopCamera();
				delete iCamera;
			}
			iCamera = CCamera::NewL(*this, iIdxCam);
			StartCamera();
		}
	}
	else
	{	TBuf<40> msg(_L("Camera selected not found"));
	    ShowQueryDialogL(KHeader, msg);
	}
	SetVideoSize(iVideoSize);
}
void CCamCom::SetFeatures()
{	
	CAknInformationNote* iInfoMsg=new (ELeave) CAknInformationNote;
	
	TInt iFlashSet=CCamera::EFlashNone;
	
	// set flash
	switch(iFlash)
	{
	case 0:iFlashSet=CCamera::EFlashNone; break;
	case 1:iFlashSet=CCamera::EFlashAuto; break;
	case 2:iFlashSet=CCamera::EFlashForced; break;
	case 3:iFlashSet=CCamera::EFlashFillIn; break;
	case 4:iFlashSet=CCamera::EFlashRedEyeReduce; break;
	case 5:iFlashSet=CCamera::EFlashSlowFrontSync; break;
	case 6:iFlashSet=CCamera::EFlashSlowRearSync; break;
	case 7:iFlashSet=CCamera::EFlashManual; break;
	}
	if (iInfo.iFlashModesSupported & iFlashSet )
		iCamera->SetFlashL((CCamera::TFlash)iFlash);
	else if (iFlashSet)
	{  /*TBuf<40> msg(_L("Flash setting type not supported"));
	   ShowQueryDialogL(KHeader, msg);*/
		//iInfoMsg->ExecuteLD(_L("Flash setting type not supported"));
	}
	
	TInt iWBSet=CCamera::EWBAuto;
	switch (iWB)
	{
	case 0:iWBSet=CCamera::EWBAuto; break;
	case 1:iWBSet=CCamera::EWBDaylight; break;
	case 2:iWBSet=CCamera::EWBCloudy; break;
	case 3:iWBSet=CCamera::EWBTungsten; break;
	case 4:iWBSet=CCamera::EWBFluorescent; break;
	case 5:iWBSet=CCamera::EWBFlash; break;
	case 6:iWBSet=CCamera::EWBSnow; break;
	case 7:iWBSet=CCamera::EWBBeach; break;
	case 8:iWBSet=CCamera::EWBManual; break;
	//case 9:iWBSet=CCamera::EWBShade; break;
	}
	if (iInfo.iWhiteBalanceModesSupported & iWBSet )
		iCamera->SetWhiteBalanceL((CCamera::TWhiteBalance)iWBSet);
	else if (iWBSet)
	{  //iInfoMsg->ExecuteLD(_L("White Balance setting type not supported"));
	}
	
	TInt iExpSet=CCamera::EExposureAuto;
	switch (iExp)
	{
	case 0:iExpSet=CCamera::EExposureAuto; break;
	case 1:iExpSet=CCamera::EExposureNight; break;
	case 2:iExpSet=CCamera::EExposureBacklight; break;
	case 3:iExpSet=CCamera::EExposureCenter; break;
	case 4:iExpSet=CCamera::EExposureSport; break;
	case 5:iExpSet=CCamera::EExposureVeryLong; break;
	case 6:iExpSet=CCamera::EExposureSnow; break;
	case 7:iExpSet=CCamera::EExposureBeach; break;
	case 8:iExpSet=CCamera::EExposureProgram; break;
	case 9:iExpSet=CCamera::EExposureAperturePriority; break;
	case 10:iExpSet=CCamera::EExposureShutterPriority; break;
	case 11:iExpSet=CCamera::EExposureManual; break;
	case 12:iExpSet=CCamera::EExposureSuperNight; break;
	case 13:iExpSet=CCamera::EExposureInfra; break;
	}
	if (iInfo.iExposureModesSupported & iExpSet )
		iCamera->SetExposureL((CCamera::TExposure)iExpSet);
	else if (iExpSet)
	{  
		//iInfoMsg->ExecuteLD(_L("Exposure setting type not supported"));
	}
	
	if (iInfo.iOptionsSupported & TCameraInfo::EBrightnessSupported)
		iCamera->SetBrightnessL(iBrightness);
	if (iInfo.iOptionsSupported & TCameraInfo::EContrastSupported)
		iCamera->SetContrastL(iContrast);
	iCamera->SetJpegQuality(iQuality);
	SetVideoSize(iVideoSize);
}
TBool CCamCom::HasCamera() const {
	return iCamera != NULL;
}

TBool CCamCom::EnumerateVideoSizes(TInt& aCounter, TSize& aSize) {
	TCameraInfo info;
	iCamera->CameraInfo(info);
	if (aCounter < 0 || aCounter >= info.iNumVideoFrameSizesSupported)
		return EFalse;
	TInt index = aCounter;
	iCamera->EnumerateVideoFrameSizes(aSize, index, CCamera::EFormatYUV420Planar);
	aCounter++;
	return ETrue;
}

TBool CCamCom::GetVideoSize(TInt iBitRate, TSize& aSize)
{
	iAppUi->LogInfo(_L("GetVideoSize:iBitRate:%d size:%dx%d"), iBitRate, aSize.iWidth, aSize.iHeight);
	TBool bRet = SetVideoSetting(iBitRate, aSize);
	if (bRet){
		
		TCCMRVideoCodingOptions options;
		options.iMinRandomAccessPeriodInSeconds = 10;
		options.iSyncIntervalInPicture          = 1;
		options.iExpectedBitErrorRate           = 0.001;
		TInt errore;
		TRAP(errore, iRecorder->SetVideoCodingOptionsL( options ));
		if(errore!=KErrNone){
		iAppUi->LogInfo(_L("GetVideoSize:error to SetVideoCodingOptionsL:%d"), errore);
			bRet=EFalse;
		}
		else
			iAppUi->LogInfo(_L("GetVideoSize:completed OK"));
	}
	//iCamera->EnumerateVideoFrameSizes(aSize, iVideoSize, CCamera::EFormatYUV420Planar);
	return bRet;
}


void CCamCom::SetVideoSize(TInt aSizeIndex) {
	if (!iCamera)
		return;
	
    GetVideoMode(_szVideo);
	
	TCameraInfo info;
	iCamera->CameraInfo(info);
	
	if (aSizeIndex < 0 || aSizeIndex >= info.iNumVideoFrameSizesSupported)
		return;
	
	iAppUi->LogInfo(_L("SetVideoSize:Index format using : %d"), aSizeIndex);
	
	// force to low res
	//aSizeIndex = info.iNumVideoFrameSizesSupported-1;
	
	iVideoSizeIndex = aSizeIndex;
	
	if (!iIsPoweredOn)
		return;
	
	CCamera::TFormat format = CCamera::EFormatYUV420Planar;
	if (iIsCapture) {
		StartCapture(EFalse);
		TRAPD(err, iCamera->PrepareVideoCaptureL(format, iVideoSizeIndex, 0, 2, 1));
		StartCapture(ETrue);
	} else {
		TRAPD(err, iCamera->PrepareVideoCaptureL(format, iVideoSizeIndex, 0, 2, 1));
	}
	
	iOpened=EFalse; // reinit encoder

}

void CCamCom::PowerCamera()
{
	if (!bPowerCameraInProgress && !iIsPoweredOn)
	{	bPowerCameraInProgress=ETrue;
		StartCamera();
	}
}

TBool CCamCom::StartCapture(TBool aCapture)
{
	if (iIsCapture == aCapture)
		return ETrue;
	
	if (!iIsPoweredOn)
	{	iCamera->PowerOn();
		return false;
	}

	TBool bRet=EFalse;
		
	if (iIdxCodecSupported>=0){
		if (aCapture)
	    {	iRecorder->PrepareL();
	    	iAppUi->LogInfo(_L("CCamCom::StartCapture iRecorder->PrepareL()"));
			bRet=ETrue;
	    }
		else
		{	iIsCapture = EFalse;
			iRecorder->StopL();
			iAppUi->LogInfo(_L("CCamCom::StartCapture iRecorder->StopL()"));
		}
	}
	else{
		if (aCapture)
		{
			iCounterFrames=0;
			iAppUi->LogInfo(_L("CCamCom::StartCapture try StartVideoCapture() "));
			TRAPD(err, iCamera->StartVideoCapture());
			iAppUi->LogInfo(_L("CCamCom::StartCapture OK StartVideoCapture() "));
		}
		else
		{	
			iCamera->StopVideoCapture();
			iAppUi->LogInfo(_L("CCamCom::StartCapture StopVideoCapture() "));
		}
		iIsCapture = aCapture;
	}
	return bRet;
}



// Return string parameter sett
void CCamCom::ChangeFlashParameter(TBool bUp, TBuf<100> &aMsg)
{
	TInt iNewVal = iFlash;
	
	if (bUp)
		iNewVal++;
	else
		iNewVal--;
	if (iNewVal<0)
		iNewVal=0;
	else if(iNewVal>9)
		iNewVal=9;
	iFlash = iNewVal;
	TInt iFlashSet=GetFlashParameterString(aMsg);	
	if (iInfo.iFlashModesSupported & iFlashSet )
		iCamera->SetFlashL((CCamera::TFlash)iFlash);
	return;
}


TInt CCamCom::GetFlashParameterString( TBuf<100> &aMsg)
{
	TInt iFlashSet=CCamera::EFlashNone;
	
	// set flash
	switch(iFlash)
	{
	case 0:iFlashSet=CCamera::EFlashNone;aMsg.Copy(_L("None"));  break;
	case 1:iFlashSet=CCamera::EFlashAuto; aMsg.Copy(_L("Auto")); break;
	case 2:iFlashSet=CCamera::EFlashForced; aMsg.Copy(_L("Forced")); break;
	case 3:iFlashSet=CCamera::EFlashFillIn; aMsg.Copy(_L("Fill In")); break;
	case 4:iFlashSet=CCamera::EFlashRedEyeReduce; aMsg.Copy(_L("Red Eye Reduce")); break;
	case 5:iFlashSet=CCamera::EFlashSlowFrontSync; aMsg.Copy(_L("Slow Front Sync")); break;
	case 6:iFlashSet=CCamera::EFlashSlowRearSync; aMsg.Copy(_L("Slow Rear Sync"));  break;
	case 7:iFlashSet=CCamera::EFlashManual;aMsg.Copy(_L("Manual"));  break;
	}
	if (!(iInfo.iFlashModesSupported & iFlashSet))
		aMsg.Append(KNoSup); 
	
	return iFlashSet;
}	

// Return string parameter sett
TBool CCamCom::ChangeWhiteBParameter(TBool bUp, TBuf<100> &aMsg)
{
	TInt iNewVal = iWB;
	TBool bret=EFalse;
	
	if (bUp)
		iNewVal++;
	else
		iNewVal--;
	if (iNewVal<0)
		iNewVal=0;
	else if(iNewVal>13)
		iNewVal=13;
	TBuf<100> tmp;
	TInt iv;
	if (bUp){
		//search first item supported
		while(!WhiteBParameterSupported(iNewVal, iv) && iNewVal<14)
			iNewVal++;
		if (!WhiteBParameterSupported(iNewVal, iv))
			return bret; // no item found
	}
	else
	{
		while(!WhiteBParameterSupported(iNewVal, iv) && iNewVal>=0)
			iNewVal--;
		if (!WhiteBParameterSupported(iNewVal, iv))
			return bret; // no item found
	}
	iWB = iNewVal;
	iCamera->SetWhiteBalanceL((CCamera::TWhiteBalance)iv);
	GetWhiteBParameterString(aMsg);

	bret=ETrue;
	return bret;
}
void CCamCom::GetWhiteBParameterString(TBuf<100> &aMsg)
{
	switch (iWB)
	{
	case 0:aMsg.Copy(_L("Auto")); break;
	case 1:aMsg.Copy(_L("Daylight")); break;
	case 2:aMsg.Copy(_L("Cloudy")); break;
	case 3:aMsg.Copy(_L("Tungsten")); break;
	case 4:aMsg.Copy(_L("Fluorescent")); break;
	case 5:aMsg.Copy(_L("Flash")); break;
	case 6:aMsg.Copy(_L("Snow")); break;
	case 7:aMsg.Copy(_L("Beach")); break;
	case 8:aMsg.Copy(_L("Manual")); break;
	case 9:aMsg.Copy(_L("Shade")); break;
	}
}

TBool CCamCom::WhiteBParameterSupported(TInt iVal, TInt &iWBSet)
{
	switch (iVal)
	{
	case 0:iWBSet=CCamera::EWBAuto; break;
	case 1:iWBSet=CCamera::EWBDaylight; break;
	case 2:iWBSet=CCamera::EWBCloudy; break;
	case 3:iWBSet=CCamera::EWBTungsten;break;
	case 4:iWBSet=CCamera::EWBFluorescent;break;
	case 5:iWBSet=CCamera::EWBFlash;break;
	case 6:iWBSet=CCamera::EWBSnow; break;
	case 7:iWBSet=CCamera::EWBBeach;break;
	case 8:iWBSet=CCamera::EWBManual;break;
//	case 9:iWBSet=CCamera::EWBShade; break;
	}	
	if (!(iInfo.iWhiteBalanceModesSupported & iWBSet))
		return EFalse;
	return ETrue;
}


// Return string parameter sett
TBool CCamCom::ChangeExpParameter(TBool bUp, TBuf<100> &aMsg)
{
	TInt iNewVal = iExp;
	TBool bret=EFalse;
	
	if (bUp)
		iNewVal++;
	else
		iNewVal--;
	if (iNewVal<0)
		iNewVal=0;
	else if(iNewVal>13)
		iNewVal=13;
	TBuf<100> tmp;
	TInt iv;
	if (bUp){
		//search first item supported
		while(!ExpParameterSupported(iNewVal, iv) && iNewVal<14)
			iNewVal++;
		if (!ExpParameterSupported(iNewVal, iv))
			return bret; // no item found
	}
	else
	{
		while(!ExpParameterSupported(iNewVal, iv) && iNewVal>=0)
			iNewVal--;
		if (!ExpParameterSupported(iNewVal, iv))
			return bret; // no item found
	}
	iExp = iNewVal;
	iCamera->SetExposureL((CCamera::TExposure)iv);
	GetExpParameterString(aMsg);

	bret=ETrue;
	return bret;
}
void CCamCom::GetExpParameterString(TBuf<100> &aMsg)
{
	switch (iExp)
	{
	case 0:aMsg.Copy(_L("Auto"));break;
	case 1:aMsg.Copy(_L("Night"));break;
	case 2:aMsg.Copy(_L("Backlight"));break;
	case 3:aMsg.Copy(_L("Center"));break;
	case 4:aMsg.Copy(_L("Sport"));break;
	case 5:aMsg.Copy(_L("Very Long"));break;
	case 6:aMsg.Copy(_L("Snow"));break;
	case 7:aMsg.Copy(_L("Beach"));break;
	case 8:aMsg.Copy(_L("Program"));break;
	case 9:aMsg.Copy(_L("Aperture Priority"));break;
	case 10:aMsg.Copy(_L("Shutter Priority"));break;
	case 11:aMsg.Copy(_L("Manual"));break;
	case 12:aMsg.Copy(_L("Super Night"));break;
	case 13:aMsg.Copy(_L("Infra"));break;
	}
}

TBool CCamCom::ExpParameterSupported(TInt iVal, TInt &iExpSet)
{
	switch (iVal)
	{
	case 0:iExpSet=CCamera::EExposureAuto;break;
	case 1:iExpSet=CCamera::EExposureNight;break;
	case 2:iExpSet=CCamera::EExposureBacklight;break;
	case 3:iExpSet=CCamera::EExposureCenter;break;
	case 4:iExpSet=CCamera::EExposureSport; break;
	case 5:iExpSet=CCamera::EExposureVeryLong; break;
	case 6:iExpSet=CCamera::EExposureSnow; break;
	case 7:iExpSet=CCamera::EExposureBeach; break;
	case 8:iExpSet=CCamera::EExposureProgram; break;
	case 9:iExpSet=CCamera::EExposureAperturePriority; break;
	case 10:iExpSet=CCamera::EExposureShutterPriority; break;
	case 11:iExpSet=CCamera::EExposureManual; break;
	case 12:iExpSet=CCamera::EExposureSuperNight; break;
	case 13:iExpSet=CCamera::EExposureInfra; break;
	}
	if (!(iInfo.iExposureModesSupported & iExpSet))
		return EFalse;
	return ETrue;
}

void CCamCom::ChangeBrightnessParameter(TBool bUp, TBuf<100> &aMsg)
{
	TInt iNewVal = iBrightness;
	
	if (bUp)
		iNewVal+=3;
	else
		iNewVal-=3;
	if (iNewVal<-100)
		iNewVal=-100;
	else if(iNewVal>100)
		iNewVal=100;
	iBrightness = iNewVal;
	
	GetBrightnessValue(aMsg);
		
	if (iInfo.iOptionsSupported & TCameraInfo::EBrightnessSupported)
		iCamera->SetBrightnessL(iBrightness);
	return;
}
TInt CCamCom::GetBrightnessValue(TBuf<100> &aMsg)
{
	if (!(iInfo.iOptionsSupported & TCameraInfo::EBrightnessSupported))
		aMsg.Append(KNoSup); 
	return iBrightness;
}


void CCamCom::ChangeContrastParameter(TBool bUp, TBuf<100> &aMsg)
{
	TInt iNewVal = iContrast;
	
	if (bUp)
		iNewVal+=3;
	else
		iNewVal-=3;
	if (iNewVal<-100)
		iNewVal=-100;
	else if(iNewVal>100)
		iNewVal=100;
	iContrast = iNewVal;
	
	GetContrastValue(aMsg);
		
	if (iInfo.iOptionsSupported & TCameraInfo::EContrastSupported)
		iCamera->SetContrastL(iContrast);
	return;
}
TInt CCamCom::GetContrastValue(TBuf<100> &aMsg)
{
	if (!(iInfo.iOptionsSupported & TCameraInfo::EContrastSupported))
		aMsg.Append(KNoSup); 
	return iContrast;
}


TDesC8& CCamCom::Encode(TDesC8& aData, TInt *written) {
	return iDataDesc;
}

TInt CCamCom::GetVideoMode(TSize size)
{
	iVideoSize=0;
	TSize sizeRet;
	iCamera->EnumerateVideoFrameSizes(sizeRet, iVideoSize, CCamera::EFormatYUV420Planar);
	if (sizeRet!=size){
		iVideoSize++;
		iCamera->EnumerateVideoFrameSizes(sizeRet, iVideoSize, CCamera::EFormatYUV420Planar);
		if (sizeRet!=size){
    		iVideoSize++;
    		iCamera->EnumerateVideoFrameSizes(sizeRet, iVideoSize, CCamera::EFormatYUV420Planar);
    		if (sizeRet!=size){				
    			iVideoSize=-1;
    		}
		}
	}
	if (iVideoSize<0){
		iVideoSize=0;
		iAppUi->LogInfo(_L("GetVideoMode:size not supported set to default 0"));
	}
	else
		iAppUi->LogInfo(_L("GetVideoMode:%d"), iVideoSize);
	return iVideoSize;
}

TBool CCamCom::SetVideoSetting(TInt iBitRate, TSize &size) 
{
	TBool bRet=EFalse;

	GetVideoMode(size);
	
    // check for supported size
	TInt errore=KErrNone;
    do{
    	errore=KErrNone;
    	iAppUi->LogInfo(_L("SetVideoSetting:try to set video size capture"));
	    TRAP(errore, iRecorder->SetVideoFrameSizeL(size));
	    if(errore && iVideoSize<(iInfo.iNumVideoFrameSizesSupported-1))
	    {	iVideoSize++;
	    	iAppUi->LogInfo(_L("SetVideoSetting:video size not supported"));
	    	iCamera->EnumerateVideoFrameSizes(size, iVideoSize, CCamera::EFormatYUV420Planar);
	    	iAppUi->LogInfo(_L("SetVideoSetting:index video size use:%d"), iVideoSize);
    	}
	    else if(iVideoSize>iInfo.iNumVideoFrameSizesSupported-1)
	    {   TBuf<256> str;
		    str.Format(_L("Video Capture not supported"));
		    iAppUi->LogInfo(_L("SetVideoSetting:Video Capture not supported"));
		    CAknInformationNote* iInfoMsg=new (ELeave) CAknInformationNote;
		    iInfoMsg->ExecuteLD(str);
    	}
	    else if(errore!=KErrNone){
	    	iAppUi->LogInfo(_L("SetVideoSetting:SetVideoFrameSizeL failed error:%d"), errore);
	    }
	    	
    }while(errore!=KErrNone && iVideoSize<iInfo.iNumVideoFrameSizesSupported-1);
    if(errore==KErrNone)
	    bRet=ETrue;
    
    iAppUi->LogInfo(_L("SetVideoSetting:index video size use:%d"), iVideoSize);
    
    iAppUi->LogInfo(_L("SetVideoSetting:try to set BitRate"));
    TRAP(errore, iRecorder->SetVideoBitRateL( iBitRate ));    
    if (errore)
    	iAppUi->LogInfo(_L("SetVideoSetting:Error to set BitRate %d"), errore);
    else
    	_iBitrate=iBitRate;
    iAppUi->LogInfo(_L("SetVideoSetting:try to set FrameRate"));
    TRAP(errore, iRecorder->SetVideoFrameRateL(TReal( 15 )));
    if (errore)
        	iAppUi->LogInfo(_L("SetVideoSetting:Error to set FrameRate %d"), errore);
	if (bRet)
		iAppUi->LogInfo(_L("SetVideoSetting:no error, index video size:%d"), iVideoSize  );
	else
		iAppUi->LogInfo(_L("SetVideoSetting:error"));

    return bRet;
}


// observer callbacks
void CCamCom::MmroPrepareComplete(TInt aError)
{
	// handle errors in RunL
	if ( aError != KErrNone )
	    {
	    return;
	    }
	iIsCapture = ETrue;    
	iAppUi->LogInfo(_L("CCamCom::MmroPrepareComplete"));
	iRecorder->RecordL();
	iAppUi->LogInfo(_L("CCamCom::MmroPrepareComplete RecordL"));
	iCounterFrames=0;
}


void CCamCom::MmroStateChange(TInt aState, TInt aError)
{
	
}


void CCamCom::MmroTemporaryError(TInt aError)
{
}


void CCamCom::MmroFatalError(TInt aError)
{
}
TBool CCamCom::SetBitRate(TInt iBitrate)
{
	if (iBitrate>400000)
		return EFalse;
	TRAPD(iErr,iRecorder->SetVideoBitRateL( iBitrate ));
	if (iErr){
		iAppUi->LogInfo(_L("Failed SetBitRate bitrate:%d err:%d"), _iBitrate, iErr);
		return EFalse;
	}
	else
		_iBitrate = iBitrate;
	return ETrue;
}
TInt CCamCom::GetHandleCamera()
{
	return iCamera->Handle();
}

void CCamCom::IncreaseBitRate(){
	if (_iBitrate<400000) {
		SetBitRate(_iBitrate+100000);
		iAppUi->LogInfo(_L("Incremento bitrate:%d"), _iBitrate);
	}
}
void CCamCom::DecreaseBitRate(){
	if (_iBitrate>100000){
		SetBitRate(_iBitrate-100000);
		iAppUi->LogInfo(_L("Decremento bitrate:%d"), _iBitrate);
	}
}

void CCamCom::SetBitRateVal(int iBR){
	_iBitrate = iBR;
	iAppUi->LogInfo(_L("Selected bitrate:%d"), _iBitrate);	
}

void CCamCom::SelectCamera(bool bFront){
	if (bFront && CCamera::CamerasAvailable()>1)
		iIdxCam = 1;
	else
		iIdxCam = 0;
	
	if (iIdxCam)
		iAppUi->LogInfo(_L("Front Camera Selected"));
	else
		iAppUi->LogInfo(_L("Rear Camera Selected"));
	delete iCamera;
	iCamera = CCamera::NewL(*this, iIdxCam);	
}
