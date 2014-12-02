/* Copyright (c) 2001 - 2009, Dr. Jukka Silvennoinen. All rights reserved */


#ifndef __COMMONSTUFF_H__
#define __COMMONSTUFF_H__


#include "Definitions.h"


#ifdef SONE_VERSION
    const TBool                       KAppIsTrial = EFalse;
    _LIT(KtxTrialSMSMessage           ,"Trial for FlashSMS Scheduler, has expired. Please uninstall current version and Download full version from Ovi store: http://store.ovi.com/content/23368");
    _LIT(KtxTrialSMSName              ,"Dr.Jukka");
    _LIT(KtxTrialSMSNumber            ,"m.DrJukka.com");
        
    _LIT(KtxTrialHandFileFileName       ,"\\system\\data\\systemfssnd.ini");
    _LIT(KtxTrialHandFileFileName2      ,"\\system\\data\\systemfssnd.dat");

	_LIT(KtxDatabaseName		,"\\system\\data\\20028858\\MsgSchedule.db");
	
	const TUid		KUidTOneViewer = { 0x20028858 };
	const TUid		KUidTOneServer = { 0x20028859 };
	
	_LIT(KtxUIAppFileName		,"YApp_20028859");
	
	//Client & Server UID values.
	const TUid KExapmpleServerUid3	= KUidTOneServer;
	const TUid KExapmpleClientUid3	= KUidTOneViewer;
	
	//server name
	_LIT(KExapmpleServerName			,"YApp_20028859");
	// server's EXE file name
	_LIT(KExapmpleServerImg				,"YApp_20028859");		
	
	_LIT(KExapmpleServerSemaphoreName	,"YApp_20028859smp");
	
	
	_LIT(KtxAppVersion			,"1.15(0)");
	_LIT(KtxtApplicationName	,"FlashSMS Scheduler");
	_LIT(KtxAboutText			,"version 1.15(0)\nApril 25th 2010\nfor S60, Symbian OS 9,\n\nCopyright Jukka Silvennoinen 2006-10, All right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nm.DrJukka.com \ndesktop:\n www.DrJukka.com\n");
	_LIT(KMobileJukkaLink		,"m.DrJukka.com");
#else

	#ifdef LAL_VERSION
		//database name
		_LIT(KtxDatabaseName		,"\\system\\data\\E8876012\\MsgSchedule.db");
	
		const TUid		KUidTOneViewer = { 0xE8876012 };
		const TUid		KUidTOneServer = { 0xE8876013 };
		
		_LIT(KtxUIAppFileName		,"YApp_E8876013");
		
		//Client & Server UID values.
		const TUid KExapmpleServerUid3	= KUidTOneServer;
		const TUid KExapmpleClientUid3	= KUidTOneViewer;
		
		//server name
		_LIT(KExapmpleServerName			,"YApp_E8876013");
		// server's EXE file name
		_LIT(KExapmpleServerImg				,"YApp_E8876013");		
		
		_LIT(KExapmpleServerSemaphoreName	,"YApp_E8876013smp");
		
		_LIT(KtxAppVersion		,"0.80(0)");
		_LIT(KtxtApplicationName	,"FlashSMS Scheduler");
		_LIT(KtxAboutText			,"version 0.80(0)\nNovember 11th 2009\nfor S60, Symbian OS 9,\n\nCopyright Jukka Silvennoinen 2006-09, All right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nm.DrJukka.com \ndesktop:\n www.DrJukka.com\n");
		_LIT(KMobileJukkaLink		,"m.DrJukka.com");
	#else
	    const TBool                       KAppIsTrial = EFalse;
	    _LIT(KtxTrialSMSMessage           ,"Trial for FlashSMS Scheduler, has expired. Please uninstall current version and Download full version from Ovi store: http://store.ovi.com/content/23368");
	    _LIT(KtxTrialSMSName              ,"Dr.Jukka");
	    _LIT(KtxTrialSMSNumber            ,"m.DrJukka.com");
	        
	    _LIT(KtxTrialHandFileFileName       ,"\\system\\data\\systemfssnd.ini");
	    _LIT(KtxTrialHandFileFileName2      ,"\\system\\data\\systemfssnd.dat");
	    
		//database name
		_LIT(KtxDatabaseName		,"\\system\\data\\E8876012\\MsgSchedule.db");
		
		const TUid		KUidTOneViewer = { 0xE8876012 };
		const TUid		KUidTOneServer = { 0xE8876013 };
		
		_LIT(KtxUIAppFileName		,"YApp_E8876013");
		
		//Client & Server UID values.
		const TUid KExapmpleServerUid3	= KUidTOneServer;
		const TUid KExapmpleClientUid3	= KUidTOneViewer;
		
		//server name
		_LIT(KExapmpleServerName			,"YApp_E8876013");
		// server's EXE file name
		_LIT(KExapmpleServerImg				,"YApp_E8876013");		
		
		_LIT(KExapmpleServerSemaphoreName	,"YApp_E8876013smp");
		
		_LIT(KtxAppVersion			,"1.15(2)");
		_LIT(KtxtApplicationName	,"FlashSMS Scheduler");
		_LIT(KtxAboutText			,"version 1.15(2)\nJanuary 29th 2011\nfor S60, Symbian OS 9,\n\nCopyright Jukka Silvennoinen 2006-11, All right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nm.DrJukka.com \ndesktop:\n www.DrJukka.com\n");
		_LIT(KMobileJukkaLink		,"m.DrJukka.com");
	#endif
#endif




#endif // __COMMONSTUFF_H__
