/*
 * Author Stefano Falasca, 7TouchGroup
 *
 */

#ifndef __CAMCOM_H__
#define __CAMCOM_H__

#include <e32base.h>			// CActive
#include <es_sock.h>
#include <bt_sock.h>			// TBTSockAddr
#include <ecam.h>				// CCamera
#include <fbs.h>				// CFbsBitmap
#include <hwrmlight.h>
#include <ccmrmediarecorderobserver.h>
#include <ccmrmediasink.h>
#include <ccmrmediarecorder.h>
#include <ecam.h>
#include "mainwindow.h"
#include "sink.h"
#include "TimeOutTimer.h"
#include "AudioStreamEngine.h"

// FORWARD DECLARATIONS:
class CDummySink;
class CTimeOutTimer;
class CDemoVideoCallAppUi;
class CAudioStreamEngine;
class MainWindow;

// Constants:
// Indicators
enum TCameraIndicators
    {
    ECameraQualityHighUid,  //Must be zero.
    ECameraQualityNormalUid,//Default in shared data
    ECameraQualityEcoUid,
    ECameraZoom2Uid,        //First zoom level,
    ECameraZoom4Uid,        //Second zoom level...
    ECameraZoomLimit        //Must be right after the zoom levels
    };
/**
The active object used to make BT requests.
*/
class CCamCom : public CActive,  MCameraObserver, MCMRMediaRecorderObserver, MTimeOutNotifier
{
public:
	// Construction
	static CCamCom* NewLC();
	static CCamCom* NewL();
    // Destruction
	~CCamCom();

// from MCameraObserver:
	void ReserveComplete(TInt aError);
	void PowerOnComplete(TInt aError);
	void ViewFinderFrameReady(CFbsBitmap& aFrame);
	void FrameBufferReady(MFrameBuffer* aFrameBuffer, TInt aError);
	void StartCamera();
	void StopCamera();
	TBool GetVideoSize(TInt iBitRate, TSize& aSize);
	TInt GetVideoMode(TSize size);
	
// from MSmartEncodeObserver:
	void ImageEncoded();
	TDesC8& Encode(TDesC8& aData, TInt *written);

protected:
	// Active object interface
	void RunL();
	void DoCancel();

private:
	// Construction:
	CCamCom();
    void ConstructL();
	
	void InitEncoder(int iCodec, TInt aWidth, TInt aHeight, TInt aBitrate);
	TUint8 iWriteBuffer[100000];

	TPtrC8 iDataDesc;
	TBool iOpened;
	int iCounterFrames;
	TBool bFirstThunbnail;
	
	CHWRMLight* iLight;
    CCMRMediaRecorder*    iRecorder;
    CDummySink*           iSink;
    MDataSource*          iAudioSource;
public:	
	void SetSizeRect(TRect sz);
	TInt SetZoomFactorL(TBool aEnable);
	void SetSetting(signed int &iSetFlash, signed int &iSetBrightness, signed int &iSetContrast, signed int &iSetWB, signed int &iSetExp, signed int &iSetIdxCam, signed int &aVideoSize);
	void SetFeatures();
	TBool HasCamera() const;
	TBool EnumerateVideoSizes(TInt& aCounter, TSize& aSize);
	void SetVideoSize(TInt aSizeIndex);
	TBool WhiteBParameterSupported(TInt iVal, TInt &iWBSet);
	void GetWhiteBParameterString(TBuf<100> &aMsg);
	TBool ChangeWhiteBParameter(TBool bUp, TBuf<100> &aMsg);
	void ChangeBrightnessParameter(TBool bUp, TBuf<100> &aMsg);
	void ChangeContrastParameter(TBool bUp, TBuf<100> &aMsg);
	void ChangeFlashParameter(TBool bUp, TBuf<100> &aMsg);
	TBool ExpParameterSupported(TInt iVal, TInt &iExpSet);
	void GetExpParameterString(TBuf<100> &aMsg);
	TBool ChangeExpParameter(TBool bUp, TBuf<100> &aMsg);
	TInt GetBrightnessValue(TBuf<100> &aMsg);
	TInt GetContrastValue(TBuf<100> &aMsg);
	TInt GetFlashParameterString( TBuf<100> &aMsg);
	void ImageReady(CFbsBitmap* aBitmap, HBufC8* aData, TInt aError);
	void TimerExpired();
	TBool IsStandby();
        TBool IsPowered();
	void PowerCamera();	
	TBool SetBitRate(TInt iBitrate);
	TInt GetHandleCamera();
	TBool StartCapture(TBool aCapture);
	TBool IsCamptureMode(){return iIsCapture;}
        void SetBitRateVal(int iBR);
        void SelectCamera(bool bFront);
private:
	void DisplayFrame();
	TBool SetVideoSetting(TInt iBitRate, TSize &size);
	void MmroFatalError(TInt aError);
	void MmroTemporaryError(TInt aError);
	void MmroStateChange(TInt aState, TInt aError);
	void MmroPrepareComplete(TInt aError);

	// helper Camera methods:
	TInt GetNearestSizeIndexAvailable(
		CCamera::TFormat desiredCaptureFormat, TSize desiredCaptureSize);
	
	// Data:
	// Global AppUi
        MainWindow* iAppUi;

	CCamera* iCamera;
	CTimeOutTimer*  iTimer;
	TInt iVideoSizeIndex;
	TBool iIsPoweredOn;
	TBool iIsCapture;	
	TInt iIdxCodecSupported;
        TBool bPowerCameraInProgress;
	
public:
	TBool iConnected;
	TCameraInfo iInfo;
	TInt iZoomFactor;
	TInt iFlash;
	TInt iBrightness;
	TInt iContrast;
	TInt iWB;
	TInt iExp;
	TInt iIdxCam;
	TInt iVideoSize;
	TInt iResolution;
	TInt iQualityLevel;
        TInt _iBitrate;
	TSize _szVideo;
        TPtr8 iCrtSendBufferPtr;
        HBufC8* iEncodedFrame2Send;
	CAudioStreamEngine *iAudioEngine;
	
private:
        CFbsBitmap* iFrameBmp;
        TRect m_szSizeArea;
        TUint32 iSentBytesCount;
        TPckg<TUint32> iPckgdFrameSize;
        TUint32 iFrameSize;
};

#endif//__CAMCOM_H__
