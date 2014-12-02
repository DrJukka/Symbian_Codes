/* Copyright (c) 2001 - 2008, Dr. Jukka Silvennoinen. All rights reserved */


#ifndef __COMMONSTUFF_H__
#define __COMMONSTUFF_H__


#include "Definitions.h"


#ifdef SONE_VERSION
    const TBool                       KAppIsTrial = ETrue;
    _LIT(KtxTrialSMSMessage1           ,"Trial for ");
    _LIT(KtxTrialSMSMessage2           ,", has expired. Please uninstall current version and Download full version from Ovi store: ");
        
    _LIT(KtxTrialOviLink              ,"http://store.ovi.com/content/23365");
      
    _LIT(KtxTrialSMSName              ,"Dr.Jukka");
    _LIT(KtxTrialSMSNumber            ,"m.DrJukka.com");
        
    _LIT(KtxTrialHandFileFileName       ,"\\system\\data\\systempefsch.ini");
    _LIT(KtxTrialHandFileFileName2      ,"\\system\\data\\systempefsch.dat");
    
	_LIT(KtxDisclaimerFileName			,"\\private\\2002884E\\NoDisclaimer");
	
	_LIT(KtxDisclaimerTitle				,"Welcome: Profile Scheduler");
    _LIT(KtxDisclaimer                  ,"This version is trial version for 4 days usage. Visit Ovi store for getting the full version. Note also that visiting Ovi store or DrJukka.com with web browser will require internet connection and standard internet connection charges will apply\n\nPress Ok/Yes to continue using this Trial version\n");
 
	//database name
	_LIT(KtxDatabaseName		,"\\system\\data\\2002884E\\ProfSchedule.db");
	
	const TUid		KUidTOneViewer = { 0x2002884E };
	const TUid		KUidTOneServer = { 0x2002884F };

	_LIT(KtxUIAppFileName		,"YApp_2002884E");

	//server name
	_LIT(KExapmpleServerName		,"YApp_2002884F");
	// server's EXE file name
	_LIT(KExapmpleServerImg			,"YApp_2002884F");		

	_LIT(KExapmpleServerSemaphoreName	,"YApp_2002884Fsmp");

	_LIT(KtxAppVersion			,"1.15(0)");
	_LIT(KtxtApplicationName	,"Profile Scheduler");
	_LIT(KtxAboutText1			,"version 1.15(0)\nApril 24th 2009\nfor S60, Symbian OS 9,\n\nCopyright Jukka Silvennoinen 2006-09, All right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nm.DrJukka.com \ndesktop:\n www.DrJukka.com\nGraphics by P@sko\nwww.jaspp.net\n");
    _LIT(KtxAboutText2          ,"version 1.15(0) 4-day Trial\nMay 1st 2009\nfor S60, Symbian OS 9,\n\nCopyright Jukka Silvennoinen 2006-09, All right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nm.DrJukka.com \ndesktop:\n www.DrJukka.com\nGraphics by P@sko\nwww.jaspp.net\n");

	_LIT(KMobileJukkaLink		,"m.DrJukka.com");
#else

	#ifdef LAL_VERSION
		//database name
		_LIT(KtxDatabaseName		,"\\system\\data\\20022080\\ProfSchedule.db");

		const TUid		KUidTOneViewer = { 0x20022080 };
		const TUid		KUidTOneServer = { 0x20022081 };

		_LIT(KtxUIAppFileName		,"YApp_20022080");

		//server name
		_LIT(KExapmpleServerName		,"YApp_20022081");
		// server's EXE file name
		_LIT(KExapmpleServerImg			,"YApp_20022081");		

		_LIT(KExapmpleServerSemaphoreName	,"YApp_20022081smp");


		_LIT(KtxAppVersion			,"1.10(2)");
		_LIT(KtxtApplicationName	,"Profile Scheduler");
		_LIT(KtxAboutText			,"version 1.10(2)\nOctober 24th 2009\nfor S60, Symbian OS 9,\n\nCopyright Jukka Silvennoinen 2006-09, All right reserved.\n\nPowered by adtronic. www.adplace.com\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nm.DrJukka.com \ndesktop:\n www.DrJukka.com\nGraphics by P@sko\nwww.jaspp.net\n");
		_LIT(KMobileJukkaLink		,"m.DrJukka.com");
	#else
	    const TBool                       KAppIsTrial = EFalse;
	    _LIT(KtxTrialSMSMessage1           ,"Trial for ");
	    _LIT(KtxTrialSMSMessage2           ,", has expired. Please uninstall current version and Download full version from Ovi store: ");
	        
	    _LIT(KtxTrialOviLink              ,"http://store.ovi.com/content/23365");
	      
	    _LIT(KtxTrialSMSName              ,"Dr.Jukka");
	    _LIT(KtxTrialSMSNumber            ,"m.DrJukka.com");
	        
	    _LIT(KtxTrialHandFileFileName       ,"\\system\\data\\systempefsch.ini");
	    _LIT(KtxTrialHandFileFileName2      ,"\\system\\data\\systempefsch.dat");
	    
		_LIT(KtxDisclaimerFileName			,"\\private\\E8876005\\NoDisclaimer");
		
		_LIT(KtxDisclaimerTitle				,"Welcome: Profile Scheduler");
		_LIT(KtxDisclaimer					,"Visiting DrJukka.com with web browser will require internet connection and standard internet connection charges will apply\n\nBy pressing Ok/Yes, you will agree on that\n");
		
		//database name
		_LIT(KtxDatabaseName		,"\\system\\data\\E8876005\\ProfSchedule.db");

		const TUid		KUidTOneViewer = { 0xE8876005 };
		const TUid		KUidTOneServer = { 0xE8876006 };

		_LIT(KtxUIAppFileName		,"YApp_E8876005");

		//server name
		_LIT(KExapmpleServerName		,"YApp_E8876006");
		// server's EXE file name
		_LIT(KExapmpleServerImg			,"YApp_E8876006");		

		_LIT(KExapmpleServerSemaphoreName	,"YApp_E8876006smp");

		_LIT(KtxAppVersion			,"1.15(2)");
		_LIT(KtxtApplicationName	,"Profile Scheduler");
		_LIT(KtxAboutText1			,"version 1.15(2)\nJanuary 29th 2011\nfor S60, Symbian OS 9,\n\nCopyright Jukka Silvennoinen 2006-11, All right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nm.DrJukka.com \ndesktop:\n www.DrJukka.com\nGraphics by P@sko\nwww.jaspp.net\n");
        _LIT(KtxAboutText2          ,"version 1.15(2)\nJanuary 29th 2011\nfor S60, Symbian OS 9,\n\nCopyright Jukka Silvennoinen 2006-11, All right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nm.DrJukka.com \ndesktop:\n www.DrJukka.com\nGraphics by P@sko\nwww.jaspp.net\n");
		_LIT(KMobileJukkaLink		,"m.DrJukka.com");
	#endif
#endif





#endif // __COMMONSTUFF_H__
