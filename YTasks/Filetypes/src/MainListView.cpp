/* ====================================================================
 * File: MainListView.cpp
 * Created: 01/07/07
 * Author: Jukka Silvennoinen
 * Copyright (c): 2007, All rights reserved
 * ==================================================================== */

#include <coemain.h>
#include <aknappui.h>
#include <EIKENV.H>
#include <AknUtils.h>
#include <AknIconArray.h>
#include <eikclbd.h>
#include <APGCLI.H>
#include <GULICON.H>
#include <aknmessagequerydialog.h>
#include "YTools_A0000F5D.hrh"

#include "MainListView.h"
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

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CMainListView* CMainListView::NewL(CEikButtonGroupContainer& aCba)
    {
    CMainListView* self = CMainListView::NewLC(aCba);
    CleanupStack::Pop(self);
    return self;
    }
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CMainListView* CMainListView::NewLC(CEikButtonGroupContainer& aCba)
    {
    CMainListView* self = new (ELeave) CMainListView(aCba);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CMainListView::CMainListView(CEikButtonGroupContainer& aCba)
:iCba(aCba)
    {
	// no implementation required
    }
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CMainListView::~CMainListView()
{
	delete iFindBox;
	iFindBox = NULL;
	
	delete iListBox;
	delete iAppUIDs;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainListView::ConstructL()
{
    CreateWindowL();

    SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
   
   	MakeListBoxL();
   
   	ActivateL();
    SetMenuL();
    DrawNow();
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CMainListView::MakeListBoxL()
{
	TInt MySetIndex(0);
	
	if(iListBox)
	{
		MySetIndex = GetSelectedIndexL();
	}
    
	delete iListBox;
	iListBox = NULL;
	
    iListBox   = new( ELeave ) CAknSingleGraphicStyleListBox();
	iListBox->ConstructL(this,EAknListBoxSelectionList);
	
	CArrayPtr<CGulIcon>* icons =new( ELeave ) CAknIconArray(10);
	CleanupStack::PushL(icons);	
	
	iListBox->Model()->SetItemTextArray(GetApplicationArrayL(icons));
    iListBox->Model()->SetOwnershipType(ELbmOwnsItemArray);
	
	CleanupStack::Pop(icons);	
	iListBox->ItemDrawer()->ColumnData()->SetIconArray(icons);
	iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
	iListBox->SetRect(Rect());
	
	delete iFindBox;
	iFindBox = NULL;
	iFindBox = CreateFindBoxL(iListBox, iListBox->Model(),CAknSearchField::ESearch);
	SizeChanged();
	
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
CAknSearchField* CMainListView::CreateFindBoxL(CEikListBox* aListBox,CTextListBoxModel* aModel, CAknSearchField::TSearchFieldStyle aStyle )
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
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CDesCArray* CMainListView::GetApplicationArrayL(CArrayPtr<CGulIcon>* aIcons)
{
	delete iAppUIDs;
	iAppUIDs = NULL;
	
	iAppUIDs = new(ELeave)CArrayFixFlat<TUid>(10);
	
	CDesCArrayFlat* MyArray = new(ELeave)CDesCArrayFlat(10);
	CleanupStack::PushL(MyArray);
	
	RApaLsSession ls;
	User::LeaveIfError(ls.Connect());
	CleanupClosePushL(ls);
	
	User::LeaveIfError(ls.GetAllApps());

	TInt Errnono(KErrNone);
	TFileName HelpFn;
	TApaAppInfo AppInfo;
	
	MAknsSkinInstance* skin = AknsUtils::SkinInstance();
	
	do
	{
		Errnono = ls.GetNextApp(AppInfo);
		if(KErrNone == Errnono && AppInfo.iCaption.Length())
		{	
			TRAPD(Err,
			CFbsBitmap*	AppIcon(NULL);
			CFbsBitmap*	AppIconMsk(NULL);
		    AknsUtils::CreateAppIconLC(skin,AppInfo.iUid,  EAknsAppIconTypeContext,AppIcon,AppIconMsk);
		
			if(AppIcon && AppIconMsk)
			{
				HelpFn.Num(aIcons->Count());
				HelpFn.Append(_L("\t"));
				HelpFn.Append(AppInfo.iCaption);
				HelpFn.TrimAll();
				
				iAppUIDs->AppendL(AppInfo.iUid);
				
				MyArray->AppendL(HelpFn);
				
				aIcons->AppendL(CGulIcon::NewL(AppIcon,AppIconMsk));
				CleanupStack::Pop(2);	
			});				
		}

	}while(KErrNone == Errnono);
	
	CleanupStack::PopAndDestroy(); // ls	

	CleanupStack::Pop(MyArray);
	return MyArray;
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CMainListView::UpdateScrollBar(CEikListBox* aListBox)
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
-----------------------------------------------------------------------------
*/
void CMainListView::SizeChanged()
{	
	if (iListBox)
    {
        if (iFindBox)
        {
        	CAknColumnListBox* aknListBox = STATIC_CAST(CAknColumnListBox*, iListBox);
        	AknFind::HandleFixedFindSizeChanged(this, aknListBox, iFindBox);
        
        }
        else
        {
            iListBox->SetRect(Rect()); // Sets rectangle of lstbox.
    	}
    }
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainListView::SizeChangedForFindBox()
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
void CMainListView::LaunchApplicationL(TUid aAppUid)
{
	RApaLsSession ls;
	User::LeaveIfError(ls.Connect());
	CleanupClosePushL(ls);
	
	TApaAppInfo AppInfo;
		
	ls.GetAppInfo(AppInfo,aAppUid);
	
	TParsePtrC TitleHelp(AppInfo.iFullName);
	
	CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
	cmdLine->SetExecutableNameL(TitleHelp.NameAndExt());
	cmdLine->SetCommandL(EApaCommandRun);

	ls.StartApp(*cmdLine);	

	CleanupStack::PopAndDestroy(2);//ls,cmdLine
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
HBufC* CMainListView::GetApplicationInfoLC(TUid aAppUid,TDes& aTitle)
{
	HBufC* RetBuff = HBufC::NewLC(10000);
	
	RApaLsSession ls;
	User::LeaveIfError(ls.Connect());
	CleanupClosePushL(ls);
	
	TApaAppInfo AppInfo;
		
	ls.GetAppInfo(AppInfo,aAppUid);
	
	TParsePtrC TitleHelp(AppInfo.iFullName);
	
	aTitle.Copy(TitleHelp.NameAndExt());
	
	RetBuff->Des().Copy(KtxAppUid);
	RetBuff->Des().AppendNum((TUint)aAppUid.iUid,EHex);
	
	RetBuff->Des().Append(KtxAppCaption);
	RetBuff->Des().Append(AppInfo.iCaption);
	RetBuff->Des().Append(KtxAppShortCaption);
	RetBuff->Des().Append(AppInfo.iShortCaption);
	
	TPckgBuf<TApaAppCapability> TApaAppCapabilityBuf;
	
	ls.GetAppCapability(TApaAppCapabilityBuf,aAppUid);
	
	RetBuff->Des().Append(KtxAppEmbeddability);
	
	switch(TApaAppCapabilityBuf().iEmbeddability)
	{
	case TApaAppCapability::ENotEmbeddable: /** An application cannot be embedded. */
		RetBuff->Des().Append(KtxAppNotEmbeddable);
		break;
	case TApaAppCapability::EEmbeddable:/** An application can be run embedded or standalone and can read/write embedded document-content. */
		RetBuff->Des().Append(KtxAppEmbeddable);
		break;
	case TApaAppCapability::EEmbeddableOnly:/** An application can only be run embedded and can read/write embedded document-content. */
		RetBuff->Des().Append(KtxAppEmbeddableOnly);
		break;
	case TApaAppCapability::EEmbeddableUiOrStandAlone:/** An application can be run embedded or standalone and cannot read/write embedded document-content. */
		RetBuff->Des().Append(KtxAppEmbeddableUiOrStandAlone);
		break;
	case TApaAppCapability::EEmbeddableUiNotStandAlone:/** An application can only be run embedded and cannot read/write embedded document-content. */
	 	RetBuff->Des().Append(KtxAppEmbeddableUiNotStandAlone);
	 	break;
	};
		
	RetBuff->Des().Append(KtxAppSupportsNewFile);

	if(TApaAppCapabilityBuf().iSupportsNewFile)
	{
		RetBuff->Des().Append(KtxYes);
	}
	else
	{
		RetBuff->Des().Append(KtxNo);
	}
	
	RetBuff->Des().Append(KtxAppHidden);
	
	if(TApaAppCapabilityBuf().iAppIsHidden)
	{
		RetBuff->Des().Append(KtxYes);
	}
	else
	{
		RetBuff->Des().Append(KtxNo);
	}
	
	RetBuff->Des().Append(KtxAppLaunchBackGround);
	
	if(TApaAppCapabilityBuf().iLaunchInBackground)	
	{
		RetBuff->Des().Append(KtxYes);
	}
	else
	{
		RetBuff->Des().Append(KtxNo);
	}
	
	if(TApaAppCapabilityBuf().iGroupName.Length())
	{
		RetBuff->Des().Append(KtxAppGroupName);
		RetBuff->Des().Append(TApaAppCapabilityBuf().iGroupName);
	}

	TLanguage appLanguage(ELangTest);
	
	ls.ApplicationLanguage(aAppUid,appLanguage);
	
	RetBuff->Des().Append(KtxAppLanguageCode);
	RetBuff->Des().AppendNum(appLanguage,EDecimal);

	CDesCArrayFlat* AppOwnedFiles = new(ELeave)CDesCArrayFlat(1);
	CleanupStack::PushL(AppOwnedFiles);
	
	ls.GetAppOwnedFiles(*AppOwnedFiles,aAppUid);
	
	if(AppOwnedFiles->MdcaCount())
	{
		RetBuff->Des().Append(KtxAppOwedFiles);
	}
	
	for(TInt f=0; f < AppOwnedFiles->MdcaCount(); f++)
	{
		RetBuff->Des().Append(KtxLineFeedAndTab);
		RetBuff->Des().Append(AppOwnedFiles->MdcaPoint(f));
	}
	
	if(AppOwnedFiles->MdcaCount())
	{
		RetBuff->Des().Append(KtxLineFeed);
	}
	
	CleanupStack::PopAndDestroy(AppOwnedFiles);

	CDataTypeArray* allDataTypes = new(ELeave)CDataTypeArray(50);
	CleanupStack::PushL(allDataTypes);
	ls.GetSupportedDataTypesL(*allDataTypes);
	
	TBool HasHeaderAdded(EFalse);
	
	for(TInt i=0; i < allDataTypes->Count(); i++)
	{
		TUid HandlerAppUid;
		
		ls.AppForDataType(allDataTypes->At(i),HandlerAppUid);
		
		if(HandlerAppUid == aAppUid)
		{
			if(!HasHeaderAdded)
			{
				HasHeaderAdded = ETrue;
				RetBuff->Des().Append(KtxAppSupportsFileTypes);
			}
			
			RetBuff->Des().Append(allDataTypes->At(i).Des());
			RetBuff->Des().Append(KtxLineFeedAndTab);
		}
	}
	
	CleanupStack::PopAndDestroy(allDataTypes);
	
	RetBuff->Des().Append(KtxLineFeed);
	RetBuff->Des().Append(KtxLineFeed);
	
	
	CleanupStack::PopAndDestroy();//ls
	return RetBuff;	
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TBool CMainListView::GetSelUid(TUid& aAppUid)
{
	TBool Ret(EFalse);
	if(iListBox && iAppUIDs)
	{
		TInt Curr = GetSelectedIndexL();	
		if(Curr < iAppUIDs->Count() && Curr >= 0)
		{
			aAppUid = iAppUIDs->At(Curr);
			Ret = ETrue;
		}
	}
	
	return Ret;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TInt CMainListView::CMainListView::GetSelectedIndexL(void)
{
	TInt Ret(-1);

	if(iListBox)
	{
		TInt CurrItemInd = iListBox->CurrentItemIndex();
		
		CAknFilteredTextListBoxModel* model = STATIC_CAST(CAknFilteredTextListBoxModel*,iListBox->Model());		
		
		if(model && CurrItemInd >= 0)
		{	
			Ret = model->Filter()->FilteredItemIndex(CurrItemInd);
		}
	}
	
	return Ret;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainListView::HandleViewCommandL(TInt aCommand)
    {
    switch(aCommand)
        {
        case EShowInfo:
        	if(iListBox && iAppUIDs)
			{
				TInt Curr = GetSelectedIndexL();	
				if(Curr < iAppUIDs->Count() && Curr >= 0)
				{	
					TBuf<200> AppTitle;
					HBufC* Abbout = GetApplicationInfoLC(iAppUIDs->At(Curr),AppTitle);
					
					TPtr Pointter(Abbout->Des());
					CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL(Pointter);
					dlg->PrepareLC(R_ABOUT_HEADING_PANE);
					dlg->SetHeaderTextL(AppTitle);  
					dlg->RunLD();
					
					CleanupStack::PopAndDestroy(Abbout);
				}
			}
			SetMenuL();
			DrawNow();
			break;
		case ELaunchApp:
			if(iListBox && iAppUIDs)
			{
				TInt Curr = GetSelectedIndexL();	
				if(Curr < iAppUIDs->Count() && Curr >= 0)
				{
					LaunchApplicationL(iAppUIDs->At(Curr));
				}
			}
			break;
        default:
            break;
        }
    }

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
TKeyResponse CMainListView::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aEventCode)
{
	TKeyResponse Ret = EKeyWasNotConsumed;	

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
		default:
			if(iListBox)
			{
				if ( iFindBox )
	            {
	            TBool needRefresh( EFalse );
	            
	            // Offers the key event to find box.
	            if ( AknFind::HandleFindOfferKeyEventL( aKeyEvent, aEventCode, this,
	                                                    iListBox, iFindBox,
	                                                    EFalse,
	                                                    needRefresh ) == EKeyWasConsumed )
	                {
	                	if ( needRefresh )
	                    {
	                	    SizeChanged();
	                    	DrawNow();
	                    }

	                	return EKeyWasConsumed;
	                }
	            }
	            
				Ret = iListBox->OfferKeyEventL(aKeyEvent,aEventCode);
			}
			break;
		}
	}
	
	return Ret;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
// Draw this application's view to the screen
void CMainListView::Draw(const TRect& /*aRect*/) const
{
	CWindowGc& gc = SystemGc();
   	gc.Clear(Rect());
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CCoeControl* CMainListView::ComponentControl( TInt aIndex) const
{
	{
		if(iFindBox && aIndex)
			return iFindBox;
		else
			return iListBox;
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TInt CMainListView::CountComponentControls() const
{
	if(iListBox && iFindBox)
	{
		return 2;
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
void CMainListView::SetMenuL(void)
{
	CEikonEnv::Static()->AppUiFactory()->MenuBar()->StopDisplayingMenuBar();		
	
	{
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->SetMenuTitleResourceId(R_APPLIST_MENUBAR);

		iCba.SetCommandSetL(R_APPLIST_CBA);
		iCba.DrawDeferred();
	}
}


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainListView::InitMenuPanelL(TInt aResourceId, CEikMenuPane* aMenuPane)
{	
	if(R_MAIN_MENU == aResourceId)
	{
		TBool DimSel(ETrue);
		if(iListBox && iAppUIDs)
		{
			if(GetSelectedIndexL() >= 0)
			{
				DimSel= EFalse;
			}
		}
		
		if(DimSel)
		{
			aMenuPane->SetItemDimmed(ELaunchApp,ETrue);
			aMenuPane->SetItemDimmed(EShowInfo,ETrue);
		}
	}
}




