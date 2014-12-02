/* Copyright (c) 2001 - 2009, Dr. Jukka Silvennoinen. All rights reserved */


#ifndef __COMMONSTUFF_H__
#define __COMMONSTUFF_H__

#include "Definitions.h"

#ifdef SONE_VERSION

	//Y-Tasks, main application
	const TUid		KUidTOneViewer = { 0x2002B09F};
	
	_LIT(KtxINIFileName			,"\\system\\data\\2002B09F\\settings.dic");
	_LIT(KtxIconFileName		,"\\resource\\apps\\YTools_2002B09F.mif");
	
	
	_LIT(KtxDisclaimerFileName			,"\\private\\2002B09F\\NoDisclaimer");
	
	_LIT(KtxDisclaimerTitle				,"Welcome: Y-Tasks");
	_LIT(KtxDisclaimer					,"This application may or may not function properly, functionalities of this application includes killing and ending application processes and threads, as well as changing mime handling settings, these functionalities along side with other functionalities of this application or other modules or applications used with or without this application may cause serious harm to your device and even render it inoperable.\n\nBy pressing Ok/Yes, you will take all responsibility of using this application or any other product used with/by this application and accept that developer of this software or any other party will not give any warranty nor compensate any loss or damage which ever way caused by this application or any other application\n\nNote that visiting DrJukka.com with web browser will require internet connection and standard internet connection charges will apply\n");
	
	// Ecom Id:
	static const TUid KUidYTaskHandUID = {0x2002B09F};

	_LIT(KtxAppVersion					,"1.00");
	_LIT(KtxtApplicationName			,"Y-Tasks");
	_LIT(KtxAboutText					,"version 1.01(0)\nApril 21st 2012\nfor S60, Symbian OS 9,\n\nCopyright Jukka Silvennoinen 2006-12, All right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nm.DrJukka.com \ndesktop:\n www.DrJukka.com");

	_LIT(KMobileJukkaLink				,"m.DrJukka.com");
	
	// processes & threads plug-in
	_LIT(KProssResourceFileName			,"\\resource\\apps\\YTools_2002B0A0_res.RSC");
	_LIT(KProssIconsFileName			,"\\system\\data\\2002B09F\\2002B0A0\\icons.mif");
	_LIT(KProssApplicationName			,"Processes & Threads");
	_LIT(KProssAbbout					,"version 1.01(0)\nApril 21st 2012\nfor S60, Symbian OS 9,\n\nCopyright:\nJukka Silvennoinen 2006-09\nAll right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nwww.DrJukka.com");

	//Crash monitor Client & Server
	static const TUid KUidCrashServerUID = {0x2002B0A2};
	_LIT(KCrashServerName				,"YSrv_2002B0A2");
	_LIT(KCrashServerImg				,"YSrv_2002B0A2");		
	_LIT(KCrashServerSemaphoreName		,"YSrv_2002B0A2smp");
	
	_LIT(KCrashResourceFileName			,"\\resource\\apps\\YTools_2002B0A1_res.RSC");
	_LIT(KCrashIconsFileName			,"\\system\\data\\2002B09F\\2002B0A1\\icons.mif");
	_LIT(KCrashApplicationName			,"Crash monitor");
	_LIT(KCrashAbbout					,"version 1.01(0)\nApril 21st 2012\nfor S60, Symbian OS 9,\n\nCopyright:\nJukka Silvennoinen 2006-09\nAll right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nwww.DrJukka.com");
	
	//Memory status
	_LIT(KtxSystemTemp					,"\\YTaskMemoryStat\\");

	_LIT(KMEmStatResourceFileName		,"\\resource\\apps\\YTools_2002B0A3_res.RSC");
	_LIT(KMEmStatIconsFileName			,"\\system\\data\\2002B09F\\2002B0A3\\icons.mif");
	_LIT(KMEmStatApplicationName		,"Memory status");
	_LIT(KMEmStatAbbout					,"version 1.01(0)\nApril 21st 2012\nfor S60, Symbian OS 9,\n\nCopyright:\nJukka Silvennoinen 2006-09\nAll right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nwww.DrJukka.com");

	//Trace client & Server
	static const TUid KUidTraceServerUID = {0x2002B0A5};
	_LIT(KTraceServerName				,"YSrv_2002B0A5");
	_LIT(KTraceServerImg				,"YSrv_2002B0A5");		
	_LIT(KTraceServerSemaphoreName		,"YSrv_2002B0A5Esmp");
	
	_LIT(KTraceSetFileName				,"\\system\\data\\2002B09F\\2002B0A4\\settings.ini");
	
	_LIT(KTraceResourceFileName			,"\\resource\\apps\\YTools_2002B0A4_res.RSC");
	_LIT(KTraceIconsFileName			,"\\system\\data\\2002B09F\\2002B0A4\\icons.mif");
	
	_LIT(KTraceApplicationName			,"Trace");
	_LIT(KTraceAbbout					,"version 1.01(0)\nApril 21st 2012\nfor S60, Symbian OS 9,\n\nCopyright:\nJukka Silvennoinen 2006-09\nAll right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nwww.DrJukka.com");

	//Memory Status
	_LIT(KFontsResourceFileName			,"\\resource\\apps\\YTools_2002B0A6_res.RSC");
	_LIT(KFontsIconsFileName			,"\\system\\data\\2002B09F\\2002B0A6\\icons.mif");

	_LIT(KFontsApplicationName			,"Fonts");
	_LIT(KFontsAbbout					,"version 1.01(0)\nApril 21st 2012\nfor S60, Symbian OS 9,\n\nCopyright:\nJukka Silvennoinen 2006-09\nAll right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nwww.DrJukka.com");

	//Apps and tasks
	_LIT(KAppTaResourceFileName			,"\\resource\\apps\\YTools_2002B0A7_res.RSC");
	_LIT(KAppTaIconsFileName			,"\\system\\data\\2002B09F\\2002B0A7\\icons.mif");

	_LIT(KAppTaApplicationName			,"Apps & Tasks");
	_LIT(KAppTaAbbout					,"version 1.01(0)\nApril 21st 2012\nfor S60, Symbian OS 9,\n\nCopyright:\nJukka Silvennoinen 2006-09\nAll right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nwww.DrJukka.com");
		
	// Filetypes
	_LIT(KFilleResourceFileName			,"\\resource\\apps\\YTools_2002B0A8_res.RSC");
	_LIT(KFilleIconsFileName			,"\\system\\data\\2002B09F\\2002B0A8\\icons.mif");
	
	_LIT(KFilleDbFileName				,"\\system\\data\\2002B09F\\2002B0A8\\Rec.db");
	
	_LIT(KFilleApplicationName			,"Filetypes");
	_LIT(KFilleAbbout					,"version 1.01(0)\nApril 21st 2012\nfor S60, Symbian OS 9,\n\nCopyright:\nJukka Silvennoinen 2006-12\nAll right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nwww.DrJukka.com");
		
	
	//Client & Server UID values. main-server
	const TUid KMAINServerUid3	={0x2002B09F};
	const TUid KMAINYTasksUid31	={0x2002B09F};
	const TUid KMAINYTasksUid32	={0x2002B09F};
	const TUid KMAINYTasksUid33	={0x2002B09F};
	const TUid KMAINYTasksUid34	={0x2002B09F};
	
	
	
	//server name
	_LIT(KMainTaskServerName				,"YSrv_2002B0A9");
	// server's EXE file name
	_LIT(KMainTaskServerImg					,"YSrv_2002B0A9");		
	
	_LIT(KMainTaskServerSemaphoreName		,"YSrv_2002B0A9Esmp");


#else
	#ifdef LAL_VERSION
		//Y-Tasks, main application
		const TUid		KUidTOneViewer = { 0xA000257B};	

		_LIT(KtxINIFileName			,"\\system\\data\\A000257B\\settings.dic");
		_LIT(KtxIconFileName		,"\\resource\\apps\\YTools_A000257B.mif");
		
		// Ecom Id:
		static const TUid KUidYTaskHandUID = {0x20009999};
		
		_LIT(KtxAppVersion					,"1.00");
		_LIT(KtxtApplicationName			,"Y-Tasks");
		_LIT(KtxAboutText					,"version 1.00\nOctober 20th 2009\nfor S60, Symbian OS 9,\n\nCopyright Jukka Silvennoinen 2006-12, All right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nm.DrJukka.com \ndesktop:\n www.DrJukka.com");

		_LIT(KMobileJukkaLink				,"m.DrJukka.com");
		
		// processes & threads plug-in
		_LIT(KProssResourceFileName		,"\\resource\\apps\\YTools_A000257C_res.RSC");
		_LIT(KProssIconsFileName		,"\\system\\data\\A000257B\\A000257C\\icons.mif");
		_LIT(KProssApplicationName		,"Processes & Threads");
		_LIT(KProssAbbout				,"version 1.00\nOctober 20th 2009\nfor S60, Symbian OS 9,\n\nCopyright:\nJukka Silvennoinen 2006-12\nAll right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nwww.DrJukka.com");

		//Crash monitor Client & Server
		static const TUid KUidCrashServerUID = {0xA000257E};
		_LIT(KCrashServerName				,"YSrv_A000257E");
		_LIT(KCrashServerImg				,"YSrv_A000257E");		
		_LIT(KCrashServerSemaphoreName		,"YSrv_A000257Esmp");
		
		_LIT(KCrashResourceFileName	,"\\resource\\apps\\YTools_A000257D_res.RSC");
		_LIT(KCrashIconsFileName		,"\\system\\data\\A000257B\\A000257D\\icons.mif");
		_LIT(KCrashApplicationName		,"Crash monitor");
		_LIT(KCrashAbbout				,"version 1.00\nOctober 20th 2009\nfor S60, Symbian OS 9,\n\nCopyright:\nJukka Silvennoinen 2006-12\nAll right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nwww.DrJukka.com");
		
		//Memory status
		_LIT(KtxSystemTemp			,"\\YTaskMemStat\\");
		
		_LIT(KMEmStatResourceFileName	,"\\resource\\apps\\YTools_A0003128_res.RSC");
		_LIT(KMEmStatIconsFileName		,"\\system\\data\\A000257B\\A0003128\\icons.mif");
		_LIT(KMEmStatApplicationName		,"Memory status");
		_LIT(KMEmStatAbbout				,"version 1.00\nOctober 20th 2009\nfor S60, Symbian OS 9,\n\nCopyright:\nJukka Silvennoinen 2006-12\nAll right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nwww.DrJukka.com");

		//Trace client & Server
		static const TUid KUidTraceServerUID = {0x2002B0A5};
		_LIT(KTraceServerName			,"YSrv_A000312A");
		_LIT(KTraceServerImg				,"YSrv_A000312A");		
		_LIT(KTraceServerSemaphoreName	,"YSrv_A000312AEsmp"
		
		_LIT(KTraceSetFileName			,"\\system\\data\\A000257B\\A0003129\\settings.ini");
		
		_LIT(KTraceResourceFileName	,"\\resource\\apps\\YTools_A0003129_res.RSC");
		_LIT(KTraceIconsFileName		,"\\system\\data\\A000257B\\A0003129\\icons.mif");
			
		_LIT(KTraceApplicationName		,"Trace");
		_LIT(KTraceAbbout				,"version 1.00\nOctober 20th 2009\nfor S60, Symbian OS 9,\n\nCopyright:\nJukka Silvennoinen 2006-12\nAll right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nwww.DrJukka.com");

		//Memory Status
		_LIT(KFontsResourceFileName	,"\\resource\\apps\\YTools_A000312C_res.RSC");
		_LIT(KFontsIconsFileName		,"\\system\\data\\A000257B\\A000312C\\icons.mif");

		_LIT(KFontsApplicationName		,"Fonts");
		_LIT(KFontsAbbout				,"version 1.00\nOctober 20th 2009\nfor S60, Symbian OS 9,\n\nCopyright:\nJukka Silvennoinen 2006-12\nAll right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nwww.DrJukka.com");

		//Apps and tasks
		_LIT(KAppTaResourceFileName	,"\\resource\\apps\\YTools_A0000F60_res.RSC");
		_LIT(KAppTaIconsFileName		,"\\system\\data\\A000257B\\A0000F60\\icons.mif");

		_LIT(KAppTaApplicationName		,"Apps & Tasks");
		_LIT(KAppTaAbbout				,"version 1.00\nOctober 20th 2009\nfor S60, Symbian OS 9,\n\nCopyright:\nJukka Silvennoinen 2006-12\nAll right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nwww.DrJukka.com");
				
		// Filetypes
		_LIT(KFilleResourceFileName			,"\\resource\\apps\\YTools_A0000F5D_res.RSC");
		_LIT(KFilleIconsFileName		,"\\system\\data\\A000257B\\A0000F5D\\icons.mif");
		
		_LIT(KFilleDbFileName					,"\\system\\data\\A000257B\\A0000F5D\\Rec.db");

		_LIT(KFilleApplicationName			,"Filetypes");
		_LIT(KFilleAbbout					,"version 1.00\nOctober 20th 2009\nfor S60, Symbian OS 9,\n\nCopyright:\nJukka Silvennoinen 2006-12\nAll right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nwww.DrJukka.com");
			
		
		//Client & Server UID values.  main-server
		const TUid KMAINServerUid3	={0xE887600E};
		const TUid KMAINYTasksUid31	={0xA000257B};
		const TUid KMAINYTasksUid32	={0xA00007A6};
		const TUid KMAINYTasksUid33	={0x2000713D};
		const TUid KMAINYTasksUid34	={0x20022339};
		
		
		//server name
		_LIT(KMainTaskServerName			,"YSrv_E887600E");
		// server's EXE file name
		_LIT(KMainTaskServerImg				,"YSrv_E887600E");		
		
		_LIT(KMainTaskServerSemaphoreName		,"YSrv_E887600EEsmp");
		
		
	#else
		_LIT(KtxDisclaimerFileName			,"\\private\\A000257B\\NoDisclaimer");
		
		_LIT(KtxDisclaimerTitle				,"Welcome: Y-Tasks");
		_LIT(KtxDisclaimer					,"This application may or may not function properly, functionalities of this application includes killing and ending application processes and threads, as well as changing mime handling settings, these functionalities along side with other functionalities of this application or other modules or applications used with or without this application may cause serious harm to your device and even render it inoperable.\n\nBy pressing Ok/Yes, you will take all responsibility of using this application or any other product used with/by this application and accept that developer of this software or any other party will not give any warranty nor compensate any loss or damage which ever way caused by this application or any other application\n\nNote that visiting DrJukka.com with web browser will require internet connection and standard internet connection charges will apply\n");
				
		
		//Y-Tasks, main application
		const TUid		KUidTOneViewer = { 0xA000257B};
		
		_LIT(KtxINIFileName			,"\\system\\data\\A000257B\\settings.dic");
		_LIT(KtxIconFileName		,"\\resource\\apps\\YTools_A000257B.mif");
		
		// Ecom Id:
		static const TUid KUidYTaskHandUID = {0x20009999};
		
		_LIT(KtxAppVersion					,"1.00(4)");
		_LIT(KtxtApplicationName			,"Y-Tasks");
		_LIT(KtxAboutText					,"version 1.00(4)\nDecember 29th 2009\nfor S60, Symbian OS 9,\n\nCopyright Jukka Silvennoinen 2006-12, All right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nm.DrJukka.com \ndesktop:\n www.DrJukka.com");

		_LIT(KMobileJukkaLink				,"m.DrJukka.com");
		
		// processes & threads plug-in
		_LIT(KProssResourceFileName	,"\\resource\\apps\\YTools_A000257C_res.RSC");
		_LIT(KProssIconsFileName		,"\\system\\data\\A000257B\\A000257C\\icons.mif");
		_LIT(KProssApplicationName		,"Processes & Threads");
		_LIT(KProssAbbout				,"version 1.00(4)\nDecember 29th 2009\nfor S60, Symbian OS 9,\n\nCopyright:\nJukka Silvennoinen 2006-12\nAll right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nwww.DrJukka.com");

		//Crash monitor Client & Server
/**/		static const TUid KUidCrashServerUID = {0xA000257E};
		_LIT(KCrashServerName				,"YSrv_A000257E");
		_LIT(KCrashServerImg				,"YSrv_A000257E");		
		_LIT(KCrashServerSemaphoreName		,"YSrv_A000257Esmp");
		
		_LIT(KCrashResourceFileName		,"\\resource\\apps\\YTools_A000257D_res.RSC");
		_LIT(KCrashIconsFileName		,"\\system\\data\\A000257B\\A000257D\\icons.mif");
		_LIT(KCrashApplicationName		,"Crash monitor");
		_LIT(KCrashAbbout				,"version 1.00(4)\nDecember 29th 2009\nfor S60, Symbian OS 9,\n\nCopyright:\nJukka Silvennoinen 2006-12\nAll right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nwww.DrJukka.com");
		
		//Memory status
		_LIT(KtxSystemTemp				,"\\YTaskMemStat\\");
		
		_LIT(KMEmStatResourceFileName	,"\\resource\\apps\\YTools_A0003128_res.RSC");
		_LIT(KMEmStatIconsFileName		,"\\system\\data\\A000257B\\A0003128\\icons.mif");
		_LIT(KMEmStatApplicationName	,"Memory status");
		_LIT(KMEmStatAbbout				,"version 1.00(4)\nDecember 29th 2009\nfor S60, Symbian OS 9,\n\nCopyright:\nJukka Silvennoinen 2006-12\nAll right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nwww.DrJukka.com");

		//Trace client & Server
		static const TUid KUidTraceServerUID = {0x2002B0A5};
		_LIT(KTraceServerName			,"YSrv_A000312A");
		_LIT(KTraceServerImg			,"YSrv_A000312A");		
		_LIT(KTraceServerSemaphoreName	,"YSrv_A000312AEsmp");	
/**/		
		_LIT(KTraceSetFileName			,"\\system\\data\\A000257B\\A0003129\\settings.ini");
				
		_LIT(KTraceResourceFileName	,"\\resource\\apps\\YTools_A0003129_res.RSC");
		_LIT(KTraceIconsFileName		,"\\system\\data\\A000257B\\A0003129\\icons.mif");
		
		_LIT(KTraceApplicationName		,"Trace");
		_LIT(KTraceAbbout				,"version 1.00(4)\nDecember 29th 2009\nfor S60, Symbian OS 9,\n\nCopyright:\nJukka Silvennoinen 2006-12\nAll right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nwww.DrJukka.com");

		//Memory Status
		_LIT(KFontsResourceFileName	,"\\resource\\apps\\YTools_A000312C_res.RSC");
		_LIT(KFontsIconsFileName		,"\\system\\data\\A000257B\\A000312C\\icons.mif");

		_LIT(KFontsApplicationName		,"Fonts");
		_LIT(KFontsAbbout				,"version 1.00(4)\nDecember 29th 2009\nfor S60, Symbian OS 9,\n\nCopyright:\nJukka Silvennoinen 2006-12\nAll right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nwww.DrJukka.com");

		//Apps and tasks
		_LIT(KAppTaResourceFileName	,"\\resource\\apps\\YTools_A0000F60_res.RSC");
		_LIT(KAppTaIconsFileName		,"\\system\\data\\A000257B\\A0000F60\\icons.mif");

		_LIT(KAppTaApplicationName		,"Apps & Tasks");
		_LIT(KAppTaAbbout				,"version 1.00(4)\nDecember 29th 2009\nfor S60, Symbian OS 9,\n\nCopyright:\nJukka Silvennoinen 2006-12\nAll right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nwww.DrJukka.com");
			
		// Filetypes
		_LIT(KFilleResourceFileName			,"\\resource\\apps\\YTools_A0000F5D_res.RSC");
		_LIT(KFilleIconsFileName		,"\\system\\data\\A000257B\\A0000F5D\\icons.mif");
		
		_LIT(KFilleDbFileName					,"\\system\\data\\A000257B\\A0000F5D\\Rec.db");
		
		_LIT(KFilleApplicationName			,"Filetypes");
		_LIT(KFilleAbbout					,"version 1.00(4)\nDecember 29th 2009\nfor S60, Symbian OS 9,\n\nCopyright:\nJukka Silvennoinen 2006-12\nAll right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nwww.DrJukka.com");
			
		
		//Client & Server UID values. main-server
		const TUid KMAINServerUid3	={0xE887600E};
		const TUid KMAINYTasksUid31	={0xA000257B};
		const TUid KMAINYTasksUid32	={0xA00007A6};
		const TUid KMAINYTasksUid33	={0x2000713D};
		const TUid KMAINYTasksUid34	={0x20022339};
		
		
		//server name
		_LIT(KMainTaskServerName				,"YSrv_E887600E");
		_LIT(KMainTaskServerImg					,"YSrv_E887600E");		
		_LIT(KMainTaskServerSemaphoreName		,"YSrv_E887600EEsmp");
	#endif
#endif

#endif // __COMMONSTUFF_H__
