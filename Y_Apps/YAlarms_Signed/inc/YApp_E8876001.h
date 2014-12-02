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

#include "CTimeOutTimer.h"

#include "YApp_E8876001.mig"

#ifndef __YUCCA_VERSION__
#define __YUCCA_VERSION__
#endif // __YUCCA_VERSION__


#include "Definitions.h"

#ifdef SONE_VERSION
	const TInt KFrameReadInterval = 5000000;
	_LIT(KtxIconFileName				,"\\resource\\apps\\YApp_2002DD36.mif");
	_LIT(KtxIconFileName2				,"\\resource\\apps\\YApp_2002DD36.mif");
	_LIT(KtxDisclaimerFileName			,"\\private\\2002DD36\\NoDisclaimer");
	
	_LIT(KtxApplicationName				,"Y-Alarms");
	_LIT(KtxDisclaimerTitle				,"Welcome: Y-Alarms");
	_LIT(KtxDisclaimer					,"Visiting DrJukka.com with web browser will require internet connection and standard internet connection charges will apply\n\nBy pressing Ok/Yes, you will agree on that\n");

	
	static const TUid KUidYBrowserApp = {0x2002DD36};
#else

	#ifdef LAL_VERSION

	#else

		const TInt KFrameReadInterval = 5000000;
		_LIT(KtxIconFileName			,"\\resource\\apps\\YApp_E8876001.mif");
		
		
		
		
		
		_LIT(KtxIconFileName2				,"\\resource\\apps\\YApp_E8876001.mif");
		_LIT(KtxDisclaimerFileName			,"\\private\\E8876001\\NoDisclaimer");
		
		_LIT(KtxApplicationName				,"Y-Alarms");
		_LIT(KtxDisclaimerTitle				,"Welcome: Y-Alarms");
		_LIT(KtxDisclaimer					,"This software is freeware, so make sure you are not asked to pay to obtain this application.\n\nThis application comes with no warranty what so ever, thus you are using this software completely with your own risk.\n\nThis application may or may not function properly, functionalities of this application includes setting, modifying and deleting user data including system wide alarms, these functionalities along side with other functionalities of this application or other modules or applications used with or without this application may cause serious harm to your device and even render it inoperable.\n\nBy pressing Ok/Yes, you will take all responsibility of using this application or any other product used with/by this application and accept that developer of this software or any other party will not give any warranty nor compensate any loss or damage which ever way caused by this application or any other application\nAlso you by pressing Ok, you acknowledge that all rights are reserved by the author of this software and these rights also include any distribution rights, thus you are not allowed to re-distribute any parts of this software without written approval from Dr. Jukka Silvennoinen\n\n");
		
		
		static const TUid KUidYBrowserApp = {0xE8876001};
	#endif
#endif



class CSendUi;
class CItemsContainer;
class CMySplashScreen;
/*! 
--------------------------------------------------------------------------------
  */
class CYBrowserAppUi : public CAknAppUi,MTimeOutNotify
    {
public:
    void ConstructL();
    CYBrowserAppUi();
    ~CYBrowserAppUi();
public: // from CAknAppUi
    void HandleCommandL(TInt aCommand);
    void HandleForegroundEventL(TBool aForeground);
protected:
	void TimerExpired();
private:
	TBool ShowDisclaimerL(void);
	void FinalizeMeL(void);
private:
    CItemsContainer*		iItemsContainer;
    CTimeOutTimer*			iTimeOutTimer;
	CMySplashScreen*		iMySplashScreen;
	TInt 					iTimeCount;
	TBool					iOkToContinue;
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

