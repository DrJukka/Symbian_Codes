/* 	
	Copyright (c) 2001 - 2007, 
	J.P. Silvennoinen.
	All rights reserved 
*/
#ifndef __MG_H__
#define __MG_H__


#if !defined(__EIKAPP_H__) 
#include <eikapp.h>                         // for CEikApplication
#endif

#if !defined(__EIKDOC_H__)
#include <eikdoc.h>                         // for CEikDocument
#endif

#if !defined(__MSVAPI_H__)
#include <msvapi.h>                         // for MMsvSessionObserver 
#endif


#ifdef __SERIES60_3X__
	#include <AknProgressDialog.h>
	#include <aknwaitdialog.h> 
	#include <akntitle.h> 
	#include <aknapp.h>
	#include <akndoc.h>
	#include <aknviewappui.h> 
#else
	#include <QikApplication.h>
	#include <QikDocument.h>
	#include <QikAppUi.h>
#endif //  __SERIES60_3X__

#include <eikdoc.h>                         // for CEikDocument
#include <cntdef.h> 
#include <eikmenub.h> 

	
const TUid		KUidTOneViewer = { 0xA000257B};


class CMySplashScreen;
class CMainContainer;



// Application Ui class, this contains all application functionality
//
#ifdef __SERIES60_3X__
class CMgAppUi : public CAknAppUi
#else
class CMgAppUi : public CQikAppUi
#endif
    {
public:
    void ConstructL();
    ~CMgAppUi();
	CMgAppUi();
    void HandleCommandL(TInt aCommand);
  	void ShowNoteL(const TDesC& aMessage);
	void HandleForegroundEventL(TBool aForeground);
private:
	void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);
private:
	CMainContainer*		iMainContainer;

    };

//
// Application document class
//
#ifdef __SERIES60_3X__
class CMgDocument : public CAknDocument
#else
class CMgDocument : public CQikDocument
#endif
    {
public:
    static CMgDocument* NewL(CEikApplication& aApp);
    CEikAppUi* CreateAppUiL();
private:
    CMgDocument(CEikApplication& aApp);
    void ConstructL();
private:
    };


// Application class
//
#ifdef __SERIES60_3X__
class CMgApplication : public CAknApplication
#else
class CMgApplication : public CQikApplication
#endif
    {
public:
    TUid AppDllUid() const;
private:
    CApaDocument* CreateDocumentL();
    };


#endif // __MG_H__
