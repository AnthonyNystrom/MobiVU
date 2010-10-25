/*
 ============================================================================
 Name		: Engine.cpp
 Author	  : 7TouchGroup
 Version	 : 1.0
 Copyright   : 7TouchGroup
 Description : CEngine implementation
 ============================================================================
 */

#include "Engine.h"

#define MAX_RINGS 20

CEngine::CEngine() :
	CActive(EPriorityStandard) // Standard priority
	{
	}

CEngine* CEngine::NewLC()
	{
	CEngine* self = new ( ELeave ) CEngine();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CEngine* CEngine::NewL()
	{
	CEngine* self = CEngine::NewLC();
	CleanupStack::Pop(); // self;
	return self;
	}

void CEngine::ConstructL()
	{
	iAppUi = static_cast<CDemoVideoCallAppUi*> (CEikonEnv::Static()->EikAppUi());
	User::LeaveIfError(iTimer.CreateLocal() ); // Initialize timer
	CActiveScheduler::Add( this); // Add to scheduler
	_iCntRing=_iCnt=0;
	}

CEngine::~CEngine()
	{
	Cancel(); // Cancel any request, if outstanding
	iTimer.Close(); // Destroy the RTimer object
	// Delete instance variables if any
	}

void CEngine::DoCancel()
	{
	iTimer.Cancel();
	}

void CEngine::StartL(TTimeIntervalMicroSeconds32 aDelay)
	{
	Cancel(); // Cancel any request, just to be sure
	iState = EUninitialized;
	iTimer.After(iStatus, aDelay); // Set for later
	SetActive(); // Tell scheduler a request is active
	}

void CEngine::RunL()
{
	if (iState == EUninitialized)
	{
		// Do something the first time RunL() is called
		iState = ELogin;
	}
	else if (iState != EError)
	{
		switch (iState){
		case ELogin:
			if (_iCnt==10){
				iAppUi->SendCmd(CMD_LOGIN);
				_iCnt=1;
			}
			else
				_iCnt++;
			break;
		case ENoneState:			
			break;
		case EShowUsersOnline:
			iState = ELogin;
			iAppUi->ShowUsersLogged();			
			break;
		case EMakeCall:
			if (iAppUi->GetStatus()==STATUS_TALKING){
				iState=EConversation;
			}
			else{
				if (_iCntRing>=MAX_RINGS){
					iState=ELogin;
					iAppUi->LogInfo(_L("STOP CALLING MAX RING"));
					iAppUi->SendCmd(CMD_LOGIN);// reset status calling to available
					iAppUi->Reject();
				}
				else{
					if (!_iCntRing)
						iAppUi->ShowOutComing();
					iAppUi->LogInfo(_L("CALLING..."));
					if (!(_iCntRing%3))
						iAppUi->MakeCall();
					_iCntRing++;
				}
			}
			break;
		case EIncoming0:
			iAppUi->StartCamera(ETrue);
			iState=EIncoming1;
			_iCntRing=0;
			break;
		case EIncoming1:
			if (iAppUi->_iStatus==STATUS_TALKING){
				iState=EConversation;
				_iCntForCloseConv=0;
			}
			else{
				_iCntRing++;
				// no more rings ?
				if (_iCntRing==20){
					iAppUi->LogInfo(_L("Too much ring"));
					iAppUi->Reject(ETrue);
				}
			}
			// waiting init camera and video capture			
			break;
		case EConversation:
			/*if (iAppUi->_bFlagRxData)
				iAppUi->_bFlagRxData=EFalse;
			else if (_iCntForCloseConv < 5)
				_iCntForCloseConv++;
			else{
				iAppUi->LogInfo(_L("Conversation fall"));
				iAppUi->Reject(ETrue);
			}*/
			break;
		}
	}
	iTimer.After(iStatus, 1000000); // Set for 1 sec later
	SetActive(); // Tell scheduler a request is active
}

// Call this function at the end of init camera and video capture, to start the streaming.
// if the user has reject the call, when StartConversation is called, it close the camera and video caputure
// return : STATUS_INCOMING mean wait
// return : STATUS_TALKING  mean can streaming
// return : STATUS_LOGGED conversation rejected
TInt CEngine::CanStartConversation()
{
	TInt iRet=STATUS_INCOMING;
	if (iState==EConversation)
		iRet=STATUS_TALKING;
	else if ((iState==EIncoming0 || iState==EIncoming1) && iAppUi->_iStatus==STATUS_TALKING){
		iState=EConversation;
		iRet=STATUS_TALKING;
	}
	else if ((iState==EIncoming0 || iState==EIncoming1) && iAppUi->_iStatus==STATUS_INCOMING){
		// Wait the user
		iRet=STATUS_INCOMING;
	}
	else if (iState==EMakeCall)
		iRet=STATUS_OUTCOMING;
	else{
		// Hangup
		iRet=STATUS_LOGGED;
	}
	//iAppUi->LogInfo(_L("CEngine::CanStartConversation():%d"), iRet);
	return iRet;
}

TInt CEngine::RunError(TInt aError)
	{
	return aError;
	}
