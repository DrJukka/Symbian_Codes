/* 	
	Copyright (c) 2001 - 2009, 
	J.P. Silvennoinen.
	All rights reserved 
*/
#include "YTools_A000257B.hrh"                     // own definitions
#include "YTools_A000257B.h"                   // own resource header


#include <e32std.h>
#include <bautils.h> 
#include <HAL.H>

#include <APGWGNAM.H> 

#include <EIKPROGI.H>
#include <IMCVCODC.H>
#include <bacline.h>
#include <apgcli.h>
#include <APACMDLN.H>
#include <REENT.H>

#include "Main_Container.h"
#include "Splash_Screen.h"
#include <eikstart.h>
#include <pathinfo.h> 
#include <aknquerydialog.h> 
#include <aknmessagequerydialog.h>
#include <stringloader.h> 
#include <AknGlobalNote.h>

const TInt KSplashTimeOut		 = 800000;

#include "Common.h"

#ifdef SONE_VERSION
	#include <YTools_2002B09F.rsg>
#else
	#ifdef LAL_VERSION
		#include <YTools_A000257B.rsg>
	#else
		#include <YTools_A000257B.rsg>
	#endif
#endif
//
// CMgAppUi
//
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
	
CMgAppUi::~CMgAppUi()
{
	if(iTimeOutTimer)
		iTimeOutTimer->Cancel();
	
	delete iTimeOutTimer;
	iTimeOutTimer = NULL;

		
	if(iMySplashScreen)
		RemoveFromStack(iMySplashScreen);
	
	delete iMySplashScreen;
	iMySplashScreen = NULL;
	
	if(iMainContainer)
		RemoveFromStack(iMainContainer);
			  
	delete iMainContainer;
	iMainContainer = NULL;
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CMgAppUi::CMgAppUi()
{

}


 
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMgAppUi::ConstructL()
{
	BaseConstructL(CAknAppUi::EAknEnableSkin);
	
	#ifdef __WINS__
		ChangeView(EActivateMainContainer, -1);
	#else	
		ChangeView(EActivateSplashContainer, -1);// starts the timer also..		 
	#endif

}


/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
TBool CMgAppUi::ShowDisclaimerL(void)
{
	TBool OkToContinue(EFalse);

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
	
	return OkToContinue;
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/

void CMgAppUi::ChangeView(TInt aCommand, TInt /*aExtra*/)
{
	switch(aCommand)
	{
	case EActivateSplashContainer:
		{
			if(iMainContainer)
			{
				RemoveFromStack(iMainContainer);
			}
			
			if(iTimeOutTimer)
			{
				iTimeOutTimer->Cancel();
			}
			delete iTimeOutTimer;
			iTimeOutTimer = NULL;
			
			iMySplashScreen = CMySplashScreen::NewL();
			AddToStackL(iMySplashScreen);
			
			iOkToContinue = ShowDisclaimerL();
			
			Cba()->SetCommandSetL(R_EXIT_CBA);
			Cba()->DrawDeferred();
						
			iTimeOutTimer = CTimeOutTimer::NewL(CActive::EPriorityStandard,*this);
			iTimeOutTimer->After(KSplashTimeOut);
						
			delete iMainContainer;
			iMainContainer = NULL;
		}
		break;
	case EActivateMainContainer:
		{
			if(iMySplashScreen)
			{
				RemoveFromStack(iMySplashScreen);
			}
			
			if(iMainContainer)
			{
				RemoveFromStack(iMainContainer);
			}
	
			delete iMainContainer;
			iMainContainer = NULL;
		
			iMainContainer = new(ELeave)CMainContainer(Cba());
			AddToStackL(iMainContainer);
			iMainContainer->SetMopParent(this);
			iMainContainer->ConstructL();
			
			
			delete iMySplashScreen;
			iMySplashScreen = NULL;
		}
		break;
	default:
		break;
	}
}


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMgAppUi::TimerExpired(void)
{
	if(iOkToContinue){
		ChangeView(EActivateMainContainer,-1);
	}else{
		HandleCommandL(EAknSoftkeyExit);
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TInt CMgAppUi::OpenMobileWEBSiteL(TAny* /*aAny*/)
{

	const TInt KWmlBrowserUid = 0x10008D39;
	TUid id( TUid::Uid( KWmlBrowserUid ) );
	TApaTaskList taskList( CEikonEnv::Static()->WsSession() );
	TApaTask task = taskList.FindApp( id );
	if ( task.Exists() )
	{
		HBufC8* param = HBufC8::NewLC( KMobileJukkaLink().Length() + 2);
				//"4 " is to Start/Continue the browser specifying a URL
		param->Des().Append(_L("4 "));
		param->Des().Append(KMobileJukkaLink);
		task.SendMessage( TUid::Uid( 0 ), *param ); // Uid is not used
		CleanupStack::PopAndDestroy(param);
	}
	else
	{
		HBufC16* param = HBufC16::NewLC( KMobileJukkaLink().Length() + 2);
				//"4 " is to Start/Continue the browser specifying a URL
		param->Des().Append(_L("4 "));
		param->Des().Append(KMobileJukkaLink);
		RApaLsSession appArcSession;
				// connect to AppArc server 
		User::LeaveIfError(appArcSession.Connect()); 
		TThreadId id;
		appArcSession.StartDocument( *param, TUid::Uid( KWmlBrowserUid), id );
		appArcSession.Close(); 
		CleanupStack::PopAndDestroy(param);
	}
	
	return KErrNone;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMgAppUi::HandleCommandL(TInt aCommand)
    {
    switch (aCommand)
    {
    case EVisitJukka:
    	{
			OpenMobileWEBSiteL(NULL);
    	}
    	break;
    case EChangeLayout:
    	{
    		if(Orientation() == EAppUiOrientationLandscape)
    		{
    			SetOrientationL(EAppUiOrientationPortrait);	
    		}
    		else
    		{
    			SetOrientationL(EAppUiOrientationLandscape);
    		}
    	}
    	break;
    case EAbout:
		{										
			HBufC* Abbout = KtxAboutText().AllocLC();
			TPtr Pointter(Abbout->Des());
						
			CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL(Pointter);
			dlg->PrepareLC(R_ABOUT_HEADING_PANE);
			dlg->SetHeaderTextL(_L("About"));  
						
			TCallBack callback1(OpenMobileWEBSiteL);
			dlg->SetLink(callback1);
			dlg->SetLinkTextL(KMobileJukkaLink);
								    
			dlg->RunLD();
							
			CleanupStack::PopAndDestroy(1);//Abbout
		}
    	break;
    case EClose:
    	{
    		// splash screen do nothing..
    	}
    	break;
    case EAknSoftkeyExit:
//    case EEikCmdExit:
		Exit();
        break;
    default:
    	if(iMainContainer)
    	{
    		iMainContainer->HandleViewCommandL(aCommand);
    	}
        break;
        }
    }
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMgAppUi::ShowNoteL(const TDesC& aMessage)
{
	CAknGlobalNote* dialog = CAknGlobalNote::NewLC();
	TInt NoteId = dialog->ShowNoteL(EAknGlobalInformationNote,aMessage);
	User::After(2000000);
	dialog->CancelNoteL(NoteId);
	CleanupStack::PopAndDestroy(dialog);	
}
  


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
		
void CMgAppUi::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
{
	if (aResourceId == R_MAIN_MENU)
	{
/*		if(iRegStatus == KFullVersion)
		{
			aMenuPane->SetItemDimmed(ERegisterMe,ETrue);
		}
		else if(iRegStatus == KInvalidVers)
		{
			aMenuPane->SetItemDimmed(EBrowseForComntent,ETrue);	
			aMenuPane->SetItemDimmed(ESendLinkToFriend,ETrue);
		}
*/
	}
	
	if(iMainContainer)
	{
		iMainContainer->InitMenuPanelL(aResourceId,aMenuPane);
	}
}
/*
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/
void CMgAppUi::HandleForegroundEventL(TBool aForeground)
{
	if(iMainContainer)
	{
		iMainContainer->ForegroundChangedL(aForeground);
	}
}
//
// CMgDocument
//
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CMgDocument* CMgDocument::NewL(CEikApplication& aApp)
    {
    CMgDocument* self = new(ELeave) CMgDocument(aApp);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); //self.
    return self;
    }

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/

CMgDocument::CMgDocument(CEikApplication& aApp)
	: CAknDocument(aApp)
    {
    }

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMgDocument::ConstructL()
    {
    }

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CEikAppUi* CMgDocument::CreateAppUiL()
{
   return (new(ELeave) CMgAppUi);
}


//
// CMgApplication
//

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TUid CMgApplication::AppDllUid() const
    {
    return KUidTOneViewer;
    }

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CApaDocument* CMgApplication::CreateDocumentL()
    {
    return (CMgDocument::NewL(*this));
    }

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
EXPORT_C CApaApplication* NewApplication()
    {
    return (new CMgApplication);
    }
  /*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/  

	GLDEF_C TInt E32Main()
		{
		return EikStart::RunApplication( NewApplication );
		}






