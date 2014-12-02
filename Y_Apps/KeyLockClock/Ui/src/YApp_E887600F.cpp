/* Copyright (c) 2001 - 2009 , Dr Jukka Silvennoinen. All rights reserved */




#include <aknquerydialog.h> 
#include <aknmessagequerydialog.h>
#include <bautils.h> 
#include <eikstart.h>
#include <EIKPROGI.H>
#include <IMCVCODC.H>
#include <AknGlobalNote.h>
#include <bacline.h>
#include <apgcli.h>
#include <PathInfo.h> 

#include "YApp_E887600F.h"                     // own definitions
#include "YApp_E887600F.hrh"                   // own resource header
#include "Main_Container.h"                     // own definitions
#include "Splash_Screen.h"
#include "TrialHanler.h"

#include "Common.h"

#ifdef SONE_VERSION
	#include <YApp_20028854.rsg>
#else

	#ifdef LAL_VERSION
		#include <YApp_20029072.rsg>
	#else
		#include <YApp_E887600F.rsg>
	#endif
#endif

const TInt KSplashTimeOut = 1000000;
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
	{
		iTimeOutTimer->Cancel();
	}
	
	delete iTimeOutTimer;
	iTimeOutTimer = NULL;

	delete imyPsObserver;
	imyPsObserver = NULL;
	
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
	
	iMySplashScreen = CMySplashScreen::NewL();
	AddToStackL(iMySplashScreen);
	
	iOkToContinue = ShowDisclaimerL();
	
	iTimeOutTimer = CTimeOutTimer::NewL(EPriorityNormal,*this);
	iTimeOutTimer->After(KSplashTimeOut);
	
	imyPsObserver = CmyPsObserver::NewL(*this,KUidServer, KAppChnages,ETrue);
	imyPsObserver->Start(EFalse);	
}

/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
TBool CMgAppUi::ShowDisclaimerL(void)
{
	TBool OkToContinue(EFalse);

	if(KAppIsTrial){
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
void CMgAppUi::TimerExpired()
{
	if(iOkToContinue)
	{    
        if(KAppIsTrial){
        
            TBuf<255> trialBuff;
            TBool isFirstTime(EFalse);
            TInt hoursLeft(0);
            if(CTrialHandler::IsNowOkL(isFirstTime,hoursLeft)){
                CTrialHandler::SetDateNowL();
                
                trialBuff.Copy(_L("You have "));
                trialBuff.AppendNum(hoursLeft);
                trialBuff.Append(_L("hours left on your Trial."));   
                
                ShowNoteL(trialBuff);
            }else{
                trialBuff.Copy(KtxTrialSMSMessage1);
                trialBuff.Append(KtxtApplicationName);
                trialBuff.Append(KtxTrialSMSMessage2);
                trialBuff.Append(KtxTrialOviLink);
                CAknQueryDialog* dlg = CAknQueryDialog::NewL();
                if(dlg->ExecuteLD(R_QUERY,trialBuff))
                {
                    OpenOviSiteL(KtxTrialOviLink);
                }
            }
        }
    
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
	
		if(StatusPane()->CurrentLayoutResId() != R_AVKON_STATUS_PANE_LAYOUT_USUAL)
		{
			StatusPane()->SwitchLayoutL(R_AVKON_STATUS_PANE_LAYOUT_USUAL);
		}
	
		StatusPane()->DrawNow();
	
		iMainContainer = new(ELeave)CMainContainer(Cba());
		iMainContainer->SetMopParent(this);
		AddToStackL(iMainContainer);
		iMainContainer->ConstructL();
	
		delete iMySplashScreen;
		iMySplashScreen = NULL;
	}
	else
	{
		HandleCommandL(EQuickExit);
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMgAppUi::OpenOviSiteL(const TDesC& aUrl)
{
    const TInt KWmlBrowserUid = 0x10008D39;
    TUid id( TUid::Uid( KWmlBrowserUid ) );
    TApaTaskList taskList( CEikonEnv::Static()->WsSession() );
    TApaTask task = taskList.FindApp( id );
    if ( task.Exists() )
    {
        HBufC8* param = HBufC8::NewLC( aUrl.Length() + 2);
                //"4 " is to Start/Continue the browser specifying a URL
        param->Des().Append(_L("4 "));
        param->Des().Append(aUrl);
        task.SendMessage( TUid::Uid( 0 ), *param ); // Uid is not used
        CleanupStack::PopAndDestroy(param);
    }
    else
    {
        HBufC16* param = HBufC16::NewLC( aUrl.Length() + 2);
                //"4 " is to Start/Continue the browser specifying a URL
        param->Des().Append(_L("4 "));
        param->Des().Append(aUrl);
        RApaLsSession appArcSession;
                // connect to AppArc server 
        User::LeaveIfError(appArcSession.Connect()); 
        TThreadId id;
        appArcSession.StartDocument( *param, TUid::Uid( KWmlBrowserUid), id );
        appArcSession.Close(); 
        CleanupStack::PopAndDestroy(param);
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
    case EVisitDrJukka:
    	{
			OpenMobileWEBSiteL(this);
    	}
    	break;
    case EAbout:
		{
            HBufC* Abbout(NULL);
            
            if(KAppIsTrial)
                Abbout = KtxAboutText2().AllocLC();
            else
                Abbout = KtxAboutText1().AllocLC();
            
			TPtr Pointter(Abbout->Des());
		
		    CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL(Pointter); 
		    dlg->PrepareLC(R_AVKON_MESSAGE_QUERY_DIALOG);
		    dlg->QueryHeading()->SetTextL(KtxtApplicationName());
		    
		    TCallBack callback1(OpenMobileWEBSiteL);
		    dlg->SetLink(callback1);
		    dlg->SetLinkTextL(KMobileJukkaLink);

		    dlg->RunLD();
			
			CleanupStack::PopAndDestroy(Abbout);
		}
    	break;
    case EClose2:
    	if(iTimeOutTimer)
    	{
    		iTimeOutTimer->Cancel();
    		TimerExpired();
    	}
    	break;
    case EClose:
    case EAknSoftkeyExit:
    case EEikCmdExit:
    	if(iMainContainer)
		{
			TRAPD(Errr,iMainContainer->SaveValuesL());
		
			RemoveFromStack(iMainContainer);
			// will close the DB...	  
			delete iMainContainer;
			iMainContainer = NULL;
		}
		TRAPD(Errr,DoExitChecksNowL());
    	Exit();
        break;
    case EQuickExit:     
        Exit();
        break;        
    default:
    	if(iMainContainer)
    		iMainContainer->HandleViewCommandL(aCommand);
        break;
        }
    }
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/


void CMgAppUi::DoExitChecksNowL(void)
{
	if(!IsDrawerOn())
	{
		CApaCommandLine* cmdLine=CApaCommandLine::NewLC();

		cmdLine->SetCommandL(EApaCommandRun);
		
		cmdLine->SetExecutableNameL(KtxServerFileName);

		RApaLsSession ls;
		ls.Connect();
		ls.StartApp(*cmdLine);
		ls.Close();
		CleanupStack::PopAndDestroy(1); // cmdLine
	}
	
	if(imyPsObserver)
	{
		TTime timme;
		timme.HomeTime();
	
		imyPsObserver->SetPropertyL(timme.DateTime().MicroSecond());
	}	
	
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
TBool CMgAppUi::IsDrawerOn(void)
{
	TBool Ret(EFalse);
	
	TFileName res;
	TFindProcess find;
  	while(find.Next(res) == KErrNone)
    {
    	RProcess ph;
	  	ph.Open(res);
	  	  			
	  	if(ph.SecureId() == (TUint32)(KUidServer.iUid))
	 	{
	  		Ret = ETrue;
	  		break;
	  	}
	  	
	  	ph.Close();
    }
 
    return Ret;
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
	CleanupStack::PopAndDestroy(dialog);;
}


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMgAppUi::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
{
	if(aResourceId == R_MAIN_MENU)
	{
		
	}
	
	if(iMainContainer)
	{
		iMainContainer->InitMenuPanelL(aResourceId,aMenuPane);
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
    return KUidUIApp;
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


