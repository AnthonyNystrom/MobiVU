/*
 ============================================================================
 Name		: DemoVideoCallAppView.cpp
 Author	  : 7TouchGroup
 Copyright   : 7TouchGroup
 Description : Application view implementation
 ============================================================================
 */

// INCLUDE FILES
#include <coemain.h>
#include <eikenv.h>
#include "DemoVideoCallAppView.h"
#include "DemoVideoCall.mbg"
#include <aknutils.h>
#include <gdi.h> 
// ============================ MEMBER FUNCTIONS ===============================

_LIT(KCalling, "Calling...");
_LIT(KFontName, "S60 Sans");
// -----------------------------------------------------------------------------
// CDemoVideoCallAppView::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDemoVideoCallAppView* CDemoVideoCallAppView::NewL(const TRect& aRect)
	{
	CDemoVideoCallAppView* self = CDemoVideoCallAppView::NewLC(aRect);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CDemoVideoCallAppView::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDemoVideoCallAppView* CDemoVideoCallAppView::NewLC(const TRect& aRect)
	{
	CDemoVideoCallAppView* self = new ( ELeave ) CDemoVideoCallAppView;
	CleanupStack::PushL(self);
	self->ConstructL(aRect);
	return self;
	}

// -----------------------------------------------------------------------------
// CDemoVideoCallAppView::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDemoVideoCallAppView::ConstructL(const TRect& aRect)
{
	_ffDecode = NULL;
	TFontSpec fontSpec(KFontName, 200);
	iEikonEnv->ScreenDevice()->GetNearestFontInTwips(_font, fontSpec);

	iAppUi = static_cast<CDemoVideoCallAppUi*> (CEikonEnv::Static()->EikAppUi());
	_LIT( mbm, "\\resource\\apps\\DemoVideoCall.mbm" );
	int ierror;
	TRAP(ierror, _bmpLogo = iEikonEnv->CreateBitmapL(mbm, EMbmDemovideocallLogo));
	TRAP(ierror, _bmpLogoLogged = iEikonEnv->CreateBitmapL(mbm, EMbmDemovideocallLogo_logged));
	if (iAppUi->iData->Brand()){
		TRAP(ierror, _bmpOutComing = iEikonEnv->CreateBitmapL(mbm, EMbmDemovideocallB_outcoming));
		TRAP(ierror, _bmpInComing = iEikonEnv->CreateBitmapL(mbm, EMbmDemovideocallB_incoming));
		TRAP(ierror, _bmpTalking = iEikonEnv->CreateBitmapL(mbm, EMbmDemovideocallB_speak));
		TRAP(ierror, _bmpTalkingBoth = iEikonEnv->CreateBitmapL(mbm, EMbmDemovideocallB_speak_both));
		TRAP(ierror, _bmpListen = iEikonEnv->CreateBitmapL(mbm, EMbmDemovideocallB_listen));
	}
	else{
		TRAP(ierror, _bmpOutComing = iEikonEnv->CreateBitmapL(mbm, EMbmDemovideocall7_outcoming));
		TRAP(ierror, _bmpInComing = iEikonEnv->CreateBitmapL(mbm, EMbmDemovideocall7_incoming));
		TRAP(ierror, _bmpTalking = iEikonEnv->CreateBitmapL(mbm, EMbmDemovideocall7_speak));
		TRAP(ierror, _bmpTalkingBoth = iEikonEnv->CreateBitmapL(mbm, EMbmDemovideocall7_speak_both));
		TRAP(ierror, _bmpListen = iEikonEnv->CreateBitmapL(mbm, EMbmDemovideocall7_listen));
	}
	
	_bValidReceiver = EFalse;
	_bmpReceiver = _bmpMy = NULL;	
	_bmpReceiver = new(ELeave)CFbsBitmap();
	_szVideo.SetSize(176,144);
	
	 User::LeaveIfError(_bmpReceiver->Create(_szVideo, EColor16M));
	
	// Create a window for this application view
	CreateWindowL();

	// Set the windows size
	SetRect(aRect);

	// Activate the window, which makes it ready to be drawn
	ActivateL();
	InitFFmpeg(CODEC_ID_H263);

	// position the video bitmap in the center
	_topLeft.iX= (Rect().Width() - _szVideo.iWidth) / 2;
	_topLeft.iY= ((Rect().Height() - _szVideo.iHeight) / 2)-15;
}

// -----------------------------------------------------------------------------
// CDemoVideoCallAppView::CDemoVideoCallAppView()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CDemoVideoCallAppView::CDemoVideoCallAppView()
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// CDemoVideoCallAppView::~CDemoVideoCallAppView()
// Destructor.
// -----------------------------------------------------------------------------
//
CDemoVideoCallAppView::~CDemoVideoCallAppView()
	{	
	delete _ffDecode;
	if (_bmpReceiver!=NULL)
		delete _bmpReceiver;
	if (_bmpLogo!=NULL)
		delete _bmpLogo;
	if (_bmpLogoLogged!=NULL)
		delete _bmpLogoLogged;
	if (_bmpOutComing!=NULL)
		delete _bmpOutComing;
	if (_bmpInComing!=NULL)
		delete _bmpInComing;
	if (_bmpTalking!=NULL)
		delete _bmpTalking;
	if (_bmpListen!=NULL)
		delete _bmpListen;
	if (_bmpTalkingBoth!=NULL)
		delete _bmpTalkingBoth;
	}
// -----------------------------------------------------------------------------
// CDemoVideoCallAppView::Draw()
// Draws the display.
// -----------------------------------------------------------------------------
//
void CDemoVideoCallAppView::Draw(const TRect& aRect) const
{
	// Get the standard graphics context
	CWindowGc& gc = SystemGc();

	gc.SetBrushColor(KRgbBlack);
    gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	// Gets the control's extent
	TRect drawRect(Rect());
	// Clears the screen
	gc.Clear(drawRect);	
	TPoint topLeft;	
	if (aRect.iTl.iX<=0){
		CFbsBitmap *bmp;
		switch (iAppUi->GetStatus())
		{
		case STATUS_UNLOGGED:
			bmp=_bmpLogo;
			break;
		case STATUS_LOGGED:
			bmp=_bmpLogoLogged;
			break;
		case STATUS_INCOMING:
			bmp=_bmpInComing;
			break;
		case STATUS_OUTCOMING:
			bmp=_bmpOutComing;		
			break;
		case STATUS_TALKING:
			if (iAppUi->_bSpeaking){
				if (iAppUi->_bSpeakingBoth)
					bmp=_bmpTalkingBoth;
				else
					bmp=_bmpTalking;
			}
			else{
				bmp=_bmpListen;
			}
			break;
		}
		if (bmp!=NULL){		
			// position the drawn bitmap in the center
			topLeft.iX= (Rect().Width() - bmp->SizeInPixels().iWidth) / 2;
			topLeft.iY= (Rect().Height() - bmp->SizeInPixels().iHeight) / 2;	
			gc.BitBlt(topLeft, bmp);
		}
	}
	
	
	switch (iAppUi->GetStatus())
	{
	case STATUS_UNLOGGED:
		break;
	case STATUS_LOGGED:
		break;
	case STATUS_INCOMING:			
	case STATUS_OUTCOMING:
		gc.UseFont(_font);
		gc.SetPenColor(KRgbWhite);			
		topLeft.iX = Rect().Width()/2-_font->TextWidthInPixels(iAppUi->_sUser2Call)/2;
		topLeft.iY = Rect().Height()-20;
		gc.DrawText(iAppUi->_sUser2Call, topLeft);
		break;
	case STATUS_TALKING:
		if (_bmpReceiver!=NULL && _bValidReceiver){
			gc.BitBlt(_topLeft, _bmpReceiver);
			
			/*if (_sInfo.Length()>0 || _sBuffering.Length()>0){			
				CFont* font;
				iEikonEnv->ScreenDevice()->GetNearestFontInTwips(font, fontSpec);
				gc.UseFont(font);
				gc.SetPenColor(KRgbWhite);			
				topLeft.iX = 0;
				topLeft.iY = Rect().Height()-30;
				if (_sInfo.Length()>0)
					gc.DrawText(_sInfo, topLeft);
				topLeft.iY = Rect().Height()-15;
				if (_sBuffering.Length()>0)
					gc.DrawText(_sBuffering, topLeft);
			}*/			
		}
		break;
	}	
}

void CDemoVideoCallAppView::SetInfo(TDesC& info)
{
	_sInfo=info;
}

void CDemoVideoCallAppView::Buffering(int iBuffering)
{
	_sBuffering.Format(_L("Buf:%d"), iBuffering);
}

// -----------------------------------------------------------------------------
// CDemoVideoCallAppView::SizeChanged()
// Called by framework when the view size is changed.
// -----------------------------------------------------------------------------
//
void CDemoVideoCallAppView::SizeChanged()
{
	DrawNow();
}

void CDemoVideoCallAppView::DisplayFrame(CFbsBitmap* aFrame)
{/*
	_bmpMy = aFrame;
	_bDrawFrame=ETrue;
	DrawNow();
	*/
}	

TRect CDemoVideoCallAppView::ClientRect()
{
	return Rect();
}

void CDemoVideoCallAppView::InitFFmpeg(int iCodec){
	_iCodec = iCodec;
	if (_ffDecode==NULL)
		_ffDecode = new CFFDecode();
	if (!_ffDecode->Init(_iCodec, _szVideo.iWidth, _szVideo.iHeight, 1000, NULL, 0, NULL))	
		iAppUi->LogInfo(_L("_ffDecode.Init FAILED"));
	else
		iAppUi->LogInfo(_L("_ffDecode.Init correct %dx%d"), _szVideo.iWidth, _szVideo.iHeight);
}

void CDemoVideoCallAppView::DecodeAndShow(TDesC8& aBuffer, TBool bShow)
{
	AVFrame *pFrame = NULL;
	TUint8* frame = const_cast<TUint8*>(aBuffer.Ptr());
	bool bKeyF;
	
	if (_ffDecode->DecodePic(frame, aBuffer.Size(), 0, &pFrame, &bKeyF))
	{
		//iAppUi->LogInfo(_L("DecodeAndShow: DECODEC"));
		/*TBitmapUtil bmpUtil(_bmpReceiver);
		//AppUi->LogInfo(_L("DecodeAndShow: bmpUtil.Begin"));
		bmpUtil.Begin(TPoint(0,0));

		unsigned int pixel32;
		unsigned char *pix = (unsigned char *)&pixel32;
		unsigned char *pRGB = _ffDecode->pFrameRGB->data[0];
		unsigned char *p=pRGB;		
		for(int y = 0; y < _szVideo.iHeight; y++)    
		{	
			for(int x = 0; x < _szVideo.iWidth; x++){
				
				pix[0]=(unsigned char)*(p);p++;
				pix[1]=(unsigned char)*(p);p++;
				pix[3]=(unsigned char)*(p);p++;
				pix[4]=0;
				
				//iAppUi->LogInfo(_L("%d %d %X"), y, x, pixel);
				bmpUtil.SetPixel(pixel32);
				bmpUtil.IncXPos();
			}
		}
		
		bmpUtil.End();*/
		
		//if (bShow){
		if (1){
			TUint8* pRGB = (TUint8*)_ffDecode->pFrameRGB->data[0];
			_bmpReceiver->LockHeap(EFalse);
			TUint8* pImage = (TUint8*)_bmpReceiver->DataAddress();
			memcpy(pImage, pRGB, _szVideo.iHeight*_szVideo.iWidth*3);
			_bmpReceiver->UnlockHeap();
			TRect rc(_topLeft, _szVideo);
			_bValidReceiver=ETrue;
			DrawNow(rc);			
		}
	}
	else
		iAppUi->LogInfo(_L("DecodeAndShow: NOT DECODEC"));
	
}

// End of File


