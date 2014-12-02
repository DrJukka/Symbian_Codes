/* 
	Copyright (c) 2006-2007, 
	Jukka Silvennoinen
	All rights reserved 
*/
#ifndef __YBROWSER_SELECTIONCONTAINER_H__
#define __YBROWSER_SELECTIONCONTAINER_H__


#include <coecntrl.h>
#include <AknProgressDialog.h>
#include <aknlists.h> 
#include <aknsfld.h> 

#include "Folder_reader.h"
#include "FileManHandler.h"
#include "IconHandler.h"
#include "Yucca_Settings.h"
	
/*! 
  @class CSelectContainer
  
  @discussion An instance of the Application View object for the HelloWorld 
  example application
  */
   
  
class CSelectContainer : public CCoeControl,MMyFolderReaderObserver
,MProgressDialogCallback 
    {
public:
    static CSelectContainer* NewL(CFFolderReader& aReader,MYBrowserFileHandlerUtils& aUtils,TSettingsItem aSettings,TBool aFolderSel,const TDesC& aFolder,const TDesC& aTitle,CDesCArrayFlat* aTypeFilter);
    static CSelectContainer* NewLC(CFFolderReader& aReader,MYBrowserFileHandlerUtils& aUtils,TSettingsItem aSettings,TBool aFolderSel,const TDesC& aFolder,const TDesC& aTitle,CDesCArrayFlat* aTypeFilter);
    void SetCallBack(MYbSelectorCallBack* aCallBack);
    ~CSelectContainer();
public:  
	void PageUpDownL(TBool aUp);
	void MarkUnMarkL(TBool aMarkAll, TBool aJustTogle);
	void InverseSelectionL(void);
	void SelectionWithWildL(const TDesC& aWild);
	TBool UpdateCallBackL(TBool aCancel);
	CDesCArray* PasteArray(void){return iPasteArray;};
	
	void InitMenuPanelL(TInt aResourceId, CEikMenuPane* aMenuPane);
	void SetMenuL(void);
	
	void AddSelectedFilesL(void);
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
	
	void HandleViewCommandL(TInt aCommand);
	
	void FolderReadOnProcessL(TInt aReadCount,TInt aFullCount, TInt aError);
	void FolderReadFinnishL(CFFolderReader* aObject, TInt aError);

protected:
	void DialogDismissedL (TInt aButtonId);
private:
	void StartReadingFolder(const TDesC& aFileName);
	void SetNaviTextL(void);
	void ChangeViewTypeL(void);
	void GetTimeBuffer(const TDateTime& aDateTime,TDes& aBuffer,TBool aDate,TBool aList);
	void Draw(const TRect& aRect) const;
	TInt CountComponentControls() const;
	CCoeControl* ComponentControl(TInt aIndex) const;
	void DisplayListBoxL(void);
	void UpdateScrollBar(CEikListBox* aListBox);
	virtual void SizeChanged();
	virtual void HandleResourceChange(TInt aType);
	void GetFileType(TInt& aTypeId, TInt aIndex);
	TBool AppendFileName(TDes& aFileName, TInt aIndex);
    void ConstructL(const TDesC& aFolder,const TDesC& aTitle,CDesCArrayFlat* aTypeFilter);
    CSelectContainer(CFFolderReader& aReader,MYBrowserFileHandlerUtils& aUtils,TSettingsItem aSettings,TBool aFolderSel);
private:
	CFFolderReader&					iFFolderReader;
	MYBrowserFileHandlerUtils& 		iUtils;
	TSettingsItem 					iSettings;	
	MYbSelectorCallBack* 			iCallBack;
	CEikTextListBox* 				iSelectionBox;
	CAknProgressDialog* 			iProgressDialog;
	CEikProgressInfo* 				iProgressInfo;
	TFileName						iLastFolder;
	CDesCArrayFlat* 				iPasteArray;
	TBool							iExtendedInfoOn,iFolderSel;	
    };


#endif // __YBROWSER_SELECTIONCONTAINER_H__
