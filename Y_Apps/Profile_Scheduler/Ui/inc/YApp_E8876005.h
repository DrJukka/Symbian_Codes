/* Copyright (c) 2001 - 2008, Dr. Jukka Silvennoinen. All rights reserved */

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
#include <cprofilechangenotifyhandler.h>
#include <mprofile.h>
#include <mprofilechangeobserver.h>
#include <mprofileengine.h>
#include <mprofileextratones.h>
#include <mprofilename.h>
#include <mprofilesnamesarray.h>
#include <mprofiletones.h>
#include <profile.hrh>
#include <tprofiletonesettings.h>


#include "CTimeOutTimer.h"
#include "Common.h"


class CMainContainer;
class CMySplashScreen;


//
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
 	static TInt OpenMobileWEBSiteL(TAny* aAny);
public:
	void GetRepeatTypeName(TInt aTypeId,TDes& aName);
	void GetProfileNameL(TInt aTypeId,TDes& aName);
//	void GetProfileNameAndRingToneL(TInt aTypeId,TDes& aName,TDes& aRingtone);
	MProfileEngine* ProfileEngine(){return iProfileEngine;};
protected:
	TBool ShowDisclaimerL(void);
	void TimerExpired();
private:
	void OpenOviSiteL(const TDesC& aUrl);
	void DoExitChecksNowL(void);
	TBool IsDrawerOn(void);
	void ChangeViewL(TInt aCommand);
	void FinalizeConstructL(void);
	void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);
private:
	CMainContainer* 	iMainContainer;
	CTimeOutTimer*		iTimeOutTimer;
	CMySplashScreen*	iMySplashScreen;
	MProfileEngine* 	iProfileEngine;
	TBool				iOkToContinue;
};

//
// Application document class
//
class CMgDocument : public CAknDocument//CEikDocument
    {
public:
    static CMgDocument* NewL(CEikApplication& aApp);
    CEikAppUi* CreateAppUiL();
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
