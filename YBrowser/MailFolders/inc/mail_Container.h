/* 
	Copyright (c) 2006, 
	Jukka Silvennoinen
	All rights reserved 
*/
#ifndef __MYMAILBOX_VIEW_H__
#define __MYMAILBOX_VIEW_H__


#include <coecntrl.h>
#include <AknProgressDialog.h>
#include <aknlists.h> 
#include <aknsfld.h> 
#include <msvapi.h> 
#include <AknsBasicBackgroundControlContext.h> 
#include <aknsdrawutils.h>
#include <aknscontrolcontext.h>
#include <AknsSkinInstance.h>
#include <aknsutils.h>

#include <maknfilefilter.h> 

#include "RFileCopier.h"

#include "Public_Interfaces.h"


#include <CMSVATTACHMENT.H>

struct TMyMailEntry
{
	TMsvId				iEnt;
	TMsvId				iMsg;
	TMsvAttachmentId  	iAtt;
};


class CAknTabGroup;
class CAknNavigationControlContainer;
class CAknNavigationDecorator;



	class CMailFldItem :public CBase
		{
	public:
		~CMailFldItem(){delete iTitle;};
		CMailFldItem():iTitle(NULL),iMscId(-1){};
	public:
		HBufC* 	iTitle;
		TMsvId 	iMscId;		
		};
		
/*! 
  @class CMailBoxContainer
  
  @discussion An instance of the Application View object for the HelloWorld 
  example application
  */
class CMailBoxContainer : public CCoeControl,MProgressDialogCallback,MMsvSessionObserver
,MYBrowserRFileCopyObserver,MYbSelectorCallBack
    {
public:
    static CMailBoxContainer* NewL(const TRect& aRect,MYBrowserFileHandlerUtils& aUtils,CYBrowserFileHandler& aHandler);
    static CMailBoxContainer* NewLC(const TRect& aRect,MYBrowserFileHandlerUtils& aUtils,CYBrowserFileHandler& aHandler);
    ~CMailBoxContainer();
public:  
	void InitMenuPanelL(TInt aResourceId, CEikMenuPane* aMenuPane);
	void SetMenuL(void);
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
	void HandleViewCommandL(TInt aCommand);
	void ShowNoteL(const TDesC&  aMessage, TBool aError);
	TInt GetSelectedIndexL(void);
	TInt CountComponentControls() const;
	CCoeControl* ComponentControl(TInt aIndex) const;
protected:
	TTypeUid::Ptr MopSupplyObject(TTypeUid aId);
protected:
	void SelectedFilesL(const CDesCArray& aArray);
	void SelectedFolderL(const TDesC& aFolderName);
	void SelectorCancelledL(void);
protected:
	void RFileCopyProgressL(TInt aProgress,TInt aFinalValue);
	void RFileCopyDoneL(CRFileCopier* aObject,TInt aError);
	virtual void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3);
	void DialogDismissedL (TInt aButtonId);
private:
	void AddEmailFoldersL(CMsvSession* aSession);
	void GetAttachmentsL(MMsvAttachmentManager& attMngr,CMsvEntry& aEntry,TMsvId& aMsvId,TMsvId& aBtAtt);
	TSize GetIconSize(void);
	void CopyFileL(const TDesC& aFolderName);
	void OpenFileL(TBool aOpenWith);
	TMsvId GetFoldersTypeL(CMsvSession* aSession);
	void GetFoldersL(void);
	void ReadMailFolderL(TMsvId aSelected);
	void ChangeViewTypeL(void);
	void Draw(const TRect& aRect) const;
	void DisplayListBoxL(void);
	void UpdateScrollBar(CEikListBox* aListBox);
	virtual void SizeChanged();
	virtual void HandleResourceChange(TInt aType);
	void GetFileTypeL(const TDesC& aFileName, TDes& aType);
	void GetFileTypeL(TInt& aTypeId, TInt aIndex);
    void ConstructL(const TRect& aRect);
    CMailBoxContainer(MYBrowserFileHandlerUtils& aUtils,CYBrowserFileHandler& aHandler);
	void RemoveNavipanel(void);
	void MakeNavipanelL(void);
private:

	MYBrowserFileHandlerUtils& 			iUtils;
	CYBrowserFileHandler& 				iHandler;
	
	CAknDoubleGraphicStyleListBox* 		iSelectionBox;
	CAknProgressDialog* 				iProgressDialog;
	CEikProgressInfo* 					iProgressInfo;

	CArrayFixFlat<TMyMailEntry>*		iIDArray;
	
	TMsvId								iCurrentMailBox;		
	CMsvSession* 						iSession;
	TFileName							iCurrPath;
	CRFileCopier*						iRFileCopier;
	CAknTabGroup* 						iTabGroup;
	CAknNavigationControlContainer* 	iNaviPane;
	CAknNavigationDecorator* 			iNaviDecorator;
	CAknNavigationDecorator* 			iNaviDecoratorForTabs;
	RPointerArray<CMailFldItem> 		iFolderArray;
	TInt 								iCurrentFolder;
	CAknsBasicBackgroundControlContext*	iBgContext;
	TInt 								iSelCopyItemIndex;
    
    CDesCArrayFlat* 					iListArray;
    };


#endif // __MYMAILBOX_VIEW_H__
