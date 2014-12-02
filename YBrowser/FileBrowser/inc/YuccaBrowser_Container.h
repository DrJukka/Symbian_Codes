/* 
	Copyright (c) 2006, 
	Jukka Silvennoinen
	All rights reserved 
*/
#ifndef __FILESELECTION_VIEW_H__
#define __FILESELECTION_VIEW_H__


#include <coecntrl.h>
#include <AknProgressDialog.h>
#include <aknlists.h> 
#include <aknsfld.h> 
#include <AknsBasicBackgroundControlContext.h> 
#include <aknsdrawutils.h>
#include <aknscontrolcontext.h>
#include <AknsSkinInstance.h>
#include <aknsutils.h>

#include "YuccaBrowser.h" //__YTOOLS_SIGNED
#include "Folder_reader.h"
#include "FileManHandler.h"
#include "Yucca_Settings.h"
#include "IconHandler.h"
#include "CTimeOutTimer.h"

class CFileMan;
class CYuccaNavi;
class CShowString;
class CYuccaSettings;
class CIconHandler;

class CSearchSettings;
class CSearchValues;
class CSelectContainer;
class CGeneralSettings;
class CSettingsContainer;
class CMyHelpContainer;

/*! 
  @class CFileListContainer
  
  @discussion An instance of the Application View object for the HelloWorld 
  example application
  */
   
  
class CFileListContainer : public CCoeControl,MMyFolderReaderObserver
,MProgressDialogCallback,MMyFileManReaderObserver,MYBrowserFileUtilsHelp
,MYBrowserFileHandlerUtils,MAknServerAppExitObserver,MTimeOutNotify 
    {
public:
    static CFileListContainer* NewL(const TRect& aRect,CEikButtonGroupContainer* aCba);
    static CFileListContainer* NewLC(const TRect& aRect,CEikButtonGroupContainer* aCba);
    ~CFileListContainer();
public:  
	void InitMenuPanelL(TInt aResourceId, CEikMenuPane* aMenuPane);
	void SetMenuL(void);
	void AddSelectedFilesL(CDesCArray* aArray,TBool AlsoFolders);
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
	void HandleViewCommandL(TInt aCommand);
	void ShowNoteL(const TDesC&  aMessage, TBool aError);
	TInt GetSelectedIndex(void);
	TBool AskToConfirmExit(void){return iSettings.iConfirmExit;};
	TBool SendWithSendUIL(void);
	void SavePathL(void);
protected:
    void TimerExpired();	
	void HandlePointerEventL(const TPointerEvent& aPointerEvent);
	void HandleServerAppExit(TInt aReason);
	void HandleExitL(TAny* aFileHandler, TInt aChanges);
	CEikButtonGroupContainer& GetCba();
	MYBrowserFileUtils& GetFileUtils();
	MYBrowserIconUtils& GetIconUtils();
protected:
	void AddFileHandler(CYBrowserBaseHandler* aHandler);
	void StartSelectionViewL(MYbSelectorCallBack* aCallback,TBool aFolderSelector,const TDesC& aFolder,const TDesC& aTitle,CDesCArrayFlat* aTypeFilter);
	void SetNaviTextL(const TDesC& aText);
	void GetCurrentPath(TDes& aFullPath);
	void FileManOperatioDoneL(CEikFileHandler* aObject,TInt aOperation, TInt aError,TFileManError aErrorInfo);
	void FolderReadOnProcessL(TInt aReadCount,TInt aFullCount, TInt aError);
	void FolderReadFinnishL(CFFolderReader* aObject, TInt aError);
	void DialogDismissedL (TInt aButtonId);
	TTypeUid::Ptr MopSupplyObject(TTypeUid aId);
private:	
	void ChangeStatusPaneLayoutL(void);
	CYBrowserBaseHandler* GetCurrentHandler(void) const;
	void RemoveLastHandler(void);
	void ReFreshListL(TBool aFullReFresh,HBufC* aTmpFolder);	
	void ChangeViewTypeL(TBool aSearchChange);
	CAknSearchField* CreateFindBoxL(CEikListBox* aListBox,CTextListBoxModel* aModel, CAknSearchField::TSearchFieldStyle aStyle );	
	void Draw(const TRect& aRect) const;
	TInt CountComponentControls() const;
	CCoeControl* ComponentControl(TInt aIndex) const;
	void DisplayListBoxL(void);
	virtual void SizeChanged();
	virtual void HandleResourceChange(TInt aType);
	void GetFileType(TInt& aTypeId, TInt aIndex);
	TBool AppendFileName(TDes& aFileName, TInt aIndex);
    void ConstructL(const TRect& aRect);
    CFileListContainer(CEikButtonGroupContainer* aCba); 
private:
	CEikButtonGroupContainer*		iCba;	
	CEikTextListBox* 				iSelectionBox;//*
	CAknSearchField* 				iFindBox;
	CFFolderReader*					iFFolderReader;
	CAknProgressDialog* 			iProgressDialog;
	CEikProgressInfo* 				iProgressInfo;
	TFileName						iLastFolder;
	CDesCArrayFlat* 				iPasteArray;
	TBool							iIsCutOperation,iDriveReadOnly,iExtendedInfoOn,iSearchOn;
	CEikFileHandler*				iEikFileHandler;
	CDesCArrayFlat* 				iFilemanNameArray;
	TFileName 						iFilemanNameBuf;
	TInt							iFilemanSelIndex;
	CYuccaNavi*						iYuccaNavi;
	CShowString*					iShowString;
	CYuccaSettings*					iYuccaSettings;
	TSettingsItem 					iSettings;
	CIconHandler*					iIconHandler;
	CSearchSettings*				iSearchSettings;
	CSearchValues* 					iSearchValues;
	TBool 								iOkToContinue;
	RPointerArray<CYBrowserBaseHandler>	iHandlerArray;
	CSelectContainer*					iSelectContainer;
	HBufC*								iTmpPath;
	CAknsBasicBackgroundControlContext*	iBgContext;
	CGeneralSettings*				iGeneralSettings;
	CSettingsContainer*				iSettingsContainer;
	CMyHelpContainer*				iMyHelpContainer;
	TPoint     iStartPoint;
	TBool      iDragLeft, iDragRight;
	CTimeOutTimer*                  iTimeOutTimer;
    };


#endif // __FILESELECTION_VIEW_H__
