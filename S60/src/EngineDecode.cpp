/*
 ============================================================================
 Name		: EngineDecode.cpp
 Author	  : 7TouchGroup
 Version	 : 1.0
 Copyright   : 7TouchGroup
 Description : CEngineDecode implementation
 ============================================================================
 */

#include "EngineDecode.h"

CEngineDecode::CEngineDecode() :
	CActive(EPriorityStandard) // Standard priority
	{
	}

CEngineDecode* CEngineDecode::NewLC()
	{
	CEngineDecode* self = new (ELeave) CEngineDecode();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CEngineDecode* CEngineDecode::NewL()
	{
	CEngineDecode* self = CEngineDecode::NewLC();
	CleanupStack::Pop(); // self;
	return self;
	}

void CEngineDecode::ConstructL()
	{
	User::LeaveIfError(iTimer.CreateLocal()); // Initialize timer
	CActiveScheduler::Add(this); // Add to scheduler
	}

CEngineDecode::~CEngineDecode()
	{
	Cancel(); // Cancel any request, if outstanding
	iTimer.Close(); // Destroy the RTimer object
	// Delete instance variables if any
	}

void CEngineDecode::DoCancel()
	{
	iTimer.Cancel();
	}

void CEngineDecode::StartL(TTimeIntervalMicroSeconds32 aDelay)
	{
	Cancel(); // Cancel any request, just to be sure
	iState = EUninitialized;
	iTimer.After(iStatus, aDelay); // Set for later
	SetActive(); // Tell scheduler a request is active
	}

void CEngineDecode::RunL()
	{
	if (iState == EUninitialized)
		{
		// Do something the first time RunL() is called
		iState = EInitialized;
		}
	else if (iState != EError)
		{
		// Do something
		}
	iTimer.After(iStatus, 1000000); // Set for 1 sec later
	SetActive(); // Tell scheduler a request is active
	}

TInt CEngineDecode::RunError(TInt aError)
	{
	return aError;
	}
