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




_LIT(KtxFormatShortCutlList			,"<%d>: %S");
_LIT(KtxDisclaimerTitle				,"Welcome: Y-Browser");
_LIT(KtxDisclaimer					,"This software is freeware, so make sure you are not asked to pay to obtain this application.\n\nThis application comes with no warranty what so ever, thus you are using this software completely with your own risk.\n\nThis application may or may not function properly, functionalities of this application includes deletion of files and data, these functionalities along side with other functionalities of this application or other modules or applications used with or without this application may cause serious harm to your device and even render it inoperable.\n\nBy pressing Ok/Yes, you will take all responsibility of using this application or any other product used with/by this application and accept that developer of this software or any other party will not give any warranty nor compensate any loss or damage which ever way caused by this application or any other application\n");


class CFileMan;
class CYuccaNavi;
class CShowString;
class CYuccaSettings;
class CIconHandler;
class CCommandSCSettings;
class CSearchSettings;
class CSearchValues;
class CSelectContainer;
/*! 
  @class CFileListContainer
  
  @discussion An instance of the Application View object for the HelloWorld 
  example application
  */
   
  
class CFileListContainer : public CCoeControl,MMyFolderReaderObserver
,MProgressDialogCallback,MMyFileManReaderObserver,MYBrowserFileUtilsHelp
,MYBrowserFileHandlerUtils,MAknServerAppExitObserver 
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
	void SavePathL(void);
	TInt GetSelectedIndex(void);
	TBool AskToConfirmExit(void){return iSettings.iConfirmExit;};
	TBool SendWithSendUIL(void);
protected:
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
	void SelectAndChangeIconL(void);
	void GetTimeBuffer(const TDateTime& aDateTime,TDes& aBuffer,TBool aDate,TBool aList);
	CYBrowserBaseHandler* GetCurrentHandler(void) const;
	void RemoveLastHandler(void);
	void ReFreshListL(TBool aFullReFresh,HBufC* aTmpFolder);
	TBool ShowDisclaimerL(void);
	void ShowInfoL(const TDesC& aFileName,TInt aFileType);
	TKeyResponse SelectAndExecuteSCCommandL(TInt aKey);
	void ChangeViewTypeL(TBool aSearchChange);
	CAknSearchField* CreateFindBoxL(CEikListBox* aListBox,CTextListBoxModel* aModel, CAknSearchField::TSearchFieldStyle aStyle );
	void OpenFileL(const TDesC& aFileName);
	void OpenFileWithL(const TDesC& aFileName);
	void OpenWithApplicationL(const TDesC& aFileName);
	void SaveIconNameL(const TDesC& aFileName);
	void GetIconNameL(TDes& aFileName);
	void GetPathL(TDes& aFolder);
	void UpdateSettingsForReadingL(void);
	void SetSettingsValuesL(void);
	void GetSettingsValuesL(void);
	void GetCommandSCFromStoreL(void);
	void GetValuesL(CDictionaryFileStore* aDStore,TInt aUID,TInt& aValue);
	TInt ShowShortCutListL(void);
	void SetShortCutL(TInt aShortCut);
	TKeyResponse MoveToShortCutL(TInt aShortCut);
	void ShowPasteFilesL(CDesCArray* aArray,const TDesC& aTitle);
	void Draw(const TRect& aRect) const;
	TInt CountComponentControls() const;
	CCoeControl* ComponentControl(TInt aIndex) const;
	void DisplayListBoxL(void);
	void UpdateScrollBar(CEikListBox* aListBox);
	void SizeChangedForFindBox();
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
	CArrayFixFlat<TInt>*			iCommandSCArray;
	CCommandSCSettings*				iCommandSCSettings;	
	CSearchSettings*				iSearchSettings;
	CSearchValues* 					iSearchValues;
	TBool 								iOkToContinue;
	RPointerArray<CYBrowserBaseHandler>	iHandlerArray;
	CSelectContainer*					iSelectContainer;
	HBufC*								iTmpPath;
	CAknsBasicBackgroundControlContext*	iBgContext;
    };


#endif // __FILESELECTION_VIEW_H__
