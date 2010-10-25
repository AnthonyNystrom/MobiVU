/*
 ============================================================================
 Name		: DemoVideoCallApplication.cpp
 Author	  : 7TouchGroup
 Copyright   : 7TouchGroup
 Description : Main application class
 ============================================================================
 */

// INCLUDE FILES
#include "DemoVideoCall.hrh"
#include "DemoVideoCallDocument.h"
#include "DemoVideoCallApplication.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDemoVideoCallApplication::CreateDocumentL()
// Creates CApaDocument object
// -----------------------------------------------------------------------------
//
CApaDocument* CDemoVideoCallApplication::CreateDocumentL()
	{
	// Create an DemoVideoCall document, and return a pointer to it
	return CDemoVideoCallDocument::NewL(*this);
	}

// -----------------------------------------------------------------------------
// CDemoVideoCallApplication::AppDllUid()
// Returns application UID
// -----------------------------------------------------------------------------
//
TUid CDemoVideoCallApplication::AppDllUid() const
	{
	// Return the UID for the DemoVideoCall application
	return KUidDemoVideoCallApp;
	}

// End of File
