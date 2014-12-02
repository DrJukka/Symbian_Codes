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

_LIT(KtxAppVersion					,"0.50");
_LIT(KtxtApplicationName			,"Y-Tasks");
_LIT(KtxAboutText					,"version 0.50\nJune 9th 2007\nfor S60, Symbian OS 9,\n\nCopyright:\nJukka Silvennoinen 2006-07\nAll right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nwww.DrJukka.com");

//
// CMgAppUi
//
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
	
CMgAppUi::~CMgAppUi()
{
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
	BaseConstructL(CAknAppUi::EAknEnableSkin);
	
	iMainContainer = new(ELeave)CMainContainer(Cba());

	AddToStackL(iMainContainer);
	iMainContainer->SetMopParent(this);
	iMainContainer->ConstructL();
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





