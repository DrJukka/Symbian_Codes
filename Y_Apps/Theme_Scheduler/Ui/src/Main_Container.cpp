/* Copyright (c) 2001 - 2008 , Dr Jukka Silvennoinen. All rights reserved */

#include "Definitions.h"

#include <barsread.h>

#include <EIKSPANE.H>
#include <AknAppUi.h>
#include <EIKBTGPC.H>
#include <aknnavi.h> 
#include <aknnavide.h> 
#include <akntabgrp.h> 
#include <aknlists.h> 
#include <akniconarray.h> 
#include <eikmenub.h>
#include <aknquerydialog.h> 
#include <AknIconArray.h>
#include <bautils.h>
#include <stringloader.h> 

#include "Main_Container.h"                     // own definitions
#include "ThemeDataBase.h"

#include "YApp_E8876002.hrh"                   // own resource header
#ifdef SONE_VERSION
	#include <YApp_2002884B.rsg>
#else

	#ifdef LAL_VERSION
		#include <YApp_2002207B.rsg>
	#else
		#include <YApp_E8876002.rsg>
	#endif
#endif
#include "YApp_E8876002.h"
#include "Help_Container.h"


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
	delete iMyHelpContainer;
	iMyHelpContainer = NULL;
	
    delete iProfileBox;
    
    iItemArray.Reset();
//    iItemArray.Close();
    
#ifdef LAL_VERSION    
    delete iApi;
#endif    
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::ConstructL(void)
{
	CreateWindowL();

#ifdef SONE_VERSION
	iAddIsValid = ETrue;
#else
	#ifdef LAL_VERSION
		iAddIsValid = EFalse;
		iApi=MAdtronicApiClient::NewL((const TUint8* const)"D375B8CD");
	#else
		iAddIsValid = ETrue;
	#endif
#endif
		
	SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
	
	MakeProfileBoxL();
	
	ActivateL();
	SetMenuL();

#ifdef LAL_VERSION
	iApi->StatusL(this);
//	installationValid();
#endif
	
	DrawNow();
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::SizeChanged()
{
	if(iProfileBox)
	{
		iProfileBox->SetRect(Rect());
	}
	
 	if(iProfileSettings)
    {
    	iProfileSettings->SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
    }	
 	
 	if(iMyHelpContainer)
 	{
 		iMyHelpContainer->SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
 	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::HandleResourceChange(TInt aType)
{
    if ( aType==KEikDynamicLayoutVariantSwitch )
    {  	
		TRect rect;
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, rect);

	    SetRect(rect);
 	}
 	
	CCoeControl::HandleResourceChange(aType);
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CMainContainer::installationValid()
{
	iAddIsValid = ETrue;
	MakeProfileBoxL();
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CMainContainer::installationInvalid()
{
	iAddIsValid = EFalse;
	STATIC_CAST(CMgAppUi*,CEikonEnv::Static()->AppUi())->ShowNoteL(_L("You need to install and activate adtronic"));
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CMainContainer::adtronicNotInstalled()
{
	iAddIsValid = EFalse;
	STATIC_CAST(CMgAppUi*,CEikonEnv::Static()->AppUi())->ShowNoteL(_L("You need to install and activate adtronic"));
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CMainContainer::MakeProfileBoxL()
{
	TInt MySetIndex(0);
	
	if(iProfileBox)
	{
		MySetIndex = iProfileBox->CurrentItemIndex();
	}
    
	delete iProfileBox;
	iProfileBox = NULL;
	
    iProfileBox   = new( ELeave ) CAknDoubleTimeStyleListBox();
	iProfileBox->ConstructL(this,EAknListBoxSelectionList);
	iProfileBox->Model()->SetItemTextArray(GetProfilesArrayL());
	
    iProfileBox->Model()->SetOwnershipType(ELbmOwnsItemArray);;
	iProfileBox->CreateScrollBarFrameL( ETrue );
    iProfileBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
	iProfileBox->SetRect(Rect());
	iProfileBox->ActivateL();

	TInt ItemsCount = iProfileBox->Model()->ItemTextArray()->MdcaCount();
	
	if(ItemsCount > MySetIndex && MySetIndex >= 0)
		iProfileBox->SetCurrentItemIndex(MySetIndex);
	else if(ItemsCount > 0)
		iProfileBox->SetCurrentItemIndex(0);
	
	UpdateScrollBar(iProfileBox);
	iProfileBox->DrawNow();
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CDesCArray* CMainContainer::GetProfilesArrayL(void)
{
	CDesCArrayFlat* MyArray = new(ELeave)CDesCArrayFlat(1);
	CleanupStack::PushL(MyArray);

	iItemArray.Reset();
	
	if(iAddIsValid)
	{
		CScheduleDB* ScheduleDB = new(ELeave)CScheduleDB();
		CleanupStack::PushL(ScheduleDB);
		ScheduleDB->ConstructL();
		ScheduleDB->ReadDbItemsL(iItemArray);	
		CleanupStack::PopAndDestroy(ScheduleDB);
	
		
		TBuf<250> Bufff;
		TBuf<100> HjelpBuf1,HjelpBuf2;
		
		for(TInt i=0; i < iItemArray.Count(); i++)
		{
			Bufff.Zero();
		
			if(iItemArray[i].iType)
			{
				StringLoader::Load(HjelpBuf1,R_SH_STR_PROFFILLE1);
				StringLoader::Load(HjelpBuf2,R_SH_STR_PROFFILLE2);
			
				Bufff.Copy(HjelpBuf1);
				Bufff.Append(_L("\t"));
				
				if(iItemArray[i].iEnabled){
					Bufff.Append(HjelpBuf2);
				}
				
				Bufff.Append(_L("\t"));	
				
				STATIC_CAST(CMgAppUi*,CEikonEnv::Static()->EikAppUi())->GetThemeNameL(iItemArray[i].iThemeId,HjelpBuf1,iItemArray[i].iIndex);
				
				STATIC_CAST(CMgAppUi*,CEikonEnv::Static()->EikAppUi())->GetProfileNameL(iItemArray[i].iProfileId,HjelpBuf2);
			
				if(!iItemArray[i].iEnabled){
					StringLoader::Load(HjelpBuf2,R_SH_STR_DISABLED);
				}
				
				Bufff.Append(HjelpBuf1);
				Bufff.Append(_L("\t"));
				Bufff.Append(HjelpBuf2);
				Bufff.Append(_L("\t"));	
	
			}
			else
			{
				STATIC_CAST(CMgAppUi*,CEikonEnv::Static()->EikAppUi())->GetRepeatTypeName(iItemArray[i].iRepeat,HjelpBuf1);
				STATIC_CAST(CMgAppUi*,CEikonEnv::Static()->EikAppUi())->GetThemeNameL(iItemArray[i].iThemeId,HjelpBuf2,iItemArray[i].iIndex);
			
				Bufff.Num(iItemArray[i].iTime.DateTime().Hour());
				Bufff.Append(_L(":"));
				if(iItemArray[i].iTime.DateTime().Minute() < 10)
				{
					Bufff.AppendNum(0);	
				}
				Bufff.AppendNum(iItemArray[i].iTime.DateTime().Minute());
				Bufff.Append(_L("\t"));
				
				Bufff.Append(_L("\t"));
				Bufff.Append(HjelpBuf2);
				Bufff.Append(_L("\t"));
				
				if(!iItemArray[i].iEnabled){
					StringLoader::Load(HjelpBuf2,R_SH_STR_DISABLED);
					Bufff.Append(HjelpBuf2);
				}else{
					Bufff.Append(HjelpBuf1);
				}
			}
			
			MyArray->AppendL(Bufff);
		}
	}
	
	CleanupStack::Pop(MyArray);
	return MyArray;
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CMainContainer::SetRandomProfsL(void)
{
	RAknsSrvSession AknsSrvSession = STATIC_CAST(CMgAppUi*,CEikonEnv::Static()->EikAppUi())->AknsSrvSession();
	
	CArrayPtr< CAknsSrvSkinInformationPkg >* Arrr = AknsSrvSession.EnumerateSkinPackagesL(EAknsSrvAll);
	CleanupStack::PushL( Arrr );
	
	CScheduleDB* ScheduleDB = new(ELeave)CScheduleDB();
	CleanupStack::PushL(ScheduleDB);
	ScheduleDB->ConstructL();
	
	CArrayFixFlat<TInt>* SelectedItems = new(ELeave)CArrayFixFlat<TInt>(10);
	CleanupStack::PushL(SelectedItems);
	
	CDesCArrayFlat* SelectArrray = new(ELeave)CDesCArrayFlat(20);
	CleanupStack::PushL(SelectArrray);
	
	CAknListQueryDialog* dialog = new(ELeave)CAknListQueryDialog(SelectedItems);
	dialog->PrepareLC(R_RANDOM_SELECTION_QUERY);
			
	TFileName Hjelpper;
	
	CArrayFixFlat<TUint32>* HadItems = new(ELeave)CArrayFixFlat<TUint32>(10);
	CleanupStack::PushL(HadItems);
	
	ScheduleDB-> GetRandomProfilesL(*HadItems);
	
	for (TInt i=0; i < Arrr->Count(); i++)
	{
		Hjelpper.Num(1);
		Hjelpper.Append(_L("\t"));
		Hjelpper.Append(Arrr->At(i)->Name());
		
		for(TInt p=0; p < HadItems->Count(); p++)
		{
			if(HadItems->At(p) == (TUint32)(Arrr->At(i)->PID().Uid ().iUid))
			{
				dialog->ListBox()->View()->ToggleItemL(i);
			}
		}
				
		SelectArrray->AppendL(Hjelpper);
	}	
	
	CleanupStack::PopAndDestroy(HadItems);
		
	dialog->SetItemTextArray(SelectArrray); 
	dialog->SetOwnershipType(ELbmDoesNotOwnItemArray); 

	CArrayPtr<CGulIcon>* icons =new( ELeave ) CAknIconArray(1);
	CleanupStack::PushL(icons);
	
	_LIT(KtxAvkonMBM	,"Z:\\resource\\apps\\avkon2.mif");
	
	CFbsBitmap* FrameMsk2(NULL);
	CFbsBitmap* FrameImg2(NULL);
	AknIconUtils::CreateIconL(FrameImg2,FrameMsk2,KtxAvkonMBM,EMbmAvkonQgn_indi_checkbox_on,EMbmAvkonQgn_indi_checkbox_on_mask);

	icons->AppendL(CGulIcon::NewL(FrameImg2,FrameMsk2));
	
	CFbsBitmap* FrameMsk(NULL);
	CFbsBitmap* FrameImg(NULL);
	AknIconUtils::CreateIconL(FrameImg,FrameMsk,KtxAvkonMBM,EMbmAvkonQgn_indi_checkbox_off,EMbmAvkonQgn_indi_checkbox_off_mask);

	icons->AppendL(CGulIcon::NewL(FrameImg,FrameMsk));
	
	CleanupStack::Pop(icons);
	dialog->SetIconArrayL(icons); 

	if (dialog->RunLD ())
	{
		CArrayFixFlat<TUint32>* AddItems = new(ELeave)CArrayFixFlat<TUint32>(10);
		CleanupStack::PushL(AddItems);
		// clear all, and set... 
		for(TInt ii=0; ii < SelectedItems->Count(); ii++)
		{
			TInt SelType = SelectedItems->At(ii);
			if(SelType >= 0 && SelType < Arrr->Count())
			{
				AddItems->AppendL((TUint32)(Arrr->At(SelType)->PID().Uid ().iUid));
			}
		}
		
		ScheduleDB->DeleteRandomProfilesL();
		ScheduleDB->SetRandomProfilesL(*AddItems);
	
		CleanupStack::PopAndDestroy(AddItems);
	}

	CleanupStack::PopAndDestroy(2);  //SelectedItems,SelectArrray,
	CleanupStack::PopAndDestroy(ScheduleDB);
	CleanupStack::PopAndDestroy(1);  //Arrr,
}
	
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
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

	if(iMyHelpContainer)
	{
		Ret = iMyHelpContainer->OfferKeyEventL(aKeyEvent,aEventCode);
	}
	else if(iProfileSettings)
	{
		return iProfileSettings->OfferKeyEventL(aKeyEvent,aEventCode);
	}
	else
	{
		switch (aKeyEvent.iCode)
	    {
	    case EKeyDevice3:
			CEikonEnv::Static()->AppUiFactory()->MenuBar()->TryDisplayMenuBarL();
			break;
		case EKeyRightArrow:
		case EKeyLeftArrow:
		default:
			if(iProfileBox)
			{
				Ret = iProfileBox->OfferKeyEventL(aKeyEvent,aEventCode);
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
void CMainContainer::GetImageName(TDes& aFileName, const TInt& aIndex)
{
	for(TInt i=0; i < iItemArray.Count(); i++)
	{
		if(iItemArray[i].iIndex == aIndex){
			aFileName.Copy(iItemArray[i].iImageFile);
			break;
		}
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::HandleViewCommandL(TInt aCommand)
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
	case ERamdThemes:
		{
			SetRandomProfsL();
		}
		break;
	case EProfDisable:
	case EProfEnable:
		if(iProfileBox)
		{
			TInt Curr = iProfileBox->CurrentItemIndex();
			if(Curr >= 0 && Curr < iItemArray.Count())
			{
				if(aCommand == EProfEnable)
					iItemArray[Curr].iEnabled = ETrue;	
				else // EProfDisable
					iItemArray[Curr].iEnabled = EFalse;	
			
				
				CScheduleDB* ScheduleDB = new(ELeave)CScheduleDB();
				CleanupStack::PushL(ScheduleDB);
				ScheduleDB->ConstructL();				
				ScheduleDB->UpdateDatabaseL(iItemArray[Curr].iType,iItemArray[Curr].iProfileId,iItemArray[Curr].iThemeId,iItemArray[Curr].iRepeat,iItemArray[Curr].iTime,iItemArray[Curr].iIndex,iItemArray[Curr].iEnabled,iItemArray[Curr].iImageFile);
			
				CleanupStack::PopAndDestroy(ScheduleDB);
			
				iProfileBox->Model()->SetItemTextArray(GetProfilesArrayL());
			}
		}
		DrawNow();
		break;
	case EProfModify:
		if(iProfileBox)
		{
			TInt Curr = iProfileBox->CurrentItemIndex();
			if(Curr >= 0 && Curr < iItemArray.Count())
			{
				// get current and add data to settings..
				delete iProfileSettings;
				iProfileSettings = NULL;
				iProfileSettings = new(ELeave)CProfileSettings(iCba,iItemArray[Curr].iType);
				iProfileSettings->ConstructL();
				iProfileSettings->SetDataL(iItemArray[Curr].iProfileId,iItemArray[Curr].iThemeId,iItemArray[Curr].iRepeat,iItemArray[Curr].iTime,iItemArray[Curr].iIndex,iItemArray[Curr].iEnabled,iItemArray[Curr].iImageFile);	
			
				
			}
		}
		SetMenuL();
		DrawNow();
		break;
	case EProfRemove:
		if(iProfileBox)
		{
			TInt Curr = iProfileBox->CurrentItemIndex();
			if(Curr >= 0 && Curr < iItemArray.Count())
			{
				TBuf<60> tmpBuff;
				StringLoader::Load(tmpBuff,R_SH_STR_REMOVESCHED);	
			
				CAknQueryDialog* dlg = CAknQueryDialog::NewL();
				if(dlg->ExecuteLD(R_QUERY,tmpBuff))
				{
					CScheduleDB* ScheduleDB = new(ELeave)CScheduleDB();
					CleanupStack::PushL(ScheduleDB);
					ScheduleDB->ConstructL();
					ScheduleDB->DeleteFromDatabaseL(iItemArray[Curr].iIndex);	
					CleanupStack::PopAndDestroy(ScheduleDB);
					
					MakeProfileBoxL();
				}
			}
		}
		SetMenuL();
		DrawNow();
		break;
	case EThemNewProf:
	case EThemNewTimed:
		{		
			TBool IsProfile(EFalse);
			
			if(aCommand == EThemNewProf)
			{
				IsProfile = ETrue;
			}
			
			delete iProfileSettings;
			iProfileSettings = NULL;
			iProfileSettings = new(ELeave)CProfileSettings(iCba,IsProfile);
			iProfileSettings->ConstructL();
		}
		SetMenuL();
		DrawNow();
		break;
	case ESettOk:
		if(iProfileSettings)
		{
			TInt 		MyIndex(-1);
        	TBool		MyType(EFalse),myEnabled(EFalse);
        	TInt		MyProfileId(-1);
        	TUint32		MyThemeId(0);
        	TInt		MyRepeat(0);
        	TTime		MyTime;
        	TFileName	my_Fille;
        	MyTime.HomeTime();	
        	
        	iProfileSettings->GetValuesL(MyType,MyProfileId,MyThemeId,MyRepeat,MyTime,MyIndex,myEnabled,my_Fille);
        	
        	CScheduleDB* ScheduleDB = new(ELeave)CScheduleDB();
			CleanupStack::PushL(ScheduleDB);
			ScheduleDB->ConstructL();
			
        	if(MyIndex < 0)
				ScheduleDB->SaveToDatabaseL(MyType, MyProfileId,MyThemeId,MyRepeat,MyTime,MyIndex,myEnabled,my_Fille);
			else		
				ScheduleDB->UpdateDatabaseL(MyType, MyProfileId,MyThemeId,MyRepeat,MyTime,MyIndex,myEnabled,my_Fille);
			
			CleanupStack::PopAndDestroy(ScheduleDB);
			
			MakeProfileBoxL();
		}
	case ESettCancel:
		{
			delete iProfileSettings;
			iProfileSettings = NULL;
		}
		SetMenuL();
		DrawNow();
		break;
	default:

		break;
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CCoeControl* CMainContainer::ComponentControl( TInt /*aIndex*/) const
{
	CCoeControl* RetCont(NULL);

	if(iMyHelpContainer)
	{
		RetCont = iMyHelpContainer;		
	}
	else if(iProfileSettings)
	{
		RetCont = iProfileSettings;
	}
	else 
	{
		RetCont = iProfileBox;
	}
	
	return RetCont;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TInt CMainContainer::CountComponentControls() const
{
	TInt RetCount(0);
	
	if(iMyHelpContainer)
	{
		RetCount = 1;
	}
	else if(iProfileSettings)
	{
		RetCount = 1;
	}
	else 
	{
		RetCount = 1;
	}
 
	return RetCount;
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::SetMenuL(void)
{
	CEikonEnv::Static()->AppUiFactory()->MenuBar()->StopDisplayingMenuBar();

	if(iMyHelpContainer)
	{
		if(iCba)
		{
			iCba->SetCommandSetL(R_APPHELP_CBA);
			iCba->DrawDeferred();
		}
	}
	else if(iProfileSettings)
	{
		iProfileSettings->SetMenuL();
	}
	else
	{
		TInt MenuRes(R_MAIN_MENUBAR);
		TInt ButtomRes(R_MAIN_CBA);
	
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->SetMenuTitleResourceId(MenuRes);
	
		if(iCba)
		{
			iCba->SetCommandSetL(ButtomRes);
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
	if(aResourceId == R_MAIN_MENU)
	{
		TBool Dim(ETrue),DimEnable(ETrue);
	
		if(iProfileBox)
		{
			TInt Curr = iProfileBox->CurrentItemIndex();
		
			if(Curr >= 0)
			{
				Dim = EFalse;
			}
			
			if(Curr >= 0 && Curr < iItemArray.Count())
			{
				DimEnable = iItemArray[Curr].iEnabled;
			}
		}
		// not implemented
		aMenuPane->SetItemDimmed(EProfPlay,ETrue);
		
#ifdef SONE_VERSION
    if(!KAppIsTrial){
        aMenuPane->SetItemDimmed(EVisitDrJukka,ETrue);
    } 
#else

#endif  	
		
		if(!iAddIsValid)
		{
			aMenuPane->SetItemDimmed(EProfRemove,ETrue);
			aMenuPane->SetItemDimmed(EProfModify,ETrue);
			aMenuPane->SetItemDimmed(EProfDisable,ETrue);
			aMenuPane->SetItemDimmed(EProfEnable,ETrue);
			
			aMenuPane->SetItemDimmed(EThem,ETrue);
			aMenuPane->SetItemDimmed(ERamdThemes,ETrue);
		}
		
		if(Dim){
			aMenuPane->SetItemDimmed(EProfRemove,ETrue);
			aMenuPane->SetItemDimmed(EProfModify,ETrue);
			aMenuPane->SetItemDimmed(EProfDisable,ETrue);
			aMenuPane->SetItemDimmed(EProfEnable,ETrue);
			
		}else  {
		
			if(DimEnable)
				aMenuPane->SetItemDimmed(EProfEnable,ETrue);
			else
				aMenuPane->SetItemDimmed(EProfDisable,ETrue);
		}
	}
}

		

