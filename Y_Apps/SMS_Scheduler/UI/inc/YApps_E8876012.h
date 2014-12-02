/* 
	Copyright (c) 2001 - 2008, 
	Dr. Jukka Silvennoinen. 
	All rights reserved 
*/
#ifndef __MG_H__
#define __MG_H__

#if !defined(__AKNVIEW_H__)
#include <aknview.h>                        // for CAknView
#endif

#if !defined(__AKNVIEWAPPUI_H__)
#include <aknviewappui.h>                   // for CAknViewAppUi
#endif

#if !defined(__EIKAPP_H__) 
#include <eikapp.h>                         // for CEikApplication
#endif

#if !defined(__EIKDOC_H__)
#include <eikdoc.h>                         // for CEikDocument
#endif

#if !defined(__MSVAPI_H__)
#include <msvapi.h>                         // for MMsvSessionObserver 
#endif

#if !defined(__MMSCLIENT_H__)
#include <mmsclient.h>                      // for CMmsClientMtm
#endif

#include <eikdoc.h>                         // for CEikDocument
#include <AknDoc.h>                        // for CEikApplication
#include <aknapp.h>
#include <cntdef.h> 
#include <eikmenub.h> 
#include <akntitle.h> 
#include <AknProgressDialog.h>
#include <aknwaitdialog.h> 

#include "CTimeOutTimer.h"
#include "ContactReader.h"

class CMySplashScreen;
class CMainContainer;

// Application Ui class, this contains all application functionality
//
class CMgAppUi : public CAknAppUi,MTimeOutNotify, MContactReadUpdate
    {
public:
    void ConstructL();
    ~CMgAppUi();
	CMgAppUi();
    void HandleCommandL(TInt aCommand);
  	void ShowNoteL(const TDesC& aMessage);
public:
	void GetRepeatTypeName(TInt aTypeId,TDes& aName);
	
	static TInt OpenMobileWEBSiteL(TAny* aAny);
	TBool GetContactsArrayL(RPointerArray<CMyCItem>& aArray);
protected:
	void TimerExpired(void);
	void ReadProgress(TInt aCurrent, TInt aCount);
private:
	void DoExitChecksNowL(void);
	void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);
private:
	CMainContainer*		iMainContainer;
	CTimeOutTimer*		iTimeOutTimer;
	CMySplashScreen*	iMySplashScreen;
	CContactsReader*	iContactsReader;
    };

//
// Application document class
//
class CMgDocument : public CAknDocument//CEikDocument
    {
public:
    static CMgDocument* NewL(CEikApplication& aApp);
    CEikAppUi* CreateAppUiL();
    CFileStore* OpenFileL(TBool aDoOpen, const TDesC& aFilename,RFs& aFs);
private:
    CMgDocument(CEikApplication& aApp);
    void ConstructL();
private:
	CMgAppUi* iMyAppUI;
    };


// Application class
//
class CMgApplication : public CAknApplication
    {
public:
    TUid AppDllUid() const;
private:
    CApaDocument* CreateDocumentL();
    };


#endif // __MG_H__
