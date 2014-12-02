/* Copyright (c) 2001 - 2003 , J.P. Silvennoinen. All rights reserved */

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
#include <aknssrvclient.h>
#include <AknSkinsInternalCRKeys.h>
#include <AknSettingItemList.h>

#include "Common.h"
#include "CTimeOutTimer.h"
#include "Ps_Observer.h"

class CMainContainer;
class CMySplashScreen;


//
// Application Ui class, this contains all application functionality
//
class CMgAppUi : public CAknAppUi,MTimeOutNotify,MMyPsObsCallBack
    {
public:
    void ConstructL();
    ~CMgAppUi();
	CMgAppUi();
    void HandleCommandL(TInt aCommand);
 	void ShowNoteL(const TDesC& aMessage);
 	void GetRepeatTypeName(TInt aTypeId,TDes& aName);
	void GetThemeNameL(TUint32 aTypeId,TDes& aName,const TInt& aIndex);
	void GetProfileNameL(TInt aTypeId,TDes& aName);
	MProfileEngine* ProfileEngine(){return iProfileEngine;};
	RAknsSrvSession& AknsSrvSession(){ return iAknsSrvSession;};
	static TInt OpenMobileWEBSiteL(TAny* aAny);
protected:
	TBool ShowDisclaimerL(void);
	void TimerExpired();
	void PsValueChangedL(const TDesC8& /*aValue*/,TInt /*aError*/){};
	void PsValueChangedL(TInt /*aValue*/,TInt /*aError*/){};
private:
	void OpenOviSiteL(const TDesC& aUrl);
	TBool IsDrawerOn(void);
	void DoExitChecksNowL(void);
	void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);
private:
	CMainContainer* 	iMainContainer;
	CTimeOutTimer*		iTimeOutTimer;
	CMySplashScreen*	iMySplashScreen;
	MProfileEngine* 	iProfileEngine;
	RAknsSrvSession		iAknsSrvSession;
	CmyPsObserver*		imyPsObserver;
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
