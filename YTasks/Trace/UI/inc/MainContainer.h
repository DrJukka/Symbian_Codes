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
#include "CTimeOutTimer.h"
#include "Common.h"


#include <eikstart.h>
	


class CMyHelpContainer;	
class CCPUControl;
class CMemControl;
class CProfileSettings;

class CExampleItem: public CBase
    {
    public:
        CExampleItem(){};
        ~CExampleItem(){};
    public:
    	TUint8 		iCpuLoad;
    	TUint32		iMemory;
        TTime		iTime;
    };


//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
class CTraceContainer : public CYTasksContainer,MTimeOutNotify
    {
public:
	~CTraceContainer();
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
	void TimerExpired();
private:
	void ShowNoteL(const TDesC& aMessage);
	void GetInfoBufferL(TExampleItem& aItem,TDes& aBuffer);
	void AddItemListL(TExampleItem& aItem);
	virtual void SizeChanged();
	virtual void HandleResourceChange(TInt aType);
	void ChangeIconAndNameL(void);
	TBool IsServerOn();
	void Draw(const TRect& aRect) const;
	void DrawCPU(const TRect& aRect, CWindowGc& gc) const;
	void DrawMemory(const TRect& aRect, CWindowGc& gc) const;
	CGulIcon* LoadImageL(const TDesC& aFileName, TInt aImg, TInt aMsk,const TSize& aSize,TScaleMode aAspect);
	void PieDrawer(CWindowGc& aGc, TPoint aLeftTop, TInt aSize, TInt aProsentages, const TBool& aReverceColor) const;

private:
	CEikButtonGroupContainer* 		iCba;
	TInt 							iResId;
	CAknsBasicBackgroundControlContext*	iBgContext; 
	TBool							iServerIsOn;
	RPointerArray<CExampleItem>		iArray;
	TInt 							iTotalRamInBytes;
	CGulIcon* 						iMaskImg;
	CCPUControl*					iCPUControl;
	CMemControl*					iMemControl;
	CTimeOutTimer*					iTimeOutTimer;
	TUint32 						iMaxCpuCount;
	CProfileSettings*				iProfileSettings;
	CMyHelpContainer*				iMyHelpContainer;
};



#endif // __YUCCATOOLS_BASICAPPSTUFF_H__

