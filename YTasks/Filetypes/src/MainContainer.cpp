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
#include <DocumentHandler.h>
#include <APGCLI.H>

#include "MainServerSession.h"

#include "ShowString.h"
#include "MainListView.h"
#include "MainContainer.h"
#include "NewTypesContainer.h"

#include "YTools_A0000F5D.hrh"
#include "Help_Container.h"
#include "Common.h"

#ifdef SONE_VERSION
	#include "YTools_2002B0A8_res.rsg"
#else
	#ifdef LAL_VERSION
		#include "YTools_A0000F5D_res.rsg"
	#else
		#include "YTools_A0000F5D_res.rsg"
	#endif
#endif



const TInt KAknExListFindBoxTextLength = 20;


_LIT(KtxSelectPriority			,"Select mapping priority");
_LIT(KtxPriorityLow				,"Priority low");
_LIT(KtxPriorityNormal			,"Priority normal");
_LIT(KtxPriorityHigh			,"Priority High");
_LIT(KtxPriorityMax				,"Priority Max");

/*
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/
CMainContainer::~CMainContainer()
{
	delete iMyHelpContainer;
	iMyHelpContainer = NULL;
	
	delete iShowString;
	iShowString = NULL;
	
	delete iMainListView;
	iMainListView = NULL;
	
	delete iMyTepesView;
	iMyTepesView = NULL;
	
	delete iListBox;
	iListBox = NULL;
	
	delete iBgContext;
	iBgContext = NULL;
	
	if(iResId > 0)
	{
		CEikonEnv::Static()->DeleteResourceFile(iResId);
	}
	
	REComSession::DestroyedImplementation(iDestructorIDKey);
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CMainContainer::HandleServerAppExit(TInt /*aReason*/)
{
	
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CMainContainer::ConstructListBoxL()
{
	TInt MySetIndex(0);
	
	if(iListBox)
	{
		MySetIndex = iListBox->CurrentItemIndex();
	}
	
	delete iListBox;
	iListBox = NULL;	
	iListBox   = new( ELeave )CAknDoubleStyleListBox();	
	iListBox->ConstructL(this,EAknListBoxMarkableList);	
	
	CDesCArrayFlat* MyArray = new(ELeave)CDesCArrayFlat(10);
	CleanupStack::PushL(MyArray);
	
	CDocumentHandler* DocHand = CDocumentHandler::NewLC();
	
	RApaLsSession ls;
	User::LeaveIfError(ls.Connect());
	CleanupClosePushL(ls);
	
	CDataTypeArray* allDataTypes = new(ELeave)CDataTypeArray(50);
	CleanupStack::PushL(allDataTypes);
	ls.GetSupportedDataTypesL(*allDataTypes);
	
	TKeyArrayFix SortKey(0, ECmpNormal);	
	allDataTypes->Sort(SortKey);

	TApaAppInfo AppInfo;	
	TUid HandlerAppUid;
	TFileName HelpFn;
	TBool CheckUID(EFalse);
	TInt Errnum(KErrNone);

	for(TInt i=(allDataTypes->Count()- 1); i >=0 ; i--)
	{
		if(allDataTypes->At(i).Des().Length())
		{
			AppInfo.iUid.iUid = 0;
			AppInfo.iFullName.Zero();
			AppInfo.iCaption.Zero();
			AppInfo.iShortCaption.Zero();

			HandlerAppUid.iUid = 0;
			
			Errnum = KErrNone;
			CheckUID = EFalse;
			
			TRAP(Errnum,
			if(DocHand->CanOpenL(allDataTypes->At(i)))
			{
				CheckUID = ETrue;
			}
			
			if(!CheckUID)
			{
				if(DocHand->CanHandleL(allDataTypes->At(i)))
				{
					CheckUID = ETrue;
				}	
			}
			
			if(!CheckUID)
			{
				if(DocHand->CanSaveL(allDataTypes->At(i)))
				{
					CheckUID = ETrue;
				}	
			}
			
			if(CheckUID)
			{
				DocHand->HandlerAppUid(HandlerAppUid);
			}	
			);
			
			if(Errnum != KErrNone && Errnum != KErrNotSupported)
			{
				ls.AppForDataType(allDataTypes->At(i),HandlerAppUid);
			}
			
			if(((TUint)HandlerAppUid.iUid) > 0)
			{
				ls.GetAppInfo(AppInfo,HandlerAppUid);
			}
			
			HelpFn.Copy(_L("\t"));
			HelpFn.Append(allDataTypes->At(i).Des());
			HelpFn.Append(_L("\t"));
			
			if(((TUint)HandlerAppUid.iUid) > 0)
			{
				if(AppInfo.iCaption.Length())
					HelpFn.Append(AppInfo.iCaption);
				else if(AppInfo.iShortCaption.Length())
					HelpFn.Append(AppInfo.iShortCaption);
				else
				{
					TParsePtrC TitleHelp(AppInfo.iFullName);
					HelpFn.Append(TitleHelp.NameAndExt());
				}
			}
			else
			{
				HelpFn.Append(_L("<none>"));
			}
			
			MyArray->AppendL(HelpFn);	
		}
	}
		
	CleanupStack::PopAndDestroy(3);//ls,allDataTypes,DocHand
//	CleanupStack::PopAndDestroy(2);//ls,allDataTypes
	
	CleanupStack::Pop(MyArray);
	iListBox->Model()->SetItemTextArray(MyArray);
    iListBox->Model()->SetOwnershipType(ELbmOwnsItemArray);
    
	iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
	iListBox->SetRect(Rect());
	
//	iListBox->View()->SetListEmptyTextL(NoBuffer);
	
	iListBox->ActivateL();

	TInt ItemsCount = iListBox->Model()->ItemTextArray()->MdcaCount();
	
	if(ItemsCount > MySetIndex && MySetIndex >= 0)
		iListBox->SetCurrentItemIndex(MySetIndex);
	else if(ItemsCount > 0)
		iListBox->SetCurrentItemIndex(0);
	
	UpdateScrollBar(iListBox);
	iListBox->DrawNow();
	DrawNow();
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CMainContainer::UpdateScrollBar(CEikListBox* aListBox)
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
This will be called when this file shandler is preraped to be used for, 
opening or constructing a file. The open/new etc. functions are called
strait after calling this function.
-------------------------------------------------------------------------
*/
void CMainContainer::ConstructL(CEikButtonGroupContainer* aCba)
{
	iCba = aCba;
	
 	CreateWindowL();
 	
 	iBgContext = CAknsBasicBackgroundControlContext::NewL(KAknsIIDQsnBgScreen,TRect(0,0,1,1), ETrue);
	
 	iResId = -1;
 	TFindFile AppFile(CCoeEnv::Static()->FsSession());
	if(KErrNone == AppFile.FindByDir(KFilleResourceFileName, KNullDesC))
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
	
	RMainServerClient MainServerClient;
	if(MainServerClient.Connect() == KErrNone)
	{
		iHasServer = ETrue;
		MainServerClient.Close();
	}
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CMainContainer::ChangeIconAndNameL(void)
{	
 	CEikStatusPane*		sp = ((CAknAppUi*)iEikonEnv->EikAppUi())->StatusPane();
	if(sp)
	{
		CAknTitlePane* TitlePane = STATIC_CAST(CAknTitlePane*,sp->ControlL(TUid::Uid(EEikStatusPaneUidTitle)));
		TitlePane->SetTextL(KFilleApplicationName);
			
    	CAknContextPane* 	ContextPane = (CAknContextPane *)sp->ControlL(TUid::Uid(EEikStatusPaneUidContext));
		if(ContextPane)
		{
			TFindFile AppFile(CCoeEnv::Static()->FsSession());
			if(KErrNone == AppFile.FindByDir(KFilleIconsFileName, KNullDesC))
			{
				TInt BgIndex(0),BgMask(1);
				
			/*	if(iServerIsOn)
				{
					BgIndex = 2;
					BgMask = 3;
				}
			*/	
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
Y-Browser will call setsize for client rect when lay-out or size changes,
This will be then called by the framework
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
	
 	ConstructListBoxL();
 	
 	if(iMyHelpContainer)
 	{
		iMyHelpContainer->SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect()); // Sets rectangle of lstbox.
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
 		if(iMyTepesView)
 		{
 			iMyTepesView->SetRect(rect);
 		}
 		
 		if(iMainListView)
 		{
 			iMainListView->SetRect(rect);
 		}
 		
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
	if(iMyHelpContainer)
	{
		iCba->SetCommandSetL(R_APPHELP_CBA);
		iCba->DrawDeferred();
	}
	else if(iShowString)
	{
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->SetMenuTitleResourceId(R_SHOW_MENUBAR);	
		iCba->SetCommandSetL(R_SHOW_CBA);				
		iCba->DrawDeferred();
	}
	else if(iMyTepesView)
	{
		iMyTepesView->SetMenuL();
	}
	else if(iMainListView)
	{
		iMainListView->SetMenuL();
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
void CMainContainer::InitMenuPanelL(TInt aResourceId, CEikMenuPane* aMenuPane)
{
	if(aResourceId == R_MAIN_MENU)
	{
		TBool Dimopen(ETrue);
		
		if(iListBox && iHasServer)
		{
			TInt CurrInd = iListBox->CurrentItemIndex();
			if(CurrInd >= 0)
			{
				Dimopen = EFalse;		
			}
		}
		
		if(Dimopen)
		{
			aMenuPane->SetItemDimmed(EMapFileType,ETrue);
			aMenuPane->SetItemDimmed(ERemoveMap,ETrue);
		}
		
		// implement later..
		aMenuPane->SetItemDimmed(EMyOwnFT,ETrue);
	}
	
	if(iMyTepesView)
	{
		iMyTepesView->InitMenuPanelL(aResourceId,aMenuPane);
	}
	
	if(iMainListView)
	{
		iMainListView->InitMenuPanelL(aResourceId,aMenuPane);
	}
}
/*
-------------------------------------------------------------------------

-------------------------------------------------------------------------
*/

void CMainContainer::HandleCommandL(TInt aCommand)
{
	switch(aCommand)
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
	case EShowFT:
		{
			TBuf<200> FileType;
			if(GetSelFileType(FileType))	
			{	
				delete iShowString;
				iShowString = NULL;
				iShowString = new(ELeave)CShowString();
				iShowString->ConstructL(FileType);
			}
		}
		SetMenuL();
		DrawNow();
		break;
	case ESSBack:
		{
			delete iShowString;
			iShowString = NULL;
		}
		SetMenuL();
		DrawNow();
		break;
	case EMapFileType:
		{
			delete iMainListView;
			iMainListView = NULL;
			iMainListView = CMainListView::NewL(*iCba);
		}
		break;
	case ERemoveMap:
		{		
			TBuf<100> TmpBuf;
			if(GetSelFileType(TmpBuf))	
			{
				TBuf8<100> FileType;
				FileType.Copy(TmpBuf);
				
				TFileName AskNow(_L("Set default mapping for "));
				AskNow.Append(TmpBuf);
				
				CAknQueryDialog* dlg = CAknQueryDialog::NewL();
				if(dlg->ExecuteLD(R_QUERY,AskNow))
				{
					TFileMappingItem SetMe;
					SetMe.iDataType  = TDataType(FileType);
					SetMe.iUid.iUid = 0;
					
					RMainServerClient MainServerClient;
					MainServerClient.Connect();
					MainServerClient.SetFileMappingL(SetMe);
					MainServerClient.Close();
		
					ConstructListBoxL();
				}
			}
		}
		break;
	case ESelectApp:
		if(iMainListView)
		{
			TUid MapAppUid;
			if(iMainListView->GetSelUid(MapAppUid))
			{
				TBuf<100> TmpBuf;
				if(GetSelFileType(TmpBuf))	
				{
					TBuf8<100> FileType;
					FileType.Copy(TmpBuf);
								
					TInt32 MapPriority(0);

					if(AskMappingPriorityL(MapPriority))
					{
						TFileMappingItem SetMe;
						SetMe.iDataType  = TDataType(FileType);
						SetMe.iUid = MapAppUid;
						SetMe.iPriority = MapPriority;
						
						RMainServerClient MainServerClient;
						MainServerClient.Connect();
						MainServerClient.SetFileMappingL(SetMe);
						MainServerClient.Close();
	
						ConstructListBoxL();
					}
				}
			}
		}
	case EAppListBack:
		{
			delete iMainListView;
			iMainListView = NULL;
		}
		SetMenuL();
		DrawNow();
		break;
	case EAbout:
    	{
    		HBufC* Abbout = KFilleAbbout().AllocLC();
			TPtr Pointter(Abbout->Des());
			CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL(Pointter);
			dlg->PrepareLC(R_ABOUT_HEADING_PANE);
			dlg->SetHeaderTextL(KFilleApplicationName);  
			dlg->RunLD();
			
			CleanupStack::PopAndDestroy(Abbout);
    	}
    	break;
    case EBacktoYTasks:
    	if(iTasksHandlerExit)
    	{
    		iTasksHandlerExit->HandlerExitL(this);	
    	}
    	break;
    case EMyOwnFT:
    	{
    		iMyTepesView = CMyTepesView::NewL(*iCba);
    	}
    	SetMenuL();
		DrawNow();
    	break;
	case EMyTypeBack:
		{
    		delete iMyTepesView;
    		iMyTepesView = NULL;
    		ConstructListBoxL();	
    	}
    	SetMenuL();
		DrawNow();
    	break;
	default:
		if(iMyTepesView)
		{
			iMyTepesView->HandleViewCommandL(aCommand);
		}
		else if(iMainListView)
		{
			iMainListView->HandleViewCommandL(aCommand);
		}
		else if(iShowString)
    	{
	        iShowString->HandleViewCommandL(aCommand);
    	}
		break;
	}	
}

/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
TBool CMainContainer::AskMappingPriorityL(TInt32& aPriority) 
{
	TBool Ret(EFalse);
	
	// list ask here...
	CAknSinglePopupMenuStyleListBox* list = new(ELeave) CAknSinglePopupMenuStyleListBox;
    CleanupStack::PushL(list);

	CAknPopupList* popupList = CAknPopupList::NewL(list,R_AVKON_SOFTKEYS_OK_CANCEL);
    CleanupStack::PushL(popupList);
	
    list->ConstructL(popupList, CEikListBox::ELeftDownInViewRect);
	list->CreateScrollBarFrameL(ETrue);
    list->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,CEikScrollBarFrame::EAuto);
    
    CDesCArrayFlat* Ittems = new(ELeave)CDesCArrayFlat(5);
    CleanupStack::PushL(Ittems);
    
    Ittems->AppendL(KtxPriorityLow);
    Ittems->AppendL(KtxPriorityNormal);
    Ittems->AppendL(KtxPriorityHigh);
    Ittems->AppendL(KtxPriorityMax);
    
    CleanupStack::Pop(Ittems);
    list->Model()->SetItemTextArray(Ittems);
	list->Model()->SetOwnershipType(ELbmOwnsItemArray);

	popupList->SetTitleL(KtxSelectPriority);
	
	if (popupList->ExecuteLD())
    {
    	Ret = ETrue;
    	
		switch(list->CurrentItemIndex())
		{
		case 1:
			aPriority = KDataTypePriorityNormal;
			break;
		case 2:
			aPriority = KDataTypePriorityHigh;
			break;
		case 3:
			aPriority = KDataTypeUnTrustedPriorityThreshold;
			break;
		default:
			aPriority = KDataTypePriorityLow;
			break;
		}
	}				
	
    CleanupStack::Pop();            // popuplist
    CleanupStack::PopAndDestroy();  // list
    
    return Ret;
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
TBool CMainContainer::GetSelFileType(TDes& aFileType) 
{
	TBool Ret(EFalse);
	
	if(iListBox)
	{
		CDesCArray* itemArray = STATIC_CAST( CDesCArray* ,iListBox->Model()->ItemTextArray());
		TInt Curr = iListBox->CurrentItemIndex();	
		if(Curr >= 0 && itemArray)
		{
			TPtrC ItName,Item;
			Item.Set(itemArray->MdcaPoint(Curr));
			if(KErrNone == TextUtils::ColumnText(ItName,1,&Item))
			{				
				Ret = ETrue;
				aFileType.Copy(ItName);
			}
		}
	}	
	
	return Ret;
}	
/*
-----------------------------------------------------------------------

-----------------------------------------------------------------------
*/
TKeyResponse CMainContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
	TKeyResponse Ret = EKeyWasNotConsumed;
	
	if(iMyHelpContainer)
	{
		Ret = iMyHelpContainer->OfferKeyEventL(aKeyEvent,aType);
	}
	else if(iMyTepesView)
	{
		Ret = iMyTepesView->OfferKeyEventL(aKeyEvent,aType);
	}
	else if(iMainListView)
	{
		Ret = iMainListView->OfferKeyEventL(aKeyEvent,aType);
	}
	else if(iShowString)
	{
		Ret = iShowString->OfferKeyEventL(aKeyEvent,aType);
	}
	else
	{
		switch (aKeyEvent.iCode)
	    {
		case EKeyDevice3:
			CEikonEnv::Static()->AppUiFactory()->MenuBar()->TryDisplayMenuBarL();
			break;
		case '2':
			if(iListBox)
			{
				iListBox->View()->MoveCursorL(CListBoxView::ECursorPreviousPage,CListBoxView::ENoSelection);
				iListBox->UpdateScrollBarsL();
				iListBox->DrawNow();
			}
			DrawNow();
			break;
		case '8':
			if(iListBox)
			{
				iListBox->View()->MoveCursorL(CListBoxView::ECursorNextPage,CListBoxView::ENoSelection);
				iListBox->UpdateScrollBarsL();
				iListBox->DrawNow();
			}
			DrawNow();
			break;
		case EKeyLeftArrow:
		case EKeyRightArrow:
		case EKeyDownArrow:
		case EKeyUpArrow:
		default:
			if(iListBox)
			{
				Ret = iListBox->OfferKeyEventL(aKeyEvent,aType);
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
void CMainContainer::Draw(const TRect& aRect) const
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
	if(iMyHelpContainer)
	{
		return 1;
	}
	else if(iMyTepesView)
		return 1;
	else if(iMainListView)
		return 1;
	else if(iShowString && iListBox)
		return 2;
	else if(iListBox)
		return 1;
	else
		return 0;
}
/*
-------------------------------------------------------------------------

-------------------------------------------------------------------------
*/
CCoeControl* CMainContainer::ComponentControl(TInt aIndex) const
{
	if(iMyHelpContainer)
		return iMyHelpContainer;
	else if(iMyTepesView)
		return iMyTepesView;
	else if(iMainListView)
		return iMainListView;
	else if(iShowString && aIndex)
		return iShowString;
	else
		return iListBox;
}

/*
-------------------------------------------------------------------------
interface function for getting the implementation instance
-------------------------------------------------------------------------
*/
CYTasksContainer* NewFileHandler()
    {
    return (new CMainContainer);
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
	IMPLEMENTATION_PROXY_ENTRY(0x0F5D,NewFileHandler)
};
#else

// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = 
    {
    {{0x0F5D}, NewFileHandler}
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

