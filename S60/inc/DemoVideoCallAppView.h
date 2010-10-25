/*
 ============================================================================
 Name		: DemoVideoCallAppView.h
 Author	  : 7TouchGroup
 Copyright   : 7TouchGroup
 Description : Declares view class for application.
 ============================================================================
 */

#ifndef __DEMOVIDEOCALLAPPVIEW_h__
#define __DEMOVIDEOCALLAPPVIEW_h__

// INCLUDES
#include <coecntrl.h>
#include "camera.h"
#include "ffdecode.h"
#include "DemoVideoCallAppUi.h"

class CCamCom;
class CFFDecode;
class CDemoVideoCallAppUi;

// CLASS DECLARATION
class CDemoVideoCallAppView : public CCoeControl
	{
public:
	// New methods

	/**
	 * NewL.
	 * Two-phased constructor.
	 * Create a CDemoVideoCallAppView object, which will draw itself to aRect.
	 * @param aRect The rectangle this view will be drawn to.
	 * @return a pointer to the created instance of CDemoVideoCallAppView.
	 */
	static CDemoVideoCallAppView* NewL(const TRect& aRect);

	/**
	 * NewLC.
	 * Two-phased constructor.
	 * Create a CDemoVideoCallAppView object, which will draw itself
	 * to aRect.
	 * @param aRect Rectangle this view will be drawn to.
	 * @return A pointer to the created instance of CDemoVideoCallAppView.
	 */
	static CDemoVideoCallAppView* NewLC(const TRect& aRect);

	/**
	 * ~CDemoVideoCallAppView
	 * Virtual Destructor.
	 */
	virtual ~CDemoVideoCallAppView();

public:
	// Functions from base classes

	/**
	 * From CCoeControl, Draw
	 * Draw this CDemoVideoCallAppView to the screen.
	 * @param aRect the rectangle of this view that needs updating
	 */
	void Draw(const TRect& aRect) const;	
	TBuf<255> _sInfo;
	TBuf<255> _sBuffering;
	int _iCodec;

	/**
	 * From CoeControl, SizeChanged.
	 * Called by framework when the view size is changed.
	 */	
	virtual void SizeChanged();
	void SetInfo(TDesC& info);
	TBool SetCurrentBmp() const;
	void Buffering(int iBuffering);
	void DisplayFrame(CFbsBitmap* aFrame);
	void DecodeAndShow(TDesC8& aBuffer, TBool bShow);
	void SetCamera(CCamCom *iCamera){_iCamera=iCamera;}
	TRect ClientRect();
	void InitFFmpeg(int iCodec);
private:
	CCamCom *_iCamera;
	CFbsBitmap* _bmpReceiver, *_bmpMy, *_bmpLogo, *_bmpLogoLogged, *_bmpOutComing, *_bmpInComing, *_bmpTalking, *_bmpListen, *_bmpTalkingBoth;
	CFFDecode *_ffDecode;
	TSize _szVideo;	
	CFont* _font;
	CDemoVideoCallAppUi *iAppUi;
	TPoint _topLeft;
	TBool _bValidReceiver;
	
	// Constructors

	/**
	 * ConstructL
	 * 2nd phase constructor.
	 * Perform the second phase construction of a
	 * CDemoVideoCallAppView object.
	 * @param aRect The rectangle this view will be drawn to.
	 */
	void ConstructL(const TRect& aRect);

	/**
	 * CDemoVideoCallAppView.
	 * C++ default constructor.
	 */
	CDemoVideoCallAppView();	

	};

#endif // __DEMOVIDEOCALLAPPVIEW_h__
// End of File
