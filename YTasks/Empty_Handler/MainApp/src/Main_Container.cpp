/* 	
	Copyright (c) 2001 - 2007, 
	J.P. Silvennoinen.
	All rights reserved 
*/

#include <barsread.h>
#include <CHARCONV.H>
#include <EIKSPANE.H>
#include <EIKBTGPC.H>
#include <eikmenub.h>
#include <bautils.h>
#include <EIKPROGI.H>
#include <APGCLI.H>
#include <eikclbd.h>
#include <EIKFUTIL.H>
#include <AknGlobalNote.h>
#include <akncontext.h> 

#include "YTools_A000257B.h" 

#include "Main_Container.h"
#include "YTools_A000257B.h"
#include "YTools_A000257B.hrh"


#include <YTools_A000257B.rsg>

_LIT(KtxIconFileName		,"\\resource\\apps\\YTools_A000257B.mif");

_LIT(KtxBtSendText1			,"You can now install Y-Tasks from another phones inbox\nNOTE: to allow sending Y-Tasks please run Y-Tasks before removing the installation file from the inbox\n\n If you are having prolems on the instalaltion, check that the time settings (year, month & day) are correctly set and that installation of \"All\" is enabled from the Application managers settings\n");
_LIT(KtxBtSendText2			,"Y-SuperDistribution was disabled, only a link to Y-Tasks homepage was sent. Please download Y-Tasks using the link.\n");

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CMainContainer::CMainContainer(CEikButtonGroupContainer* aCba)
:iCba(aCba)
{		

}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CMainContainer::~CMainContainer()
{  	
	delete iYTasksContainer;
	iYTasksContainer = NULL;
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::ConstructL()
{
	CreateWindowL();	

	SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());

	ActivateL();
	SetMenuL();
	DrawNow();

}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::SizeChanged()
{

}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/

void CMainContainer::HandleResourceChange(TInt aType)
{

	TRect rect;
	TBool SetR(EFalse);
	
    if ( aType==KEikDynamicLayoutVariantSwitch )
    {  	
    	SetR = ETrue;
    	
//		AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EStatusPaneBottom, Brect);
//		AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EStatusPane, Srect);
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, rect);
    }
 
 
 	if(SetR)
 	{
 	   
	    SetRect(rect);
 	}
 	
	CCoeControl::HandleResourceChange(aType);
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::Draw(const TRect& /*aRect*/) const
{
	CWindowGc& gc = SystemGc();
	gc.Clear(Rect());
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
TKeyResponse CMainContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aEventCode)
{
	TKeyResponse Ret = EKeyWasNotConsumed;	

	if(iYTasksContainer)
	{
		Ret = iYTasksContainer->OfferKeyEventL(aKeyEvent,aEventCode);
	}
	else
	{
		switch (aKeyEvent.iCode)
	    {
	    case EKeyDevice3:
	    	//CEikonEnv::Static()->AppUiFactory()->MenuBar()->TryDisplayMenuBarL();
			break;
		case EKeyDownArrow:
		case EKeyUpArrow:
		default:
			
			break;
		}
	}
	
	return Ret;
}
/*
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/
void CMainContainer::ForegroundChangedL(TBool aForeground)
{
	if(iYTasksContainer)
	{
		iYTasksContainer->ForegroundChangedL(aForeground);
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::HandlerExitL(CYTasksContainer* /*aHandler*/)
{
	delete iYTasksContainer;
	iYTasksContainer = NULL;
	
	CEikStatusPane*		sp = ((CAknAppUi*)iEikonEnv->EikAppUi())->StatusPane();
	if(sp)
	{
		CAknTitlePane* TitlePane = STATIC_CAST(CAknTitlePane*,sp->ControlL(TUid::Uid(EEikStatusPaneUidTitle)));
		TitlePane->SetTextToDefaultL();

		CAknContextPane* 	ContextPane = (CAknContextPane *)sp->ControlL(TUid::Uid(EEikStatusPaneUidContext));
		if(ContextPane)
		{
			ContextPane->SetPictureToDefaultL();
		}

		if(sp->CurrentLayoutResId() != R_AVKON_STATUS_PANE_LAYOUT_USUAL)
		{
			sp->SwitchLayoutL(R_AVKON_STATUS_PANE_LAYOUT_USUAL);
		}
		
		sp->DrawNow();
	}
	
	SetMenuL();
	DrawNow();	
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::ShowNoteL(const TDesC& aMessage)
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

void CMainContainer::HandleViewCommandL(TInt aCommand)
{
	switch(aCommand)
	{
	case EAppOpen:
		if(!iYTasksContainer)
		{
			const TUid		KMyUID = { 0x312C};

			TAny *impl;  // pointer to interface implementation
			impl = REComSession::CreateImplementationL(KMyUID, _FOFF(CYTasksContainer,iDestructorIDKey));
			if ( impl )
			{
				CYTasksContainer* Ret = ((CYTasksContainer*)impl);
				Ret->iTasksHandlerExit = this;				
				CleanupStack::PushL(Ret);
				Ret->ConstructL(iCba);
				CleanupStack::Pop();//Ret
				
				iYTasksContainer = Ret;
			}
			
			impl = NULL;

		}
		SetMenuL();
		DrawNow();
		break;
	default:
		if(iYTasksContainer)
		{
			iYTasksContainer->HandleCommandL(aCommand);
		}
		break;
	}	
}

	
			
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CCoeControl* CMainContainer::ComponentControl( TInt /*aIndex*/) const
{
	return iYTasksContainer;	
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TInt CMainContainer::CountComponentControls() const
{	
	if(iYTasksContainer)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
   

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::SetMenuL(void)
{
	if(iYTasksContainer)
	{
		iYTasksContainer->SetMenuL();
	}
	else
	{	
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->StopDisplayingMenuBar();
		if(iCba)
		{
			CEikonEnv::Static()->AppUiFactory()->MenuBar()->SetMenuTitleResourceId(R_MAIN_MENUBAR);
			
			iCba->SetCommandSetL(R_MAIN_CBA);
			iCba->DrawDeferred();
		}
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::InitMenuPanelL(TInt aResourceId, CEikMenuPane* aMenuPane)
{

}



