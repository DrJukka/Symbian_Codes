/*	Copyright (C) 2006 Jukka Silvennoinen
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
#include "MainServerSession.h"
#include "InfoContainer.h"

#include "ProcessesAndTasks.h"
#include "ProcessesAndTasks.hrh"
#include "Help_Container.h"

#include "Common.h"

#ifdef SONE_VERSION
	#include "YTools_2002B0A0_res.rsg"
#else
	#ifdef LAL_VERSION
		#include "YTools_A000257C_res.rsg"
	#else
		#include "YTools_A000257C_res.rsg"
	#endif
#endif

#include <eikstart.h>
	
	
const TInt KAknExListFindBoxTextLength = 20;
/*
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/
CProcessContainer::~CProcessContainer()
{
	delete iYuccaInfo;
	iYuccaInfo  = NULL;

	if (!iNaviPane)
    {
        CEikStatusPane *sp = ((CAknAppUi*)iEikonEnv->EikAppUi())->StatusPane();
        iNaviPane = (CAknNavigationControlContainer *)sp->ControlL(TUid::Uid(EEikStatusPaneUidNavi));
    }
    
    iNaviPane->Pop(NULL); 

    delete iNaviDecoratorForTabs;
    delete iNaviDecorator;
    
    delete iMyHelpContainer;
    iMyHelpContainer = NULL;
    	
	delete iFindBox;
	iFindBox = NULL;
	delete iListBox;
	iListBox = NULL;
	delete iThreadArray;
	iThreadArray = NULL;
	delete iProcessArray;
	iProcessArray = NULL;
	
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
void CProcessContainer::ConstructL(CEikButtonGroupContainer* aCba)
{
	iCba = aCba;
	
 	CreateWindowL();
 	
 	iBgContext = CAknsBasicBackgroundControlContext::NewL(KAknsIIDQsnBgScreen,TRect(0,0,1,1), ETrue);
	
 	iResId = -1;
 	TFindFile AppFile(CCoeEnv::Static()->FsSession());
	if(KErrNone == AppFile.FindByDir(KProssResourceFileName, KNullDesC))
	{
 		TFileName resourceName(AppFile.File());
		BaflUtils::NearestLanguageFile(CEikonEnv::Static()->FsSession(), resourceName);
		iResId = CEikonEnv::Static()->AddResourceFileL(resourceName);
	}
	
	SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect()); 	
	
	iThreadArray = GetThreadListL();
	iProcessArray = GetProcessListL();
	
	
	CEikStatusPane *sp = ((CAknAppUi*)iEikonEnv->EikAppUi())->StatusPane();
	iNaviPane = (CAknNavigationControlContainer *)sp->ControlL(TUid::Uid(EEikStatusPaneUidNavi));

	CAknNavigationDecorator* iNaviDecoratorForTabsTemp;
	iNaviDecoratorForTabsTemp = iNaviPane->CreateTabGroupL();
	delete iNaviDecoratorForTabs;
    iNaviDecoratorForTabs = NULL;
	iNaviDecoratorForTabs = iNaviDecoratorForTabsTemp;
   	iTabGroup = STATIC_CAST(CAknTabGroup*, iNaviDecoratorForTabs->DecoratedControl());
	iTabGroup->SetTabFixedWidthL(KTabWidthWithTwoTabs);

	
	TBuf<50> AddBuf(_L("Pr:"));
	AddBuf.AppendNum(iProcessArray->Count());
	iTabGroup->AddTabL(0,AddBuf);
	
	AddBuf.Copy(_L("Th:"));
	AddBuf.AppendNum(iThreadArray->Count());
	iTabGroup->AddTabL(1,AddBuf);
	
	iTabGroup->SetActiveTabByIndex(0);
	iNaviPane->PushL(*iNaviDecoratorForTabs);
	
	ConstructListBoxL(iProcessArray);
	
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
void CProcessContainer::ChangeIconAndNameL(void)
{	
 	CEikStatusPane*		sp = ((CAknAppUi*)iEikonEnv->EikAppUi())->StatusPane();
	if(sp)
	{
		CAknTitlePane* TitlePane = STATIC_CAST(CAknTitlePane*,sp->ControlL(TUid::Uid(EEikStatusPaneUidTitle)));
		TitlePane->SetTextL(KProssApplicationName);
			
    	CAknContextPane* 	ContextPane = (CAknContextPane *)sp->ControlL(TUid::Uid(EEikStatusPaneUidContext));
		if(ContextPane)
		{
			TFindFile AppFile(CCoeEnv::Static()->FsSession());
			if(KErrNone == AppFile.FindByDir(KProssIconsFileName, KNullDesC))
			{
				TInt BgIndex(0),BgMask(1);
				
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
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CProcessContainer::ConstructListBoxL(CDesCArray* aArray)
{
	delete iListBox;
	iListBox = NULL;	
	iListBox   = new( ELeave )CAknSingleNumberStyleListBox();	
	iListBox->ConstructL(this,EAknListBoxMarkableList);
	iListBox->Model()->SetItemTextArray(aArray);
    iListBox->Model()->SetOwnershipType(ELbmDoesNotOwnItemArray);
	iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
		
	delete iFindBox;
	iFindBox = NULL;
	iFindBox = CreateFindBoxL(iListBox, iListBox->Model(),CAknSearchField::ESearch);
	SizeChanged();
	iListBox->ActivateL();
	
	iListBox->UpdateScrollBarsL();	
	iListBox->HandleItemAdditionL();
	
	UpdateScrollBar(iListBox);	
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CAknSearchField* CProcessContainer::CreateFindBoxL(CEikListBox* aListBox,CTextListBoxModel* aModel, CAknSearchField::TSearchFieldStyle aStyle )
{
    CAknSearchField* findbox = NULL;

    if ( aListBox && aModel )
	{
        // Gets pointer of CAknFilteredTextListBoxModel.
        CAknFilteredTextListBoxModel* model =STATIC_CAST( CAknFilteredTextListBoxModel*, aModel );
        // Creates FindBox.
        findbox = CAknSearchField::NewL( *this,aStyle,NULL,KAknExListFindBoxTextLength);
        CleanupStack::PushL(findbox);
        // Creates CAknListBoxFilterItems class.
        model->CreateFilterL( aListBox, findbox );
        //Filter can get by model->Filter();
        CleanupStack::Pop(findbox); // findbox
 	}

    return findbox;
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CDesCArrayFlat* CProcessContainer::GetSelectedArrayL(TBool& aProcess)
{
	CDesCArrayFlat* NewArray = new(ELeave)CDesCArrayFlat(1);
	CleanupStack::PushL(NewArray); 

	if(iListBox)
	{
		const CListBoxView::CSelectionIndexArray* Arrr = iListBox->SelectionIndexes() ;
		if(Arrr && iTabGroup)
		{
			MDesCArray* itemList = iProcessArray;
			if(iTabGroup->ActiveTabIndex() == 1)
			{
				itemList = iThreadArray;
				aProcess = EFalse;
			}
			else
			{
				aProcess = ETrue;
			}
				
			for(TInt i =0; i < Arrr->Count(); i ++)
			{
				TPtrC Item,ItemName;
				Item.Set(itemList->MdcaPoint(Arrr->At(i)));

				if(KErrNone == TextUtils::ColumnText(ItemName,1,&Item))
				{
					NewArray->AppendL(ItemName);
				}
			}
		}
	}
	
	CleanupStack::Pop(NewArray);
	return NewArray; 
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CProcessContainer::UpdateScrollBar(CEikListBox* aListBox)
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
void CProcessContainer::SizeChanged()
{    
    if(iYuccaInfo)
    {
		delete iYuccaInfo;
		iYuccaInfo = NULL;
		SetMenuL();
    
/*		TRect InfoRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
		TRect ButRect1(iCba->ControlOrNull(iCba->ButtonGroup()->CommandId(0))->Rect());		
		TRect ButRect3(iCba->ControlOrNull(iCba->ButtonGroup()->CommandId(2))->Rect());

		TInt BurHeight = ButRect1.Height();
		if(ButRect1.Height() > ButRect3.Height())
		{
			 BurHeight = ButRect3.Height();
		}
		switch(AknLayoutUtils::CbaLocation())
		{
		case AknLayoutUtils::EAknCbaLocationBottom: 
			{
				InfoRect.iBr.iY = (InfoRect.iBr.iY - BurHeight);
			}
			break;
		case AknLayoutUtils::EAknCbaLocationRight: 
		case AknLayoutUtils::EAknCbaLocationLeft:
			{
				InfoRect.iTl.iY = (InfoRect.iTl.iY + BurHeight);
				InfoRect.iBr.iY = (InfoRect.iBr.iY - BurHeight);
			}
			break;
		}
		
    	iYuccaInfo->SetRect(InfoRect);*/
    }
    
	if ( iBgContext )
	{
		iBgContext->SetRect(Rect());

		if ( &Window() )
		{
			iBgContext->SetParentPos( PositionRelativeToScreen() );
		}
	}
	
    if (iListBox)
    {
    if (iFindBox)
        {
        SizeChangedForFindBox();
        }
    else
        {
        iListBox->SetRect(Rect()); // Sets rectangle of lstbox.
        }
    }	

    if(iMyHelpContainer)
    {
		iMyHelpContainer->SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect()); // Sets rectangle of lstbox.
    } 
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CProcessContainer::HandleResourceChange(TInt aType)
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
TTypeUid::Ptr CProcessContainer::MopSupplyObject(TTypeUid aId)
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
void CProcessContainer::SetMenuL(void)
{
	if(iMyHelpContainer)
	{
		iCba->SetCommandSetL(R_APPHELP_CBA);
		iCba->DrawDeferred();
	}
	else if(iYuccaInfo)
	{
		iCba->SetCommandSetL(R_INFO_CBA);
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
void CProcessContainer::InitMenuPanelL(TInt aResourceId, CEikMenuPane* aMenuPane)
{
	if(aResourceId == R_MAIN_MENU)
	{
		TBool Dimopen(ETrue),DimKill(ETrue);
		
		if(iListBox)
		{
			TInt CurrInd = iListBox->CurrentItemIndex();
			if(CurrInd >= 0)
			{
				Dimopen = EFalse;		
			}
		}
		
		if(Dimopen)
		{
			aMenuPane->SetItemDimmed(EProsInfo,ETrue);
		}
		
		if(iTabGroup)
		{	
			TInt TabIndex = iTabGroup->ActiveTabIndex();
			if(TabIndex == 0 && iHasServer)
			{
				DimKill = EFalse;
			}
		}
		
		if(DimKill)
		{
			aMenuPane->SetItemDimmed(EProsKill,ETrue);
		}
	}
}

/*
-------------------------------------------------------------------------

-------------------------------------------------------------------------
*/

void CProcessContainer::HandleCommandL(TInt aCommand)
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
	case EInfoBack:
		{
			delete iYuccaInfo;
			iYuccaInfo = NULL;
		}
		SetMenuL();
		DrawNow();
		break;
	case EProsReFresh:
		if(iTabGroup)
		{
			delete iThreadArray;
			iThreadArray = NULL;
			iThreadArray = GetThreadListL();
			delete iProcessArray;
			iProcessArray = NULL;
			iProcessArray = GetProcessListL();
			
			TBuf<50> AddBuf(_L("Pr:"));
			AddBuf.AppendNum(iProcessArray->Count());
		 	iTabGroup->ReplaceTabL(iTabGroup->TabIdFromIndex(0),AddBuf);
		 	AddBuf.Copy(_L("Th:"));
			AddBuf.AppendNum(iThreadArray->Count());
			iTabGroup->ReplaceTabL(iTabGroup->TabIdFromIndex(1),AddBuf);
	
			TInt TabCurrent = iTabGroup->ActiveTabIndex();
			
			if(TabCurrent == 0)
			{
				ConstructListBoxL(iProcessArray);
			}
			else
			{
				ConstructListBoxL(iThreadArray);
			}
		}
		DrawNow();
		break;
	case EProsKill:
		if(iListBox && iTabGroup)
		{
			CAknFilteredTextListBoxModel* model = STATIC_CAST(CAknFilteredTextListBoxModel*,iListBox->Model());		
			TInt CurrInd = model->Filter()->FilteredItemIndex(iListBox->CurrentItemIndex());
		    if (CurrInd >= 0)
    	    {
				KillProcessThreadL(CurrInd);	
    	    
    	    	TInt TabCurrent = iTabGroup->ActiveTabIndex();
    	    	if(TabCurrent == 0)
				{
					ConstructListBoxL(iProcessArray);
				}
				else
				{
					ConstructListBoxL(iThreadArray);
				}
    	    }
		}
		DrawNow();
		break;;
	case EProsInfo:
		if(iListBox)
		{
			CAknFilteredTextListBoxModel* model = STATIC_CAST(CAknFilteredTextListBoxModel*,iListBox->Model());		
			TInt CurrInd = model->Filter()->FilteredItemIndex(iListBox->CurrentItemIndex());
		    if (CurrInd >= 0)
    	    {
				ShowDetailsDialogL(CurrInd);	
    	    }
		}
		DrawNow();
		break;
    case EAbout:
    	{
    		HBufC* Abbout = KProssAbbout().AllocLC();
			TPtr Pointter(Abbout->Des());
			CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL(Pointter);
			dlg->PrepareLC(R_ABOUT_HEADING_PANE);
			dlg->SetHeaderTextL(KProssApplicationName);  
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
	default:
		break;
    };
}


/*
-----------------------------------------------------------------------

-----------------------------------------------------------------------
*/
TKeyResponse CProcessContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
	TKeyResponse Ret = EKeyWasNotConsumed;
	
	if(iMyHelpContainer)
	{
		Ret = iMyHelpContainer->OfferKeyEventL(aKeyEvent,aType);
	}
	else if(iYuccaInfo)
	{
		return iYuccaInfo->OfferKeyEventL(aKeyEvent,aType);
	}
	switch (aKeyEvent.iCode)
    {
	case EKeyDevice3:
		HandleCommandL(EProsInfo); 
		break;
	case EKeyLeftArrow:
		if(iTabGroup)
		{
			TInt TabCurrent = iTabGroup->ActiveTabIndex();
			if(TabCurrent != 0)
			{
				iTabGroup->SetActiveTabByIndex(0);
				ConstructListBoxL(iProcessArray);
			}
			
			SetMenuL();
			DrawNow();
		}
		break;
	case EKeyRightArrow:
		if(iTabGroup)
		{
			TInt TabCurrent = iTabGroup->ActiveTabIndex();
			if(TabCurrent != 1)
			{
				iTabGroup->SetActiveTabByIndex(1);
				ConstructListBoxL(iThreadArray);
			}
			
			SetMenuL();
			DrawNow();
		}
		break;
	case EKeyDownArrow:
	case EKeyUpArrow:
	default:
		if(iListBox)
		{
			if ( iFindBox )
            {
            TBool needRefresh( EFalse );
            
            // Offers the key event to find box.
            if ( AknFind::HandleFindOfferKeyEventL( aKeyEvent, aType, this,
                                                    iListBox, iFindBox,
                                                    EFalse,
                                                    needRefresh ) == EKeyWasConsumed )
                {
                	if ( needRefresh )
                    {
                	    SizeChangedForFindBox();
                    	DrawNow();
                    }

                	return EKeyWasConsumed;
                }
            }
            
			iListBox->OfferKeyEventL(aKeyEvent,aType);
		}
		break;
	}
    
	return Ret;
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CProcessContainer::SizeChangedForFindBox()
{
    if ( iListBox && iFindBox )
   	{
        CAknColumnListBox* aknListBox = STATIC_CAST(CAknColumnListBox*, iListBox);
        AknFind::HandleFixedFindSizeChanged(this, aknListBox, iFindBox); 
   	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CDesCArrayFlat* CProcessContainer::GetProcessListL(void)
{
	CDesCArrayFlat* NewArray = new(ELeave)CDesCArrayFlat(1);
	CleanupStack::PushL(NewArray);
	
	TBuf<500> ResBuffer;
	
  	TFullName res;
  	TFindProcess find;
  	while(find.Next(res) == KErrNone)
    {
      	RProcess ph;
      	ph.Open(find);
      	
      	ResBuffer.Num(NewArray->Count());
      	ResBuffer.Append(_L("\t"));
      	
      	res.TrimAll();
      	ResBuffer.Append(res);
	
      	ph.Close();
      	
      	NewArray->AppendL(ResBuffer);
    }
    
    CleanupStack::Pop(NewArray);
    return NewArray;
} 
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CDesCArrayFlat* CProcessContainer::GetThreadListL(void)
{
	CDesCArrayFlat* NewArray = new(ELeave)CDesCArrayFlat(1);
	CleanupStack::PushL(NewArray);
	
	TBuf<500> ResBuffer;
	
  	TFullName res;
  	TFindThread find;
  	while(find.Next(res) == KErrNone)
    {
      	RThread ph;
      	ph.Open(find);
      	
      	ResBuffer.Num(NewArray->Count());
      	ResBuffer.Append(_L("\t"));
      	
      	res.TrimAll();
      	ResBuffer.Append(res);
      	ResBuffer.Append(_L("\t"));
      	
      	ph.Close();
      	
      	NewArray->AppendL(ResBuffer);
    }
    
    CleanupStack::Pop(NewArray);
    return NewArray;
}
/*
-----------------------------------------------------------------------

-----------------------------------------------------------------------
*/
void CProcessContainer::Draw(const TRect& /*aRect*/) const
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
TInt CProcessContainer::CountComponentControls() const
{
	if(iMyHelpContainer)
	{
		return 1;
	}
	else if(iYuccaInfo)
		return 1;
	else if(iListBox && iFindBox)
		return 2;
	else
		return 0;
}
/*
-------------------------------------------------------------------------

-------------------------------------------------------------------------
*/
CCoeControl* CProcessContainer::ComponentControl(TInt aIndex) const
{
	if(iMyHelpContainer)
	{
		return iMyHelpContainer;
	}
	else if(iYuccaInfo)
		return iYuccaInfo;
	else if(iFindBox && aIndex)
		return iFindBox;
	else
		return iListBox;
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CProcessContainer::ShowDetailsDialogL(TInt aIndex) 
{
	if(iTabGroup && iListBox)
	{	
		TInt TabIndex = iTabGroup->ActiveTabIndex();
		if(aIndex >= 0)
		{	
			TFileName res;
			MDesCArray* itemList = iProcessArray;
	
			if(TabIndex == 1)
			{
				itemList = iThreadArray;
			}

			TPtrC Item,ItemName;
			Item.Set(itemList->MdcaPoint(aIndex));

			if(KErrNone == TextUtils::ColumnText(ItemName,1,&Item))
			{	
				if(TabIndex == 0) //  process
				{
					TFindProcess find;
				  	while(find.Next(res) == KErrNone)
				    {	
				    	if(ItemName == res)
				    	{   	
				    		TRect InfoRect(CEikonEnv::Static()->EikAppUi()->ClientRect());

/*							TRect ButRect1(iCba->ControlOrNull(iCba->ButtonGroup()->CommandId(0))->Rect());		
							TRect ButRect3(iCba->ControlOrNull(iCba->ButtonGroup()->CommandId(2))->Rect());
							
							TInt BurHeight = ButRect1.Height();
							if(ButRect1.Height() > ButRect3.Height())
							{
								 BurHeight = ButRect3.Height();
							}
				    		switch(AknLayoutUtils::CbaLocation())
							{
							case AknLayoutUtils::EAknCbaLocationBottom: 
								{
									InfoRect.iBr.iY = (InfoRect.iBr.iY - BurHeight);
								}
								break;
							case AknLayoutUtils::EAknCbaLocationRight: 
							case AknLayoutUtils::EAknCbaLocationLeft:
								{
									InfoRect.iTl.iY = (InfoRect.iTl.iY + BurHeight);
									InfoRect.iBr.iY = (InfoRect.iBr.iY - BurHeight);
								}
								break;
							}*/
				    		
					      	iYuccaInfo = CYuccaInfo::NewL(InfoRect);
					      	//iYuccaInfo->SetMopParent(this);
					      	iYuccaInfo->AddProcessDetailsL(find);
					      	SetMenuL();
					      	DrawNow();
					      	break;
				    	}
				    }
				}
				else // Thread
				{
					TFindThread find;
				  	while(find.Next(res) == KErrNone)
				    {
				    	if(ItemName == res)
				    	{
				    		TRect InfoRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
/*							TRect ButRect1(iCba->ControlOrNull(iCba->ButtonGroup()->CommandId(0))->Rect());		
							TRect ButRect3(iCba->ControlOrNull(iCba->ButtonGroup()->CommandId(2))->Rect());
							
							TInt BurHeight = ButRect1.Height();
							if(ButRect1.Height() > ButRect3.Height())
							{
								 BurHeight = ButRect3.Height();
							}
				    		switch(AknLayoutUtils::CbaLocation())
							{
							case AknLayoutUtils::EAknCbaLocationBottom: 
								{
									InfoRect.iBr.iY = (InfoRect.iBr.iY - BurHeight);
								}
								break;
							case AknLayoutUtils::EAknCbaLocationRight: 
							case AknLayoutUtils::EAknCbaLocationLeft:
								{
									InfoRect.iTl.iY = (InfoRect.iTl.iY + BurHeight);
									InfoRect.iBr.iY = (InfoRect.iBr.iY - BurHeight);
								}
								break;
							}*/
							iYuccaInfo = CYuccaInfo::NewL(InfoRect);
							//iYuccaInfo->SetMopParent(this);
					      	iYuccaInfo->AddThreadDetailsL(find);
					      	SetMenuL();
					      	DrawNow();
					      	break;
				    	}
				    }
				}
			}
		}
	}
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CProcessContainer::KillProcessThreadL(TInt aIndex) 
{
	if(iTabGroup && iListBox)
	{	
		TInt TabIndex = iTabGroup->ActiveTabIndex();
		if(aIndex >= 0)
		{	
			TFileName res;
			MDesCArray* itemList = iProcessArray;
	
			if(TabIndex == 1)
			{
				itemList = iThreadArray;
			}

			TPtrC Item,ItemName;
			Item.Set(itemList->MdcaPoint(aIndex));

			if(KErrNone == TextUtils::ColumnText(ItemName,1,&Item))
			{	
				TBuf<255> AksBuf(_L("Kill "));
				if(ItemName.Length() > 200)
				{
					AksBuf.Append(ItemName.Left(200));
				}
				else
				{
					AksBuf.Append(ItemName);
				}
				
				CAknQueryDialog* dlg = CAknQueryDialog::NewL();
				if(dlg->ExecuteLD(R_QUERY,AksBuf))
				{
					TBuf8<255> Hjelpper;
					if(ItemName.Length() > 255)
					{
						Hjelpper.Copy(ItemName.Left(255));
					}
					else
					{
						Hjelpper.Copy(ItemName);
					}
				
					if(Hjelpper.Length())
					{
						if(TabIndex == 0) //  process
						{
							RMainServerClient MainServerClient;
							if(MainServerClient.Connect() == KErrNone)
							{
								MainServerClient.KillProcessL(Hjelpper);
								MainServerClient.Close();
							}
						}
						else // Thread
						{
							RMainServerClient MainServerClient;
							if(MainServerClient.Connect() == KErrNone)
							{
								MainServerClient.KillThreadL(Hjelpper);
								MainServerClient.Close();
							}
						}
					}
				}
			}
		}
	}
}

/*
-------------------------------------------------------------------------
interface function for getting the implementation instance
-------------------------------------------------------------------------
*/
CYTasksContainer* NewFileHandler()
    {
    return (new CProcessContainer);
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
	IMPLEMENTATION_PROXY_ENTRY(0x257C,NewFileHandler)
};
#else

// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = 
    {
    {{0x257C}, NewFileHandler}
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

