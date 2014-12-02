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
#include "Common.h"



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
	void LogToFileL(const TDesC8& aStuff);
	void LogToFileL(const TDesC& aStuff);
	static TInt OpenMobileWEBSiteL(TAny* aAny);
protected:
	void NotifyExit(TExitMode aMode);
private:
	void SelectAndChangeIconL(void);
private:
    CFileListContainer* 	iFileView;
	CSendUi* 				iSendUi;
	CApaDocument* 			iHandlerDoc;
	RFile					iDebugFile;
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

