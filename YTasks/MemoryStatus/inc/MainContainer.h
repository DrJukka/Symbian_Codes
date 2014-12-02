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
#include "File_Writer.h" 

class CMyHelpContainer;
//
//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
class CMainContainer : public CYTasksContainer,MProgressDialogCallback,MMemWriteNotify
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
	void DialogDismissedL (TInt aButtonId);
	TTypeUid::Ptr MopSupplyObject(TTypeUid aId);
	void IncrementWriteL(TInt64 aDone, TInt64 aTotal);
	void WriteFinishedL(void);
private:	
	void HandlePointerEventL(const TPointerEvent& aPointerEvent);
	void PieDrawer(CWindowGc& aGc, TPoint aLeftTop, TInt aSize, TInt aProsentages) const;
	void CompDrawer(CWindowGc& aGc, TRect aRect, TInt64 aValue,TInt64 aTotal, TBool aSelected, const TDesC& aText) const;
	void GetDrivesInfoL(void);
	void GetAmountText(TDes& aBuffer,TInt64 aValue,const TDesC& aHeader) const;
	CGulIcon* LoadImageL(const TDesC& aFileName, TInt aImg, TInt aMsk,const TSize& aSize,TScaleMode aAspect);
	void ShowNoteL(const TDesC& aMessage);
	void ReserveRAMMemoryL(TInt aSize);
	void ReserveMemoryL(TDesC& aPath,TInt64 aSize);
	void ClearTempFolderL(const TDesC& aDrive);
	TBool HasTempFilesL(const TDesC& aDrive);
	virtual void SizeChanged();
	virtual void HandleResourceChange(TInt aType);
	void ChangeIconAndNameL(void);
	void Draw(const TRect& aRect) const;
private:
	CEikButtonGroupContainer* 		iCba;
	TInt 							iResId;
	CAknsBasicBackgroundControlContext*	iBgContext; 
	TInt64 iCFree,iCTotal,iDFree,iDTotal,iEFree,iETotal,iFreeRam,iTotalRam;
	TInt 				iSelected,iTempFiles;
	TRgb 				iTextColor;
	CGulIcon* 			iBgSelImg;
	HBufC8*				iRamBuffer;
	CAknProgressDialog* iProgressDialog;
	CEikProgressInfo*   iProgressInfo;
	CFileWriter*		iFileWriter;
	TBool				iCHasTmp,iDHasTmp,iEHasTmp;
	CMyHelpContainer*	iMyHelpContainer;
};



#endif // __YUCCATOOLS_BASICAPPSTUFF_H__

