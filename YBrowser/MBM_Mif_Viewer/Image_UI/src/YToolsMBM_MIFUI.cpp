/*
	Copyright (C) 2006 Jukka Silvennoinen
	
*/

// INCLUDE FILES
#include <avkon.hrh>
#include <APPARC.H>
#include <aknnotewrappers.h>
#include <stringloader.h>
#include <aknnotewrappers.h>
#include <BAUTILS.H> 
#include <ecom.h>
#include <implementationproxy.h>
#include <aknmessagequerydialog.h> 
#include <akniconarray.h> 
#include <akntitle.h> 
#include <akncontext.h> 
#include <eikmenub.h> 

#include "YToolsMBM_MIFUI.h"
#include "YToolsMBM_MIFUI.hrh"
#include "YToolsMBM_MIFUI_res.rsg"

#include "CommonStuff.h"
#include "ImageView.h"

#include "icons.mbg"


#ifdef __SERIES60_3X__
	#include <eikstart.h>
#else
#endif

/*
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/
CYBrowserFileHandler1::~CYBrowserFileHandler1()
{
	
	if(iResId > 0)
	{
		CEikonEnv::Static()->DeleteResourceFile(iResId);
	}
	
	if(iRFile.SubSessionHandle())
	{
		iRFile.Close();
	}

	if(iDebugFile.SubSessionHandle())
	{
		iDebugFile.Write(_L8("Bye, bye, "));	
	  	iDebugFile.Close();
	}
	
	delete iImageView;
	iImageView = NULL;
	
	REComSession::DestroyedImplementation(iDestructorIDKey);
}
/*
-------------------------------------------------------------------------
this will be called right after first constructor has finished
there shouldn't be any need to chnage this one.
-------------------------------------------------------------------------
*/
void CYBrowserFileHandler1::SetUtils(MYBrowserFileHandlerUtils* aFileHandlerUtils)
{
	iFileHandlerUtils = aFileHandlerUtils;
}

/*
-------------------------------------------------------------------------
	else if(aIndex == 2)
	{
		aPartialString = EFalse;
		aFlags = EYBFilehandlerSupportsRFile;
	
		return 	KtxTypeImageOLP().Alloc();
	}
	else if(aIndex == 3)
	{
		aPartialString = EFalse;
		aFlags = EYBFilehandlerSupportsRFile;
	
		return 	KtxTypeImageOTAtwo().Alloc();
	}
	else if(aIndex == 4)
	{
		aPartialString = EFalse;
		aFlags = EYBFilehandlerSupportsRFile;
	
		return 	KtxTypeImageOTA().Alloc();
	}
This will be called when this file shandler is preraped to be used for, 
opening or constructing a file. The open/new etc. functions are called
strait after calling this function.
-------------------------------------------------------------------------
*/
void CYBrowserFileHandler1::ConstructL()
{
 	CreateWindowL();
 	
 	iResId = -1;
 	TFindFile AppFile(CCoeEnv::Static()->FsSession());
	if(KErrNone == AppFile.FindByDir(KMyResourceFileName, KNullDesC))
	{
 		TFileName resourceName(AppFile.File());
		BaflUtils::NearestLanguageFile(CEikonEnv::Static()->FsSession(), resourceName);
		iResId = CEikonEnv::Static()->AddResourceFileL(resourceName);
	}
	
//	_LIT(KRecFilename			,"C:\\ZipUI.txt");
//	CEikonEnv::Static()->FsSession().Delete(KRecFilename);		
//	User::LeaveIfError(iDebugFile.Create(CEikonEnv::Static()->FsSession(),KRecFilename,EFileWrite|EFileShareAny));	

	if(iDebugFile.SubSessionHandle())
	{	
		iDebugFile.Write(_L8("start, "));
	}
	
	iFileHandlerUtils->GetFileUtils().SetNaviTextL(_L(""));	
	
	SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect()); 	
	
 	ActivateL();
	SetMenuL();
}

/*
-----------------------------------------------------------------------------
Y-Browser will call setsize for client rect when lay-out or size changes,
This will be then called by the framework
-----------------------------------------------------------------------------
*/
void CYBrowserFileHandler1::SizeChanged()
{
	if(iImageView)
	{
		iImageView->SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
	}
}

/*
-------------------------------------------------------------------------
Y-Browser will call this when focus changes.

i.e. When you ask y-browser to open a file, and it will be opened by
other y-browser handler, then you will be called with focus false, 
right before new handler takes focus.

And when it returns back to y-browser, this function will be called with
ETrue,
-------------------------------------------------------------------------
*/
void CYBrowserFileHandler1::SetFocusL(TBool aFocus)
{
	if(iDebugFile.SubSessionHandle())
	{	
		iDebugFile.Write(_L8("focusing, "));
	}
	
	if(aFocus)
	{			
		CEikStatusPane*		sp = ((CAknAppUi*)iEikonEnv->EikAppUi())->StatusPane();
		if(sp)
		{	
			if(iContextName.Length())
			{
				CAknTitlePane* TitlePane = STATIC_CAST(CAknTitlePane*,sp->ControlL(TUid::Uid(EEikStatusPaneUidTitle)));
				TitlePane->SetTextL(iContextName);
			}
			
	    	CAknContextPane* 	ContextPane = (CAknContextPane *)sp->ControlL(TUid::Uid(EEikStatusPaneUidContext));
			if(ContextPane)
			{
				TFindFile AppFile(CCoeEnv::Static()->FsSession());
				if(KErrNone == AppFile.FindByDir(KtxIconsFileName, KNullDesC))
				{
					ContextPane->SetPictureFromFileL(AppFile.File(),EMbmIconsConicon,EMbmIconsConicon_m);  
				}
			}				
		}
		
		SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
		SetMenuL();
		DrawNow();	
	}
}
/*
-------------------------------------------------------------------------
internal function for updating menu and CBA's for the application
-------------------------------------------------------------------------
*/
void CYBrowserFileHandler1::SetMenuL(void)
{
	if(iImageView)
	{
		iImageView->SetMenuL();
	}
	else if(iFileHandlerUtils)
	{
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->SetMenuTitleResourceId(R_MAIN_MENUBAR);	
		
		iFileHandlerUtils->GetCba().SetCommandSetL(R_MAIN_CBA);				
		iFileHandlerUtils->GetCba().DrawDeferred();
	}
}
/*
-------------------------------------------------------------------------
Normal DynInitMenuPaneL, which Y-Browser will call.

See SDK documentation for more information
-------------------------------------------------------------------------
*/
void CYBrowserFileHandler1::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
{
	if(iDebugFile.SubSessionHandle())
	{	
		iDebugFile.Write(_L8("DynMenu, "));
	}
	
	if(iImageView)
	{
		iImageView->InitMenuPanelL(aResourceId, aMenuPane);
	}
}

/*
-------------------------------------------------------------------------
we only support one file type, so we don't care what aFileTypeID is
-------------------------------------------------------------------------
*/
void CYBrowserFileHandler1::OpenFileL(const TDesC& aFileName,const TDesC& aFileTypeID)	
{
	if(iDebugFile.SubSessionHandle())
	{	
		iDebugFile.Write(_L8("Openfile, "));
	}
	
	if(BaflUtils::FileExists(CCoeEnv::Static()->FsSession(),aFileName))
	{
		iZipFileName.Copy(aFileName);
		
		iImageView = CImageView::NewL(iFileHandlerUtils, CEikonEnv::Static()->EikAppUi()->ClientRect());
		
		if(aFileTypeID == KtxTypeIMageMIF
		|| aFileTypeID == KtxTypeIMageMBM)
		{
			if(aFileTypeID == KtxTypeIMageMIF)
			{
				iContextName.Copy(KtxMIFViewer);
			}
			else
			{
				iContextName.Copy(KtxMBMViewer);
			}
			
			iImageView->OpenMBMFileL(iZipFileName);
		
			CEikStatusPane*		sp = ((CAknAppUi*)iEikonEnv->EikAppUi())->StatusPane();
			if(sp)
			{
				CAknTitlePane* TitlePane = STATIC_CAST(CAknTitlePane*,sp->ControlL(TUid::Uid(EEikStatusPaneUidTitle)));
				TitlePane->SetTextL(iContextName);
			}
			
			SetMenuL();
			DrawNow();
		}
		else
		{
			if(iFileHandlerUtils)
			{
				iFileHandlerUtils->GetFileUtils().ShowFileErrorNoteL(iZipFileName,KErrNotSupported);
			}
		
			HandleCommandL(EBacktoYBrowser);
		}
		
		
	}
	else
	{
		if(iFileHandlerUtils)
		{
			iFileHandlerUtils->GetFileUtils().ShowFileErrorNoteL(iZipFileName,KErrNotFound);
		}
		
		HandleCommandL(EBacktoYBrowser);
	}
}
/*
-------------------------------------------------------------------------
we only support one file type, so we don't care what aFileTypeID is
-------------------------------------------------------------------------
*/
void CYBrowserFileHandler1::OpenFileL(RFile& /*aOpenFile*/,const TDesC& /*aFileTypeID*/)
{
	HandleCommandL(EBacktoYBrowser);
}

/*
-------------------------------------------------------------------------
we only support one filetype, so no need to wonder what aFileTypeID is

-------------------------------------------------------------------------
*/
void CYBrowserFileHandler1::NewFileL(const TDesC& /*aPath*/,const TDesC& /*aFileTypeID*/)
{
	HandleCommandL(EBacktoYBrowser);
}
void CYBrowserFileHandler1::NewFileL(const TDesC& /*aPath*/,const TDesC& /*aFileTypeID*/,MDesCArray& /*aFileArray*/)
{
	HandleCommandL(EBacktoYBrowser);
}
/*
-------------------------------------------------------------------------
we are not supporting adding files to existing zip files
so no need to implement this function
-------------------------------------------------------------------------
*/
void CYBrowserFileHandler1::AddFilesL(const TDesC& /*aFileName*/,MDesCArray& /*aFileArray*/)
{	
	HandleCommandL(EBacktoYBrowser);
}
/*
-------------------------------------------------------------------------
Handle command called by Y-Browser,make sure you set your menu & CBA's 
when focus chnages, and that your commands are started at least with
first command as 0x7000.

-------------------------------------------------------------------------
*/

void CYBrowserFileHandler1::HandleCommandL(TInt aCommand)
{
	if(iDebugFile.SubSessionHandle())
	{	
		iDebugFile.Write(_L8("cmd, "));
	}
	
	switch (aCommand)
    { 
    case EAbout:
    	{
    		HBufC* Abbout = KtxAbbout().AllocLC();
			TPtr Pointter(Abbout->Des());
			CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL(Pointter);
			dlg->PrepareLC(R_ABOUT_HEADING_PANE);
			dlg->SetHeaderTextL(KtxApplicationName);  
			dlg->RunLD();
			
			CleanupStack::PopAndDestroy(Abbout);
    	}
    	break;
    case EBacktoYBrowser:
    	if(iFileHandlerUtils)
    	{
    		iFileHandlerUtils->HandleExitL(this,MYBrowserFileHandlerUtils::ENoChangesMade);	
    	}
    	break; 
	default:
		if(iImageView)
		{
			iImageView->HandleViewCommandL(aCommand);
		}
		break;
    };
}

/*
-----------------------------------------------------------------------------
internal function implementing MAknFileFilter
----------------------------------------------------------------------------
*/
TBool CYBrowserFileHandler1::Accept (const TDesC& /*aDriveAndPath*/, const TEntry &aEntry) const 
{
	TBool Ret(EFalse);

	if(aEntry.IsDir())
	{
		if(aEntry.IsReadOnly()
		|| aEntry.IsHidden()
		|| aEntry.IsSystem())
		{
			
		}
		else 
		{
			Ret = ETrue;
		}		
	}

	return Ret;
}

/*
-----------------------------------------------------------------------
normal implementation of the OfferKeyEventL, called  by Y-Browser when in focus

See SDK documentations for more information
-----------------------------------------------------------------------
*/
TKeyResponse CYBrowserFileHandler1::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
	TKeyResponse Ret = EKeyWasNotConsumed;
	
	if(iImageView)
	{
		Ret = iImageView->OfferKeyEventL(aKeyEvent,aType);
	}
	else
	{
		switch (aKeyEvent.iCode)
	    {
		case EKeyDevice3:
			CEikonEnv::Static()->AppUiFactory()->MenuBar()->TryDisplayMenuBarL();
			break;
		case EKeyRightArrow:
		default:
			break;
	    };
	}
	
	return Ret;
}
/*
-----------------------------------------------------------------------
just normal Draw

See SDK documentations for more information
-----------------------------------------------------------------------
*/
void CYBrowserFileHandler1::Draw(const TRect& aRect) const
{
 	CWindowGc& gc = SystemGc();
 	gc.Clear(aRect);
}
/*
-------------------------------------------------------------------------
just normal CountComponentControls

See SDK documentations for more information
-------------------------------------------------------------------------
*/
TInt CYBrowserFileHandler1::CountComponentControls() const
{
	if(iImageView)
	{
		return iImageView->CountComponentControls();	
	}
	else
	{
		return 0;
	}
}
/*
-------------------------------------------------------------------------
just normal ComponentControl

See SDK documentations for more information
-------------------------------------------------------------------------
*/
CCoeControl* CYBrowserFileHandler1::ComponentControl(TInt aIndex) const
{
	if(iImageView)
	{
		return iImageView->ComponentControl(aIndex);	
	}
	else
	{
		return NULL;
	}
}
	
/*
-------------------------------------------------------------------------
interface function for getting the implementation instance
-------------------------------------------------------------------------
*/
CYBrowserFileHandler* NewFileHandler()
    {
    return (new CYBrowserFileHandler1);
    }
/*
-------------------------------------------------------------------------
ECom ImplementationTable function

See SDK documentations for more information
-------------------------------------------------------------------------
*/
#ifdef __SERIES60_3X__
LOCAL_D const TImplementationProxy ImplementationTable[] = 
{
	IMPLEMENTATION_PROXY_ENTRY(0x0F65,NewFileHandler)
};
#else

// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = 
    {
    {{0x0F65}, NewFileHandler}
    };
    
// DLL Entry point
GLDEF_C TInt E32Dll(TDllReason /*aReason*/)
    {
    return(KErrNone);
    }
#endif

/*
-------------------------------------------------------------------------
ECom ImplementationGroupProxy function

See SDK documentations for more information
-------------------------------------------------------------------------
*/
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
{
	aTableCount = sizeof(ImplementationTable)/sizeof(ImplementationTable[0]);
	return ImplementationTable;
}

