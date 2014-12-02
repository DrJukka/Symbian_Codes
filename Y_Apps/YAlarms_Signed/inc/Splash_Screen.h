/* 
	Copyright (c) 2001 - 2007 ,
	Jukka Silvennoinen. 
	All rights reserved 
*/
#ifndef __SCCOPY_SPLASSCREEN_H__
#define __SCCOPY_SPLASSCREEN_H__

#include <coecntrl.h>       // CCoeControl
#include <AknsBasicBackgroundControlContext.h> 
#include <aknsdrawutils.h>
#include <aknscontrolcontext.h>
#include <AknsSkinInstance.h>
#include <aknsutils.h>

class CFbsBitmap;

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
class CMySplashScreen : public CCoeControl
    {
public:
	static CMySplashScreen* NewL();
	~CMySplashScreen();
	void NextImage(void);
private:
	virtual void SizeChanged();
	virtual void HandleResourceChange(TInt aType);
    CMySplashScreen();
    void ConstructL(void);
	void Draw(const TRect& aRect) const;
	void DrawImages(CWindowGc& aGc,const TRect& aRect,CGulIcon* aIcon) const;
	CGulIcon* LoadImageL(const TDesC& aFileName, TInt aImg, TInt aMsk,const TSize& aSize);

private:
	CFbsBitmap*				iBackGround;
	RPointerArray<CGulIcon>	iBackAdd;
	TInt 		iCurrent;
};

  
#endif // __SCCOPY_SPLASSCREEN_H__
