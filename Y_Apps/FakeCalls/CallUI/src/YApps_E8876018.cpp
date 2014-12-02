/*
* ==============================================================================

* ==============================================================================
*/

// INCLUDE FILES
#include <avkon.hrh>
#include <aknnotewrappers.h>
#include <stringloader.h>
#include <aknglobalnote.h> 
#include <f32file.h>
#include <s32file.h>
#include <aknlists.h> 
#include <eikstart.h>
#include <APGCLI.H>
#include <aknmessagequerydialog.h> 


#include "YApps_E8876018.h"
#include "YApps_E8876018.hrh"

#include "Common.h"
#ifdef SONE_VERSION
	#include <YApps_2002DD34.rsg>
#else
	#ifdef LAL_VERSION
		#include <YApps_E8876018.rsg>
	#else
		#include <YApps_E8876018.rsg>
	#endif
#endif

#include "Common.h"
#include "Play_Container.h"

const TUid    KPSUidTelephonyCallHandling = { 0x101f8787 };
const TUint32 KTelephonyCallState         = 0x00000004;

/*
-----------------------------------------------------------------------------
    
-----------------------------------------------------------------------------
*/
void CCallEffectsAppUi::ConstructL()
{      	
    // Initialise app UI with standard value.
    BaseConstructL(CAknAppUi::EAknEnableSkin);

    SetOrientationL(EAppUiOrientationPortrait);
    
    iCallIsOn  = iAlarmIsOn = EFalse;
    
    GetTelephonyStateAndType();
   
    
    TRect rect;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, rect);
    
    // Create view object
    iPlayContainer = CPlayContainer::NewL(rect,Cba());
	AddToStackL(iPlayContainer);
	
	iTelephonyMonitor = new(ELeave)CTelephonyMonitor(*this);
	iTelephonyMonitor->ConstructL();
	iCalAlarmObserver = CCalAlarmObserver::NewL(*this);
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CCallEffectsAppUi::GetTelephonyStateAndType()
{
	TInt aVal(0);  
    RProperty kkkRP;
    TInt Err = kkkRP.Get(KPSUidTelephonyCallHandling,KTelephonyCallState,aVal);
    
    if(Err == KErrNone){
        if(aVal == 0 || aVal == 1){
            iCallIsOn = EFalse;
        }else{
            iCallIsOn = ETrue;
        }
    }else{
        iCallIsOn = EFalse;
    }
}

           
           
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CCallEffectsAppUi::CCallEffectsAppUi()
    {
    // No implementation required
    }


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CCallEffectsAppUi::~CCallEffectsAppUi()
    {
	delete iTelephonyMonitor;
	iTelephonyMonitor = NULL;
	
	delete iCalAlarmObserver;
	iCalAlarmObserver = NULL;  	
    
	if ( iPlayContainer )
    {
        RemoveFromStack(iPlayContainer);
    }
        
    delete iPlayContainer;
    iPlayContainer = NULL;
    }

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TInt CCallEffectsAppUi::OpenMobileWEBSiteL(TAny* /*aAny*/)
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
void CCallEffectsAppUi::HandleCommandL( TInt aCommand )
{
    
    switch( aCommand )
        {
    	case EVisitDrJukka:
    	   	{
    	   		OpenMobileWEBSiteL(this);
    	   	}
    	   	break;
    	case EAbout:
    		{				
                HBufC* Abbout(NULL);
                
                if(KAppIsTrial)
                    Abbout = KtxAboutText2().AllocLC();
                else
                    Abbout = KtxAboutText1().AllocLC();
            
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
        case EMeHide:
        	{
				if(iPlayContainer){
					iPlayContainer->SnoozeL();
				}
        	}
        	break;
        case EEikCmdExit:
        case EAknSoftkeyExit:
            Exit();
            break;
        default:
        	if(iPlayContainer){
				iPlayContainer->HandleViewCommandL(aCommand);
        	}
            break;
        }
    } 
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CCallEffectsAppUi::ShowNoteL(const TDesC& aMessage)
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
void CCallEffectsAppUi::HandleStatusPaneSizeChange()
{
	if(iPlayContainer)
	{
		TRect rect;
	    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, rect);
	    
		iPlayContainer->SetRect(rect);
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CCallEffectsAppUi::DoShowAlarmNoteL(const TDes& /*aMessage*/)
{
	iAlarmIsOn = ETrue;
	CCoeEnv::Static()->RootWin().SetOrdinalPosition(-1,0);
	if(iPlayContainer){
		iPlayContainer->SnoozeL();
	}	
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CCallEffectsAppUi::DoHideAlarmNoteL(void)
{
	iAlarmIsOn = EFalse;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CCallEffectsAppUi::NotifyChangeInStatus(CTelephony::TCallStatus& aStatus, TInt aError)
{
	if(aStatus == CTelephony::EStatusUnknown || aStatus == CTelephony::EStatusIdle ){
		iCallIsOn = EFalse;
	}else{
		CCoeEnv::Static()->RootWin().SetOrdinalPosition(-1,0);

		iCallIsOn = ETrue;
		
		if(iPlayContainer){
			iPlayContainer->SnoozeL();
		}
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CCallEffectsAppUi::HandleForegroundEventL(TBool aForeground)
{
	if(iPlayContainer)
	{

	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CCallEffectsAppUi::PsValueChangedL(TInt /*aValue*/,TInt /*aError*/)
{
	if(iPlayContainer)
	{
		iPlayContainer->CheckNormalSettingsL();
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CCallEffectsAppUi::PsValueChangedL(const TDesC8& aValue,TInt aError)
{
	if(aError == KErrNone){
		TBuf<200> hjelpper;
		
		hjelpper.Copy(aValue);
		
		OpenFileL(hjelpper);
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CCallEffectsAppUi::OpenFileL(const TDesC& aFileName)
{
//	ShowNoteL(_L("OpenFileL"));
//	ShowNoteL(aFileName);
	if(iPlayContainer)
	{            
		iPlayContainer->AddContactL(aFileName);
		if(iCallIsOn || iAlarmIsOn){
			iPlayContainer->SnoozeL();
		}else{
			iPlayContainer->StartTimerL();
			iPlayContainer->CheckProfileSettingsL();
			iPlayContainer->StartRingtoneL();
			iPlayContainer->StartCallerImageL();
		}
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CCallEffectsAppUi::ProcessMessageL(TUid aUid,const TDesC8& aParams)
{	
	CEikAppUi::ProcessMessageL(aUid,aParams);
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TBool CCallEffectsAppUi::ProcessCommandParametersL(TApaCommand aCommand,TFileName& aDocumentName,const TDesC8& aTail)
{		
	return CEikAppUi::ProcessCommandParametersL(aCommand, aDocumentName);	
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CCallEffectsDocument* CCallEffectsDocument::NewL( CEikApplication&
                                                          aApp )
    {
    CCallEffectsDocument* self = NewLC( aApp );
    CleanupStack::Pop( self );
    return self;
    }

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CCallEffectsDocument* CCallEffectsDocument::NewLC( CEikApplication&
                                                           aApp )
    {
    CCallEffectsDocument* self =
        new ( ELeave ) CCallEffectsDocument( aApp );

    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CCallEffectsDocument::ConstructL()
    {
    // No implementation required
    }

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CCallEffectsDocument::CCallEffectsDocument( CEikApplication& aApp )
    : CAknDocument( aApp )
    {
    // No implementation required
    }

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CCallEffectsDocument::~CCallEffectsDocument()
    {
    // No implementation required
    }
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CFileStore* CCallEffectsDocument::OpenFileL(TBool aDoOpen, const TDesC& aFilename,RFs& aFs)
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
CEikAppUi* CCallEffectsDocument::CreateAppUiL()
    {
    // Create the application user interface, and return a pointer to it;
    // the framework takes ownership of this object
    
    iMyAppUI = new(ELeave) CCallEffectsAppUi();
    
    return ( static_cast <CEikAppUi*> (iMyAppUI));
    }

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CApaDocument* CCallEffectsApplication::CreateDocumentL()
    {
    // Create an CallEffects document, and return a pointer to it
    return (static_cast<CApaDocument*>
                    ( CCallEffectsDocument::NewL( *this ) ) );
    }

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TUid CCallEffectsApplication::AppDllUid() const
    {
    // Return the UID for the CallEffects application
    return KUidCallUIApp;
    }
    

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
LOCAL_C CApaApplication* NewApplication()
	{
	return new CCallEffectsApplication();
	}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication( NewApplication );
	}    

// End of File

