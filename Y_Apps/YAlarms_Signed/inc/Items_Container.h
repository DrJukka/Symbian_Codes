/* 	
	Copyright (c) 2001 - 2007, 
	Dr. Jukka Silvennoinen.
	All rights reserved 
*/

#ifndef __RSS_FEEDS_CONTAINER_H__
#define __RSS_FEEDS_CONTAINER_H__

#include <COECNTRL.H>
#include <AknLists.h>
#include <D32DBMS.H>
#include <AknProgressDialog.h>
#include <documenthandler.h> 
#include <aknserverapp.h> 
#include <AknsBasicBackgroundControlContext.h> 
#include <aknsdrawutils.h>
#include <aknscontrolcontext.h>
#include <AknsSkinInstance.h>
#include <aknsutils.h>
#include <ASCliSession.h>
#include <ASShdAlarm.h>

#include "Status_Control.h"
#include "MenuNotes_Control.h"

/*
----------------------------------------------------------------
*/
class CEikButtonGroupContainer;
class CAknDoubleNumberStyleListBox;
class CEikMenuPane;
class CEikListBox;
class CGulIcon;
class CMyStatusControl;
class CMyButtonControl;
class CMyBatteryControl;
class CMySignalControl;
class CGulIcon;
class CMainContainer;


/*
----------------------------------------------------------------
*/
class CItemsContainer : public CCoeControl, MMyBgProvided,MMyMenuProvided
,MMyGrapProvider
    {
public:
    CItemsContainer(CEikButtonGroupContainer* aCba);
    void ConstructL(void);
     ~CItemsContainer();
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
    void Draw(const TRect& aRect) const;
	CCoeControl* ComponentControl( TInt aIndex) const;
	TInt CountComponentControls() const;
	void HandleViewCommandL(TInt aCommand);
public:	
	void SetMenuL(void);
	void ForegroundChangedL(TBool aForeground);
protected:
	void GetFileName(TDes& aFileName);
	CMyButtonControl* NaviPanel(void){ return iNaviPanel;};
	CGulIcon* ListSel(void){ return iListSel;};
	RPointerArray<CGulIcon>& BackAdd(void){ return iBackAdd;};
	CGulIcon* BackMask(void){ return iBackMask;};
	CFbsBitmap*	BackGround(void){ return iBackGround;};
	RPointerArray<CGulIcon>	Numbers(void){ return iNumbers;};
	CGulIcon* ScrollBack(void){ return iScrollBack;};
	CGulIcon* Scroller(void){ return iScroller;};
	TBool IsShowingMenu(void);
	void SetCbaL(TInt aResource,const TDesC& aLeft,const TDesC&  aRight);
	void UpOneOnAdds(void);
	TInt CurrnetAddIndex(void);
protected:
	CFbsBitmap* BagImageL(const CCoeControl* aControl,TRect& aBgRect);
	void AppendMenuL(CArrayFixFlat<TInt>& aCommands,CDesCArray& aTexts);
private:
	TBool IsNormalOrNot(TInt aTop,TInt aBottom);
	void HandleMenuCommandL(TInt aCommand);
	void ShowNoteL(const TDesC& aMessage);
	void SetRectanglesL(TBool& aNormalLayout);
	CGulIcon* LoadImageL(const TDesC& aFileName, TInt aImg, TInt aMsk,const TSize& aSize,TScaleMode aScaleMode = EAspectRatioPreservedAndUnusedSpaceRemoved);
	virtual void SizeChanged();
	virtual void HandleResourceChange(TInt aType);
 private:
	CEikButtonGroupContainer* 		iCba;
	CMainContainer*					iMainContainer;
	CMyStatusControl*				iMyStatusControl;
	CMyButtonControl*				iMyButtonControl1;
	CMyButtonControl*				iMyButtonControl3;
	CMySignalControl*				iMySignalControl;
	CMyBatteryControl*				iMyBatteryControl;
	CMyIconControl*					iMyIconControl;
	CMyButtonControl*				iNaviPanel;
	CMyMenuControl*					iMyMenuControl;
	TRect							iBackRect,iStatRect,iStatUpper,iStatLower;
	TRect							iBattRect,iSignRect,iButRect1,iButRect3,iNaviRect,iIconRect;
	CGulIcon* 						iListSel;
	RPointerArray<CGulIcon>			iBackAdd;
	CGulIcon* 						iBackMask;
	CFbsBitmap*						iBackGround;
	CFbsBitmap*						iStatGround;
	CFbsBitmap*						iStatGround2;
	TRgb							iButColor1,iButColor3A,iButColor3B,iUseButColor3,iNaviColor,iItemTxtColor,iItemTxtColorSel;
	CGulIcon* 						iScrollBack;
	CGulIcon* 						iScroller;
	RPointerArray<CGulIcon>			iNumbers;
	TInt							iMaxItems,iCurrentAdd;
	CGraphicsContext::TTextAlign 	iAllig1,iAllig3;
	RFile							iDbgFile;
	TBool							iShowMenu;
};



#endif // __RSS_FEEDS_CONTAINER_H__
