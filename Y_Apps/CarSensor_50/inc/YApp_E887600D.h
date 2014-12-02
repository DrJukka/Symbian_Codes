/*
============================================================================

============================================================================
*/

#ifndef __MOVINGBALLAPPUI_h__
#define __MOVINGBALLAPPUI_h__

// INCLUDES
#include <aknappui.h>
#include <akndoc.h>
#include <aknapp.h>

#include "CTimeOutTimer.h"
#include "Definitions.h"

// FORWARD DECLARATIONS
class CMovingBallAppView;


#ifdef SONE_VERSION
	static const TUid KUidYBrowserApp = {0x20028857};
	
	_LIT(KtxImageFileName		,"\\resource\\apps\\YApp_20028857.mif");
	
	_LIT(KtxDisclaimerFileName	,"\\private\\20028857\\NoDisclaimer");
	
	_LIT(KtxApplicationName		,"Inclinometer");
	_LIT(KtxDisclaimerTitle		,"Welcome: Y-Inclinometer");
	_LIT(KtxDisclaimer			,"This application may or may not function properly, also this application is not a safety application, any values shown in the application can be wrong at any time.\n\nBy pressing Ok/Yes, you will take all responsibility of using this application or any other product used with/by this application and accept that developer of this software or any other party will not give any warranty nor compensate any loss or damage which ever way caused by usage of this application or any other application\nAlso you by pressing Ok, you acknowledge that all rights are reserved by the author of this software and these rights also include any distribution rights, thus you are not allowed to re-distribute any parts of this software without written approval from Dr. Jukka Silvennoinen\n\nAlso note that visiting DrJukka.com with web browser will require internet connection and standard internet connection charges will apply\n");
	
	_LIT(KtxAbbout				,"version 1.10(0)\nApril 24th 2010\nfor S60, Symbian OS 9,\n\nCopyright Jukka Silvennoinen 2006-10, All right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nm.DrJukka.com \ndesktop:\n www.DrJukka.com\nGraphics by P@sko\nwww.jaspp.net\n");
	_LIT(KMobileJukkaLink		,"m.DrJukka.com");
#else

	#ifdef LAL_VERSION
		static const TUid KUidYBrowserApp = {0xE887600D};
		
		_LIT(KtxImageFileName		,"\\resource\\apps\\YApp_E887600D.mif");
		
		_LIT(KtxDisclaimerFileName	,"\\private\\E887600D\\NoDisclaimer");
		
		
		_LIT(KtxApplicationName		,"Inclinometer");
		_LIT(KtxDisclaimerTitle		,"Welcome: Y-Inclinometer");
		_LIT(KtxDisclaimer		,"This software is freeware, so make sure you are not asked to pay to obtain this application.\n\nThis application comes with no warranty what so ever, thus you are using this software completely with your own risk.\n\nThis application may or may not function properly, also this application is not a safety application, any values shown in the application can be wrong at any time.\n\nBy pressing Ok/Yes, you will take all responsibility of using this application or any other product used with/by this application and accept that developer of this software or any other party will not give any warranty nor compensate any loss or damage which ever way caused by usage of this application or any other application\nAlso you by pressing Ok, you acknowledge that all rights are reserved by the author of this software and these rights also include any distribution rights, thus you are not allowed to re-distribute any parts of this software without written approval from Dr. Jukka Silvennoinen\n\n");
		
		
		
		_LIT(KtxAbbout			,"version 1.00(2)\nOctober 31st 2009\nfor S60, Symbian OS 9,\n\nCopyright Jukka Silvennoinen 2006-09, All right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nm.DrJukka.com \ndesktop:\n www.DrJukka.com\nGraphics by P@sko\nwww.jaspp.net\n");
		_LIT(KMobileJukkaLink	,"m.DrJukka.com");
	#else
		static const TUid KUidYBrowserApp = {0xE887600D};
		
		_LIT(KtxImageFileName		,"\\resource\\apps\\YApp_E887600D.mif");
		
		_LIT(KtxDisclaimerFileName	,"\\private\\E887600D\\NoDisclaimer");
		
		
		_LIT(KtxApplicationName		,"Inclinometer");
		_LIT(KtxDisclaimerTitle		,"Welcome: Inclinometer");
		_LIT(KtxDisclaimer		,"This software is freeware, so make sure you are not asked to pay to obtain this application.\n\nThis application comes with no warranty what so ever, thus you are using this software completely with your own risk.\n\nThis application may or may not function properly, also this application is not a safety application, any values shown in the application can be wrong at any time.\n\nBy pressing Ok/Yes, you will take all responsibility of using this application or any other product used with/by this application and accept that developer of this software or any other party will not give any warranty nor compensate any loss or damage which ever way caused by usage of this application or any other application\nAlso you by pressing Ok, you acknowledge that all rights are reserved by the author of this software and these rights also include any distribution rights, thus you are not allowed to re-distribute any parts of this software without written approval from Dr. Jukka Silvennoinen\n\n");
		
		
		
		_LIT(KtxAbbout			,"version 1.10(2)\nJanuary 29th 2011\nfor S60, Symbian OS 9,\n\nCopyright Jukka Silvennoinen 2006-11, All right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nm.DrJukka.com \ndesktop:\n www.DrJukka.com\nGraphics by P@sko\nwww.jaspp.net\n");
		_LIT(KMobileJukkaLink	,"m.DrJukka.com");
	#endif
#endif




class CMySplashScreen;

// CLASS DECLARATION
/**
*/
class CMovingBallAppUi : public CAknAppUi,MTimeOutNotify
	{
	public: // Constructors and destructor
		void ConstructL();
		CMovingBallAppUi();
		virtual ~CMovingBallAppUi();
	protected:
		void TimerExpired(void);
	private:  // Functions from base classes
		static TInt OpenMobileWEBSiteL(TAny* aAny);
		void HandleCommandL( TInt aCommand );
		void HandleStatusPaneSizeChange();	
		TBool ShowDisclaimerL(void);
		void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);
	private: // Data
		CMovingBallAppView* iAppView;
		CMySplashScreen*	iMySplashScreen;	
		CTimeOutTimer*		iTimeOutTimer;	
		TBool iOkToContinue;
	};


class CMovingBallDocument : public CAknDocument
	{
	public: // Constructors and destructor
		static CMovingBallDocument* NewL( CEikApplication& aApp );
		static CMovingBallDocument* NewLC( CEikApplication& aApp );
		virtual ~CMovingBallDocument();
	public: // Functions from base classes
		CEikAppUi* CreateAppUiL();

	private: // Constructors
		void ConstructL();
		CMovingBallDocument( CEikApplication& aApp );

	};


class CMovingBallApplication : public CAknApplication
	{
	public: // Functions from base classes
		TUid AppDllUid() const;

	protected: // Functions from base classes
		CApaDocument* CreateDocumentL();
	};



#endif // __MOVINGBALLAPPUI_h__

// End of File
