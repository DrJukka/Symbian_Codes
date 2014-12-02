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
#include <maknfilefilter.h> 
#include <akncommondialogs.h> 

#include "YTasks_PublicInterface.h"
#include "FileReader.h"

class CMyHelpContainer;

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
class CMainContainer : public CYTasksContainer
,MProgressDialogCallback,MMyFileReadObserver,MAknFileFilter
    {
public:
	~CMainContainer();
    void ConstructL(CEikButtonGroupContainer* aCba);
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
	CCoeControl* ComponentControl( TInt aIndex) const;
	TInt CountComponentControls() const;
	void HandleCommandL(TInt aCommand);
	void SetMenuL(void);
	void InitMenuPanelL(TInt aResourceId, CEikMenuPane* aMenuPane);
	void ForegroundChangedL(TBool aForeground);
protected:
	void ReadprocessL(const TDesC& aStuff, TInt aLeft);
	void ReadDoneL(TInt aError);
    void DialogDismissedL (TInt aButtonId);
	TTypeUid::Ptr MopSupplyObject(TTypeUid aId);
	TBool Accept (const TDesC &aDriveAndPath, const TEntry &aEntry) const; 
private:	
	void MakeEdwinL(void);
	TBool AskToSelectReadStyleL(void);
	TBool AskToSelectConverterL(TUint& aConverter);
	void OpenFileL(const TDesC& aFileName);
	void SelectAndSetFontL(void);
	TBool SelectLogicalFontL(void);
	TBool SelectSystemFontL(void);
	TBool SelectFontL(void);
	TBool SelectTypeFaceFontL(void);
	virtual void SizeChanged();
	virtual void HandleResourceChange(TInt aType);
	void ChangeIconAndNameL(void);
	void Draw(const TRect& aRect) const;
private:
	CEikButtonGroupContainer* 		iCba;
	TInt 							iResId;
	CAknsBasicBackgroundControlContext*	iBgContext; 
	CAknProgressDialog* 			iProgressDialog;
	CEikProgressInfo* 				iProgressInfo;
	CFileReader*					iFileReader;
	CEikGlobalTextEditor* 			iEditor;
	CParaFormatLayer* 				iParaformMe;
	CCharFormatLayer* 				iCharformMe;
	CRichText* 						iRichMessageMe;
	TCharFormat						iCharFormat;	
	TCharFormatMask 				iCharFormatMask;	
	CMyHelpContainer*				iMyHelpContainer;
};



#endif // __YUCCATOOLS_BASICAPPSTUFF_H__

