
/* 
	Copyright (c) 2001 - 2008, 
	Dr. Jukka Silvennoinen. 
	All rights reserved 
*/

#include "YApps_E8876012.hrh"                     // own definitions
#include "YApps_E8876012.h"                   // own resource header

#include <aknquerydialog.h> 
#include <aknmessagequerydialog.h>
#include <bautils.h> 
#include <HAL.H>
#include <stringloader.h> 
#include <APGWGNAM.H> 
#include <stringloader.h> 
#include <EIKPROGI.H>
#include <IMCVCODC.H>
#include <AknGlobalNote.h>
#include <bacline.h>
#include <apgcli.h>
#include <stringloader.h> 
#include <APACMDLN.H>


#include "Main_Container.h"                     // own definitions

#include "Splash_Screen.h"
#include "ExampleServerSession.h"

#include <centralrepository.h>
#include <ProfileEngineSDKCRKeys.h>

#include <eikstart.h>

#include <pathinfo.h> 
#include "Common.h"
#ifdef SONE_VERSION
	#include <YApps_20028858.rsg>
#else
	#ifdef LAL_VERSION
		#include <YApps_E8876012.rsg>
	#else
		#include <YApps_E8876012.rsg>
	#endif
#endif


const TInt KSplashTimeOut		 = 1000000;


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
	
	delete iContactsReader;
	iContactsReader = NULL;
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
	
	iTimeOutTimer = CTimeOutTimer::NewL(CActive::EPriorityStandard,*this);
	iTimeOutTimer->After(KSplashTimeOut);
	
	iContactsReader = CContactsReader::NewL(this);
	iContactsReader->StartTheReadL();
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMgAppUi::ReadProgress(TInt /*aCurrent*/, TInt /*aCount*/)
	{
	
	}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TBool CMgAppUi::GetContactsArrayL(RPointerArray<CMyCItem>& aArray)
{
	TBool Ret(EFalse);

	if(iContactsReader)
	{
		aArray =  iContactsReader->ItemArray();
	}

	return Ret;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMgAppUi::TimerExpired(void)
{
	if(iMySplashScreen)
	{
		RemoveFromStack(iMySplashScreen);
	}

	if(iMainContainer)
	{
		RemoveFromStack(iMainContainer);
	}


	if(StatusPane()->CurrentLayoutResId() != R_AVKON_STATUS_PANE_LAYOUT_USUAL)
	{
		StatusPane()->SwitchLayoutL(R_AVKON_STATUS_PANE_LAYOUT_USUAL);
	}

	StatusPane()->DrawNow();

	delete iMainContainer;
	iMainContainer = NULL;

	iMainContainer = new(ELeave)CMainContainer(Cba());
	AddToStackL(iMainContainer);
	iMainContainer->SetMopParent(this);
	iMainContainer->ConstructL();

	delete iMySplashScreen;
	iMySplashScreen = NULL;
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
		StringLoader::Load(aName,R_SH_STR_EVERYDAY);
		break;
	case 2:	//0
		StringLoader::Load(aName,R_SH_STR_WORKDAYA);
		break;
	case 3:		//1
		StringLoader::Load(aName,R_SH_STR_NONWORKD);
		break;
	case 4://2
		StringLoader::Load(aName,R_SH_STR_MONDAYS);
		break;
	case 5:	//3
		StringLoader::Load(aName,R_SH_STR_TUESDAYS);
		break;
	case 6:	//4
		StringLoader::Load(aName,R_SH_STR_WEDNESDAYS);
		break;
	case 7:	//5
		StringLoader::Load(aName,R_SH_STR_THURSDAYS);
		break;
	case 8:	//6
		StringLoader::Load(aName,R_SH_STR_FRIDAYS);
		break;
	case 9:	//7
		StringLoader::Load(aName,R_SH_STR_SATURDAYS);
		break;
	case 10:	//8
		StringLoader::Load(aName,R_SH_STR_SUNDAYS);
		break;
	default:	//0
		StringLoader::Load(aName,R_SH_STR_ONCE);
		break;
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
			HBufC* Abbout = KtxAboutText().AllocLC();
			TPtr Pointter(Abbout->Des());
			
			CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL(Pointter);
			dlg->PrepareLC(R_ABOUT_HEADING_PANE);
			dlg->SetHeaderTextL(KtxtApplicationName());  
			
			TCallBack callback1(OpenMobileWEBSiteL);
			dlg->SetLink(callback1);
			dlg->SetLinkTextL(KMobileJukkaLink);
					    
			dlg->RunLD();
				
			CleanupStack::PopAndDestroy(1);//Abbout
		}
    	break;
    case EClose:
    	// ask to exit, splash screen...
    case EAknSoftkeyExit:
    case EEikCmdExit:
		{
			RemoveFromStack(iMainContainer);
			// will close the DB..
			delete iMainContainer;
			iMainContainer = NULL;
			
			TRAPD(Errr,DoExitChecksNowL());
			Exit();
		}
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
void CMgAppUi::DoExitChecksNowL(void)
{
	RExampleServerClient Serrrverrr;
	Serrrverrr.Connect();
	Serrrverrr.ReReadValuesNow();
	Serrrverrr.Close();
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
CFileStore* CMgDocument::OpenFileL(TBool /*aDoOpen*/, const TDesC& aFilename,RFs& /*aFs*/)
{
	if(iMyAppUI)
	{
		iMyAppUI->OpenFileL(aFilename);
	}
	
	return NULL;//CEikDocument::OpenFileL(aDoOpen,aFilename,aFs);
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
//    return (new(ELeave) CMgAppUi);
	iMyAppUI = new(ELeave) CMgAppUi();
    return iMyAppUI;
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





