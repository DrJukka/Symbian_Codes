/* Copyright (c) 2001 - 2005 , Solution One Telecom LTd. All rights reserved */

#ifndef __SCCOPY_APPU_H__
#define __SCCOPY_APPU_H__


#include <AknAppUi.h>

#include <CNTDEF.H>
#include <AknProgressDialog.h>
#include <eikprogi.h>
#include <eikapp.h>                         // for CEikApplication
#include <aknapp.h> 
#include <eikdoc.h>                         // for CEikDocument
#include <AknDoc.h> 
#include <DocumentHandler.h> 
#include <aknwaitdialog.h> 
#include <AknProgressDialog.h>

#include "MainContainer.h"



const TUid KUidPhoneManager				= { 0x20007145 }; 

_LIT(KtxAppVersion					,"0.10");
_LIT(KtxtApplicationName			,"Y-Filehandler");
_LIT(KtxAboutText					,"version 0.10\nSeptember 24th 2007\nfor S60, Symbian OS 9,\n\nCopyright:\nJukka Silvennoinen 2006-07\nAll right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nwww.DrJukka.com");


// Application Ui class, this contains all application functionality
//



class CMgAppUi : public CAknAppUi
    {
public:
    void ConstructL();
    ~CMgAppUi();
	void GlobalMsgNoteL(const TDesC&  aMessage);
	void SetFileData(TFileName& aFileName, TDes8& aData);
public:
	void OpenFileL( const TDesC& aFileName );  
    void HandleCommandL(TInt aCommand);
    void HandleResourceChangeL(TInt aType);
    TBool ProcessCommandParametersL(TApaCommand aCommand,TFileName& aDocumentName);
    TBool ProcessCommandParametersL(TApaCommand aCommand,TFileName& aDocumentName,const TDesC8& aTail);      
	void HandleForegroundEventL(TBool aForeground);
private:
	void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);
private:
	CMainContainer* 		iMainContainer;
	
    };



//
// Application Ui class, this contains all application functionality
//
class CHandlerDocument : public CAknDocument
    {
public:
    static CHandlerDocument* NewL(CEikApplication& aApp);
    static CHandlerDocument* NewLC(CEikApplication& aApp);
    ~CHandlerDocument();
public: // from CAknDocument
    CEikAppUi* CreateAppUiL();    
public:	// from CEikDocument
    CFileStore* OpenFileL(TBool aDoOpen,const TDesC& aFilename,RFs& aFs);    
    void OpenFileL(CFileStore*& aFileStore, RFile& aFile);
private:
    void ConstructL();
    CHandlerDocument(CEikApplication& aApp);   
private:
    CEikAppUi* iAppUi;   
    TFileName iFileName;
    };
//
// Application Ui class, this contains all application functionality
//
class CHandlerApplication : public CAknApplication
    {
public:  // from CAknApplication
    TUid AppDllUid() const;
protected: // from CAknApplication
    CApaDocument* CreateDocumentL();
    };

#endif // __SCCOPY_APPU_H__
