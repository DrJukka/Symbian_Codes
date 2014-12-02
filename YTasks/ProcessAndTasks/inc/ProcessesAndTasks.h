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

class CYuccaInfo;
class CMyHelpContainer;

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
class CProcessContainer : public CYTasksContainer
    {
public:
	~CProcessContainer();
    void ConstructL(CEikButtonGroupContainer* aCba);
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
	CCoeControl* ComponentControl( TInt aIndex) const;
	TInt CountComponentControls() const;
	void HandleCommandL(TInt aCommand);
	CDesCArrayFlat* GetSelectedArrayL(TBool& aProcess);
	void SetMenuL(void);
	void InitMenuPanelL(TInt aResourceId, CEikMenuPane* aMenuPane);
	void ReFreshL(void);
	void ForegroundChangedL(TBool /*aForeground*/){};
protected:
	TTypeUid::Ptr MopSupplyObject(TTypeUid aId);
private:
	void KillProcessThreadL(TInt aIndex);
	virtual void SizeChanged();
	virtual void HandleResourceChange(TInt aType);
	void ChangeIconAndNameL(void);	
	void ShowDetailsDialogL(TInt aIndex);
	CDesCArrayFlat* GetThreadListL(void);
	CDesCArrayFlat* GetProcessListL(void);
	void SelecUnSelectAllL(TBool aSelect);
	void SelecUnSelectL(TInt aItem);
	void SizeChangedForFindBox();
	CAknSearchField* CreateFindBoxL(CEikListBox* aListBox,CTextListBoxModel* aModel, CAknSearchField::TSearchFieldStyle aStyle);
	void UpdateScrollBar(CEikListBox* aListBox);
	void ConstructListBoxL(CDesCArray* aArray);
	void Draw(const TRect& aRect) const;
private:
	CEikButtonGroupContainer* 		iCba;
	CAknSingleNumberStyleListBox*	iListBox;
	CAknSearchField* 				iFindBox;
	CDesCArrayFlat* 				iProcessArray;
	CDesCArrayFlat* 				iThreadArray;
	TBool							iHasTreadsView;
	CAknTabGroup* 					iTabGroup;
	CAknNavigationControlContainer* iNaviPane;
	CAknNavigationDecorator* 		iNaviDecorator;
	CAknNavigationDecorator* 		iNaviDecoratorForTabs;
	CYuccaInfo*						iYuccaInfo;
	TInt 							iResId;
	CAknsBasicBackgroundControlContext*	iBgContext; 
	TBool							iHasServer;
	CMyHelpContainer*				iMyHelpContainer;
};



#endif // __YUCCATOOLS_BASICAPPSTUFF_H__

