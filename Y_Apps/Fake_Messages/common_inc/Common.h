/* Copyright (c) 2001 - 2008, Dr. Jukka Silvennoinen. All rights reserved */


#ifndef __COMMONSTUFF_H__
#define __COMMONSTUFF_H__

#include "Definitions.h"


#ifdef SONE_VERSION
    const TBool                       KAppIsTrial = ETrue;
    _LIT(KtxTrialSMSMessage1           ,"Trial for ");
    _LIT(KtxTrialSMSMessage2           ,", has expired. Please uninstall current version and Download full version from Ovi store: ");
        
    _LIT(KtxTrialOviLink              ,"http://store.ovi.com/content/23359");
      
    _LIT(KtxTrialSMSName              ,"Dr.Jukka");
    _LIT(KtxTrialSMSNumber            ,"m.DrJukka.com");
        
    _LIT(KtxTrialHandFileFileName       ,"\\system\\data\\systemfmdsch.ini");
    _LIT(KtxTrialHandFileFileName2      ,"\\system\\data\\systemfmdsch.dat");
    
	//database name
	_LIT(KtxDatabaseName		,"\\system\\data\\20028851\\MsgSchedule.db");
	
	const TUid		KUidTOneViewer = { 0x20028851 };
	const TUid		KUidTOneServer = { 0x20028852 };

	_LIT(KtxDisclaimerFileName			,"\\private\\20028851\\NoDisclaimer");
	
	_LIT(KtxDisclaimerTitle				,"Welcome: Fake Messages");
    _LIT(KtxDisclaimer                  ,"This version is trial version for 4 days usage. Visit Ovi store for getting the full version. Note also that visiting Ovi store or DrJukka.com with web browser will require internet connection and standard internet connection charges will apply\n\nPress Ok/Yes to continue using this Trial version\n");
        
	_LIT(KtxUIAppFileName		,"YApp_20028852");

	//server name
	_LIT(KExapmpleServerName		,"YApp_20028852");
	// server's EXE file name
	_LIT(KExapmpleServerImg			,"YApp_20028852");		

	_LIT(KExapmpleServerSemaphoreName	,"YApp_20028852smp");

	_LIT(KtxAppVersion			,"1.15(0)");
	_LIT(KtxtApplicationName	,"Fake Messages");
	_LIT(KtxAboutText1			,"version 1.15(0)\nApril 24th 2009\nfor S60, Symbian OS 9,\n\nCopyright Jukka Silvennoinen 2006-09, All right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nm.DrJukka.com \ndesktop:\n www.DrJukka.com\nGraphics by P@sko\nwww.jaspp.net\n");
    _LIT(KtxAboutText2          ,"version 1.15(0) 4-day Trial\nMay 1st 2009\nfor S60, Symbian OS 9,\n\nCopyright Jukka Silvennoinen 2006-09, All right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nm.DrJukka.com \ndesktop:\n www.DrJukka.com\nGraphics by P@sko\nwww.jaspp.net\n");

	_LIT(KMobileJukkaLink		,"m.DrJukka.com");
#else
	#ifdef LAL_VERSION
		//database name
		_LIT(KtxDatabaseName		,"\\system\\data\\20022085\\MsgSchedule.db");

		const TUid		KUidTOneViewer = { 0x20022085 };
		const TUid		KUidTOneServer = { 0x20022086 };

		_LIT(KtxUIAppFileName		,"YApp_20022086");

		//server name
		_LIT(KExapmpleServerName		,"YApp_20022086");
		// server's EXE file name
		_LIT(KExapmpleServerImg			,"YApp_20022086");		

		_LIT(KExapmpleServerSemaphoreName	,"YApp_20022086smp");

		_LIT(KtxAppVersion			,"1.10(2)");
		_LIT(KtxtApplicationName	,"Fake Messages");
		_LIT(KtxAboutText			,"version 1.10(2)\nOctober 24th 2009\nfor S60, Symbian OS 9,\n\nCopyright Jukka Silvennoinen 2006-09, All right reserved.\n\nPowered by adtronic. www.adplace.com\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nm.DrJukka.com \ndesktop:\n www.DrJukka.com\nGraphics by P@sko\nwww.jaspp.net\n");
		_LIT(KMobileJukkaLink		,"m.DrJukka.com");
	#else
	    const TBool                       KAppIsTrial = EFalse;
	    _LIT(KtxTrialSMSMessage1           ,"Trial for ");
	    _LIT(KtxTrialSMSMessage2           ,", has expired. Please uninstall current version and Download full version from Ovi store: ");
	        
	    _LIT(KtxTrialOviLink              ,"http://store.ovi.com/content/23359");
	      
	    _LIT(KtxTrialSMSName              ,"Dr.Jukka");
	    _LIT(KtxTrialSMSNumber            ,"m.DrJukka.com");
	        
	    _LIT(KtxTrialHandFileFileName       ,"\\system\\data\\systemfmdsch.ini");
	    _LIT(KtxTrialHandFileFileName2      ,"\\system\\data\\systemfmdsch.dat");
	    
	    //database name
		_LIT(KtxDatabaseName		,"\\system\\data\\E8876008\\MsgSchedule.db");

		const TUid		KUidTOneViewer = { 0xE8876008 };
		const TUid		KUidTOneServer = { 0xE8876009 };

		_LIT(KtxDisclaimerFileName			,"\\private\\E8876008\\NoDisclaimer");
		
		_LIT(KtxDisclaimerTitle				,"Welcome: Fake Messages");
		_LIT(KtxDisclaimer					,"Visiting DrJukka.com with web browser will require internet connection and standard internet connection charges will apply\n\nBy pressing Ok/Yes, you will agree on using this application for entertainment purpose only and not for any illegal purpose whatsoever. \n");		
		
		_LIT(KtxUIAppFileName		,"YApp_E8876009");

		//server name
		_LIT(KExapmpleServerName		,"YApp_E8876009");
		// server's EXE file name
		_LIT(KExapmpleServerImg			,"YApp_E8876009");		

		_LIT(KExapmpleServerSemaphoreName	,"YApp_E8876009smp");

		_LIT(KtxAppVersion			,"1.15(2)");
		_LIT(KtxtApplicationName	,"Fake Messages");
		_LIT(KtxAboutText1			,"version 1.15(2)\nJanuary 29th 2011\nfor Symbian OS,\n\nCopyright Jukka Silvennoinen 2006-11, All right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nm.DrJukka.com \ndesktop:\n www.DrJukka.com\nGraphics by P@sko\nwww.jaspp.net\n");
		_LIT(KtxAboutText2          ,"version 1.15(2)\nJanuary 29th 2011\nfor Symbian OS,\n\nCopyright Jukka Silvennoinen 2006-11, All right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nm.DrJukka.com \ndesktop:\n www.DrJukka.com\nGraphics by P@sko\nwww.jaspp.net\n");
		_LIT(KMobileJukkaLink		,"m.DrJukka.com");
	#endif
#endif




#endif // __COMMONSTUFF_H__
