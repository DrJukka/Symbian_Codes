/* 	
	Copyright (c) 2001 - 2007, 
	J.P. Silvennoinen.
	All rights reserved 
*/
#include "YTools_A000257B.hrh"                     // own definitions
#include "YTools_A000257B.h"                   // own resource header



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


#ifdef __SERIES60_3X__
	#include <eikstart.h>
	#include <pathinfo.h> 
	#include <aknquerydialog.h> 
	#include <aknmessagequerydialog.h>
	#include <stringloader.h> 
	#include <AknGlobalNote.h>
#else	
	#include <eikstart.h>

#endif

#include <YTools_A000257B.rsg>


const TInt KSplashTimeOut		 = 1500000;

_LIT(KtxAppVersion					,"0.51");
_LIT(KtxtApplicationName			,"Y-Tasks");
_LIT(KtxAboutText					,"version 0.51\nApril 7th 2008\nfor S60, Symbian OS 9,\n\nCopyright:\nJukka Silvennoinen 2006-08\nAll right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nwww.DrJukka.com");

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
	
	CloseSTDLIB();// done in DLL already.
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
	
	#ifdef __WINS__
		ChangeView(EActivateMainContainer, -1);
	#else	
		ChangeView(EActivateSplashContainer, -1);// starts the timer also..		 
	#endif
#else
	BaseConstructL(0x1008);// skins & layout
	ChangeView(EActivateMainContainer, -1);
#endif	
	

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
		#ifdef __SERIES60_3X__		
			Cba()->SetCommandSetL(R_EXIT_CBA);
			Cba()->DrawDeferred();
		#else
		
		#endif
			iTimeOutTimer = CTimeOutTimer::NewL(CActive::EPriorityStandard,*this);
			iTimeOutTimer->After(KSplashTimeOut);
					
			iMySplashScreen = CMySplashScreen::NewL();
			AddToStackL(iMySplashScreen);
			
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
		#ifdef __SERIES60_3X__
			iMainContainer = new(ELeave)CMainContainer(Cba());
		#else
			iMainContainer = new(ELeave)CMainContainer(NULL);
		#endif
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
	ChangeView(EActivateMainContainer,-1);
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMgAppUi::HandleCommandL(TInt aCommand)
    {
    switch (aCommand)
    {
    case EChangeLayout:
    	#ifdef __SERIES60_3X__
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
    	#else

		#endif
    	break;

    case EAbout:
		{	
		#ifdef __SERIES60_3X__		
			HBufC* Abbout = KtxAboutText().AllocLC();
			TPtr Pointter(Abbout->Des());
			
			CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL(Pointter);
			dlg->PrepareLC(R_ABOUT_HEADING_PANE);
			dlg->SetHeaderTextL(_L("About"));  
			dlg->RunLD();
				
			CleanupStack::PopAndDestroy(1);//,Abbout
		#else
		
		#endif
		}
    	break;
    case EClose:
    	{
    		// splash screen do nothing..
    	}
    	break;
#ifdef __SERIES60_3X__
    case EAknSoftkeyExit:
#endif
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
#ifdef __SERIES60_3X__
	CAknGlobalNote* dialog = CAknGlobalNote::NewLC();
	TInt NoteId = dialog->ShowNoteL(EAknGlobalInformationNote,aMessage);
	User::After(2000000);
	dialog->CancelNoteL(NoteId);
	CleanupStack::PopAndDestroy(dialog);
#else

#endif	
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
#ifdef __SERIES60_3X__
CMgDocument::CMgDocument(CEikApplication& aApp)
	: CAknDocument(aApp)
#else
CMgDocument::CMgDocument(CEikApplication& aApp)
	: CQikDocument(aApp)
#endif
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
/*
	GLDEF_C TInt E32Dll(TDllReason)
	    {
	    return KErrNone;
	    }
*/
#else
	GLDEF_C TInt E32Main()
		{
		return EikStart::RunApplication( NewApplication );
		}
#endif





