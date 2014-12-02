/*

 */

#ifdef __SERIES60_3X__
#include <eikstart.h>
#endif 

#include <avkon.hrh>
#include <eikmenup.h>
#include <aknglobalnote.h> 
#include <stringloader.h> 
#include <aknmessagequerydialog.h>
#include <BAUTILS.H>
#include <APGCLI.H>

#include "App_2002B0B0.h"
#include <App_2002B0B0.rsg>
#include "App_2002B0B0.hrh"

_LIT(KtxAppVersion			,"1.05(0)");
_LIT(KtxtApplicationName	,"Flash SMS Sender");
_LIT(KtxAboutText			,"version 1.05(0)\nMay 22nd 2010\nfor S60, Symbian OS 9\n\nCopyright Dr. Jukka Silvennoinen 2006-10, All right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nm.DrJukka.com \ndesktop:\n www.DrJukka.com\n");
_LIT(KMobileJukkaLink		,"m.DrJukka.com");

/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
void CMMSExampleAppUi::ConstructL()
    {
#ifdef __SERIES60_3X__
    BaseConstructL(CAknAppUi::EAknEnableSkin);
#else
	#ifdef __SERIES60_28__
		BaseConstructL(CAknAppUi::EAknEnableSkin);
	#else
		BaseConstructL(0x1008);
	#endif
#endif                                  

    iAppContainer = new (ELeave) CMMSExampleContainer(Cba());
    iAppContainer->SetMopParent(this);
    iAppContainer->ConstructL(ClientRect());

    AddToStackL( iAppContainer );

    iContactDone = EFalse;
    iContactsReader = CContactsReader::NewL(this);
    iContactsReader->StartTheReadL();	
    		
    }

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CMMSExampleAppUi::~CMMSExampleAppUi()
    {
    
    delete iContactsReader;
	iContactsReader = NULL;
	
    if (iAppContainer)
        {
        RemoveFromStack( iAppContainer );
        delete iAppContainer;
        iAppContainer = NULL;
        }
   	}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMMSExampleAppUi::ShowNoteL(const TDesC& aMessage)
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
void CMMSExampleAppUi::ReReadContactL(TContactItemId aItem)
{
	if(iContactsReader)
	{
		iContactsReader->ReReadContactL(aItem);
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
_LIT(KtxEmptyString  ,"");
void CMMSExampleAppUi::GetContactsRTArrayL(RPointerArray<CMyCItem>& aArray)
{
	aArray.ResetAndDestroy();
	
	if(iContactsReader)
	{
		RPointerArray<CMyCItem> ConArray =  iContactsReader->ItemArray();
		for(TInt i=0; i < ConArray.Count(); i++)
		{
			if(ConArray[i])
			{
				CMyCItem* AddItem = new(ELeave)CMyCItem();
				CleanupStack::PushL(AddItem);
					
				if(ConArray[i]->iLastName)
				{
					AddItem->iLastName = ConArray[i]->iLastName->Des().AllocL();
				}
				else
				{
					AddItem->iLastName = KtxEmptyString().AllocL();
				}
						
				if(ConArray[i]->iFirstName)
				{
					AddItem->iFirstName = ConArray[i]->iFirstName->Des().AllocL();
				}
				else
				{
					AddItem->iFirstName = KtxEmptyString().AllocL();
				}
					
				AddItem->iId 		= ConArray[i]->iId;
									
				CleanupStack::Pop(AddItem);
				aArray.Append(AddItem);
			}
		}
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
RPointerArray<CMyCItem>& CMMSExampleAppUi::GetContactsArrayL(void)
{
    return iContactsReader->ItemArray();
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMMSExampleAppUi::ContactReadProgress(TInt /*aProgress*/, TInt /*aTotal*/)
{
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMMSExampleAppUi::ContactReadDone()
{
	iContactDone = ETrue;
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TInt CMMSExampleAppUi::OpenMobileWEBSiteL(TAny* /*aAny*/)
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
void CMMSExampleAppUi::HandleCommandL(TInt aCommand)
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
    case EAknSoftkeyBack:
    case EAknSoftkeyExit:
    case EEikCmdExit:
    case EClose:
    	{
			TBuf<100> titHelp;
    		StringLoader::Load(titHelp,R_SH_STR_EXITQ); 
    				
			CAknQueryDialog* dlg = CAknQueryDialog::NewL();
			if(!dlg->ExecuteLD(R_QUERY,titHelp))
			{
				break;
			}
			PrepareToExit();
			Exit();	
    	}
        break;
    default:
    	if(iAppContainer)
    	{
    		iAppContainer->HandleCommandL(aCommand);
    	}
        break;
        }
    }

/*
------------------------------------------------------------------------------
------------------------------------------------------------------------------
*/
void CMMSExampleAppUi::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
{
	if (aResourceId == R_MMSEXAMPLE_OPTIONS_MENU)
	{	
		if(!iContactDone)
			aMenuPane->SetItemDimmed(EMMSExampleCmdSend, ETrue);
	}
	
    if(iAppContainer)
    {
		iAppContainer->DynInitMenuPaneL(aResourceId,aMenuPane);
    }
}
    
/*
---------------------------------------------------------
---------------------------------------------------------
*/
void CMMSExampleAppUi::HandleStatusPaneSizeChange()
	{
	CAknAppUi::HandleStatusPaneSizeChange(); //call to upper class

	if(iAppContainer)
		iAppContainer->SetRect( ClientRect() );
	}


/*****************************************************************************
 * 
 *************************************************************************** */
CMMSExampleDocument* CMMSExampleDocument::NewL(CEikApplication& aApp)
    {
    CMMSExampleDocument* self = new(ELeave) CMMSExampleDocument(aApp);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); //self.
    return self;
    }

CMMSExampleDocument::CMMSExampleDocument(CEikApplication& aApp)
    : CEikDocument(aApp)
    {
    }

void CMMSExampleDocument::ConstructL()
    {    
    }


CEikAppUi* CMMSExampleDocument::CreateAppUiL()
    {
    return (new(ELeave) CMMSExampleAppUi);
    }


/*****************************************************************************
 * 
 *************************************************************************** */
EXPORT_C CApaApplication* NewApplication()
    {
    return (static_cast<CApaApplication*>(new CMMSExampleApplication));
    }

#ifndef __SERIES60_3X__

GLDEF_C TInt E32Dll( TDllReason )
{
    return KErrNone;
}
#else
// ---------------------------------------------------------
// E32Main()
// Entry point function for new (>= 9.0) EPOC Apps (exe)
// Returns: Sistem Wide error codes or KErrNone if all goes well
// ---------------------------------------------------------
//
GLDEF_C TInt E32Main()
{
	return EikStart::RunApplication( NewApplication );
}
#endif

/*****************************************************************************
 * 
 *************************************************************************** */
TUid CMMSExampleApplication::AppDllUid() const
    {
    
    return KUidMMSExample;
    }

CApaDocument* CMMSExampleApplication::CreateDocumentL()
    {
    return CMMSExampleDocument::NewL( *this );
    }
