/* 	
	Copyright (c) 2001 - 2007, 
	J.P. Silvennoinen.
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
#include <aknutils.h>
#include <AknGlobalNote.h>


class CFbsBitmap;

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
class CMySplashScreen : public CCoeControl
    {
public:
	static CMySplashScreen* NewL(void);
	~CMySplashScreen();
private:
	virtual void SizeChanged();
	TTypeUid::Ptr MopSupplyObject(TTypeUid aId); // needed for skins stuff
	virtual void HandleResourceChange(TInt aType);
    void ConstructL(void);
	void Draw(const TRect& aRect) const;
	void GetSplashIconL(void);
private:
	CAknsBasicBackgroundControlContext*	iBgContext; // skins are drawn with this one
	CFbsBitmap*		iBgPic;
	CFbsBitmap*		iBgMsk;
};

  
#endif // __YTASKS_SPLASSCREEN_H__
