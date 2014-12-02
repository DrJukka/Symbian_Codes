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



#include <AknProgressDialog.h>
#include <aknwaitdialog.h> 
#include <akntitle.h> 
#include <aknapp.h>
#include <akndoc.h>
#include <aknviewappui.h> 


#include <eikdoc.h>                         // for CEikDocument
#include <cntdef.h> 
#include <eikmenub.h> 

#include "Common.h"
#include "CTimeOutTimer.h"
	
class CMySplashScreen;
class CMainContainer;



// Application Ui class, this contains all application functionality
//
class CMgAppUi : public CAknAppUi,MTimeOutNotify
    {
public:
    void ConstructL();
    ~CMgAppUi();
	CMgAppUi();
    void HandleCommandL(TInt aCommand);
  	void ShowNoteL(const TDesC& aMessage);
	void HandleForegroundEventL(TBool aForeground);
	static TInt OpenMobileWEBSiteL(TAny* aAny);
	TBool ShowDisclaimerL(void);
protected:
	void TimerExpired(void);
private:
	void ChangeView(TInt aCommand, TInt aExtra);
	void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);
private:
	CMainContainer*		iMainContainer;
	CTimeOutTimer*		iTimeOutTimer;
	CMySplashScreen*	iMySplashScreen;
	TBool				iOkToContinue;
    };

//
// Application document class
//

class CMgDocument : public CAknDocument
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

class CMgApplication : public CAknApplication
    {
public:
    TUid AppDllUid() const;
private:
    CApaDocument* CreateDocumentL();
    };


#endif // __MG_H__
