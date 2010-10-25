/*
 ============================================================================
 Name		: Engine.h
 Author	  : 7TouchGroup
 Version	 : 1.0
 Copyright   : 7TouchGroup
 Description : CEngine declaration
 ============================================================================
 */

#ifndef ENGINE_H
#define ENGINE_H

#include <e32base.h>	// For CActive, link against: euser.lib
#include <e32std.h>		// For RTimer, link against: euser.lib
#include "DemoVideoCallAppUi.h"

class CDemoVideoCallAppUi;

class CEngine : public CActive
	{
public:
	// Cancel and destroy
	~CEngine();

	// Two-phased constructor.
	static CEngine* NewL();

	// Two-phased constructor.
	static CEngine* NewLC();

public:
	// New functions
	// Function for making the initial request
	void StartL(TTimeIntervalMicroSeconds32 aDelay);
	void ShowUsersLogged(){iState=EShowUsersOnline;}
	void MakeCall(){_iCntRing=0;iState=EMakeCall;}
	void StopCall(){iState = ELogin; _iCnt=0;}
	void StopLogin(){iState = ENoneState;}
	void InComingCall(){iState = EIncoming0;}
	void ResetRings(){_iCntRing=0;}
	TInt CanStartConversation();
private:
	// C++ constructor
	CEngine();

	// Second-phase constructor
	void ConstructL();
	CDemoVideoCallAppUi* iAppUi;

private:
	// From CActive
	// Handle completion
	void RunL();

	// How to cancel me
	void DoCancel();

	// Override to handle leaves from RunL(). Default implementation causes
	// the active scheduler to panic.
	TInt RunError(TInt aError);

private:
	enum TEngineState
		{
		EUninitialized, // Uninitialized
		EInitialized, // Initalized
		EError, // Error condition
		EShowUsersOnline,
		ENoneState, 
		EMakeCall,
		ELogin,
		EIncoming0, 
		EIncoming1,
		EConversation
		};	

private:
	TInt iState; // State of the active object
	RTimer iTimer; // Provides async timing service
	TInt _iCntRing;
	TInt _iCnt;
	TInt _iCntForCloseConv;

	};

#endif // ENGINE_H
