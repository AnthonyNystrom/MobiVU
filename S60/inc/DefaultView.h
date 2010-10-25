/*
 ============================================================================
 Name		: DefaultView.h
 Author	  : 7TouchGroup
 Version	 : 1.0
 Copyright   : 7TouchGroup
 Description : CDefaultView declaration
 ============================================================================
 */

#ifndef DEFAULTVIEW_H
#define DEFAULTVIEW_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <aknview.h>
#include "DemoVideoCallAppView.h"
#include "DemoVideoCallAppUi.h"
#include "Camera.h"

class CDemoVideoCallAppUi;
class CDemoVideoCallAppView;
class CCamCom;
// CLASS DECLARATION

/**
 *  CDefaultView
 * 
 */
class CDefaultView : public CAknView
	{
public:
	// Constructors and destructor

	/**
	 * Destructor.
	 */
	~CDefaultView();

	/**
	 * Two-phased constructor.
	 */
	static CDefaultView* NewL();

	/**
	 * Two-phased constructor.
	 */
	static CDefaultView* NewLC();
	
	void DoActivateL(const TVwsViewId& aPrevViewId, TUid aCustomMessageId, const TDesC8& aCustomMessage);
	void DoDeactivate();
	TUid Id() const;
	void HandleCommandL(TInt aCommand);
	void RecON();
	void RefreshDisplay();
	void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );
	void StartCamera(TBool bEnable);
	
	CDemoVideoCallAppView* iAppView;
	CCamCom *_iCamera;
	int _iQuality;
	int _iCameraSelected;
private:

	/**
	 * Constructor for performing 1st stage construction
	 */
	CDefaultView();

	/**
	 * EPOC default constructor for performing 2nd stage construction
	 */
	void ConstructL();
	void HandleStatusPaneSizeChange ();
	CDemoVideoCallAppUi *iAppUi;	
	};

#endif // DEFAULTVIEW_H
