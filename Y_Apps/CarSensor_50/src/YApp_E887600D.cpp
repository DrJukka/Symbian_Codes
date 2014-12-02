/*
============================================================================

============================================================================
*/

// INCLUDE FILES
#include <avkon.hrh>
#include <aknmessagequerydialog.h>
#include <aknnotewrappers.h>
#include <stringloader.h>
#include <f32file.h>
#include <s32file.h>
#include <hlplch.h>
#include <eikstart.h>
#include <BAUTILS.H>
#include <APGCLI.H>


#include "YApp_E887600D.hrh"
#include "YApp_E887600D.h"
#include "MainView.h"
#include "Splash_Screen.h"

#ifdef SONE_VERSION
	#include <YApp_20028857.rsg>
#else

	#ifdef LAL_VERSION
		#include <YApp_E887600D.rsg>
	#else
		#include <YApp_E887600D.rsg>
	#endif
#endif



const TInt KSplashTimeOut = 1500000;
/*
------------------------------------------------------------------------------
------------------------------------------------------------------------------
*/
void CMovingBallAppUi::ConstructL()
{
	// Initialise app UI with standard value.
	BaseConstructL(CAknAppUi::EAknEnableSkin);

	SetOrientationL(EAppUiOrientationPortrait);
	
	iOkToContinue = ShowDisclaimerL();	
	iTimeOutTimer = CTimeOutTimer::NewL(CActive::EPriorityStandard,*this);
	iTimeOutTimer->After(KSplashTimeOut);
		
	iMySplashScreen = CMySplashScreen::NewL();
}

/*
------------------------------------------------------------------------------
------------------------------------------------------------------------------
*/
CMovingBallAppUi::CMovingBallAppUi()
	{
	// No implementation required
	}

/*
------------------------------------------------------------------------------
------------------------------------------------------------------------------
*/
CMovingBallAppUi::~CMovingBallAppUi()
{
	if(iTimeOutTimer)
		iTimeOutTimer->Cancel();
	
	delete iTimeOutTimer;
	iTimeOutTimer = NULL;
	
	delete iMySplashScreen;
	iMySplashScreen = NULL;
	
	delete iAppView;
	iAppView = NULL;
}

/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
TBool CMovingBallAppUi::ShowDisclaimerL(void)
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
	
	return OkToContinue;
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMovingBallAppUi::TimerExpired(void)
{
	if(iOkToContinue)
	{
		Cba()->SetCommandSetL(R_MYOPTEXIT_CBA);
		Cba()->DrawDeferred();	
		// Create view object
		iAppView = CMovingBallAppView::NewL( ClientRect(),Cba());
	
		delete iMySplashScreen;
		iMySplashScreen = NULL;
	}
	else
	{
		HandleCommandL(EAknSoftkeyExit);
	}
}
/*
------------------------------------------------------------------------------
------------------------------------------------------------------------------
*/
void CMovingBallAppUi::HandleCommandL( TInt aCommand )
	{
	switch( aCommand )
		{
		case EResset:
			if(iAppView)
			{
				iAppView->Reset();
			}
			break;
		case EBGLOff:
			if(iAppView)
			{
				iAppView->SetLights(EFalse);
			}
			break;
		case EBGLOn:
			if(iAppView)
			{
				iAppView->SetLights(ETrue);
			}
			break;
		case EClose:
			// do nothing..
			break;
		case EEikCmdExit:
		case EAknSoftkeyExit:
			Exit();
			break;
	    case EVisitDrJukka:
	    	{
				OpenMobileWEBSiteL(this);
	    	}
	    	break;			
		case EAbout:
			{	
				HBufC* Abbout = KtxAbbout().AllocLC();
					
				CAknMessageQueryDialog* dlg = new (ELeave)CAknMessageQueryDialog(); 
				dlg->PrepareLC(R_ABOUT_QUERY_DIALOG);
				dlg->QueryHeading()->SetTextL(_L("About"));
				dlg->SetMessageTextL(*Abbout);
				
				TCallBack callback1(OpenMobileWEBSiteL);
				dlg->SetLink(callback1);
				dlg->SetLinkTextL(KMobileJukkaLink);
				
				dlg->RunLD(); 
				
				CleanupStack::PopAndDestroy(); //Abbout
			}
			break;
		default:
			if(iAppView)
			{
				iAppView->HandleViewCommandL(aCommand);
			}
			break;
		}
	}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TInt CMovingBallAppUi::OpenMobileWEBSiteL(TAny* /*aAny*/)
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
void CMovingBallAppUi::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
{
	if(aResourceId == R_MENU)
	{
		if(iAppView)
		{
			if(iAppView->LightOn())
				aMenuPane->SetItemDimmed(EBGLOn,ETrue);
			else
				aMenuPane->SetItemDimmed(EBGLOff,ETrue);
		}

        #ifdef SONE_VERSION
            aMenuPane->SetItemDimmed(EVisitDrJukka,ETrue);
        #else
        
            #ifdef LAL_VERSION
              
            #else
            
            #endif
        #endif	
	}
}

/*
------------------------------------------------------------------------------
------------------------------------------------------------------------------
*/
void CMovingBallAppUi::HandleStatusPaneSizeChange()
{
	if(iAppView)
	{
		iAppView->SetRect( ClientRect() );
	}
	
	if(iMySplashScreen)
	{
		iMySplashScreen->SetRect( ClientRect() );
	}
} 


/*
------------------------------------------------------------------------------
------------------------------------------------------------------------------
*/

CMovingBallDocument* CMovingBallDocument::NewL( CEikApplication& aApp )
	{
	CMovingBallDocument* self = NewLC( aApp );
	CleanupStack::Pop( self );
	return self;
	}
/*
------------------------------------------------------------------------------
------------------------------------------------------------------------------
*/
CMovingBallDocument* CMovingBallDocument::NewLC( CEikApplication& aApp )
	{
	CMovingBallDocument* self =
		new ( ELeave ) CMovingBallDocument( aApp );

	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}

/*
------------------------------------------------------------------------------
------------------------------------------------------------------------------
*/
void CMovingBallDocument::ConstructL()
	{
	// No implementation required
	}

/*
------------------------------------------------------------------------------
------------------------------------------------------------------------------
*/
CMovingBallDocument::CMovingBallDocument( CEikApplication& aApp )
	: CAknDocument( aApp )
	{
	// No implementation required
	}

/*
------------------------------------------------------------------------------
------------------------------------------------------------------------------
*/
CMovingBallDocument::~CMovingBallDocument()
	{
	// No implementation required
	}

/*
------------------------------------------------------------------------------
------------------------------------------------------------------------------
*/
CEikAppUi* CMovingBallDocument::CreateAppUiL()
	{
	// Create the application user interface, and return a pointer to it;
	// the framework takes ownership of this object
	return ( static_cast <CEikAppUi*> ( new ( ELeave )
										CMovingBallAppUi ) );
	}

/*
------------------------------------------------------------------------------
------------------------------------------------------------------------------
*/
CApaDocument* CMovingBallApplication::CreateDocumentL()
	{
	// Create an MovingBall document, and return a pointer to it
	return (static_cast<CApaDocument*>
					( CMovingBallDocument::NewL( *this ) ) );
	}

TUid CMovingBallApplication::AppDllUid() const
	{
	// Return the UID for the MovingBall application
	return KUidYBrowserApp;
	}

/*
------------------------------------------------------------------------------
------------------------------------------------------------------------------
*/
LOCAL_C CApaApplication* NewApplication()
	{
	return new CMovingBallApplication;
	}

GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication( NewApplication );
	}



// End of File
