/*
	Copyright (C) 2006 Jukka Silvennoinen
	All right reserved
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
#include <stringloader.h>
#include <eikstart.h>

#include "YToolsMailFolders.h"
#include "YuccaBrowser.hrh"

#include "Help_Container.h"
#include "mail_Container.h"

#include "Common.h"
#ifdef SONE_VERSION
    #include <YFB_2002B0AA.rsg>
#else
    #ifdef LAL_VERSION

    #else
        #ifdef __YTOOLS_SIGNED
            #include <YuccaBrowser_2000713D.rsg>
        #else
            #include <YuccaBrowser.rsg>
        #endif
    #endif
#endif

/*
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/
CYBrowserFileHandler1::~CYBrowserFileHandler1()
{	
	delete iMyHelpContainer;
	iMyHelpContainer = NULL;
	
	delete iMailBoxContainer;
	iMailBoxContainer = NULL;
	
	if(iDebugFile.SubSessionHandle())
	{
		iDebugFile.Write(_L8("Bye, bye, "));	
	  	iDebugFile.Close();
	}
	
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
This will be called when this file shandler is preraped to be used for, 
opening or constructing a file. The open/new etc. functions are called
strait after calling this function.
-------------------------------------------------------------------------
*/
void CYBrowserFileHandler1::ConstructL()
{
 	CreateWindowL();

//	_LIT(KRecFilename			,"C:\\ZipUI.txt");
//	CEikonEnv::Static()->FsSession().Delete(KRecFilename);		
//	User::LeaveIfError(iDebugFile.Create(CEikonEnv::Static()->FsSession(),KRecFilename,EFileWrite|EFileShareAny));	

	if(iDebugFile.SubSessionHandle())
	{	
		iDebugFile.Write(_L8("start, "));
	}
	
	SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect()); 	
	
	iMailBoxContainer = CMailBoxContainer::NewL(CEikonEnv::Static()->EikAppUi()->ClientRect(),*iFileHandlerUtils,*this);
	
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
	if(iMailBoxContainer)
	{
		iMailBoxContainer->SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
	}
	
	if(iMyHelpContainer)
	{
		iMyHelpContainer->SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
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
			TBuf<60> smallHelp;
			
			StringLoader::Load(smallHelp,R_SH_STR_NAME);

			CAknTitlePane* TitlePane = STATIC_CAST(CAknTitlePane*,sp->ControlL(TUid::Uid(EEikStatusPaneUidTitle)));
			TitlePane->SetTextL(smallHelp);
				
	   /* 	CAknContextPane* 	ContextPane = (CAknContextPane *)sp->ControlL(TUid::Uid(EEikStatusPaneUidContext));
			if(ContextPane)
			{
				TFindFile AppFile(CCoeEnv::Static()->FsSession());
				if(KErrNone == AppFile.FindByDir(KtxIconsFileName, KNullDesC))
				{
					ContextPane->SetPictureFromFileL(AppFile.File(),EMbmIconsConicon,EMbmIconsConicon_m);  
				}
			}*/				
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
	if(iMyHelpContainer)
	{
		iFileHandlerUtils->GetCba().SetCommandSetL(R_PLUG_APPHELP_CBA);				
		iFileHandlerUtils->GetCba().DrawDeferred();
	}
	else if(iMailBoxContainer)
	{
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->SetMenuTitleResourceId(R_MAILBOX_MENUBAR);	
		
		iFileHandlerUtils->GetCba().SetCommandSetL(R_PLUG_MAIN_CBA);				
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
	
	if(iMailBoxContainer)
	{
		iMailBoxContainer->InitMenuPanelL(aResourceId, aMenuPane);
	}
}

/*
-------------------------------------------------------------------------
we only support one file type, so we don't care what aFileTypeID is
-------------------------------------------------------------------------
*/
void CYBrowserFileHandler1::OpenFileL(const TDesC& /*aFileName*/,const TDesC& /*aFileTypeID*/)	
{
	HandleCommandL(EBacktoYBrowser);
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
	case EPlugAppHelpBack:
		{
			delete iMyHelpContainer;
			iMyHelpContainer = NULL;
		}
		SetMenuL();
		DrawNow();
		break;
	case EPlugAppHelp:
		{
			delete iMyHelpContainer;
			iMyHelpContainer = NULL;    		
			iMyHelpContainer = CMyHelpContainer::NewL();
		}
		SetMenuL();
		DrawNow();    		
		break;
    case EPlugAbout:
    	{			
    		HBufC* Abbout = KMailAbbout().AllocLC();
			TPtr Pointter(Abbout->Des());
			CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL(Pointter);
			dlg->PrepareLC(R_ABOUT_HEADING_PANE);
			dlg->SetHeaderTextL(KMailAbboutName);  
			dlg->RunLD();
			
			CleanupStack::PopAndDestroy(Abbout);
    	}
    	break;
    case EBacktoYBrowser:
    	if(iFileHandlerUtils)
    	{
    		iFileHandlerUtils->HandleExitL(this,MYBrowserFileHandlerUtils::EChangesMade);	
    	}
    	break; 
	default:
		if(iMailBoxContainer)
		{
			iMailBoxContainer->HandleViewCommandL(aCommand);
		}
		break;
    };
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
	
	if(iMyHelpContainer)
	{
		Ret = iMyHelpContainer->OfferKeyEventL(aKeyEvent,aType);
	}
	else if(iMailBoxContainer)
	{
		Ret = iMailBoxContainer->OfferKeyEventL(aKeyEvent,aType);
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
 //	CWindowGc& gc = SystemGc();
 //	gc.Clear(aRect);
}
/*
-------------------------------------------------------------------------
just normal CountComponentControls

See SDK documentations for more information
-------------------------------------------------------------------------
*/
TInt CYBrowserFileHandler1::CountComponentControls() const
{
	if(iMyHelpContainer)
	{
		return 1;
	}
	else if(iMailBoxContainer)
	{
		return 1;
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
	if(iMyHelpContainer)
	{
		return iMyHelpContainer;
	}
	else if(iMailBoxContainer)
	{
		return iMailBoxContainer;
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

LOCAL_D const TImplementationProxy ImplementationTable[] = 
{
	IMPLEMENTATION_PROXY_ENTRY(0x0F69,NewFileHandler)
};


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

