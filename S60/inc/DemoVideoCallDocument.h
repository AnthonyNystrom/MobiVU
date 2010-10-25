/*
 ============================================================================
 Name		: DemoVideoCallDocument.h
 Author	  : 7TouchGroup
 Copyright   : 7TouchGroup
 Description : Declares document class for application.
 ============================================================================
 */

#ifndef __DEMOVIDEOCALLDOCUMENT_h__
#define __DEMOVIDEOCALLDOCUMENT_h__

// INCLUDES
#include <akndoc.h>

// FORWARD DECLARATIONS
class CDemoVideoCallAppUi;
class CEikApplication;

// CLASS DECLARATION

/**
 * CDemoVideoCallDocument application class.
 * An instance of class CDemoVideoCallDocument is the Document part of the
 * AVKON application framework for the DemoVideoCall example application.
 */
class CDemoVideoCallDocument : public CAknDocument
	{
public:
	// Constructors and destructor

	/**
	 * NewL.
	 * Two-phased constructor.
	 * Construct a CDemoVideoCallDocument for the AVKON application aApp
	 * using two phase construction, and return a pointer
	 * to the created object.
	 * @param aApp Application creating this document.
	 * @return A pointer to the created instance of CDemoVideoCallDocument.
	 */
	static CDemoVideoCallDocument* NewL(CEikApplication& aApp);

	/**
	 * NewLC.
	 * Two-phased constructor.
	 * Construct a CDemoVideoCallDocument for the AVKON application aApp
	 * using two phase construction, and return a pointer
	 * to the created object.
	 * @param aApp Application creating this document.
	 * @return A pointer to the created instance of CDemoVideoCallDocument.
	 */
	static CDemoVideoCallDocument* NewLC(CEikApplication& aApp);

	/**
	 * ~CDemoVideoCallDocument
	 * Virtual Destructor.
	 */
	virtual ~CDemoVideoCallDocument();

public:
	// Functions from base classes

	/**
	 * CreateAppUiL
	 * From CEikDocument, CreateAppUiL.
	 * Create a CDemoVideoCallAppUi object and return a pointer to it.
	 * The object returned is owned by the Uikon framework.
	 * @return Pointer to created instance of AppUi.
	 */
	CEikAppUi* CreateAppUiL();

private:
	// Constructors

	/**
	 * ConstructL
	 * 2nd phase constructor.
	 */
	void ConstructL();

	/**
	 * CDemoVideoCallDocument.
	 * C++ default constructor.
	 * @param aApp Application creating this document.
	 */
	CDemoVideoCallDocument(CEikApplication& aApp);

	};

#endif // __DEMOVIDEOCALLDOCUMENT_h__
// End of File
