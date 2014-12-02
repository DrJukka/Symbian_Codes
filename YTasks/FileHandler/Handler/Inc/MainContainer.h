/*
	Copyright (C) 2006 Jukka Silvennoinen
	All right reserved
*/

#ifndef __YFILEHAND_MAINCONTAINER_H__
#define __YFILEHAND_MAINCONTAINER_H__

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
#include <maknfilefilter.h> 
#include <akncommondialogs.h> 

class CMgAppUi;

_LIT(KtxTargetCDrive		,"C:\\Data\\YFiles\\");
_LIT(KtxTargetEDrive		,"E:\\Data\\YFiles\\");

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
class CMainContainer : public CCoeControl,MProgressDialogCallback
    {
public:
	~CMainContainer();
    void ConstructL(CEikButtonGroupContainer* aCba,CMgAppUi* iAppUi);
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
	CCoeControl* ComponentControl( TInt aIndex) const;
	TInt CountComponentControls() const;
	void HandleCommandL(TInt aCommand);
	void SetMenuL(void);
	void InitMenuPanelL(TInt aResourceId, CEikMenuPane* aMenuPane);
	void ForegroundChangedL(TBool aForeground);
	void GlobalMsgNoteL(const TDesC&  aMessage);	
	void OpenFileL(const TDesC& aFileName);
protected:
    void DialogDismissedL (TInt aButtonId);
	TTypeUid::Ptr MopSupplyObject(TTypeUid aId);
private:
	TBool SelectTargetL(TDes& aTarget);
	virtual void SizeChanged();
	virtual void HandleResourceChange(TInt aType);
	void Draw(const TRect& aRect) const;
private:
	CEikButtonGroupContainer* 		iCba;
	CMgAppUi* 						iAppUi;
	CAknsBasicBackgroundControlContext*	iBgContext; 
	CAknProgressDialog* 			iProgressDialog;
	CEikProgressInfo* 				iProgressInfo;
	TFileName						iTmpFile;
    };



#endif // __YFILEHAND_MAINCONTAINER_H__

