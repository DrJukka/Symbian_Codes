/* Copyright (c) 2001 - 2008, Dr. Jukka Silvennoinen. All rights reserved */

#include "YApp_E8876005.h"                     // own definitions
#include "YApp_E8876005.hrh"                   // own resource header


#ifdef SONE_VERSION
	#include <YApp_2002884E.rsg>
#else
	#ifdef LAL_VERSION
		#include <YApp_20022080.rsg>
	#else
		#include <YApp_E8876005.rsg>
	#endif
#endif

#include <aknquerydialog.h> 
#include <aknmessagequerydialog.h>
#include <bautils.h> 

#include <EIKPROGI.H>
#include <IMCVCODC.H>
#include <AknGlobalNote.h>
#include <bacline.h>
#include <apgcli.h>
#include <PathInfo.h> 
#include <APGWGNAM.H>
#include <stringloader.h> 

#include "Main_Container.h"                     // own definitions
#include "Splash_Screen.h"
#include "ExampleServerSession.h"
#include "TrialHanler.h" 

const TInt KSplashTimeOut = 1000000;


#ifdef __SERIES60_3X__
	#include <eikstart.h>
#else
	#include <PLPVARIANT.H>
	#include <PLPVAR.H>
	#include "hal.h"
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
	{
		iTimeOutTimer->Cancel();
	}
	
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

	
	if(iProfileEngine)
	{
		iProfileEngine->Release();
	}
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
#ifdef __SERIES60_3X__
	BaseConstructL(CAknAppUi::EAknEnableSkin);
#else
	BaseConstructL(0x1008);// skins & layout
#endif
	
	FinalizeConstructL();	
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
	}else
    {
        OkToContinue = ETrue;
    }  
	
	return OkToContinue;
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/

void CMgAppUi::ChangeViewL(TInt aCommand)
{
	switch(aCommand)
	{
	case EActivateSplashContainer:
		{
			if(iTimeOutTimer)
			{
				iTimeOutTimer->Cancel();
			}
			
			delete iTimeOutTimer;
			iTimeOutTimer = NULL;
			
			if(iMainContainer)
			{
				RemoveFromStack(iMainContainer);
			}
			
			delete iMainContainer;
			iMainContainer = NULL;
				
			if(iMySplashScreen)
			{
				RemoveFromStack(iMySplashScreen);
			}
			
			delete iMySplashScreen;
			iMySplashScreen = NULL;

			iMySplashScreen = CMySplashScreen::NewL();
			AddToStackL(iMySplashScreen);
	
			iOkToContinue = ShowDisclaimerL();
			
			Cba()->SetCommandSetL(R_EXIT_CBA);
			Cba()->DrawDeferred();
						
			iTimeOutTimer = CTimeOutTimer::NewL(EPriorityNormal,*this);
			iTimeOutTimer->After(KSplashTimeOut);
			delete iMainContainer;
			iMainContainer = NULL;;
		}
		break;
	case EActivateMainContainer:
		{
			if(!iProfileEngine)
			{
				iProfileEngine = CreateProfileEngineL();
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
		break;
	default:
		break;
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMgAppUi::TimerExpired()
{    
	if(iOkToContinue){
	
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
	
		ChangeViewL(EActivateMainContainer);
	}else{
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
void CMgAppUi::FinalizeConstructL(void)
{	
	ChangeViewL(EActivateSplashContainer);
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
			RemoveFromStack(iMainContainer);
			// will close the DB..
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
	RExampleServerClient Serrrverrr;
	Serrrverrr.Connect();
	Serrrverrr.ReReadValuesNow();
	Serrrverrr.Close();

/*	if(!IsDrawerOn())
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
	*/
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
	  	  			
	  	if(ph.SecureId().iId ==(TUint32)KUidTOneServer.iUid)
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

void CMgAppUi::GetProfileNameAndRingToneL(TInt aTypeId,TDes& aName,TDes& aRingtone)
{
	TBool FoundMe(EFalse);
	
	if(iProfileEngine)
	{
		MProfilesNamesArray* Arrr = iProfileEngine->ProfilesNamesArrayLC();
				
		for (TInt i=0; i < Arrr->MdcaCount(); i++)
		{
			if(Arrr->ProfileName(i)->Id() == aTypeId)
			{
				aName.Copy(Arrr->ProfileName(i)->ShortName());
				aRingtone.Copy(Arrr->ProfileTones(i)->RingingTone1());
				FoundMe = ETrue;
				break;
			}
		}
		
		CleanupStack::PopAndDestroy(1);  // Arrr
	}
	
	if(!FoundMe)
	{
		StringLoader::Load(aName,R_SH_STR_RANDOM);	
	}
}
*/
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMgAppUi::GetProfileNameL(TInt aTypeId,TDes& aName)
{
	TBool FoundMe(EFalse);
	
	if(iProfileEngine)
	{
		MProfilesNamesArray* Arrr = iProfileEngine->ProfilesNamesArrayLC();
				
		for (TInt i=0; i < Arrr->MdcaCount(); i++)
		{
			if(Arrr->ProfileName(i)->Id() == aTypeId)
			{
				aName.Copy(Arrr->ProfileName(i)->ShortName());
				FoundMe = ETrue;
				break;
			}
		}
		
		CleanupStack::PopAndDestroy(1);  // Arrr
	}
	
	if(!FoundMe)
	{
		StringLoader::Load(aName,R_SH_STR_RANDOM);	
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMgAppUi::GetRepeatTypeName(TInt aTypeId,TDes& aName)
{
	switch(aTypeId)
	{
	case 1:	//0
		StringLoader::Load(aName,R_SH_STR_WORKDAYA);
		break;
	case 2:		//1
		StringLoader::Load(aName,R_SH_STR_NONWORKD);
		break;
	case 3://2
		StringLoader::Load(aName,R_SH_STR_MONDAYS);
		break;
	case 4:	//3
		StringLoader::Load(aName,R_SH_STR_TUESDAYS);
		break;
	case 5:	//4
		StringLoader::Load(aName,R_SH_STR_WEDNESDAYS);
		break;
	case 6:	//5
		StringLoader::Load(aName,R_SH_STR_THURSDAYS);
		break;
	case 7:	//6
		StringLoader::Load(aName,R_SH_STR_FRIDAYS);
		break;
	case 8:	//7
		StringLoader::Load(aName,R_SH_STR_SATURDAYS);
		break;
	case 9:	//8
		StringLoader::Load(aName,R_SH_STR_SUNDAYS);
		break;
	default:	//0
		StringLoader::Load(aName,R_SH_STR_EVERYDAY);
		break;
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMgAppUi::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
{
	if(aResourceId == R_MAIN_MENU)
	{

//		aMenuPane->SetItemDimmed(ESendLinkToFriend,ETrue);
		
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

