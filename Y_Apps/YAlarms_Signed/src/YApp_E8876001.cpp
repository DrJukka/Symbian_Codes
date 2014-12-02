/* 
	Copyright (c) 2006-07, 
	Jukka Silvennoinen
	All rights reserved 
*/

#include "YApp_E8876001.h"
#include "YApp_E8876001.hrh"
#include "Definitions.h"

#ifdef SONE_VERSION
	#include <YApp_2002DD36.rsg>
#else

	#ifdef LAL_VERSION

	#else
		#include <YApp_E8876001.rsg>
	#endif
#endif



#include <aknmessagequerydialog.h> 
#include <AknQueryDialog.h>
#include <sendui.h> 
#include <senduimtmuids.h>
//#include <sendnorm.rsg>
#include <BAUTILS.H>

#include "Items_Container.h" 
#include "Splash_Screen.h"

const TInt KSplashTimeOut = 500000;


#include <eikstart.h>
#include <SendUiConsts.h>
#include <cmessagedata.h> 

/*
---------------------------------------------------------------------------------------
static const TAknsItemID KAknsIIDSkinBmpNoteBgDefault =
    { EAknsIIDMajorSkinBmp, EAknsIIDMinorSkinBmpNoteBgDefault };

static const TAknsItemID KAknsIIDSkinBmpStatusPaneUsual =
    { EAknsIIDMajorSkinBmp, EAknsIIDMinorSkinBmpStatusPaneUsual };
---------------------------------------------------------------------------------------
*/
void CYBrowserAppUi::ConstructL()
{	
#ifdef __SERIES60_3X__
	BaseConstructL(CAknAppUi::EAknEnableSkin);
#else
	BaseConstructL(0x1008);// skins & layout
#endif

	iTimeCount= 0;
	
	iOkToContinue = ShowDisclaimerL();
	
	iMySplashScreen = CMySplashScreen::NewL();
	AddToStackL(iMySplashScreen);

	iTimeOutTimer = CTimeOutTimer::NewL(EPriorityNormal,*this);
	iTimeOutTimer->After(KSplashTimeOut);	
}

/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
TBool CYBrowserAppUi::ShowDisclaimerL(void)
{
	TBool OkToContinue(EFalse);

#ifdef SONE_VERSION
	OkToContinue = ETrue;
#else
	TFindFile AppFile(CCoeEnv::Static()->FsSession());
	if(KErrNone != AppFile.FindByDir(KtxDisclaimerFileName, KNullDesC))
	{
		HBufC* Abbout = KtxDisclaimer().AllocLC();
		TPtr Pointter(Abbout->Des());
		CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL(Pointter);
		dlg->PrepareLC(R_DDD_HEADING_PANE);
		dlg->SetHeaderTextL(KtxDisclaimerTitle);  
		if(dlg->RunLD())
		{
			TFileName ShortFil;
		#ifdef __SERIES60_3X__
			if(KErrNone ==CCoeEnv::Static()->FsSession().PrivatePath(ShortFil))
			{
				TFindFile privFile(CCoeEnv::Static()->FsSession());
				if(KErrNone == privFile.FindByDir(ShortFil, KNullDesC))
				{
					TParsePtrC hjelp(privFile.File());
					ShortFil.Copy(hjelp.Drive());
					ShortFil.Append(KtxDisclaimerFileName);
				}
			}
		#else
			TFindFile privFile(CCoeEnv::Static()->FsSession());
			if(KErrNone == privFile.FindByDir(KtxApplicationFileName, KNullDesC))
			{
				TParsePtrC hjelp(privFile.File());
				ShortFil.Copy(hjelp.Drive());
				ShortFil.Append(KtxDisclaimerFileName);
			}
		#endif
		
			BaflUtils::EnsurePathExistsL(CCoeEnv::Static()->FsSession(),ShortFil);
		
			RFile MyFile;
			if(KErrNone == MyFile.Create(CCoeEnv::Static()->FsSession(),ShortFil,EFileWrite))
			{
				TTime NowTime;
				NowTime.HomeTime();
				
				TBuf8<255> InfoLine;
				InfoLine.Copy(_L8("Accepted on Date\t"));
					
				InfoLine.AppendNum(NowTime.DateTime().Day() + 1);
				InfoLine.Append(_L8("."));
				InfoLine.AppendNum((NowTime.DateTime().Month() + 1));
				InfoLine.Append(_L8("."));
				InfoLine.AppendNum(NowTime.DateTime().Year());
				InfoLine.Append(_L8(" "));
				InfoLine.Append(_L8("--"));
				InfoLine.AppendNum(NowTime.DateTime().Hour());
				InfoLine.Append(_L8(":"));		
				TInt HelperInt = NowTime.DateTime().Minute();
				if(HelperInt < 10)
					InfoLine.AppendNum(0);
				InfoLine.AppendNum(HelperInt);
				InfoLine.Append(_L8(":"));
				HelperInt = NowTime.DateTime().Second();
				if(HelperInt < 10)
					InfoLine.AppendNum(0);
				InfoLine.AppendNum(HelperInt);
				InfoLine.Append(_L8(" "));
				
				MyFile.Write(InfoLine);
				MyFile.Close();
			}
			
			OkToContinue = ETrue;
		}
		
		CleanupStack::PopAndDestroy(Abbout);
	}
	else
	{
		OkToContinue = ETrue;
	}
#endif  
	
	return OkToContinue;
}
/*
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/
CYBrowserAppUi::CYBrowserAppUi()                              
    {
	// no implementation required
    }
/*
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/
CYBrowserAppUi::~CYBrowserAppUi()
{
	if(iTimeOutTimer)
	{
		iTimeOutTimer->Cancel();
	}
	
	delete iTimeOutTimer;
	iTimeOutTimer = NULL;

	if(iMySplashScreen)
		RemoveFromStack(iMySplashScreen);
	
	delete iMySplashScreen;
	iMySplashScreen = NULL;
	
	if(iItemsContainer)
	{
		RemoveFromStack(iItemsContainer);
        delete iItemsContainer;
        iItemsContainer = NULL;
	}
	

}
/*
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/
void CYBrowserAppUi::TimerExpired()
{
//	if(iTimeCount >= 3)
	{
		if(iOkToContinue)
		{
			FinalizeMeL();
		}
		else
		{
			HandleCommandL(EEikCmdExit);
		}
	}
/*	else
	{
		if(iMySplashScreen)
		{
			iMySplashScreen->NextImage();
		}
		
		iTimeCount++;
		iTimeOutTimer->After(KSplashTimeOut);	
	}*/
}
/*
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/
void CYBrowserAppUi::HandleForegroundEventL(TBool aForeground)
{
	if(iItemsContainer)
	{
		iItemsContainer->ForegroundChangedL(aForeground);
	}
}

/*
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/
void CYBrowserAppUi::FinalizeMeL(void)
{	
	if(iMySplashScreen)
		RemoveFromStack(iMySplashScreen);
	
	if(iItemsContainer)
		RemoveFromStack(iItemsContainer);
	
	delete iItemsContainer;
	iItemsContainer = NULL;
	iItemsContainer = new(ELeave)CItemsContainer(Cba());
	AddToStackL(iItemsContainer);
	iItemsContainer->ConstructL();

	delete iMySplashScreen;
	iMySplashScreen = NULL;
}

/*
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/
void CYBrowserAppUi::HandleCommandL(TInt aCommand)
    {
    switch(aCommand)
        {
        case EClose:
        	{
        		// do nothing...
        	}
        	break;
        case EAknSoftkeyExit:
        case EEikCmdExit:
            Exit();
            break;
        default:
        	if(iItemsContainer)
        	{
        		iItemsContainer->HandleViewCommandL(aCommand);
        	}
            break;
        }
    }

/*
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/
CYBrowserDocument* CYBrowserDocument::NewL(CEikApplication& aApp)
    {
    CYBrowserDocument* self = NewLC(aApp);
    CleanupStack::Pop(self);
    return self;
    }
/*
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/
CYBrowserDocument* CYBrowserDocument::NewLC(CEikApplication& aApp)
    {
    CYBrowserDocument* self = new (ELeave) CYBrowserDocument(aApp);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }
/*
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/
void CYBrowserDocument::ConstructL()
    {
	// no implementation required
    }    
/*
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/
CYBrowserDocument::CYBrowserDocument(CEikApplication& aApp) : CAknDocument(aApp) 
    {
	// no implementation required
    }
/*
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/
CYBrowserDocument::~CYBrowserDocument()
    {
	// no implementation required
    }
/*
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/
CEikAppUi* CYBrowserDocument::CreateAppUiL()
    {
    CEikAppUi* appUi = new (ELeave) CYBrowserAppUi;
    return appUi;
    }
/*
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/
CApaDocument* CYBrowserApplication::CreateDocumentL()
    {  
    CApaDocument* document = CYBrowserDocument::NewL(*this);
    return document;
    }
/*
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/
TUid CYBrowserApplication::AppDllUid() const
    {
    return KUidYBrowserApp;
    }

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/

EXPORT_C CApaApplication* NewApplication()
    {
    return (new CYBrowserApplication);
    }
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/

#ifdef __SERIES60_3X__
GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication( NewApplication );
	}
#else
GLDEF_C TInt E32Dll(TDllReason)
    {
    return KErrNone;
    }
#endif
