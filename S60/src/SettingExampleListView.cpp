/*
 * Copyright © 2008 Nokia Corporation.
 */

#include "SettingExampleListView.h"
#include <DemoVideoCall_0xE5EC06EB.RSG>
#include "DemoVideoCall.hrh"
#include "DemoVideoCallAppUi.h"
#include "SettingExampleSettingList.h"
#include <barsread.h>



// factory constructor leaving instance on cleanup
CSettingListView* CSettingListView::NewLC(CSettingsData &aData)
	{
	CSettingListView* self = new (ELeave) CSettingListView(aData);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// factory constructor - no instance left on cleanup
CSettingListView* CSettingListView::NewL(CSettingsData &aData)
	{
	CSettingListView* self = CSettingListView::NewLC(aData);
	CleanupStack::Pop(self);
	return self;
	}

// second phase constructor
void CSettingListView::ConstructL()
	{
	iAppUi = static_cast<CDemoVideoCallAppUi*> (CEikonEnv::Static()->EikAppUi());
	// construct from resource
	BaseConstructL(R_VIEW_SETTINGS);
	}

// first phase (non-leaving) constructor
CSettingListView::CSettingListView(CSettingsData &aData)
: CAknView(), iSettingsData(aData)
	{
	}

// activation of view
void CSettingListView::DoActivateL(const TVwsViewId& /*aPrevViewId*/, 
										 TUid /*aCustomMessageId*/, 
								   const TDesC8& /*aCustomMessage*/)
	{
	iAppUi->LogInfo(_L("CSettingListView::DoActivateL start"));
	if(!iSettingList)	// if we haven't already created iSettingList
		{
		// construct control and set parent
		iSettingList = CExampleSettingList::NewL(iSettingsData);
		iSettingList->SetMopParent(this);
		
		// construct control based on resource definition
		TResourceReader reader;

		// CreateResourceReaderLC will allocate a buffer to be used by
		// the TResourceReader. This buffer is pushed onto the cleanup
		// stack - not the TResourceReader itself
		CEikonEnv::Static()->CreateResourceReaderLC(reader, R_SETTINGS);
		iSettingList->ConstructFromResourceL(reader);

		// Clean up the buffer allocated above, NOT the reader itself.
		// Cannot use expected item overload of PopAndDestroy() as buffer 
		// is inaccessible. 
		CleanupStack::PopAndDestroy();	

		// register view and activate
        AppUi()->AddToStackL( *this, iSettingList );
		iSettingList->ActivateL();
		}
	iAppUi->LogInfo(_L("CSettingListView::DoActivateL end"));
	}

// deactivation of view
void CSettingListView::DoDeactivate()
	{
	iAppUi->LogInfo(_L("CSettingListView::DoDeactivate start"));
	if(iSettingList)	// if setting list has been created
		{
		// remove setting list from stack
		AppUi()->RemoveFromViewStack(*this, iSettingList);

		// clean up 
		delete iSettingList;
		iSettingList = NULL;
		}
	iAppUi->LogInfo(_L("CSettingListView::DoDeactivate end"));
	}


TUid CSettingListView::Id() const
	{
	return TUid::Uid( Settings );
	}

void CSettingListView::HandleCommandL(TInt aCommand)
    {
	switch(aCommand)
		{
	case ESettingCmdEdit :					// edit current item
		iSettingList->EditCurrentItemL(ETrue);
		break;
	case EAknSoftkeyBack:
		iAppUi->ShowDefault();
		break;
	default :								// anything else
		AppUi()->HandleCommandL(aCommand);	// handled by AppUi
		break;
		}
	}
	
// ---------------------------------------------------------
// CSettingListView::HandleResourceChange()
// Called by framework when resource is changed.
// ------
void CSettingListView::HandleStatusPaneSizeChange()
	{
	iAppUi->LogInfo(_L("CSettingListView::HandleStatusPaneSizeChange"));
	CAknView::HandleStatusPaneSizeChange(); //call to upper class
	iSettingList->SetRect(ClientRect());
	}

//EOF
