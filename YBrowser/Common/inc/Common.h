/* Copyright (c) 2001 - 2009, Dr. Jukka Silvennoinen. All rights reserved */


#ifndef __COMMONSTUFF_H__
#define __COMMONSTUFF_H__

#include "Definitions.h"

//#include "Common.h"

#ifdef SONE_VERSION
	
#else
	#ifdef LAL_VERSION

	#else

	#endif
#endif



#ifdef SONE_VERSION
	static const TUid KUidYHandlerUID = {0x2002B0AA};
	static const TUid KUidYRecogzrUID = {0x2002B0AB};
		
	static const TUid KUidYBrowserApp = {0x2002B0AA};
	_LIT(KMainApplicationName			,"Y-Browser");
			
	_LIT(KtxCommandSCFileName			,"\\private\\2002B0AA\\CommandSC");
	_LIT(KtxShortCutFileName			,"\\private\\2002B0AA\\Shortcuts");
	_LIT(KtxSettingsFileName			,"\\private\\2002B0AA\\Settings");
	_LIT(KtxDisclaimerFileName			,"\\private\\2002B0AA\\NoDisclaimer");
	_LIT(KtxPathSaveFileName			,"\\private\\2002B0AA\\Path");	
	_LIT(KtxIconFileName				,"\\private\\2002B0AA\\Iccon");
	_LIT(KtxFTASSFileName				,"\\private\\2002B0AA\\TypesAss.db");
		
	_LIT(KtxImagesFileName				,"\\system\\data\\2002B0AA\\Img\\Original.mif");
	_LIT(KtxAppIconFileName				,"\\resource\\apps\\YFB_2002B0AA_aif.mif");
		
	_LIT(KTxtMifExtension				,".mif");
	_LIT(KTxtIconsFolder				,"\\system\\data\\2002B0AA\\Img\\");
		
	_LIT(KtxDisclaimerTitle				,"Welcome: Y-Browser");
	_LIT(KtxDisclaimer					,"This application may or may not function properly, functionalities of this application includes deletion of files and data, these functionalities along side with other functionalities of this application or other modules or applications used with or without this application may cause serious harm to your device and even render it inoperable.\n\nBy pressing Ok/Yes, you will take all responsibility of using this application or any other product used with/by this application and accept that developer of this software or any other party will not give any warranty nor compensate any loss or damage which ever way caused by this application or any other application\n\nNote that visiting DrJukka.com with web browser will require internet connection and standard internet connection charges will apply\n");

	_LIT(KMainAbbout					,"version 1.02(0)\nAugust 6th 2012\nfor S60, Symbian OS 9,\n\nCopyright Jukka Silvennoinen 2006-09, All right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nm.DrJukka.com \ndesktop:\n www.DrJukka.com");

	_LIT(KMobileJukkaLink				,"m.DrJukka.com");
		
	//BT-Sender
	const TUid KUidBTObjectExchangeApp = { 0x2002B0AA };
	_LIT(KBTSendImageFileName		,"\\system\\data\\2002B0AA\\2002B0AB\\Images.mbm");	

	// own resource file
//	_LIT(KBTSendResourceFileName	,"\\resource\\apps\\YFB_2002B0AB_res.RSC");

	_LIT(KBtSendApplicationName		,"BT Obex");
	_LIT(KBtSendAbbout				,"version 1.02(0)\nAugust 6th 2012\nfor S60, Symbian OS 9,\n\nCopyright:\nJukka Silvennoinen 2006-2009\nAll right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nwww.DrJukka.com\n");
		
	//Mail folders
	_LIT(KMailAbboutName				,"Mail folders");
	_LIT(KMailAbbout				,"version 1.02(0)\nAugust 6th 2012\nfor S60, Symbian OS 9,\n\nCopyright Jukka Silvennoinen 2006-2009, All right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nwww.DrJukka.com\n");
//	_LIT(KMailResourceFileName		,"\\resource\\apps\\YFB_2002B0AC_res.RSC");
		
	//Text viewer
//	_LIT(KTextResourceFileName		,"\\resource\\apps\\YFB_2002B0AD_res.RSC");
	_LIT(KTextIconsFileName			,"\\system\\data\\2002B0AA\\2002B0AD\\icons.mbm");

	_LIT(KTextApplicationName		,"Text Viewer");
	_LIT(KTextAbbout				,"version 1.02(0)\nAugust 6th 2012\nfor S60, Symbian OS 9,\n\nCopyright Jukka Silvennoinen 2006-2009, All right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nwww.DrJukka.com\n");

	//Zipp
	_LIT(KtxApplicationName		,"YTools Zip");
	_LIT(KtxAbbout				,"version 1.02(0)\nAugust 6th 2012\nfor S60, Symbian OS 9,\n\nCopyright:\nPetteri Muilu 2004\nJukka Silvennoinen 2006-2009\nAll right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nwww.DrJukka.com\n");

	_LIT(KtxRec1Abbout			,"version 1.02(0)\nAugust 6th 2012\nfor S60, Symbian OS 9,\n\nCopyright Jukka Silvennoinen 2006-2009\nAll right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nwww.DrJukka.com\n");

	_LIT(KtxTypeName			,"Y-Zip");
	_LIT(KtxType				,"YTools/Zip");

	_LIT(KtxIconsFileName		,"\\system\\data\\2002B0AA\\2002B0AF\\icons.mbm");
	_LIT(KMyResourceFileName	,"\\resource\\apps\\YFB_2002B0AF_res.RSC");
#else
	#ifdef LAL_VERSION


		
	#else

		#ifdef __YTOOLS_SIGNED
			static const TUid KUidYBrowserApp = {0x2000713D};
			_LIT(KMainApplicationName			,"Y-FBrowser");
			
			_LIT(KtxCommandSCFileName			,"\\private\\2000713D\\CommandSC");
			_LIT(KtxShortCutFileName			,"\\private\\2000713D\\Shortcuts");
			_LIT(KtxSettingsFileName			,"\\private\\2000713D\\Settings");
			_LIT(KtxDisclaimerFileName			,"\\private\\2000713D\\NoDisclaimer");
			_LIT(KtxPathSaveFileName			,"\\private\\2000713D\\Path");
			_LIT(KtxIconFileName				,"\\private\\2000713D\\Iccon");
			_LIT(KtxFTASSFileName				,"\\private\\2000713D\\TypesAss.db");
				
			_LIT(KtxImagesFileName				,"\\system\\data\\A00007A6\\Img\\Original2.mif");
				
			_LIT(KtxAppIconFileName				,"\\resource\\apps\\YuccaBrowser_2000713D_aif.mif");
		#else
			static const TUid KUidYBrowserApp = {0xA00007A6};
			_LIT(KMainApplicationName			,"Y-Browser");
			
			_LIT(KtxCommandSCFileName			,"\\private\\A00007A6\\CommandSC");
			_LIT(KtxShortCutFileName			,"\\private\\A00007A6\\Shortcuts");
			_LIT(KtxSettingsFileName			,"\\private\\A00007A6\\Settings");
			_LIT(KtxDisclaimerFileName			,"\\private\\A00007A6\\NoDisclaimer");
			_LIT(KtxPathSaveFileName			,"\\private\\A00007A6\\Path");	
			_LIT(KtxIconFileName				,"\\private\\A00007A6\\Iccon");
			_LIT(KtxFTASSFileName				,"\\private\\A00007A6\\TypesAss.db");
			
			_LIT(KtxImagesFileName				,"\\system\\data\\A00007A6\\Img\\Original.mif");
			_LIT(KtxAppIconFileName				,"\\resource\\apps\\YuccaBrowser_aif.mif");
		#endif

		static const TUid KUidYHandlerUID = {0x20009991};
		static const TUid KUidYRecogzrUID = {0x20009992};
			
		_LIT(KTxtMifExtension				,".mif");
		_LIT(KTxtIconsFolder				,"\\system\\data\\A00007A6\\Img\\");
		
		_LIT(KtxDisclaimerTitle				,"Welcome: Y-Browser");
		_LIT(KtxDisclaimer					,"This software is freeware, so make sure you are not asked to pay to obtain this application.\n\nThis application comes with no warranty what so ever, thus you are using this software completely with your own risk.\n\nThis application may or may not function properly, functionalities of this application includes deletion of files and data, these functionalities along side with other functionalities of this application or other modules or applications used with or without this application may cause serious harm to your device and even render it inoperable.\n\nBy pressing Ok/Yes, you will take all responsibility of using this application or any other product used with/by this application and accept that developer of this software or any other party will not give any warranty nor compensate any loss or damage which ever way caused by this application or any other application\n");

		_LIT(KMainAbbout					,"version 1.01(0)\nJune 13th 2010\nfor S60, Symbian OS 9,\n\nCopyright Jukka Silvennoinen 2006-10, All right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nm.DrJukka.com \ndesktop:\n www.DrJukka.com");

		_LIT(KMobileJukkaLink				,"m.DrJukka.com");
		
		//BT-Sender
		const TUid KUidBTObjectExchangeApp = { 0xA000257B };
		_LIT(KBTSendImageFileName		,"\\system\\data\\A00007A6\\A0000F61\\Images.mbm");	

		// own resource file
		_LIT(KBTSendResourceFileName	,"\\resource\\apps\\BTSend_A0000F61_res.RSC");

		_LIT(KBtSendApplicationName		,"BT Obex");
		_LIT(KBtSendAbbout				,"version 1.01(0)\nJune 13th 2010\nfor S60, Symbian OS 9,\n\nCopyright:\nJukka Silvennoinen 2006-2009\nAll right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nwww.DrJukka.com\n");
		
		//Mail folders
		_LIT(KMailAbboutName            ,"Mail folders");
		_LIT(KMailAbbout				,"version 1.01(0)\nJune 13th 2010\nfor S60, Symbian OS 9,\n\nCopyright Jukka Silvennoinen 2006-2009, All right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nwww.DrJukka.com\n");
		_LIT(KMailResourceFileName		,"\\resource\\apps\\YToolsMailFolders_res.RSC");
		
		//Text viewer
		_LIT(KTextResourceFileName		,"\\resource\\apps\\YToolsTextViewer_res.RSC");
		_LIT(KTextIconsFileName			,"\\system\\data\\A00007A6\\A0000F6A\\icons.mbm");

		_LIT(KTextApplicationName		,"Text Viewer");
		_LIT(KTextAbbout				,"version 1.01(0)\nJune 13th 2010\nfor S60, Symbian OS 9,\n\nCopyright Jukka Silvennoinen 2006-2009, All right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nwww.DrJukka.com\n");

		
		//Zipp
		_LIT(KtxApplicationName		,"YTools Zip");
		_LIT(KtxAbbout				,"version 1.01(0)\nJune 13th 2010\nfor S60, Symbian OS 9,\n\nCopyright:\nPetteri Muilu 2004\nJukka Silvennoinen 2006-2009\nAll right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nwww.DrJukka.com\n");

		_LIT(KtxRec1Abbout			,"version 1.01(0)\nJune 13th 2010\nfor S60, Symbian OS 9,\n\nCopyright Jukka Silvennoinen 2006-2009\nAll right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nwww.DrJukka.com\n");

		_LIT(KtxTypeName			,"Y-Zip");
		_LIT(KtxType				,"YTools/Zip");

		_LIT(KtxIconsFileName		,"\\system\\data\\A00007A6\\A0000F6E\\icons.mbm");
		_LIT(KMyResourceFileName	,"\\resource\\apps\\YToolsZipperUI_res.RSC");
		
	#endif
#endif




#endif // __COMMONSTUFF_H__
