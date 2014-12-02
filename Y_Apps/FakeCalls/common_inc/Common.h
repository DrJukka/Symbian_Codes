/* Copyright (c) 2001 - 2009, Dr. Jukka Silvennoinen. All rights reserved */


#ifndef __COMMONSTUFF_H__
#define __COMMONSTUFF_H__

#include "Definitions.h"

#ifdef SONE_VERSION
    const TBool                       KAppIsTrial = EFalse;
    _LIT(KtxTrialSMSMessage1           ,"Trial for ");
    _LIT(KtxTrialSMSMessage2           ,", has expired. Please uninstall current version and Download full version from Ovi store: ");
        
    _LIT(KtxTrialOviLink              ,"http://store.ovi.com/content/27588");
      
    _LIT(KtxTrialSMSName              ,"Dr.Jukka");
    _LIT(KtxTrialSMSNumber            ,"m.DrJukka.com");
        
    _LIT(KtxTrialHandFileFileName       ,"\\system\\data\\systemfkksch.ini");
    _LIT(KtxTrialHandFileFileName2      ,"\\system\\data\\systemfkksch.dat");
    
    _LIT(KtxDisclaimerFileName          ,"\\private\\2002B0B1\\NoDisclaimer");
    
    _LIT(KtxDisclaimerTitle             ,"Fake Calls");
    _LIT(KtxDisclaimer                  ,"This version is trial version for 2 days usage. Visit Ovi store for getting the full version. Note also that visiting Ovi store or DrJukka.com with web browser will require internet connection and standard internet connection charges will apply\n\nPress Ok/Yes to continue using this Trial version\n");
        
	//database name
	_LIT(KtxDefSettingsFile		,"\\system\\data\\2002B0B1\\Settings.db");
	_LIT(KtxKeySettingsFile		,"\\system\\data\\2002B0B1\\KeyHandling.db");
	_LIT(KtxDatabaseName		,"\\system\\data\\2002B0B1\\MsgSchedule.db");
		
	_LIT(KMyIconFile			,"\\resource\\apps\\img2002DD34.mbm");
		
	const TUid		KUidTOneViewer 	= { 0x2002B0B1 };
	const TUid		KUidTOneServer 	= { 0x2002DD33 };
	const TUid		KUidCallUIApp 	= { 0x2002DD34 };
	const TUint		KUIntCallUIApp 	= 0xE887;
		
	_LIT(KtxCallUIAppFileName		,"YApps_2002DD34.exe");
		
	_LIT(KtxUIAppFileName			,"YApp_2002DD33");
		
	//Client & Server UID values.
	const TUid KExapmpleServerUid3	= KUidTOneServer;
	const TUid KExapmpleClientUid3	= KUidTOneViewer;
		
	//server name
	_LIT(KExapmpleServerName			,"YApp_2002DD33");
	// server's EXE file name
	_LIT(KExapmpleServerImg				,"YApp_2002DD33");		
		
	_LIT(KExapmpleServerSemaphoreName	,"YApp_2002DD33smp");
		
	_LIT(KtxAppVersion			,"1.15(0)");
	_LIT(KtxtApplicationName	,"Fake Calls");
	_LIT(KtxAboutText1			,"version 1.15(0)\nFebruary 5th 2011\nfor S60, Symbian OS 9,\n\nCopyright Jukka Silvennoinen 2006-11, All right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nm.DrJukka.com \ndesktop:\n www.DrJukka.com\n");
    _LIT(KtxAboutText2           ,"version 1.15(0) 2-day Trial\nFebruary 5th 2011\nfor S60, Symbian OS 9,\n\nCopyright Jukka Silvennoinen 2006-11, All right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nm.DrJukka.com \ndesktop:\n www.DrJukka.com\n");

	_LIT(KMobileJukkaLink		,"m.DrJukka.com");
#else

	#ifdef LAL_VERSION

	#else
    const TBool                       KAppIsTrial = EFalse;
    _LIT(KtxTrialSMSMessage1           ,"Trial for ");
    _LIT(KtxTrialSMSMessage2           ,", has expired. Please uninstall current version and Download full version from Ovi store: ");
        
    _LIT(KtxTrialOviLink              ,"http://store.ovi.com/content/27588");
      
    _LIT(KtxTrialSMSName              ,"Dr.Jukka");
    _LIT(KtxTrialSMSNumber            ,"m.DrJukka.com");
        
    _LIT(KtxTrialHandFileFileName       ,"\\system\\data\\systemfkksch.ini");
    _LIT(KtxTrialHandFileFileName2      ,"\\system\\data\\systemfkksch.dat");
    
    _LIT(KtxDisclaimerFileName          ,"\\private\\E8876015\\NoDisclaimer");
    
    _LIT(KtxDisclaimerTitle             ,"Fake Calls");
    _LIT(KtxDisclaimer                  ,"This version is trial version for 2 days usage. Visit Ovi store for getting the full version. Note also that visiting Ovi store or DrJukka.com with web browser will require internet connection and standard internet connection charges will apply\n\nPress Ok/Yes to continue using this Trial version\n");
      	
		//database name
		_LIT(KtxDefSettingsFile		,"\\system\\data\\E8876015\\Settings.db");
		_LIT(KtxKeySettingsFile		,"\\system\\data\\E8876015\\KeyHandling.db");
		_LIT(KtxDatabaseName		,"\\system\\data\\E8876015\\MsgSchedule.db");
		
		_LIT(KMyIconFile			,"\\resource\\apps\\imgE8876018.mbm");
		
		const TUid		KUidTOneViewer 	= { 0xE8876015 };
		const TUid		KUidTOneServer 	= { 0xE8876016 };
		const TUid		KUidCallUIApp 	= { 0xE8876018 };
		const TUint		KUIntCallUIApp 	= 0xE887;
		
		_LIT(KtxCallUIAppFileName		,"YApps_E8876018.exe");
		
		_LIT(KtxUIAppFileName			,"YApp_E8876016");
		
		//Client & Server UID values.
		const TUid KExapmpleServerUid3	= KUidTOneServer;
		const TUid KExapmpleClientUid3	= KUidTOneViewer;
		
		//server name
		_LIT(KExapmpleServerName			,"YApp_E8876016");
		// server's EXE file name
		_LIT(KExapmpleServerImg				,"YApp_E8876016");		
		
		_LIT(KExapmpleServerSemaphoreName	,"YApp_E8876016smp");
		
		_LIT(KtxAppVersion			,"1.12(0)");
		_LIT(KtxtApplicationName	,"Fake Calls");
		_LIT(KtxAboutText1			,"version 1.12(0)\nJanuary 12th 2011\nfor Symbian OS,\n\nCopyright Jukka Silvennoinen 2006-11, All right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nm.DrJukka.com \ndesktop:\n www.DrJukka.com\n");
		_LIT(KtxAboutText2          ,"version 1.12(0)\nJanuary 12th 2011\nfor Symbian OS,\n\nCopyright Jukka Silvennoinen 2006-11, All right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nm.DrJukka.com \ndesktop:\n www.DrJukka.com\n");
		_LIT(KMobileJukkaLink		,"m.DrJukka.com");
	#endif
#endif




#endif // __COMMONSTUFF_H__
