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
#include <aknglobalconfirmationquery.h> 
#include "MainContainer.h"


#include "ExampleServerSession.h"
#include "Help_Container.h"
#include "YTools_A000257D.hrh"
#include "Common.h"

#ifdef SONE_VERSION
	#include "YTools_2002B0A1_res.rsg"
#else
	#ifdef LAL_VERSION
		#include "YTools_A000257D_res.rsg"
	#else
		#include "YTools_A000257D_res.rsg"
	#endif
#endif





#include <eikstart.h>
	

const TInt KAknExListFindBoxTextLength = 20;
/*
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/
CCrashContainer::~CCrashContainer()
{
	iArray.ResetAndDestroy();

	delete iMyHelpContainer;
	iMyHelpContainer = NULL;
		
	delete iSelectionBox;
	iSelectionBox = NULL;
	
	delete iBgContext;
	iBgContext = NULL;
	
	if(iResId > 0)
	{
		CEikonEnv::Static()->DeleteResourceFile(iResId);
	}
	
	REComSession::DestroyedImplementation(iDestructorIDKey);
}


/*
-------------------------------------------------------------------------
This will be called when this file shandler is preraped to be used for, 
opening or constructing a file. The open/new etc. functions are called
strait after calling this function.
-------------------------------------------------------------------------
*/
void CCrashContainer::ConstructL(CEikButtonGroupContainer* aCba)
{
	iCba = aCba;
	
 	CreateWindowL();
 	
 	iServerIsOn = IsServerOn();
 	iBgContext = CAknsBasicBackgroundControlContext::NewL(KAknsIIDQsnBgAreaMain,TRect(0,0,1,1), ETrue);
	
 	iResId = -1;
 	TFindFile AppFile(CCoeEnv::Static()->FsSession());
	if(KErrNone == AppFile.FindByDir(KCrashResourceFileName, KNullDesC))
	{
 		TFileName resourceName(AppFile.File());
		BaflUtils::NearestLanguageFile(CEikonEnv::Static()->FsSession(), resourceName);
		iResId = CEikonEnv::Static()->AddResourceFileL(resourceName);
	}
	
	SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect()); 	
	
	
	ActivateL();
	SetMenuL();
	DrawNow();
	
	ChangeIconAndNameL();
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CCrashContainer::ChangeIconAndNameL(void)
{	
 	CEikStatusPane*		sp = ((CAknAppUi*)iEikonEnv->EikAppUi())->StatusPane();
	if(sp)
	{
		CAknTitlePane* TitlePane = STATIC_CAST(CAknTitlePane*,sp->ControlL(TUid::Uid(EEikStatusPaneUidTitle)));
		TitlePane->SetTextL(KCrashApplicationName);
			
    	CAknContextPane* 	ContextPane = (CAknContextPane *)sp->ControlL(TUid::Uid(EEikStatusPaneUidContext));
		if(ContextPane)
		{
			TFindFile AppFile(CCoeEnv::Static()->FsSession());
			if(KErrNone == AppFile.FindByDir(KCrashIconsFileName, KNullDesC))
			{
				TInt BgIndex(0),BgMask(1);
				
				if(iServerIsOn)
				{
					BgIndex = 2;
					BgMask = 3;
				}
				
				if(AknIconUtils::IsMifFile(AppFile.File()))
				{
					AknIconUtils::ValidateLogicalAppIconId(AppFile.File(), BgIndex,BgMask); 
				}
			
				ContextPane->SetPictureFromFileL(AppFile.File(),BgIndex,BgMask);  
			}
		}				
	}	
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
TBool CCrashContainer::IsServerOn()
{
	TBool Ret(EFalse);
	
	TFileName res;
	TFindProcess find;
  	while(find.Next(res) == KErrNone)
    {
    	RProcess ph;
	  	ph.Open(res);
	  	  			
	  	if(ph.SecureId() == (TUint32)KUidCrashServerUID.iUid)
	 	{
	  		Ret = ETrue;
	  		break;
	  	}
	  	
	  	ph.Close();
    }
 
    return Ret;
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CCrashContainer::DisplayListBoxL(void)
{
	iArray.ResetAndDestroy();

	delete iSelectionBox;
	iSelectionBox = NULL;
    iSelectionBox   = new( ELeave ) CAknDoubleStyleListBox();//CAknDoubleNumberStyleListBox();
	
	iSelectionBox->ConstructL(this,EAknListBoxMarkableList);
	
	CDesCArrayFlat* ListArray = new(ELeave)CDesCArrayFlat(1);
	CleanupStack::PushL(ListArray);	

	RExampleServerClient CrashClient;
	CrashClient.Connect();
	
	TExampleItem ItemBuffer;
	
	TInt ItemCount(0);
	CrashClient.GetItemCount(ItemCount);
	for(TInt i=0; i < ItemCount; i++)
	{
		TBool showMe(ETrue);
		
		ItemBuffer.iIndex = i;
		CrashClient.GetItemL(ItemBuffer);	
		
		if(iHideKill0)
		{
			if(ItemBuffer.iExitType == EExitKill && ItemBuffer.iExitReason == 0)
			{
				showMe = EFalse;
			}
		}
		
		if(showMe)
		{
			AddToTheListL(ItemBuffer.iName,ItemBuffer.iExitType,ItemBuffer.iExitReason,ItemBuffer.iExitCatogory,*ListArray);
			AddItemListL(ItemBuffer);
		}
	}
	
	if(!iServerIsOn)
	{
		CrashClient.ShutServerDownNow();
	}
	
	CrashClient.Close();

	CleanupStack::Pop(); //ListArray
	iSelectionBox->Model()->SetItemTextArray(ListArray);
    iSelectionBox->Model()->SetOwnershipType(ELbmOwnsItemArray);
	iSelectionBox->CreateScrollBarFrameL( ETrue );
    iSelectionBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );

	iSelectionBox->SetRect(Rect());
	iSelectionBox->ActivateL();

	UpdateScrollBar(iSelectionBox);
}
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CCrashContainer::AddItemListL(TExampleItem& aItem)
{
	CExampleItem* NewIttem = new(ELeave)CExampleItem();
	
	NewIttem->iProcess = aItem.iProcess.Alloc();
	NewIttem->iName = aItem.iName.Alloc();
	NewIttem->iExitCatogory = aItem.iExitCatogory.Alloc();
	
    NewIttem->iExitType = aItem.iExitType;
    NewIttem->iExitReason = aItem.iExitReason;
	NewIttem->iTime = aItem.iTime;
	
	iArray.Append(NewIttem);
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CCrashContainer::AddToTheListL(const TDesC& aName,const TExitType&  aExitType,const TInt& aExitReason,const TDesC& aExitCategory,CDesCArray& aItemArray)
{
	TBuf<500> AddBuffer(_L("\t"));
	AddBuffer.Append(aName);
	AddBuffer.Append(_L("\t"));

	switch(aExitType)
	{
	case EExitKill:
		AddBuffer.Append(_L("Kill: "));
		break;
	case EExitTerminate:
		AddBuffer.Append(_L("Terminate: "));
		break;
	case EExitPanic:
		AddBuffer.Append(_L("Panic: "));
		break;
	case EExitPending:
		AddBuffer.Append(_L("Pend: "));
		break;
	}
	
	AddBuffer.AppendNum(aExitReason);
	AddBuffer.Append(_L(","));
	AddBuffer.Append(aExitCategory);
	
	aItemArray.AppendL(AddBuffer); 
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CCrashContainer::UpdateScrollBar(CEikListBox* aListBox)
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
-----------------------------------------------------------------------------
Y-Browser will call setsize for client rect when lay-out or size changes,
This will be then called by the framework
-----------------------------------------------------------------------------
*/
void CCrashContainer::SizeChanged()
{
	if ( iBgContext )
	{
		iBgContext->SetRect(Rect());

		if ( &Window() )
		{
			iBgContext->SetParentPos( PositionRelativeToScreen() );
		}
	}
	
 	DisplayListBoxL();
 	
 	if(iMyHelpContainer)
 	{
		iMyHelpContainer->SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect()); // Sets rectangle of lstbox.
 	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CCrashContainer::HandleResourceChange(TInt aType)
{ 		
	TRect rect;
	TBool SetR(EFalse);
	
#ifdef __SERIES60_3X__
    if ( aType==KEikDynamicLayoutVariantSwitch )
    {  	
    	SetR = ETrue;
    	
//		AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EStatusPaneBottom, Brect);
//		AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EStatusPane, Srect);
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
TTypeUid::Ptr CCrashContainer::MopSupplyObject(TTypeUid aId)
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
void CCrashContainer::SetMenuL(void)
{
	if(iMyHelpContainer)
	{
		iCba->SetCommandSetL(R_APPHELP_CBA);
		iCba->DrawDeferred();
	}
	else 
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
void CCrashContainer::InitMenuPanelL(TInt aResourceId, CEikMenuPane* aMenuPane)
{
	if(aResourceId == R_MAIN_MENU)
	{
		TBool Dimopen(ETrue);
		
		if(iSelectionBox)
		{
			TInt CurrInd = iSelectionBox->CurrentItemIndex();
			if(CurrInd >= 0)
			{
				Dimopen = EFalse;		
			}
		}
		
		if(Dimopen)
		{
			aMenuPane->SetItemDimmed(EClearMe,ETrue);
			aMenuPane->SetItemDimmed(EItemInfo,ETrue);
		}
		
		if(iServerIsOn)
		{
			aMenuPane->SetItemDimmed(ESetMeOn,ETrue);
		}
		else
		{
			aMenuPane->SetItemDimmed(ESetMeOff,ETrue);
		}
		
		if(iHideKill0)
		{
			aMenuPane->SetItemDimmed(EHideKill0,ETrue);
		}
		else
		{
			aMenuPane->SetItemDimmed(EShowKill0,ETrue);
		}
	}
}
	
/*
-------------------------------------------------------------------------

-------------------------------------------------------------------------
*/
void CCrashContainer::ForegroundChangedL(TBool aForeground)
{
	if(aForeground)
	{
		HandleCommandL(EResetMe);
	}
}
/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
void CCrashContainer::GetInfoBufferL(CExampleItem& aItem,TDes& aBuffer)
{
	aBuffer.Zero();
	
/*	if(aItem.iProcess)
	{
		aBuffer.Append(_L("Executable:\n"));
		aBuffer.Append(*aItem.iProcess);
	
		aBuffer.Append(_L("\n"));
	}
*/	
	if(aItem.iName)
	{
		aBuffer.Append(_L("Thread name:\n"));
		aBuffer.Append(*aItem.iName);
	
		aBuffer.Append(_L("\n"));
	}
	
	if(aItem.iExitCatogory)
	{
		aBuffer.Append(_L("Exit category:\n"));
		aBuffer.Append(*aItem.iExitCatogory);
	
		aBuffer.Append(_L("\n"));
	}
	
	aBuffer.Append(_L("Exit type:\n"));

	switch(aItem.iExitType)
	{
	case EExitKill:
		aBuffer.Append(_L("Kill"));
		break;
	case EExitTerminate:
		aBuffer.Append(_L("Terminate"));
		break;
	case EExitPanic:
		aBuffer.Append(_L("Panic"));
		break;
	case EExitPending:
		aBuffer.Append(_L("Pend"));
		break;
	}

	aBuffer.Append(_L("\n"));
	aBuffer.Append(_L("Exit reason:\n"));
	aBuffer.AppendNum(aItem.iExitReason);
	
	aBuffer.Append(_L("\n"));
	aBuffer.Append(_L("Time:\n"));
	aBuffer.AppendNum(aItem.iTime.DateTime().Hour());
	aBuffer.Append(_L(":"));
	if(aItem.iTime.DateTime().Minute() < 10)
	{
		aBuffer.AppendNum(0);	
	}
	aBuffer.AppendNum(aItem.iTime.DateTime().Minute());
	aBuffer.Append(_L(":"));
	if(aItem.iTime.DateTime().Second() < 10)
	{
		aBuffer.AppendNum(0);	
	}
	aBuffer.AppendNum(aItem.iTime.DateTime().Second());
	
	aBuffer.Append(_L(" - "));

	if(aItem.iTime.DateTime().Day() < 9)
	{
		aBuffer.AppendNum(0);	
	}
	aBuffer.AppendNum(aItem.iTime.DateTime().Day() + 1);
	aBuffer.Append(_L("/"));
	
	TMonthNameAbb ThisMonth(aItem.iTime.DateTime().Month());
	aBuffer.Append(ThisMonth);
	aBuffer.Append(_L("/"));
	
	TInt Yearrr = aItem.iTime.DateTime().Year();
	TInt Hudreds = Yearrr / 100;
	
	Yearrr = (Yearrr - (Hudreds * 100));
	if(Yearrr < 10)
	{
		aBuffer.AppendNum(0);	
	}
	
	aBuffer.AppendNum(Yearrr);
	aBuffer.Append(_L("\n"));
}
/*
-------------------------------------------------------------------------

-------------------------------------------------------------------------
*/

void CCrashContainer::HandleCommandL(TInt aCommand)
{	
	switch (aCommand)
    { 
	case EAppHelpBack:
		{
			delete iMyHelpContainer;
			iMyHelpContainer = NULL;
		}
		SetMenuL();
		DrawNow();
		break;
	case EAppHelp:
		{
			delete iMyHelpContainer;
			iMyHelpContainer = NULL;    		
			iMyHelpContainer = CMyHelpContainer::NewL();
		}
		SetMenuL();
		DrawNow();    		
		break;      
    case EItemInfo:
		if(iSelectionBox)
		{
			TInt CurrInd = iSelectionBox->CurrentItemIndex();
			if(CurrInd >= 0 && CurrInd <= iArray.Count())
			{
				if(iArray[CurrInd])
				{
		    		HBufC* Informmm = HBufC::NewLC(3000);
		    		
		    		TPtr Pointter(Informmm->Des());
		    		GetInfoBufferL(*iArray[CurrInd],Pointter);
		    		
					TPtr NewPointter(Informmm->Des());
					CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL(NewPointter);
					dlg->PrepareLC(R_ABOUT_HEADING_PANE);
					dlg->SetHeaderTextL(KCrashApplicationName);  
					dlg->RunLD();
					
					CleanupStack::PopAndDestroy(Informmm);
		    	}
			}
		}
    	break;
    case ESetMeOn:
    	{
    		iServerIsOn = ETrue;
    		RExampleServerClient CrashClient;
			CrashClient.Connect();
			CrashClient.Close();
			
			ChangeIconAndNameL();
    	}
    	break;
	case ESetMeOff:
		{
			iServerIsOn = EFalse;
    		RExampleServerClient CrashClient;
			CrashClient.Connect();
			CrashClient.ShutServerDownNow();
			CrashClient.Close();
			
			ChangeIconAndNameL();
    	}
    	break;
	case EClearMe:
		{
			CAknQueryDialog* dlg = CAknQueryDialog::NewL();
			if(dlg->ExecuteLD(R_QUERY,_L("Clear list")))
			{
	    		RExampleServerClient CrashClient;
				CrashClient.Connect();
				CrashClient.ClearItemList();

				if(!iServerIsOn)
				{
					CrashClient.ShutServerDownNow();
				}

				CrashClient.Close();
				
				DisplayListBoxL();
				DisplayListBoxL();
			}
    	}
		DrawNow();
    	break;
    case EHideKill0:
    	{
    		iHideKill0 = ETrue;
    		DisplayListBoxL();
    		DisplayListBoxL();
    	}
    	DrawNow();
    	break;
	case EShowKill0:
    	{
    		iHideKill0 = EFalse;
    		DisplayListBoxL();
    		DisplayListBoxL();
    	}
    	DrawNow();
    	break;
	case EResetMe:
		{
    		DisplayListBoxL();
    	}
    	break;
    case EAbout:
    	{
    		HBufC* Abbout = KCrashAbbout().AllocLC();
			TPtr Pointter(Abbout->Des());
			CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL(Pointter);
			dlg->PrepareLC(R_ABOUT_HEADING_PANE);
			dlg->SetHeaderTextL(KCrashApplicationName);  
			dlg->RunLD();
			
			CleanupStack::PopAndDestroy(Abbout);
    	}
    	break;
    case EBacktoYTasks:
    	if(iTasksHandlerExit)
    	{
    		if(!iServerIsOn)
    		{
    			CAknQueryDialog* dlg = CAknQueryDialog::NewL();
				if(dlg->ExecuteLD(R_QUERY,_L("Set Crash monitor on")))
				{
					RExampleServerClient CrashClient;
					CrashClient.Connect();
					CrashClient.Close();
				}
    		}
    	
    		iTasksHandlerExit->HandlerExitL(this);	
    	}
    	break; 
	default:
		break;
    };
}


/*
-----------------------------------------------------------------------

-----------------------------------------------------------------------
*/
TKeyResponse CCrashContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
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
		case '2':
			if(iSelectionBox)
			{
				iSelectionBox->View()->MoveCursorL(CListBoxView::ECursorPrevScreen,CListBoxView::ENoSelection);
				iSelectionBox->UpdateScrollBarsL();
				iSelectionBox->DrawNow();
			}
			break;
		case '8':
			if(iSelectionBox)
			{
				iSelectionBox->View()->MoveCursorL(CListBoxView::ECursorNextScreen,CListBoxView::ENoSelection);
				iSelectionBox->UpdateScrollBarsL();
				iSelectionBox->DrawNow();
			}
			break;
		case EKeyDevice3:
			; 
			break;
		case EKeyLeftArrow:
	
			break;
		case EKeyRightArrow:
	
			break;
		case EKeyDownArrow:
		case EKeyUpArrow:
		default:
			if(iSelectionBox)
			{
				Ret = iSelectionBox->OfferKeyEventL(aKeyEvent,aType);
			}
			break;
		}
	}
	
	return Ret;
}

/*
-----------------------------------------------------------------------

-----------------------------------------------------------------------
*/
void CCrashContainer::Draw(const TRect& /*aRect*/) const
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
TInt CCrashContainer::CountComponentControls() const
{
	if(iMyHelpContainer)
	{
		return 1;
	}
	else if(iSelectionBox)
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

-------------------------------------------------------------------------
*/
CCoeControl* CCrashContainer::ComponentControl(TInt /*aIndex*/) const
{
	if(iMyHelpContainer)
		return iMyHelpContainer;
	else 
		return iSelectionBox;
}

/*
-------------------------------------------------------------------------
interface function for getting the implementation instance
-------------------------------------------------------------------------
*/
CYTasksContainer* NewFileHandler()
    {
    return (new CCrashContainer);
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
	IMPLEMENTATION_PROXY_ENTRY(0x257D,NewFileHandler)
};
#else

// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = 
    {
    {{0x257D}, NewFileHandler}
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

