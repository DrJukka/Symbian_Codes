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
#include <BACLIPB.H>
#include <aknglobalnote.h> 
#include <APGCLI.H>
 
#include "MainContainer.h"
#include "YApp_20007145.h"
#include "YApp_20007145.hrh" 
#include <YApp_20007145.rsg> 

/*
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/
CMainContainer::~CMainContainer()
{
	if(iProgressDialog)
	{
		iProgressDialog->ProcessFinishedL(); 
	}

	iProgressDialog = NULL;
	iProgressInfo = NULL;

	delete iBgContext;
	iBgContext = NULL;
	
}


/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
void CMainContainer::ConstructL(CEikButtonGroupContainer* aCba,CMgAppUi* aAppUi)
{
	iCba = aCba;
	iAppUi = aAppUi;
	
 	CreateWindowL();
  	iBgContext = CAknsBasicBackgroundControlContext::NewL(KAknsIIDQsnBgScreen,TRect(0,0,1,1), ETrue);
	
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
	if ( iBgContext )
	{
		iBgContext->SetRect(Rect());

		if ( &Window() )
		{
			iBgContext->SetParentPos( PositionRelativeToScreen() );
		}
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::HandleResourceChange(TInt aType)
{	
	TRect rect;
	TBool SetR(EFalse);
	
#ifdef __SERIES60_3X__
    if ( aType==KEikDynamicLayoutVariantSwitch )
    {  	
    	SetR = ETrue;
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, rect);
    }
#else
    if ( aType == 0x101F8121 )
    {
    	SetR = ETrue;
    	rect = CEikonEnv::Static()->EikAppUi()->ClientRect();
    }
#endif   
 
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
TTypeUid::Ptr CMainContainer::MopSupplyObject(TTypeUid aId)
{	
	if (iBgContext)
	{
		return MAknsControlContext::SupplyMopObject(aId, iBgContext );
	}
	
	return CCoeControl::MopSupplyObject(aId);
}

/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
void CMainContainer::ForegroundChangedL(TBool /*aForeground*/)
{
	
}
/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
void CMainContainer::SetMenuL(void)
{

	{
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->SetMenuTitleResourceId(R_MAIN_MENUBAR);				
		iCba->SetCommandSetL(R_MAIN_CBA);
		iCba->DrawDeferred();
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::InitMenuPanelL(TInt /*aResourceId*/, CEikMenuPane* /*aMenuPane*/)
{
/*	if(aResourceId == R_MAIN_MENU)
	{		
		if(!iFileReader)
		{
			aMenuPane->SetItemDimmed(EReReadFile,ETrue);
		}
	}*/
} 

/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
void CMainContainer::OpenFileL(const TDesC& aFileName)	
{
	TParsePtrC Hjelpper(aFileName);
	
	TFileName TargetFile;
	if(SelectTargetL(TargetFile))
	{
		TargetFile.Append(Hjelpper.NameAndExt());
		
		TBool OkToDo(ETrue);
		
		if(BaflUtils::FileExists(CCoeEnv::Static()->FsSession(),TargetFile))
		{
			CAknQueryDialog* dlg = CAknQueryDialog::NewL();
			if(!dlg->ExecuteLD(R_QUERY,_L("File exists, replace ?")))
			{
				OkToDo = EFalse;
			}
		}
		
		if(OkToDo)
		{
			BaflUtils::EnsurePathExistsL(CCoeEnv::Static()->FsSession(),TargetFile);
			
			TInt Err= BaflUtils::CopyFile(CCoeEnv::Static()->FsSession(),aFileName,TargetFile);
		
			if(Err == KErrNone)
			{
				GlobalMsgNoteL(_L("File Copied"));
			}
			else
			{
				TBuf<100> Errbuff(_L("Error copying file: "));
				Errbuff.AppendNum(Err);
				GlobalMsgNoteL(Errbuff);
			}
		}
	}
	
//	iAppUi->HandleCommandL(EEikCmdExit);
}

/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
TBool CMainContainer::SelectTargetL(TDes& aTarget)
{
	TBool OkToContinue(EFalse);

	CAknSinglePopupMenuStyleListBox* list = new(ELeave) CAknSinglePopupMenuStyleListBox;
    CleanupStack::PushL(list);

	CAknPopupList* popupList = CAknPopupList::NewL(list,R_AVKON_SOFTKEYS_OK_CANCEL);
    CleanupStack::PushL(popupList);
	
    list->ConstructL(popupList, CEikListBox::ELeftDownInViewRect);
	list->CreateScrollBarFrameL(ETrue);
    list->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,CEikScrollBarFrame::EAuto);
    
    CDesCArrayFlat* Ittems = new(ELeave)CDesCArrayFlat(5);
    CleanupStack::PushL(Ittems);
    
    Ittems->AppendL(_L("Phone"));
    Ittems->AppendL(_L("Memory card"));
	
    CleanupStack::Pop(Ittems);
    list->Model()->SetItemTextArray(Ittems);
	list->Model()->SetOwnershipType(ELbmOwnsItemArray);

	popupList->SetTitleL(_L("Copy to:"));
	
	if (popupList->ExecuteLD())
    {
		TInt Ret = list->CurrentItemIndex();
		switch(Ret)
		{
		case 0:
			OkToContinue = ETrue;
			aTarget.Copy(KtxTargetCDrive);
			break;
		case 1:
			OkToContinue = ETrue;
			aTarget.Copy(KtxTargetEDrive);
			break;
		};
    }
    
    CleanupStack::Pop();            // popuplist
    CleanupStack::PopAndDestroy();  // list

	return OkToContinue;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::GlobalMsgNoteL(const TDesC&  aMessage)
{
	CAknGlobalNote* dialog = CAknGlobalNote::NewLC();
	TInt NoteId = dialog->ShowNoteL(EAknGlobalInformationNote,aMessage);
	User::After(2000000);
	dialog->CancelNoteL(NoteId);
	CleanupStack::PopAndDestroy(dialog);
}
/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/

void CMainContainer::HandleCommandL(TInt /*aCommand*/)
{	
/*	switch (aCommand)
    {
    case :
    	{

    	}
    	break;
	default:
		break;
    };*/
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CMainContainer::DialogDismissedL (TInt /*aButtonId*/)
{		  		
	iProgressDialog = NULL;
	iProgressInfo = NULL;
	
/*	if(iHTTPEngine && aButtonId < 0)
	{
		iHTTPEngine->CancelTransaction();
	}*/
}

/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
TKeyResponse CMainContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode /*aType*/)
{
	TKeyResponse Ret = EKeyWasNotConsumed;
	
	switch (aKeyEvent.iCode)
    {
	case EKeyDevice3:
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->TryDisplayMenuBarL();
		break;
	case EKeyUpArrow:
	case EKeyDownArrow:
	default:
		break;
    };
    
	return Ret;
}

/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CMainContainer::Draw(const TRect& /*aRect*/) const
{
	if(iBgContext)
	{
	 	CWindowGc& gc = SystemGc();
	  	MAknsSkinInstance* skin = AknsUtils::SkinInstance();
		AknsDrawUtils::Background( skin, iBgContext, this, gc, Rect() );
	}
}
/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
TInt CMainContainer::CountComponentControls() const
{
	return 0;
}
/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
CCoeControl* CMainContainer::ComponentControl(TInt /*aIndex*/) const
{
	return NULL;
}

