/*
	Copyright (C) 2006 Jukka Silvennoinen
	All right reserved
*/

#ifndef __YUCCATOOLS_BASICAPPSTUFF_H__
#define __YUCCATOOLS_BASICAPPSTUFF_H__

#include <aknappui.h>
#include <aknapp.h>
#include <akndoc.h>
#include <aknwaitdialog.h> 
#include <AknProgressDialog.h>
#include <EIKPROGI.H>
#include <akncommondialogs.h> 
#include <maknfilefilter.h> 
#include <aknlists.h> 
#include <eikclbd.h>
#include <coecntrl.h>
#include <EIKRTED.H>

#include <coecntrl.h>       // CCoeControl
#include <eikbtgpc.h>
#include <eikmenup.h>
#include <EIKLBX.H>
#include <aknlists.h> 
#include <aknsfld.h> 
#include <CNTDEF.H>
#include <akntabgrp.h>
#include <aknnavi.h>
#include <aknnavide.h>
#include <AknsBasicBackgroundControlContext.h> 
#include <aknsdrawutils.h>
#include <aknscontrolcontext.h>
#include <AknsSkinInstance.h>
#include <aknsutils.h>

#include "YTasks_PublicInterface.h"
#include "clientservercommon.h"

class CMyHelpContainer;

class CExampleItem: public CBase
    {
    public:
        CExampleItem(): iIndex(-1){};
        ~CExampleItem(){delete iProcess;delete iName; delete iExitCatogory;};
    public:
    	TInt 		iIndex;
    	HBufC* 		iProcess;
        HBufC*  	iName;
        HBufC*  	iExitCatogory;
        TExitType	iExitType;
        TInt		iExitReason;
        TTime		iTime;
    };

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
class CCrashContainer : public CYTasksContainer
    {
public:
	~CCrashContainer();
    void ConstructL(CEikButtonGroupContainer* aCba);
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
	CCoeControl* ComponentControl( TInt aIndex) const;
	TInt CountComponentControls() const;
	void HandleCommandL(TInt aCommand);
	void SetMenuL(void);
	void InitMenuPanelL(TInt aResourceId, CEikMenuPane* aMenuPane);
	void ForegroundChangedL(TBool aForeground);
protected:
	TTypeUid::Ptr MopSupplyObject(TTypeUid aId);
private:
	void GetInfoBufferL(CExampleItem& aItem,TDes& aBuffer);
	void AddItemListL(TExampleItem& aItem);
	virtual void SizeChanged();
	virtual void HandleResourceChange(TInt aType);
	void ChangeIconAndNameL(void);
	void DisplayListBoxL(void);
	void UpdateScrollBar(CEikListBox* aListBox);
	TBool IsServerOn();
	void AddToTheListL(const TDesC& aName,const TExitType&  aExitType,const TInt& aExitReason,const TDesC& aExitCategory,CDesCArray& aItemArray);
	void Draw(const TRect& aRect) const;
private:
	CEikButtonGroupContainer* 		iCba;
	TInt 							iResId;
	CAknsBasicBackgroundControlContext*	iBgContext; 
	CAknDoubleStyleListBox*			iSelectionBox;
	TBool							iServerIsOn,iHideKill0;
	RPointerArray<CExampleItem>		iArray;
	CMyHelpContainer*				iMyHelpContainer;
};



#endif // __YUCCATOOLS_BASICAPPSTUFF_H__

