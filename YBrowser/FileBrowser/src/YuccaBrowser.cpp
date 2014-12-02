/* 
	Copyright (c) 2006, 
	Jukka Silvennoinen
	All rights reserved 
*/

#include "YuccaBrowser.h"
#include "YuccaBrowser.hrh"

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

#include "YuccaBrowser_Container.h"

#include <aknmessagequerydialog.h> 
#include <AknQueryDialog.h>

#include <sendui.h> 
#include <cmessagedata.h> 
#include <senduimtmuids.h>
#include <EIKPROC.H>
#include <APGCLI.H>
#include <stringloader.h> 



/*
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/
void CYBrowserAppUi::ConstructL()
{
	BaseConstructL(CAknAppUi::EAknEnableSkin);
	iSendUi = CSendUi::NewL(); 

/*	_LIT(KRecFilename			,"C:\\YBrwsr_Dbg.txt");
	CCoeEnv::Static()->FsSession().Delete(KRecFilename);		
	iDebugFile.Create(CCoeEnv::Static()->FsSession(),KRecFilename,EFileWrite|EFileShareAny);
*/	
	iFileView = CFileListContainer::NewL(ClientRect(),Cba());    
	AddToStackL(iFileView);
	iFileView->SetMopParent(this);
}
/*
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/
CYBrowserAppUi::CYBrowserAppUi()                              
    {
	// no implementation required
    }
/*
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/
CYBrowserAppUi::~CYBrowserAppUi()
{
    if (iFileView)
    {
        RemoveFromStack(iFileView);
        delete iFileView;
        iFileView = NULL;
    }
    
    delete iSendUi;
	iSendUi = NULL;
	
	if(iDebugFile.SubSessionHandle())
	{
		iDebugFile.Close();
	}
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CYBrowserAppUi::LogToFileL(const TDesC& aStuff)
{
	TBuf8<255> hejlpper;
	
	if(aStuff.Length() > 255)
		hejlpper.Copy(aStuff.Left(255));
	else
		hejlpper.Copy(aStuff);
	
	LogToFileL(hejlpper);
}

void CYBrowserAppUi::LogToFileL(const TDesC8& aStuff)
{
	if(iDebugFile.SubSessionHandle())
	{
		iDebugFile.Write(aStuff);
		iDebugFile.Write(_L8("\r\n"));
	}
}
/*
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/
void CYBrowserAppUi::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
{		
	if(R_Y_BROWSER_MENU == aResourceId)
	{

	}
	else if(R_EXTRAS_MENU == aResourceId)
	{

	}
		
	if(iFileView)
	{
		iFileView->InitMenuPanelL(aResourceId,aMenuPane);
	}
}


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CYBrowserAppUi::NotifyExit(TExitMode /*aMode*/)
{
	CApaProcess* DocProcess = iHandlerDoc->Process();
	DocProcess->DestroyDocument(iHandlerDoc); 
	iHandlerDoc = NULL;
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CYBrowserAppUi::HandleStatusPaneSizeChange()
{
	if(iFileView)
	{
		iFileView->SetRect(ClientRect());
	}	

//CAknAppUi::HandleStatusPaneSizeChange();
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TInt CYBrowserAppUi::OpenMobileWEBSiteL(TAny* /*aAny*/)
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
---------------------------------------------------------------------------------------

---------------------------------------------------------------------------------------
*/
void CYBrowserAppUi::HandleCommandL(TInt aCommand)
    {
    switch(aCommand)
        {
        case EExtrasLayout:
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
        	break;
	    case ESendFiles:
			if(iFileView)	
        	{
        		if(iFileView->SendWithSendUIL())
        		{
        			CreateAndSendMessageL();
        		}
        	}
        	break;
		case EAbout:
			{       		                     				
				HBufC* Abbout = KMainAbbout().AllocLC();
				TPtr Pointter(Abbout->Des());
										
				CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL(Pointter);
				dlg->PrepareLC(R_ABOUT_HEADING_PANE);
				dlg->SetHeaderTextL(KMainApplicationName);  
										
				TCallBack callback1(OpenMobileWEBSiteL);
				dlg->SetLink(callback1);
				dlg->SetLinkTextL(KMobileJukkaLink);
												    
				dlg->RunLD();
											
				CleanupStack::PopAndDestroy(1);//Abbout
			}
			break;
        case EClose:
        case EAknSoftkeyExit:
        	if(iFileView)
        	{
        		if(iFileView->AskToConfirmExit())
        		{
	        		TBuf<200> ExBuf;
	        		
	        		StringLoader::Load(ExBuf,R_CMD_EXIT);
	        		
	        		ExBuf.Append(_L(" "));
	        		ExBuf.Append(KMainApplicationName);
	        		
		        	CAknQueryDialog* dlg = CAknQueryDialog::NewL();
					if(!dlg->ExecuteLD(R_QUERY,ExBuf))
					{
						break;
					}
        		}
			
				iFileView->SavePathL();
        	}
        case EEikCmdExit:
        
            Exit();
            break;
        default:
			if(iFileView)
			{
				iFileView->HandleViewCommandL(aCommand);
			}
            break;
        }
    }


/*
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/

void CYBrowserAppUi::CreateAndSendMessageL(void)
{
	if(iFileView && iSendUi)
	{
		CDesCArrayFlat* MyArray = new(ELeave)CDesCArrayFlat(1);
		CleanupStack::PushL(MyArray);
		
		iFileView->AddSelectedFilesL(MyArray,EFalse);
		
		if(MyArray->Count())
		{	
			CMessageData*  Mymessage = CMessageData::NewLC(); 
			
			//Mymessage->AppendToAddressL(_L("pillu.pallo@pullo.com"));
            
			for(TInt i=0; i< MyArray->Count(); i++)
			{
				Mymessage->AppendAttachmentL(MyArray->MdcaPoint(i));
			}
		
			TSendingCapabilities MySendCapa;
			MySendCapa.iFlags  = TSendingCapabilities::ESupportsAttachments;//EAllServices;//
	
			TUid MyServiceUid = iSendUi->ShowSendQueryL(Mymessage,MySendCapa);
            if(iSendUi->ValidateServiceL(MyServiceUid,MySendCapa))		
            {
            	iSendUi->CreateAndSendMessageL(MyServiceUid,Mymessage);
            }
           
            CleanupStack::PopAndDestroy( Mymessage);       		
		}
		
		CleanupStack::PopAndDestroy(MyArray);
	}
}
/*
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/
CYBrowserDocument* CYBrowserDocument::NewL(CEikApplication& aApp)
    {
    CYBrowserDocument* self = NewLC(aApp);
    CleanupStack::Pop(self);
    return self;
    }
/*
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/
CYBrowserDocument* CYBrowserDocument::NewLC(CEikApplication& aApp)
    {
    CYBrowserDocument* self = new (ELeave) CYBrowserDocument(aApp);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }
/*
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/
void CYBrowserDocument::ConstructL()
    {
	// no implementation required
    }    
/*
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/
CYBrowserDocument::CYBrowserDocument(CEikApplication& aApp) : CAknDocument(aApp) 
    {
	// no implementation required
    }
/*
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/
CYBrowserDocument::~CYBrowserDocument()
    {
	// no implementation required
    }
/*
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/
CEikAppUi* CYBrowserDocument::CreateAppUiL()
    {
    CEikAppUi* appUi = new (ELeave) CYBrowserAppUi;
    return appUi;
    }
/*
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/
CApaDocument* CYBrowserApplication::CreateDocumentL()
    {  
    CApaDocument* document = CYBrowserDocument::NewL(*this);
    return document;
    }
/*
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/
TUid CYBrowserApplication::AppDllUid() const
    {
    return KUidYBrowserApp;
    }

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/

EXPORT_C CApaApplication* NewApplication()
    {
    return (new CYBrowserApplication);
    }
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
  
GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication( NewApplication );
	}

