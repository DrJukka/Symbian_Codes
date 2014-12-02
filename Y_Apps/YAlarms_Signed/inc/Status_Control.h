/* 
	Copyright (c) 2001 - 2007 ,
	Jukka Silvennoinen. 
	All rights reserved 
*/
#ifndef __UPDATE_STATUSCONTROL_H__
#define __UPDATE_STATUSCONTROL_H__

#include <coecntrl.h>       // CCoeControl
#include <GULICON.H>
#include <e32base.h>
#include <AknIconUtils.h>

#include "StatusGetters.h"
#include "CTimeOutTimer.h"
class CFbsBitmap;
class CMyButtonControl;


class MMyGrapProvider
	{
public:
	virtual CMyButtonControl* NaviPanel(void) = 0;
	virtual CGulIcon* ListSel(void) = 0;
	virtual RPointerArray<CGulIcon>& BackAdd(void) = 0;
	virtual CGulIcon* BackMask(void) = 0;
	virtual CFbsBitmap*	BackGround(void) = 0;
	virtual RPointerArray<CGulIcon>	Numbers(void) = 0;
	virtual CGulIcon* ScrollBack(void) = 0;
	virtual CGulIcon* Scroller(void) = 0;
	virtual TBool IsShowingMenu(void) = 0;
	virtual void SetCbaL(TInt aResource,const TDesC& aLeft,const TDesC&  aRight) = 0;
	virtual void UpOneOnAdds(void) = 0;
	virtual TInt CurrnetAddIndex(void) = 0;
	virtual void GetFileName(TDes& aFileName) = 0;
	};
	
	
class MMyBgProvided
	{
	public:
		virtual CFbsBitmap* BagImageL(const CCoeControl* aControl,TRect& aBgRect) = 0;
		virtual void GetFileName(TDes& aFileName) = 0;
	};

	
//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
class CMyStatusControl : public CCoeControl
    {
public:
	static CMyStatusControl* NewL(const CCoeControl& aContainer,const TRect& aRect,MMyBgProvided& aBgProvided);
	~CMyStatusControl();
protected:
private:
	virtual void SizeChanged();
    CMyStatusControl(MMyBgProvided& aBgProvided);
    void ConstructL(const CCoeControl& aContainer,const TRect& aRect);
	void Draw(const TRect& aRect) const;
	void GetApplicationIconL(void);
	CGulIcon* LoadImageL(const TDesC& aFileName, TInt aImg, TInt aMsk,const TSize& aSize);
private:
	MMyBgProvided& 	iBgProvided;
	CGulIcon* 		iNameImg;
};


//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
class CMyButtonControl : public CCoeControl
    {
public:
	static CMyButtonControl* NewL(const CCoeControl& aContainer,const TRect& aRect,MMyBgProvided& aBgProvided,const CFont* aFont);
	~CMyButtonControl();
	void SetAlligment(CGraphicsContext::TTextAlign aAlligment);
	void SetText(const TDesC& aText);
	void SetTextColot(const TRgb& aColor,const CFont* aFont);
private:
    CMyButtonControl(MMyBgProvided& aBgProvided,const CFont* aFont);
    void ConstructL(const CCoeControl& aContainer,const TRect& aRect);
	void Draw(const TRect& aRect) const;
private:	
	MMyBgProvided& 	iBgProvided;
	const CFont* 	iUseFont;
	CGraphicsContext::TTextAlign iAlligment;
	TBuf<100> 	iText;
	TRgb		iFontColor;
};

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
const TInt KBatteryTimeOut = 250000;

class CMyBatteryControl : public CCoeControl,MBatteryObserver,MTimeOutNotify
    {
public:
	static CMyBatteryControl* NewL(const CCoeControl& aContainer,const TRect& aRect,TBool aNormal,MMyBgProvided& aBgProvided);
	~CMyBatteryControl();
	void SetRectangle(const TRect& aRect,TBool aNormal);
protected:
	void TimerExpired();
	void BatteryLevelL(TUint aChangeStatus,CTelephony::TBatteryStatus aStatus);
private:
	virtual void SizeChanged();
    CMyBatteryControl(TBool aNormal,MMyBgProvided& aBgProvided);
    void ConstructL(const CCoeControl& aContainer,const TRect& aRect);
	void Draw(const TRect& aRect) const;
	CGulIcon* LoadImageL(const TDesC& aFileName, TInt aImg, TInt aMsk,const TSize& aSize,TScaleMode aScaleMode = EAspectRatioPreservedAndUnusedSpaceRemoved);
private:
	MMyBgProvided& 	iBgProvided;
	CGulIcon* 		iStatusImg;
	CGulIcon* 		iLevelImg;
	TRect 			iStatusRect,iLevelRect;
	CBatteryCheck*	iBatteryCheck;
	TUint 			iChangeStatus;
	CTelephony::TBatteryStatus iStatus;
	TBool 			iNormal;
	CTimeOutTimer*	iTimeOutTimer;
};


//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
class CMySignalControl : public CCoeControl,MNwSignalObserver
    {
public:
	static CMySignalControl* NewL(const CCoeControl& aContainer,const TRect& aRect,TBool aNormal,MMyBgProvided& aBgProvided);
	~CMySignalControl();
	void SetRectangle(const TRect& aRect,TBool aNormal);
protected:
	void SignalStatus(TInt32 aStrength,TInt8 aBars);
private:
	virtual void SizeChanged();
    CMySignalControl(TBool aNormal,MMyBgProvided& aBgProvided);
    void ConstructL(const CCoeControl& aContainer,const TRect& aRect);
	void Draw(const TRect& aRect) const;
	CGulIcon* LoadImageL(const TDesC& aFileName, TInt aImg, TInt aMsk,const TSize& aSize,TScaleMode aScaleMode = EAspectRatioPreservedAndUnusedSpaceRemoved);
private:
	MMyBgProvided& 	iBgProvided;
	CGulIcon* 		iStatusImg;
	CGulIcon* 		iLevelImg;
	TRect 			iStatusRect,iLevelRect;
	CNwSignalCheck*	iNwSignalCheck;
	TInt32 			iStrength;
	TInt8 			iBars;
	TBool 			iNormal;
};


//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
class CMyScrollControl : public CCoeControl
    {
public:
	static CMyScrollControl* NewL(const CCoeControl& aContainer,const TRect& aRect,MMyBgProvided& aBgProvided);
	~CMyScrollControl();
	void SetValues(TInt aCurrent, TInt aMax,TInt aCount);
private:
    CMyScrollControl(MMyBgProvided& aBgProvided);
    void ConstructL(const CCoeControl& aContainer,const TRect& aRect);
	void Draw(const TRect& aRect) const;
	virtual void SizeChanged();
	CGulIcon* LoadImageL(const TDesC& aFileName, TInt aImg, TInt aMsk,const TSize& aSize);
private:	
	MMyBgProvided& 	iBgProvided;
	TInt 			iCurrent,iMax,iCount;
	CGulIcon* 		iScrollBack;
	CGulIcon* 		iScroller;
};

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
class CMyIconControl : public CCoeControl
    {
public:
	static CMyIconControl* NewL(const CCoeControl& aContainer,const TRect& aRect,MMyBgProvided& aBgProvided);
	~CMyIconControl();
private:
    CMyIconControl(MMyBgProvided& aBgProvided);
    void ConstructL(const CCoeControl& aContainer,const TRect& aRect);
	void Draw(const TRect& aRect) const;
	void DrawNumber(const TRect& aRect,CWindowGc& aGc,CGulIcon* aIcon) const;
	CGulIcon* LoadImageL(const TDesC& aFileName, TInt aImg, TInt aMsk,const TSize& aSize);
	virtual void SizeChanged();
	static TInt DoCheckTimeL(TAny* aPtr);
private:	
	MMyBgProvided& 			iBgProvided;
	RPointerArray<CGulIcon>	iNumbers;
	TRect	iNumRect0,iNumRect1,iNumRect2,iNumRect3,iColRect;
	CPeriodic* 				iTimeTimer;
};


	


#endif // __UPDATE_STATUSCONTROL_H__
