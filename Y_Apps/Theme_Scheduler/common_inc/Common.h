/* Copyright (c) 2001 - 2003 , J.P. Silvennoinen. All rights reserved */

#ifndef __THEMESSCHED_COMMON_H__
#define __THEMESSCHED_COMMON_H__

#include "Definitions.h"


#ifdef SONE_VERSION
    const TBool                       KAppIsTrial = ETrue;
    _LIT(KtxTrialSMSMessage1           ,"Trial for ");
    _LIT(KtxTrialSMSMessage2           ,", has expired. Please uninstall current version and Download full version from Ovi store: ");
        
    _LIT(KtxTrialOviLink              ,"http://store.ovi.com/content/23368");
      
    _LIT(KtxTrialSMSName              ,"Dr.Jukka");
    _LIT(KtxTrialSMSNumber            ,"m.DrJukka.com");
        
    _LIT(KtxTrialHandFileFileName       ,"\\system\\data\\systemthsch.ini");
    _LIT(KtxTrialHandFileFileName2      ,"\\system\\data\\systemthsch.dat");
        
	_LIT(KtxDisclaimerFileName			,"\\private\\2002884B\\NoDisclaimer");
	
	_LIT(KtxDisclaimerTitle				,"Welcome: Theme Scheduler");
	_LIT(KtxDisclaimer					,"This version is trial version for 4 days usage. Visit Ovi store for getting the full version. Note also that visiting Ovi store or DrJukka.com with web browser will require internet connection and standard internet connection charges will apply\n\nPress Ok/Yes to continue using this Trial version\n");
			
	//database name
	_LIT(KtxDatabaseName		,"\\system\\data\\2002884B\\ThemeSchedule.db");
	
	const TUid		KUidUIApp   = { 0x2002884B};
	const TUid		KUidServer  = { 0x2002884C};
	
	const TInt		KAppChnages = { 0x010};
	
	
	_LIT(KtxServerFileName		,"YApp_2002884C.exe"); // change also on atustart..
	
	
	_LIT(KtxAppVersion			,"1.15(0)");
	_LIT(KtxtApplicationName	,"Theme Scheduler");
	_LIT(KtxAboutText1			,"version 1.15(0)\nApril 24th 2009\nfor S60, Symbian OS 9,\n\nCopyright Jukka Silvennoinen 2006-09, All right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nm.DrJukka.com \ndesktop:\n www.DrJukka.com\nGraphics by P@sko\nwww.jaspp.net\n");
    _LIT(KtxAboutText2           ,"version 1.15(0) 4-day Trial\nMay 1st 2009\nfor S60, Symbian OS 9,\n\nCopyright Jukka Silvennoinen 2006-09, All right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nm.DrJukka.com \ndesktop:\n www.DrJukka.com\nGraphics by P@sko\nwww.jaspp.net\n");

	_LIT(KMobileJukkaLink		,"m.DrJukka.com");
#else

	#ifdef LAL_VERSION
		//database name
		_LIT(KtxDatabaseName		,"\\system\\data\\2002207B\\ThemeSchedule.db");
	
		const TUid		KUidUIApp   = { 0x2002207B};
		const TUid		KUidServer  = { 0x2002207C};
	
		const TInt		KAppChnages = { 0x010};
	
	
		_LIT(KtxServerFileName		,"YApp_2002207C.exe"); // change also on atustart..
	
	
		_LIT(KtxAppVersion			,"1.10(2)");
		_LIT(KtxtApplicationName	,"Theme Scheduler");
		_LIT(KtxAboutText			,"version 1.10(2)\nOctober 24th 2009\nfor S60, Symbian OS 9,\n\nCopyright Jukka Silvennoinen 2006-09, All right reserved.\n\nPowered by adtronic. www.adplace.com\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nm.DrJukka.com \ndesktop:\n www.DrJukka.com\nGraphics by P@sko\nwww.jaspp.net\n");
		_LIT(KMobileJukkaLink		,"m.DrJukka.com");
	#else
	    const TBool                       KAppIsTrial = EFalse;
	    _LIT(KtxTrialSMSMessage1           ,"Trial for ");
	    _LIT(KtxTrialSMSMessage2           ,", has expired. Please uninstall current version and Download full version from Ovi store: ");
	        
	    _LIT(KtxTrialOviLink              ,"http://store.ovi.com/content/23368");
	      
	    _LIT(KtxTrialSMSName              ,"Dr.Jukka");
	    _LIT(KtxTrialSMSNumber            ,"m.DrJukka.com");
	        
	    _LIT(KtxTrialHandFileFileName       ,"\\system\\data\\systemthsch.ini");
	    _LIT(KtxTrialHandFileFileName2      ,"\\system\\data\\systemthsch.dat");
	    
		_LIT(KtxDisclaimerFileName			,"\\private\\E8876002\\NoDisclaimer");
		
		_LIT(KtxDisclaimerTitle				,"Welcome: Theme Scheduler");
		_LIT(KtxDisclaimer					,"Visiting DrJukka.com with web browser will require internet connection and standard internet connection charges will apply\n\nBy pressing Ok/Yes, you will agree on that\n");
				
		//database name
		_LIT(KtxDatabaseName		,"\\system\\data\\E8876002\\ThemeSchedule.db");
	
		const TUid		KUidUIApp   = { 0xE8876002};
		const TUid		KUidServer  = { 0xE8876003};
	
		const TInt		KAppChnages = { 0x010};
	
	
		_LIT(KtxServerFileName		,"YApp_E8876003.exe"); // change also on atustart..
	
	
		_LIT(KtxAppVersion			,"1.15(2)");
		_LIT(KtxtApplicationName	,"Theme Scheduler");
		_LIT(KtxAboutText1			,"version 1.15(2)\nJanuary 29th 2010\nfor S60, Symbian OS 9,\n\nCopyright Jukka Silvennoinen 2006-11, All right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nm.DrJukka.com \ndesktop:\n www.DrJukka.com\nGraphics by P@sko\nwww.jaspp.net\n");
        _LIT(KtxAboutText2          ,"version 1.15(2)\nJanuary 29th 2010\nfor S60, Symbian OS 9,\n\nCopyright Jukka Silvennoinen 2006-11, All right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nm.DrJukka.com \ndesktop:\n www.DrJukka.com\nGraphics by P@sko\nwww.jaspp.net\n");
		_LIT(KMobileJukkaLink		,"m.DrJukka.com");
	#endif
#endif
	

#endif // __THEMESSCHED_COMMON_H__
