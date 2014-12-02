/* Copyright (c) 2001 - 2009 , J.P. Silvennoinen. All rights reserved */

#ifndef __THEMESSCHED_COMMON_H__
#define __THEMESSCHED_COMMON_H__

#include "Definitions.h"


#ifdef SONE_VERSION
    const TBool        KAppIsTrial = EFalse;
    
    _LIT(KtxTrialSMSMessage1           ,"Trial for ");
    _LIT(KtxTrialSMSMessage2           ,", has expired. Please uninstall current version and Download full version from Ovi store: ");
        
    _LIT(KtxTrialOviLink              ,"http://store.ovi.com/content/23361");
    
    _LIT(KtxTrialSMSName              ,"Dr.Jukka");
    _LIT(KtxTrialSMSNumber            ,"m.DrJukka.com");
        
    _LIT(KtxTrialHandFileFileName       ,"\\system\\data\\systemklcsh.ini");
    _LIT(KtxTrialHandFileFileName2      ,"\\system\\data\\systemklcsh.dat");
    
    
	_LIT(KtxDisclaimerFileName			,"\\private\\20028854\\NoDisclaimer");
	
	_LIT(KtxDisclaimerTitle				,"Key Lock Clock");
    _LIT(KtxDisclaimer                  ,"This version is trial version for 4 days usage. Visit Ovi store for getting the full version. Note also that visiting Ovi store or DrJukka.com with web browser will require internet connection and standard internet connection charges will apply\n\nPress Ok/Yes to continue using this Trial version\n");
	
	const TUid		KUidUIApp   = { 0x20028854};
	const TUid		KUidServer  = { 0x20028855};

	const TInt		KAppChnages = { 0x010};

	//database name
	_LIT(KtxMonFile		,"\\system\\data\\20028854\\20028854.ini");
	_LIT(KtxIcconFile		,"\\resource\\apps\\YApp_20028854_aif.mif");

	_LIT(KtxServerFileName		,"YApp_20028855.exe"); // change also on atustart..
	_LIT(KtxStarterFileName		,"YApp_20028856.exe"); // change also on atustart..

	_LIT(KtxAppVersion			,"1.20(0)");
	_LIT(KtxtApplicationName	,"Key Lock Clock");
	_LIT(KtxAboutText1			,"version 1.20(0)\nJanuary 3rd 2012\nfor S60, Symbian OS 9,\n\nCopyright Jukka Silvennoinen 2006-12, All right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nm.DrJukka.com \ndesktop:\n www.DrJukka.com\n");
    _LIT(KtxAboutText2           ,"version 1.20(0) 4-day Trial\nJanuary 3rd 2012\nfor S60, Symbian OS 9,\n\nCopyright Jukka Silvennoinen 2006-12, All right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nm.DrJukka.com \ndesktop:\n www.DrJukka.com\n");

	_LIT(KMobileJukkaLink		,"m.DrJukka.com");
#else

	#ifdef LAL_VERSION
		const TUid		KUidUIApp   = { 0x20029072};
		const TUid		KUidServer  = { 0x20029073};

		const TInt		KAppChnages = { 0x010};

		//database name
		_LIT(KtxMonFile		,"\\system\\data\\20029072\\20029072.ini");
		_LIT(KtxIcconFile		,"\\resource\\apps\\YApp_20029072_aif.mif");

		_LIT(KtxServerFileName		,"YApp_20029073.exe"); // change also on atustart..
		_LIT(KtxStarterFileName		,"YApp_20029074.exe"); // change also on atustart..

		_LIT(KtxAppVersion			,"1.00(2)");
		_LIT(KtxtApplicationName	,"Key Lock Clock");
		_LIT(KtxAboutText			,"version 1.00(2)\nOctober 24th 2009\nfor S60, Symbian OS 9,\n\nCopyright Jukka Silvennoinen 2006-09, All right reserved.\n\nPowered by adtronic. www.adplace.com\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nm.DrJukka.com \ndesktop:\n www.DrJukka.com\n");
		_LIT(KMobileJukkaLink		,"m.DrJukka.com");
	#else
	    const TBool        KAppIsTrial = EFalse;
	    
	    _LIT(KtxTrialSMSMessage1           ,"Trial for ");
	    _LIT(KtxTrialSMSMessage2           ,", has expired. Please uninstall current version and Download full version from Ovi store: ");
	        
	    _LIT(KtxTrialOviLink              ,"http://store.ovi.com/content/23361");
	    
	    _LIT(KtxTrialSMSName              ,"Dr.Jukka");
	    _LIT(KtxTrialSMSNumber            ,"m.DrJukka.com");
	        
	    _LIT(KtxTrialHandFileFileName       ,"\\system\\data\\systemklcsh.ini");
	    _LIT(KtxTrialHandFileFileName2      ,"\\system\\data\\systemklcsh.dat");
	    
		_LIT(KtxDisclaimerFileName			,"\\private\\E887600F\\NoDisclaimer");
		
		_LIT(KtxDisclaimerTitle				,"Key Lock Clock");
		_LIT(KtxDisclaimer					,"Visiting DrJukka.com with web browser will require internet connection and standard internet connection charges will apply\n\nBy pressing Ok/Yes, you will agree on that\n");
				
		const TUid		KUidUIApp   = { 0xE887600F};
		const TUid		KUidServer  = { 0xE8876010};

		const TInt		KAppChnages = { 0x010};

		//database name
		_LIT(KtxMonFile		,"\\system\\data\\E887600F\\E887600F.ini");
		_LIT(KtxIcconFile		,"\\resource\\apps\\YApp_E887600F_aif.mif");

		_LIT(KtxServerFileName		,"YApp_E8876010.exe"); // change also on atustart..
		_LIT(KtxStarterFileName		,"YApp_E8876011.exe"); // change also on atustart..

		_LIT(KtxAppVersion			,"1.12(0)");
		_LIT(KtxtApplicationName	,"Key Lock Clock");
		_LIT(KtxAboutText1			,"version 1.12(0)\nJanuary 29th 2011\nfor S60, Symbian OS 9,\n\nCopyright Jukka Silvennoinen 2006-11, All right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nm.DrJukka.com \ndesktop:\n www.DrJukka.com\n");
        _LIT(KtxAboutText2          ,"version 1.12(0)\nJanuary 29th 2011\nfor S60, Symbian OS 9,\n\nCopyright Jukka Silvennoinen 2006-11, All right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nm.DrJukka.com \ndesktop:\n www.DrJukka.com\n");
		_LIT(KMobileJukkaLink		,"m.DrJukka.com");
	#endif
#endif






#endif // __THEMESSCHED_COMMON_H__
