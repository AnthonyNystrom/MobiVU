/*
 ============================================================================
 Name		: DemoVideoCallDocument.cpp
 Author	  : 7TouchGroup
 Copyright   : 7TouchGroup
 Description : CDemoVideoCallDocument implementation
 ============================================================================
 */

// INCLUDE FILES
#include "DemoVideoCallAppUi.h"
#include "DemoVideoCallDocument.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDemoVideoCallDocument::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDemoVideoCallDocument* CDemoVideoCallDocument::NewL(CEikApplication& aApp)
	{
	CDemoVideoCallDocument* self = NewLC(aApp);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CDemoVideoCallDocument::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDemoVideoCallDocument* CDemoVideoCallDocument::NewLC(CEikApplication& aApp)
	{
	CDemoVideoCallDocument* self = new ( ELeave ) CDemoVideoCallDocument( aApp );

	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// -----------------------------------------------------------------------------
// CDemoVideoCallDocument::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDemoVideoCallDocument::ConstructL()
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// CDemoVideoCallDocument::CDemoVideoCallDocument()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CDemoVideoCallDocument::CDemoVideoCallDocument(CEikApplication& aApp) :
	CAknDocument(aApp)
	{
	// No implementation required
	}

// ---------------------------------------------------------------------------
// CDemoVideoCallDocument::~CDemoVideoCallDocument()
// Destructor.
// ---------------------------------------------------------------------------
//
CDemoVideoCallDocument::~CDemoVideoCallDocument()
	{
	// No implementation required
	}

// ---------------------------------------------------------------------------
// CDemoVideoCallDocument::CreateAppUiL()
// Constructs CreateAppUi.
// ---------------------------------------------------------------------------
//
CEikAppUi* CDemoVideoCallDocument::CreateAppUiL()
	{
	// Create the application user interface, and return a pointer to it;
	// the framework takes ownership of this object
	return new ( ELeave )CDemoVideoCallAppUi;
	}

// End of File
