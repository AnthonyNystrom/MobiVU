/*
 ============================================================================
 Name		: DemoVideoCallApplication.h
 Author	  : 7TouchGroup
 Copyright   : 7TouchGroup
 Description : Declares main application class.
 ============================================================================
 */

#ifndef __DEMOVIDEOCALLAPPLICATION_H__
#define __DEMOVIDEOCALLAPPLICATION_H__

// INCLUDES
#include <aknapp.h>
#include "DemoVideoCall.hrh"

// UID for the application;
// this should correspond to the uid defined in the mmp file
const TUid KUidDemoVideoCallApp =
	{
	_UID3
	};

// CLASS DECLARATION

/**
 * CDemoVideoCallApplication application class.
 * Provides factory to create concrete document object.
 * An instance of CDemoVideoCallApplication is the application part of the
 * AVKON application framework for the DemoVideoCall example application.
 */
class CDemoVideoCallApplication : public CAknApplication
	{
public:
	// Functions from base classes

	/**
	 * From CApaApplication, AppDllUid.
	 * @return Application's UID (KUidDemoVideoCallApp).
	 */
	TUid AppDllUid() const;

protected:
	// Functions from base classes

	/**
	 * From CApaApplication, CreateDocumentL.
	 * Creates CDemoVideoCallDocument document object. The returned
	 * pointer in not owned by the CDemoVideoCallApplication object.
	 * @return A pointer to the created document object.
	 */
	CApaDocument* CreateDocumentL();
	};

#endif // __DEMOVIDEOCALLAPPLICATION_H__
// End of File
