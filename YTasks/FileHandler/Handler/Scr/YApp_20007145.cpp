/* Copyright (c) 2001 - 2005, Jukka Silvennoinen, Solution One Telecom LTd. All rights reserved */


#include <mtclreg.h>                        // for CClientMtmRegistry 
#include <msvids.h>                         // for Message type IDs
#include <mmsclient.h>                      // for CMmsClientMtm
#include <AknQueryDialog.h>                 // for CAknTextQueryDialog
#include <apparc.h>
#include <eikapp.h>
#include <BAUTILS.H>
#include <aknglobalnote.h> 
#include <sendui.h> 
#include <senduimtmuids.h>
#include <aknmessagequerydialog.h> 
#include <APGWGNAM.H> 


#include "YApp_20007145.h"                     // own definitions

#include "YApp_20007145.hrh" 
#include <YApp_20007145.rsg> 

#ifdef __SERIES60_3X__
	#include <eikstart.h>
#else

#endif


//
// CMgAppUi
//
/*
-----------------------------------------------------------------------------

    CMgAppUi::~CMgAppUi()

    Destructor.

-----------------------------------------------------------------------------
*/
CMgAppUi::~CMgAppUi()
{   	
	if(iMainContainer)
		RemoveFromStack(iMainContainer);
	
	delete iMainContainer;
	iMainContainer = NULL;


	if (iDoorObserver)
		iDoorObserver->NotifyExit(MApaEmbeddedDocObserver::ENoChanges);
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
		    
	iMainContainer = new(ELeave)CMainContainer();
	AddToStackL(iMainContainer);
	iMainContainer->ConstructL(Cba(),this);
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMgAppUi::SetFileData(TFileName& aFileName, TDes8& /*aData*/)
{
	if (iMainContainer)
	{
        iMainContainer->OpenFileL(aFileName);
    }
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMgAppUi::HandleCommandL(TInt aCommand)
    {
    switch (aCommand)
    {
    case EAbout:
		{
			HBufC* Abbout = KtxAboutText().AllocLC();
			TPtr Pointter(Abbout->Des());
			CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL(Pointter);
			dlg->PrepareLC(R_ABOUT_HEADING_PANE);
			dlg->SetHeaderTextL(KtxtApplicationName);  
			dlg->RunLD();
			
			CleanupStack::PopAndDestroy(Abbout);
		}
    	break;
    case EAknSoftkeyExit:
    case EEikCmdExit:
    case EClose:
        if(iDoorObserver)
        {
			SaveL();
        }
		Exit();
        break;
    default:
    	if(iMainContainer)
	    {
	    	iMainContainer->HandleCommandL(aCommand); 
	    }
        break;
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
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMgAppUi::OpenFileL(const TDesC& aFileName)
{
	// File changed. Open new file with documents OpenFileL method.
	CHandlerDocument* doc = static_cast<CHandlerDocument*> (Document());
	doc->OpenFileL( ETrue, aFileName, iEikonEnv->FsSession() );    
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TBool CMgAppUi::ProcessCommandParametersL(TApaCommand /*aCommand*/,TFileName& /*aDocumentName*/)
{
    return ETrue;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TBool CMgAppUi::ProcessCommandParametersL(TApaCommand /*aCommand*/,TFileName& /*aDocumentName*/,const TDesC8& /*aTail*/)
{
    return ETrue;
}
    
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------

void CMgAppUi::ProcessMessageL(TUid aUid,const TDesC8& aParams)
{
	TFileName OpenFile;
	OpenFile.Copy(aParams);
	
	if(BaflUtils::FileExists(CCoeEnv::Static()->FsSession(),OpenFile))
	{
		iMainContainer->OpenFileL(OpenFile);
	}
	
	CEikAppUi::ProcessMessageL(aUid,aParams);
}*/
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMgAppUi::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
{
	if (aResourceId == R_MAIN_MENU)
	{
	///	if(DimNew)
	//	{
	//		aMenuPane->SetItemDimmed(EVCardNew,ETrue);
	///	}
	}

	
	if(iMainContainer)
	{
		iMainContainer->InitMenuPanelL(aResourceId,aMenuPane);
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMgAppUi::GlobalMsgNoteL(const TDesC&  aMessage)
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
void CMgAppUi::HandleResourceChangeL(TInt aType)
{
	CAknAppUi::HandleResourceChangeL(aType); //call to upper class

    // ADDED FOR SCALABLE UI SUPPORT
    // *****************************
	//if ( aType == KEikDynamicLayoutVariantSwitch )
	//hard coded constant so it can be compiled with first edition
	
	if ( aType == 0x101F8121 && iMainContainer)
	{
	    iMainContainer->SetRect(ClientRect());
	}
}
//
// CMgDocument
//
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CHandlerDocument* CHandlerDocument::NewL(CEikApplication& aApp)
    {
    CHandlerDocument* self = NewLC(aApp);
    CleanupStack::Pop(self);
    return self;
    }
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CHandlerDocument* CHandlerDocument::NewLC(CEikApplication& aApp)
    {
    CHandlerDocument* self = new (ELeave) CHandlerDocument(aApp);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CHandlerDocument::ConstructL()
    {
    // no implementation required
    }
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CHandlerDocument::CHandlerDocument(CEikApplication& aApp) : CAknDocument(aApp)
    {
    // no implementation required
    }
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CHandlerDocument::~CHandlerDocument()
    {
    // no implementation required
    }
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CEikAppUi* CHandlerDocument::CreateAppUiL()
    {
    // Create the application user interface, and return a pointer to it,
    // the framework takes ownership of this object
    iAppUi = new (ELeave) CMgAppUi;
    return iAppUi;
    }
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CFileStore* CHandlerDocument::OpenFileL( TBool aDoOpen, const TDesC& aFilename, RFs& aFs )
    {
    if (aDoOpen)
        {
        TFileName name = aFilename;
        if( iFileName == name ) //The same file, do nothing
            {
            return NULL;   
            }
        iFileName = name;
        CMgAppUi *appui = static_cast<CMgAppUi *> (iAppUi);
        TBuf8<255> buf;

        RFile file;
        
        TInt err = file.Open(aFs, aFilename, EFileRead|EFileShareAny); 
        if( err == KErrNone )
        {
            CleanupClosePushL(file);
            
            TInt FilSizz(0);
		    if(file.Size(FilSizz) == KErrNone)
		    {
		    	TInt ReadNoe(254);
		    	if(FilSizz > 0)
		    	{
		    		if(FilSizz < 255)
		    		{
		    			ReadNoe = FilSizz;
		    		}
		    	
		    		file.Read(buf,ReadNoe);	
		    	}
		    } 
		    
            CleanupStack::PopAndDestroy();//file
        }
               
        appui->SetFileData(iFileName, buf);
        }
    
    return NULL;
    }
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CHandlerDocument::OpenFileL(CFileStore*& aFileStore, RFile& aFile)
{
    aFileStore = NULL; //So the other OpenFileL version is not called

    TFileName name;
    #ifdef __SERIES60_3X__
        aFile.FullName(name);
        //aFile.FullName(name); //If the location is required
    #endif
    //This function is never used in 1st or 2nd edition,
    //so the RFile::Name function is always executed.
    //That way we know that name variable contains aFile name.
    if( iFileName == name)
    {
        return;
    }
    
    iFileName = name;
    
    TBuf8<255> buf;
    
    TInt FilSizz(0);
    if(aFile.Size(FilSizz) == KErrNone)
    {
    	TInt ReadNoe(254);
    	if(FilSizz > 0)
    	{
    		if(FilSizz < 255)
    		{
    			ReadNoe = FilSizz;
    		}
    	
    		aFile.Read(buf,ReadNoe);	
    	}
    }
     
    CMgAppUi *appui = static_cast<CMgAppUi *> (iAppUi);
    appui->SetFileData(iFileName, buf);
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/

CApaDocument* CHandlerApplication::CreateDocumentL()
    {  
    // Create an handler document, and return a pointer to it
    CApaDocument* document = CHandlerDocument::NewL(*this);
    return document;
    }

TUid CHandlerApplication::AppDllUid() const
    {
    // Return the UID for the handler application
    return KUidPhoneManager;
    }

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
EXPORT_C CApaApplication* NewApplication()
    {
    return (static_cast<CApaApplication*>(new CHandlerApplication));
    }

#ifndef __SERIES60_3X__

GLDEF_C TInt E32Dll( TDllReason )
{
    return KErrNone; 
}
#else

GLDEF_C TInt E32Main()
{
	return EikStart::RunApplication( NewApplication );
}
#endif



