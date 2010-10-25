/*
 ============================================================================
 Name		: DemoVideoCallAppUi.h
 Author	  : 7TouchGroup
 Copyright   : 7TouchGroup
 Description : Declares UI class for application.
 ============================================================================
 */

#ifndef __DEMOVIDEOCALLAPPUI_h__
#define __DEMOVIDEOCALLAPPUI_h__

// INCLUDES
#include <aknappui.h>
#include <aknviewappui.h>
#include <aknnavi.h>

#include "SettingExampleSettingsData.h"
#include "SettingExampleListView.h"
#include "DefaultView.h"
#include "SocketsEngine.h"
#include "Engine.h"
#include <MdaAudioSamplePlayer.h>

//#define unsigned short unsigned short

// CMD PROTOCOL
const int CMD_NULL       = -1;
const int CMD_LOGIN      = 0;
const int CMD_LOGOUT     = 1;
const int CMD_CALL       = 2;
const int CMD_FRM_VIDEO  = 3;
const int CMD_FRM_AUDIO  = 4;
const int CMD_GET_USR_LST= 5;
const int CMD_RING       = 6;
const int CMD_CLOSE_CONV = 7;
const int CMD_TEST_AUDIO = 8;
const int CMD_TEST_VIDEO = 9;
const int CMD_TEST_AV    = 10;
const int CMD_RECORDER   = 11;
const int CMD_SETCODEC   = 12;



// FORWARD DECLARATIONS
class CCamCom;
class CSettingsData;
class CSettingListView;
class CDefaultView;
class CSckEngine;
class CEngine;

#define STATUS_UNLOGGED  0
#define STATUS_LOGGED    1
#define STATUS_OUTCOMING 2
#define STATUS_INCOMING  3
#define STATUS_TALKING   4

// CLASS DECLARATION
/**
 * CDemoVideoCallAppUi application UI class.
 * Interacts with the user through the UI and request message processing
 * from the handler class
 */
class CDemoVideoCallAppUi : public CAknViewAppUi, public MMdaAudioPlayerCallback
	{
public:
	// Constructors and destructor

	/**
	 * ConstructL.
	 * 2nd phase constructor.
	 */
	void ConstructL();

	/**
	 * CDemoVideoCallAppUi.
	 * C++ default constructor. This needs to be public due to
	 * the way the framework constructs the AppUi
	 */
	CDemoVideoCallAppUi();
	void LogInfo(TRefByValue<const TDesC> aFormat, ...);
	void LogInfo(char *pBuf);
    void InternalizeSettingsDataL();
    void ExternalizeSettingsDataL();
    void ShowSettings();
    void ShowDefault();
    void ShowMessage(const TDesC& aErrMessage, TInt aErrCode);
    void SocketReceiver(const TDesC8& aBuffer);
    unsigned short checkPktLen(unsigned short dataLenIn);
    void CheckPackets(unsigned char *entrata, unsigned short lunghezza);
    bool GiveMeAPacket(unsigned char *pbufInput, unsigned short iSizeInput, unsigned char **pBuffOutput, unsigned short *piSizeOutput);
    void xfree(void *mem);
    void *xmalloc(int size);    
    void SendCmd(int iCmd, const TDesC& aBuffer);
    void SendCmd(int iCmd);
    void RefreshStatus();
    void GetUsersLogged();
    void ShowUsersLogged();
    void GetUser(TInt iIdxUsr);
    void MakeCall();
    void ShowOutComing();
    void ShowInComing();
    void Reject(TBool bSendCmd=ETrue);
    void Accept(TBool bSendCmd=ETrue);
    void PlayRing();
    void ConnectSocket();
    TUint GetTick();
    void StartCamera(TBool bEnable);
    void SendVideoFrame(TDesC8& aBuffer);
    void SendAudioFrame(TDesC8& aBuffer);
    void SetCheckSum(TUint8 byHeader[], TUint16 iLen, TUint8 cmd);
  
    void BufferingSizeInfo(int iBuffSize);
    TKeyResponse HandleKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);    
    
    unsigned char *bufferNewPkt;
    unsigned short lenNewPkt;

    TInt iPeriod;    
    TInt GetStatus(){return _iStatus;}
    TFileName iSettingsFile;
	TBool _bClearLog;
	TInt _iCmd, _iStatus;
	TBool _bLogged;
	TBuf8<4096> _BufferTX;
	TBuf8<1024> _UsersLogged;
	TBuf8<64>_User2Call;
	TBuf<255>_sUser2Call;
	TBuf<32>_Header;
	TBool _bPLaying;	
	TBool _bSpeaking, _bSpeakingBoth;
	unsigned long _iCnt, _iCntErr;
	TInt _iStatusKey, _iKey;
	TBool _bFlagRxData;
	/**
	 * ~CDemoVideoCallAppUi.
	 * Virtual Destructor.
	 */
	virtual ~CDemoVideoCallAppUi();
	void MapcInitComplete(TInt aError,const TTimeIntervalMicroSeconds &aDuration);
	void MapcPlayComplete(TInt aError);
	
	void DisplayFrame(CFbsBitmap* aFrame);
	void CloseConnection();
	/**
	 * The application view
	 * Owned by CDemoVideoCallAppUi
	 */
	CSettingListView * _viewSettings;
	CDefaultView *_viewDefault;
	CSckEngine *_iSck;
	CEngine *_iEngine;
	CMdaAudioPlayerUtility* _iRingTone;	
	unsigned char *ritorno;
	unsigned short lenRitorno;
	TBuf<32> _ipAddr;
	CSettingsData* iData;	/* Pointer to settings data - OWNED */

private:
	// Functions from base classes

	/**
	 * From CEikAppUi, HandleCommandL.
	 * Takes care of command handling.
	 * @param aCommand Command to be handled.
	 */
	void HandleCommandL(TInt aCommand);

	/**
	 *  HandleStatusPaneSizeChange.
	 *  Called by the framework when the application status pane
	 *  size is changed.
	 */
	void HandleStatusPaneSizeChange();

	/**
	 *  From CCoeAppUi, HelpContextL.
	 *  Provides help context for the application.
	 *  size is changed.
	 */
	CArrayFix<TCoeHelpContext>* HelpContextL() const;	

private:
	// Data


	};

#endif // __DEMOVIDEOCALLAPPUI_h__
// End of File
