/* 
	Copyright (c) 2006, 
	Jukka Silvennoinen
	All rights reserved 
*/

#ifndef __YUCCABROWSER_H__
#define __YUCCABROWSER_H__

#include <aknapp.h>
#include <akndoc.h>
#include <aknappui.h>


#ifndef __YTOOLS_SIGNED
#define __YTOOLS_SIGNED
#endif


#ifdef __YTOOLS_SIGNED
	_LIT(KtxApplicationName				,"Y-FBrowser");
#else
	_LIT(KtxApplicationName				,"Y-Browser");
#endif

#ifdef __YTOOLS_SIGNED
	_LIT(KtxCommandSCFileName			,"\\private\\2000713D\\CommandSC");
	_LIT(KtxShortCutFileName			,"\\private\\2000713D\\Shortcuts");
	_LIT(KtxSettingsFileName			,"\\private\\2000713D\\Settings");
	_LIT(KtxDisclaimerFileName			,"\\private\\2000713D\\NoDisclaimer");
	_LIT(KtxPathSaveFileName			,"\\private\\2000713D\\Path");
	_LIT(KtxIconFileName				,"\\private\\2000713D\\Iccon");
	
	_LIT(KtxImagesFileName				,"\\system\\data\\A00007A6\\Img\\Original2.mif");
#else
	_LIT(KtxCommandSCFileName			,"\\private\\A00007A6\\CommandSC");
	_LIT(KtxShortCutFileName			,"\\private\\A00007A6\\Shortcuts");
	_LIT(KtxSettingsFileName			,"\\private\\A00007A6\\Settings");
	_LIT(KtxDisclaimerFileName			,"\\private\\A00007A6\\NoDisclaimer");
	_LIT(KtxPathSaveFileName			,"\\private\\A00007A6\\Path");	
	_LIT(KtxIconFileName				,"\\private\\A00007A6\\Iccon");

	_LIT(KtxImagesFileName				,"\\system\\data\\A00007A6\\Img\\Original.mif");
#endif

_LIT(KTxtMifExtension				,".mif");
_LIT(KTxtIconsFolder				,"\\system\\data\\A00007A6\\Icons\\");



#ifdef __YTOOLS_SIGNED
	static const TUid KUidYBrowserApp = {0x2000713D};
#else
	static const TUid KUidYBrowserApp = {0xA00007A6};
#endif

#include <eikstart.h>
#include <SendUiConsts.h>
#include <cmessagedata.h>


// Forward reference
class CFileListContainer;
class CSendUi;

/*! 
--------------------------------------------------------------------------------
  */
class CYBrowserAppUi : public CAknAppUi,MApaEmbeddedDocObserver
    {
public:
    void ConstructL();
    CYBrowserAppUi();
    ~CYBrowserAppUi();
	void RemoveAllViewsL(void);
	void HandleViewSwitchL(TInt aCommand);
	void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);
	void CreateAndSendMessageL(void);
public: // from CAknAppUi
    void HandleCommandL(TInt aCommand);
	void HandleStatusPaneSizeChange();
protected:
	void NotifyExit(TExitMode aMode);
private:
	void SelectAndChangeIconL(void);
private:
    CFileListContainer* 	iFileView;
	CSendUi* 				iSendUi;
	CApaDocument* 			iHandlerDoc;
    };
/*! 
--------------------------------------------------------------------------------
  */
class CYBrowserDocument : public CAknDocument
    {
public:
    static CYBrowserDocument* NewL(CEikApplication& aApp);
    static CYBrowserDocument* NewLC(CEikApplication& aApp);
    ~CYBrowserDocument();
public: // from CAknDocument
    CEikAppUi* CreateAppUiL();
private:
    void ConstructL();
    CYBrowserDocument(CEikApplication& aApp);
    };  
/*! 
--------------------------------------------------------------------------------
  */
  
 class CYBrowserApplication : public CAknApplication
    {
public:  // from CAknApplication
    TUid AppDllUid() const;
protected: // from CAknApplication
    CApaDocument* CreateDocumentL();
    }; 

#endif // __YUCCABROWSER_H__

