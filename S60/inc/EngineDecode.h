/*
 ============================================================================
 Name		: EngineDecode.h
 Author	  : 7TouchGroup
 Version	 : 1.0
 Copyright   : 7TouchGroup
 Description : CEngineDecode declaration
 ============================================================================
 */

#ifndef ENGINEDECODE_H
#define ENGINEDECODE_H

#include <e32base.h>	// For CActive, link against: euser.lib
#include <e32std.h>		// For RTimer, link against: euser.lib
class CEngineDecode : public CActive
	{
public:
	// Cancel and destroy
	~CEngineDecode();

	// Two-phased constructor.
	static CEngineDecode* NewL();

	// Two-phased constructor.
	static CEngineDecode* NewLC();

public:
	// New functions
	// Function for making the initial request
	void StartL(TTimeIntervalMicroSeconds32 aDelay);

private:
	// C++ constructor
	CEngineDecode();

	// Second-phase constructor
	void ConstructL();

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
	enum TEngineDecodeState
		{
		EUninitialized, // Uninitialized
		EInitialized, // Initalized
		EError
		// Error condition
		};

private:
	TInt iState; // State of the active object
	RTimer iTimer; // Provides async timing service

	};

#endif // ENGINEDECODE_H
