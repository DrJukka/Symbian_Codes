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
#include <TXTRICH.H>
#include <CHARCONV.H>
#include <eikmenub.h> 
#include <stringloader.h>

#include "BTSend_A0000F61.h"
#include "YuccaBrowser.hrh"

#include "Help_Container.h"
#include "ObjectExchangeClient.h"
#include <eikstart.h>

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

// I have usually put images under common folder like this
// anyway, it is up to the developer to decide where to put them

/*
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/
CMyFileSender::~CMyFileSender()
{
	if(iObjectExchangeClient)
	{
		iObjectExchangeClient->StopL();
	    iObjectExchangeClient->DisconnectL();
	}
	
	delete iObjectExchangeClient;
	iObjectExchangeClient = NULL;
	
	delete iMyHelpContainer;
	iMyHelpContainer = NULL;
		
	delete iExampleTimer;
	iExampleTimer = NULL;
	
	if(iProgressDialog)
	{
		iProgressDialog->ProcessFinishedL(); 
	}

	iProgressDialog = NULL;
	iProgressInfo = NULL;
	
	delete iSelectionBox;
	iSelectionBox = NULL;
		
	delete iBgContext;
	iBgContext = NULL;
	
	delete iSendFilesList;
	
	// do not remove.. needed for ECOM
	REComSession::DestroyedImplementation(iDestructorIDKey);
}
/*
-----------------------------------------------------------------------
just a internal function for constructing UI listbox
-----------------------------------------------------------------------
*/
void CMyFileSender::DisplayListBoxL(MDesCArray& aFileArray)
{	
	TInt Selected(-1);
	
	if(iSelectionBox)
	{
		Selected = iSelectionBox->CurrentItemIndex();
	}
	
	delete iSelectionBox;
	iSelectionBox = NULL;
	iSelectionBox   = new( ELeave ) CAknSingleStyleListBox();
	iSelectionBox->ConstructL(this);
		
	CDesCArrayFlat* SelectionBoxArray = new(ELeave)CDesCArrayFlat(10);
	
	TFileName Hjelpppp;
	for(TInt i=0; i < aFileArray.MdcaCount(); i++)
	{
		TParsePtrC Hjelpp(aFileArray.MdcaPoint(i));
		
		Hjelpppp.Copy(_L("\t"));
		Hjelpppp.Append(Hjelpp.NameAndExt());
		SelectionBoxArray->AppendL(Hjelpppp);
	}
	
	iSelectionBox->Model()->SetItemTextArray(SelectionBoxArray);
    iSelectionBox->Model()->SetOwnershipType(ELbmOwnsItemArray);
	iSelectionBox->CreateScrollBarFrameL( ETrue );
    iSelectionBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
	iSelectionBox->SetRect(Rect());	
	
//	iSelectionBox->ItemDrawer()->ColumnData()->SetIconArray(iFileHandlerUtils->GetIconUtils().GetIconArrayL(GetIconSize()));
	iSelectionBox->ActivateL();
	
//	if(Selected >= 0)
//	{
//		iSelectionBox->SetCurrentItemIndex(Selected);
//	}
	
	UpdateScrollBar(iSelectionBox);
}
/*
-------------------------------------------------------------------------------
just a internal function for updating listbox scroll bars after changes
-------------------------------------------------------------------------------
*/
void CMyFileSender::UpdateScrollBar(CEikListBox* aListBox)
    {
    if (aListBox)
        {   
        TInt pos(aListBox->View()->CurrentItemIndex());
        if (aListBox->ScrollBarFrame())
            {
            aListBox->ScrollBarFrame()->MoveVertThumbTo(pos);
            }
        }
    }
/*
-------------------------------------------------------------------------
just normal CountComponentControls

See SDK documentations for more information
-------------------------------------------------------------------------
*/
TInt CMyFileSender::CountComponentControls() const
{
	if(iMyHelpContainer)
		return 1;
	else if(iSelectionBox)
		return 1;
	else
		return 0;
}
/*
-------------------------------------------------------------------------
just normal ComponentControl

See SDK documentations for more information
-------------------------------------------------------------------------
*/
CCoeControl* CMyFileSender::ComponentControl(TInt /*aIndex*/) const
{
	if(iMyHelpContainer)
		return iMyHelpContainer;
	else
		return iSelectionBox;
}
/*
-------------------------------------------------------------------------
this will be called right after first constructor has finished
there shouldn't be any need to chnage this one.
-------------------------------------------------------------------------
*/
void CMyFileSender::SetUtils(MYBrowserFileHandlerUtils* aFileHandlerUtils)
{
	// some utilities provided by Y-Browser
	// I will re-design this a bit and add some stuff
	// might also change function definitions
	// could do with feedback on requirements !!
	iFileHandlerUtils = aFileHandlerUtils;
}

/*
-------------------------------------------------------------------------
This will be called when this file shandler is preraped to be used for, 
opening or constructing a file. The open/new etc. functions are called
strait after calling this function.
-------------------------------------------------------------------------
*/
void CMyFileSender::ConstructL()
{
 	CreateWindowL();
 	
	iBgContext = CAknsBasicBackgroundControlContext::NewL(KAknsIIDQsnBgScreen,TRect(0,0,1,1), ETrue);
	
	SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect()); 	
	
	// if you don't want to see the path in there, 
	// then just set it to empty
	iFileHandlerUtils->GetFileUtils().SetNaviTextL(_L(""));	
	
 	ActivateL();
	SetMenuL();
}

/*
-----------------------------------------------------------------------------
Y-Browser will call setsize for client rect when lay-out or size changes,
This will be then called by the framework
-----------------------------------------------------------------------------
*/
void CMyFileSender::SizeChanged()
{
	if ( iBgContext )
	{
		iBgContext->SetRect(Rect());

		if ( &Window() )
		{
			iBgContext->SetParentPos( PositionRelativeToScreen() );
		}
	}
	
	if(iMyHelpContainer)
	{
		iMyHelpContainer->SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect()); // Sets rectangle of lstbox.
	}
	
 	if (iSelectionBox)
    {
        iSelectionBox->SetRect(Rect()); // Sets rectangle of lstbox.
    }
}
/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
TTypeUid::Ptr CMyFileSender::MopSupplyObject(TTypeUid aId)
{	
	if (iBgContext)
	{
		return MAknsControlContext::SupplyMopObject(aId, iBgContext );
	}
	
	return CCoeControl::MopSupplyObject(aId);
}
/*
-------------------------------------------------------------------------
Y-Browser will call this when focus changes.

i.e. When you ask y-browser to open a file, and it will be opened by
other y-browser handler, then you will be called with focus false, 
right before new handler takes focus. Also (maybe) in future editions
this will be called (with ETrue) when file selection (add files) is cancelled

Also called with ETrue, when focus chnages back from other handlers.

-------------------------------------------------------------------------
*/
void CMyFileSender::SetFocusL(TBool aFocus)
{

	if(aFocus)
	{
		CEikStatusPane*		sp = ((CAknAppUi*)iEikonEnv->EikAppUi())->StatusPane();
		if(sp)
		{
			CAknTitlePane* TitlePane = STATIC_CAST(CAknTitlePane*,sp->ControlL(TUid::Uid(EEikStatusPaneUidTitle)));
			TitlePane->SetTextL(KBtSendApplicationName);
				
	   /* 	// if there would be icons could set them in here..
	   		CAknContextPane* 	ContextPane = (CAknContextPane *)sp->ControlL(TUid::Uid(EEikStatusPaneUidContext));
			if(ContextPane)
			{
				TFindFile AppFile(CCoeEnv::Static()->FsSession());
				if(KErrNone == AppFile.FindByDir(KtxIconsFileName, KNullDesC))
				{
					ContextPane->SetPictureFromFileL(AppFile.File(),EMbmIconsB,EMbmIconsB_m);  
				}
			}				
	*/	}		
	
		SetMenuL();
		DrawNow();	
	}
}
/*
-------------------------------------------------------------------------
internal function for updating menu and CBA's for the application
-------------------------------------------------------------------------
*/
void CMyFileSender::SetMenuL(void)
{
	// set own menu & CBA.

	if(iFileHandlerUtils)
	{
		if(iMyHelpContainer)
		{
			iFileHandlerUtils->GetCba().SetCommandSetL(R_PLUG_APPHELP_CBA);				
			iFileHandlerUtils->GetCba().DrawDeferred();
		}
		else
		{
			iFileHandlerUtils->GetCba().SetCommandSetL(R_PLUG_MAIN_CBA);				
			iFileHandlerUtils->GetCba().DrawDeferred();
		}
	}
	
	CEikonEnv::Static()->AppUiFactory()->MenuBar()->SetMenuTitleResourceId(R_BT_MAIN_MENUBAR);	
}
/*
-------------------------------------------------------------------------
Normal DynInitMenuPaneL, which Y-Browser will call.

See SDK documentation for more information
-------------------------------------------------------------------------
*/
void CMyFileSender::DynInitMenuPaneL(TInt /*aResourceId*/, CEikMenuPane* /*aMenuPane*/)
{

}

/*
-------------------------------------------------------------------------
This is the function where Y-Browser gives the file array for sending.
Called after ConstructL, it is advised to copy the array, just to be
on safe side
-------------------------------------------------------------------------
*/
void CMyFileSender::SendFilesL(MDesCArray& aFileArray)
{	
	if(aFileArray.MdcaCount())
	{
		DisplayListBoxL(aFileArray);
		
		delete iSendFilesList;
		iSendFilesList = NULL;
		iSendFilesList = new(ELeave)CDesCArrayFlat(aFileArray.MdcaCount());
		
		TFileName Hjelpppp;
		for(TInt i=0; i < aFileArray.MdcaCount(); i++)
		{
			iSendFilesList->AppendL(aFileArray.MdcaPoint(i));
		}

		iProgressDialog = new (ELeave) CAknProgressDialog((REINTERPRET_CAST(CEikDialog**,&iProgressDialog)),ETrue);
		iProgressDialog->PrepareLC(R_PROGRESS_NOTE);
		iProgressInfo = iProgressDialog->GetProgressInfoL();
		iProgressDialog->SetCallback(this);
		iProgressDialog->RunLD();
		iProgressInfo->SetFinalValue((aFileArray.MdcaCount() + 1));
	
		//Start the process...i.e. replace timer with real sender...
	
		if(iSendFilesList->MdcaCount())
		{
			iSendCount = 0;
			
			delete iObjectExchangeClient;
			iObjectExchangeClient = NULL;
   			iObjectExchangeClient = CObjectExchangeClient::NewL(*this,iSendFilesList->MdcaPoint(0));
    
			if(iProgressInfo)
			{
				iProgressInfo->SetAndDraw(iSendCount);	
			}
			
			iObjectExchangeClient->ConnectL();
		}
		else
		{
			HandleCommandL(EBacktoYBrowser);
		}
	}
	else
	{
		HandleCommandL(EBacktoYBrowser);
	}
}
/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
void CMyFileSender::LogL( const TDesC& aText )
{
	if(iProgressDialog)
	{
		iProgressDialog->SetTextL(aText);
	}
}
/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
void CMyFileSender::LogL( const TDesC& aText, const TDesC& aExtraText )
{
	if(iProgressDialog)
	{
		TFileName Hjrlppp;
		Hjrlppp.Copy(aText);
		Hjrlppp.Append(_L(": "));
		Hjrlppp.Append(aExtraText);
		
		iProgressDialog->SetTextL(Hjrlppp);
	}
}
/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
void CMyFileSender::LogL( const TDesC& aText, TInt aNumber )
{
	if(iProgressDialog)
	{
		TFileName Hjrlppp;
		Hjrlppp.Copy(aText);
		Hjrlppp.Append(_L(": "));
		Hjrlppp.AppendNum(aNumber);
		iProgressDialog->SetTextL(Hjrlppp);
	}
}
/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
void CMyFileSender::DoneL(TInt aError,const TDesC& /*aFileName*/)
{
	if(iProgressDialog && aError != KErrNone)
	{
        TFileName Hjrlppp;
        StringLoader::Load(Hjrlppp,R_STR_SNDERR);
	
		Hjrlppp.AppendNum(aError);
		iProgressDialog->SetTextL(Hjrlppp);
	}
	
	TBool MoreToSend(EFalse);
	iSendCount = iSendCount + 1;
	
	if(iSendFilesList)
	{
		if(iSendCount < iSendFilesList->Count())
		{
			MoreToSend = ETrue;
		}
	}
	
	if(iObjectExchangeClient && MoreToSend)
	{
		iObjectExchangeClient->SendObjectL(iSendFilesList->MdcaPoint(iSendCount));
			
		if(iProgressInfo)
		{
			iProgressInfo->SetAndDraw(iSendCount);	
		}
	}
	else
	{
		if(iProgressDialog)	
		{
			iProgressDialog->ProcessFinishedL(); 
		}

		iProgressDialog = NULL;
		iProgressInfo = NULL;
	}
}
/*
-------------------------------------------------------------------------
Handle command called by Y-Browser,make sure you set your menu & CBA's 
when focus changes, and that your commands are started at least with
first command as 0x7000.

-------------------------------------------------------------------------
*/

void CMyFileSender::HandleCommandL(TInt aCommand)
{
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
    case EReSend:
    	if(iSendFilesList)
    	{
    		if(iSendFilesList->MdcaCount())
    		{
	    		iProgressDialog = new (ELeave) CAknProgressDialog((REINTERPRET_CAST(CEikDialog**,&iProgressDialog)),ETrue);
				iProgressDialog->PrepareLC(R_PROGRESS_NOTE);
				iProgressInfo = iProgressDialog->GetProgressInfoL();
				iProgressDialog->SetCallback(this);
				iProgressDialog->RunLD();
				iProgressInfo->SetFinalValue((iSendFilesList->MdcaCount() + 1));
			
	    		iSendCount = 0;
				
				delete iObjectExchangeClient;
				iObjectExchangeClient = NULL;
	   			iObjectExchangeClient = CObjectExchangeClient::NewL(*this,iSendFilesList->MdcaPoint(0));
	    
				if(iProgressInfo)
				{
					iProgressInfo->SetAndDraw(iSendCount);	
				}
				
				iObjectExchangeClient->ConnectL();
    		}
    	}
    	break;
    case EPlugAbout:
    	{
    		HBufC* Abbout = KBtSendAbbout().AllocLC();
			TPtr Pointter(Abbout->Des());
			CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL(Pointter);
			dlg->PrepareLC(R_ABOUT_HEADING_PANE);
			dlg->SetHeaderTextL(KBtSendApplicationName);  
			dlg->RunLD();
			
			CleanupStack::PopAndDestroy(Abbout);
    	}
    	break;
    case EBacktoYBrowser:
    	if(iFileHandlerUtils)
    	{
    		// I'll update this function later
    		// i needs a TInt identifier added to it.
    		iFileHandlerUtils->HandleExitL(this,MYBrowserFileHandlerUtils::ENoChangesMade);	
    	}
    	break; 
	default:
		break;
    };
}


/*
-----------------------------------------------------------------------
normal implementation of the OfferKeyEventL, called  by Y-Browser when 
in focus

See SDK documentations for more information
-----------------------------------------------------------------------
*/
TKeyResponse CMyFileSender::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
	TKeyResponse Ret = EKeyWasNotConsumed;
	
	if(iMyHelpContainer)
	{
		Ret = iMyHelpContainer->OfferKeyEventL(aKeyEvent,aType);
	}
	else
	{
		switch (aKeyEvent.iCode)
		{
		case EKeyDevice3:
		//	CEikonEnv::Static()->AppUiFactory()->MenuBar()->TryDisplayMenuBarL();
			break;
		case EKeyUpArrow:
		case EKeyDownArrow:
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
void CMyFileSender::Draw(const TRect& aRect) const
{
 	CWindowGc& gc = SystemGc();
 	if(iBgContext)
	{
		MAknsSkinInstance* skin = AknsUtils::SkinInstance();
		AknsDrawUtils::Background( skin, iBgContext, this, gc, Rect() );
	}
	else
	{
	 	gc.Clear(aRect);
	}
}

/*
-----------------------------------------------------------------------------
normal MProgressDialogCallback callback function.

See SDK documentations for more information
----------------------------------------------------------------------------
*/
void CMyFileSender::DialogDismissedL (TInt /*aButtonId*/)
{		  		
	iProgressDialog = NULL;
	iProgressInfo = NULL;
	
	if(iObjectExchangeClient)
	{
		iObjectExchangeClient->StopL();
	    iObjectExchangeClient->DisconnectL();
	}
	
	delete iObjectExchangeClient;
	iObjectExchangeClient = NULL;
}

/*
-----------------------------------------------------------------------------
used just for pretending to send something...
----------------------------------------------------------------------------
*/
void CMyFileSender::TimerExpired(TAny* /*aTimer*/,TInt /*aError*/)
{
	HandleCommandL(EBacktoYBrowser);
}
/*
-------------------------------------------------------------------------
interface function for getting the implementation instance
-------------------------------------------------------------------------
*/
CYBrowserFileSender* NewFileSender()
    {
    return (new CMyFileSender);
    }
/*
-------------------------------------------------------------------------
ECOM ImplementationTable function

See SDK documentations for more information
-------------------------------------------------------------------------
*/

LOCAL_D const TImplementationProxy ImplementationTable[] = 
{
	IMPLEMENTATION_PROXY_ENTRY(0x0F61,NewFileSender)
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

