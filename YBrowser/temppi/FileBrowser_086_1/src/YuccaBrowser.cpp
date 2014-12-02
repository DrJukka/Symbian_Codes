/* 
	Copyright (c) 2006, 
	Jukka Silvennoinen
	All rights reserved 
*/

#include "YuccaBrowser.h"
#include "YuccaBrowser.hrh"

#ifdef __YTOOLS_SIGNED
	#include <YuccaBrowser_2000713D.rsg>
#else
	#include <YuccaBrowser.rsg>
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


_LIT(KtxAbbout						,"version 0.86 (Test)\nDecember 16th 2007\nfor S60, Symbian OS 9,\n\nCopyright Jukka Silvennoinen 2006, All right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nwww.DrJukka.com\nGraphics by P@sko\nwww.jaspp.net\n");



/*
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/
void CYBrowserAppUi::ConstructL()
{
	BaseConstructL(CAknAppUi::EAknEnableSkin);
	iSendUi = CSendUi::NewL(); 

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
				HBufC* Abbout = KtxAbbout().AllocLC();
				TPtr Pointter(Abbout->Des());
				CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL(Pointter);
				dlg->PrepareLC(R_ABOUT_HEADING_PANE);
				dlg->SetHeaderTextL(KtxApplicationName);  
				dlg->RunLD();
				
				CleanupStack::PopAndDestroy(Abbout);
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
	        		ExBuf.Append(KtxApplicationName);
	        		
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
			
			for(TInt i=0; i< MyArray->Count(); i++)
			{
				Mymessage->AppendAttachmentL(MyArray->MdcaPoint(i));
			}
					
			TSendingCapabilities MySendCapa;
			MySendCapa.iFlags  = TSendingCapabilities::ESupportsAttachments;
	
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

