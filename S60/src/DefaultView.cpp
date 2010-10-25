/*
 ============================================================================
 Name		: DefaultView.cpp
 Author	  : 7TouchGroup
 Version	 : 1.0
 Copyright   : 7TouchGroup
 Description : CDefaultView implementation
 ============================================================================
 */

#include "DefaultView.h"
#include "DemoVideoCall.hrh"
#include <DemoVideoCall_0xE5EC06EB.RSG>
#include <aknmessagequerydialog.h>
#include <eikedwin.h> 
#include <barsread.h>

CDefaultView::CDefaultView(): CAknView()
	{
	iAppView=NULL;
	_iQuality = EQL;
	_iCameraSelected = ECameraF;
	}

CDefaultView::~CDefaultView()
	{
	delete iAppView;
	delete _iCamera;	
	iAppView=NULL;
	}

CDefaultView* CDefaultView::NewLC()
	{
	CDefaultView* self = new (ELeave)CDefaultView();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CDefaultView* CDefaultView::NewL()
	{
	CDefaultView* self=CDefaultView::NewLC();
	CleanupStack::Pop(); // self;
	return self;
	}

void CDefaultView::ConstructL()
	{
	iAppUi = (CDemoVideoCallAppUi*)AppUi();
	BaseConstructL( R_DEFAULT_VIEW );
	_iCamera = CCamCom::NewL();
	}
void CDefaultView::HandleStatusPaneSizeChange()
	{
	CAknView::HandleStatusPaneSizeChange(); //call to upper class
	}

// activation of view
void CDefaultView::DoActivateL(const TVwsViewId& /*aPrevViewId*/, TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/)
	{
		iAppUi->LogInfo(_L("CDefaultView::DoActivateL start"));
		CEikButtonGroupContainer* cba = AppUi()->Cba();
		if ( cba != NULL ) 
			cba->MakeVisible( EFalse );
		
		if ( iAppView == NULL ){
			iAppView = CDemoVideoCallAppView::NewL(ClientRect() );
			iAppView->SetCamera(_iCamera);
			iAppView->SetMopParent( this );
			AppUi()->AddToStackL( *this, iAppView );
		} 
		iAppUi->LogInfo(_L("CDefaultView::DoActivateL end"));
	}

// deactivation of view
void CDefaultView::DoDeactivate()
	{
		iAppUi->LogInfo(_L("CDefaultView::DoDeactivate start"));
		CEikButtonGroupContainer* cba = AppUi()->Cba();
		if ( cba != NULL ) 
		{	cba->MakeVisible( ETrue );
			cba->DrawDeferred();
		}
		if ( iAppView != NULL )
		{	AppUi()->RemoveFromViewStack( *this, iAppView );
			delete iAppView;
			iAppView = NULL;
		}
		iAppUi->LogInfo(_L("CDefaultView::DoDeactivate end"));
	}

TUid CDefaultView::Id() const
	{
	return TUid::Uid( Default );
	}

void CDefaultView::StartCamera(TBool bEnable)
{
	if(_iCamera){
		if (bEnable){
			_iCamera->SetSizeRect(iAppView->ClientRect());
			_iCamera->StartCamera();
		}
		else if (!bEnable)
		{	_iCamera->StartCapture(EFalse);
			_iCamera->StopCamera();
		}
	}
}

void CDefaultView::RecON(){
	TBuf<128> text;
	// The descriptor contained the prompt text for the query. The prompt 
				// text can also be defined in the resource structure of the query
	TBuf<128> prompt(_L("Stream Name")); 
	// create dialog instance 
	CAknTextQueryDialog* dlg = new( ELeave ) CAknTextQueryDialog( text, prompt );
	// Prepares the dialog, constructing it from the specified resource
	dlg->PrepareLC( R_DEMO_DATA_QUERY );
	// Sets the maximum length of the text editor
	// dlg->SetMaxLength(10); i comment this line
	// Launch the dialog
	if (dlg->RunLD())
		iAppUi->SendCmd(CMD_RECORDER, text);
}
void CDefaultView::HandleCommandL( TInt aCommand )
	{
		switch (aCommand)
		{
		case ERecON:
			RecON();
			break;
		case ERecOFF:
			iAppUi->SendCmd(CMD_RECORDER);
			break;
		case EGetAudioRec:
			break;
		case EGetVideoRec:
			break;
		case EGetAVRec:
			break;
		case ECloseConv:
			iAppUi->Reject();
			break;
		case EAccept:
			iAppUi->Accept();
			break;
		case EReject:			
			iAppUi->Reject();
			break;			
		case EEikCmdExit:
		case EAknSoftkeyExit:
			if(_iCamera)
				_iCamera->StopCamera();		
			AppUi()->HandleCommandL(aCommand);
			break;
		case ESettings:
			iAppUi->ShowSettings();
			break;
		case ECall:
			iAppUi->GetUsersLogged();			
			break;
		case EQL:
			_iQuality = EQL; 
			_iCamera->SetBitRateVal(80000);
			break;
		case EQM:
			_iQuality = EQM;
			_iCamera->SetBitRateVal(120000);
			break;
		case EQH:
			_iQuality = EQH;
			_iCamera->SetBitRateVal(200000);
			break;
		case EQHH:
			_iQuality = EQHH;
			_iCamera->SetBitRateVal(230000);
			break;
		case ECameraF:
			if (iAppUi->_iStatus<=1){ // only if is logged or unlogged
				_iCamera->SelectCamera(true);
				_iCameraSelected = ECameraF;
			}
			break;
		case ECameraR:
			if (iAppUi->_iStatus<=1){ // only if is logged or unlogged
				_iCamera->SelectCamera(false);
				_iCameraSelected = ECameraR;
			}
			break;
		case EAbout:
			CAknMessageQueryDialog* dlg = new (ELeave)CAknMessageQueryDialog();
			dlg->PrepareLC(R_ABOUT_QUERY_DIALOG);
			HBufC* title = iEikonEnv->AllocReadResourceLC(R_ABOUT_DIALOG_TITLE);
			dlg->QueryHeading()->SetTextL(*title);
			CleanupStack::PopAndDestroy(); //title
			HBufC* msg = iEikonEnv->AllocReadResourceLC(R_ABOUT_DIALOG_TEXT);
			dlg->SetMessageTextL(*msg);
			CleanupStack::PopAndDestroy(); //msg
			dlg->RunLD();
			break;
		default:
			//Panic(EDemoVideoCallUi);
			break;
		}
	}
void CDefaultView::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
{
    if ( aResourceId == R_SUBMENUQUALITY)
		aMenuPane->SetItemButtonState(_iQuality, EEikMenuItemSymbolOn );		
    else if (aResourceId == R_SUBMENUCAMERA)
    	aMenuPane->SetItemButtonState(_iCameraSelected, EEikMenuItemSymbolOn );
}
void CDefaultView::RefreshDisplay(){
	if (iAppView)
		iAppView->DrawNow();
}
