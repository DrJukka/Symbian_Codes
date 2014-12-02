/* Copyright (c) 2001 - 2008 , Dr Jukka Silvennoinen. All rights reserved */

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
#include <akncolourselectiongrid.h> 
#include <aknmfnesettingpage.h> 
#include <AknSettingItemList.h>
#include <stringloader.h> 

#include "Main_Container.h"                     // own definitions
#include "Common.h"

#include "YApp_E887600F.hrh"                   // own resource header
#include "YApp_E887600F.h"
#include "Help_Container.h"

#ifdef SONE_VERSION
	#include <YApp_20028854.rsg>
#else

	#ifdef LAL_VERSION
		#include <YApp_20029072.rsg>
	#else
		#include <YApp_E887600F.rsg>
	#endif
#endif

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/

CAknSettingItem* CAknSettingsListListbox ::CreateSettingItemL(TInt aIdentifier)
{
	CAknSettingItem* settingItem = NULL;

	switch(aIdentifier)
	{
	case EClockOn:
		settingItem = new (ELeave) CAknBinaryPopupSettingItem(aIdentifier,iOn);
		break;
	case EkeyLockOn:
		settingItem = new (ELeave) CAknBinaryPopupSettingItem(aIdentifier,iLockEnabled);
		break;
	case EkeyLockTime:
		settingItem = new (ELeave) CAknTextSettingItem(aIdentifier,iLockTimetxt);
        break;
	case ELightsVal:
		settingItem = new (ELeave) CAknSliderSettingItem(aIdentifier,iLightVal);
		break;
	case ELightsVal2:
		settingItem = new (ELeave) CAknSliderSettingItem(aIdentifier,iLightVal2);
		break;		
	case ELightsOn:
		settingItem = new (ELeave) CAknEnumeratedTextPopupSettingItem(aIdentifier,iLightOn);
		break;	
	case EFontSel:
		settingItem = new (ELeave) CAknTextSettingItem(aIdentifier,iFont);
		break;	
	case EFontColor:
		settingItem = new (ELeave) CAknTextSettingItem(aIdentifier,iFontColor);
		break;	
	case EShowDate:
		settingItem = new (ELeave) CAknEnumeratedTextPopupSettingItem(aIdentifier,iShowDate);
		break;
	case EBackColor:
		settingItem = new (ELeave) CAknTextSettingItem(aIdentifier,iBgColor);
		break;			
	case ELightsTimeOut:  
		settingItem = new (ELeave) CAknTextSettingItem(aIdentifier,iLightTimetxt);
        break;		
	case EOrientaOn:
		settingItem = new (ELeave) CAknEnumeratedTextPopupSettingItem(aIdentifier,iOrienta);
		break;    
	}	

	
	return settingItem;
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CAknSettingsListListbox::HandleListBoxEventL(CEikListBox* aListBox,TListBoxEvent aEventType)
{
	if(ListBox())
	{
		TBuf<60> tmpBufff;
		if (aEventType == EEventEnterKeyPressed || aEventType == EEventItemDoubleClicked)
		{
			if(ListBox()->CurrentItemIndex() == 4)// light time out
			{
				StoreSettingsL();
			
				StringLoader::Load(tmpBufff,R_SH_STR_LIGHTTIM);	
				
				CAknNumberQueryDialog* Dialog = CAknNumberQueryDialog::NewL(iLightTime,CAknQueryDialog::ENoTone);
				Dialog->PrepareLC(R_BIG_DIALOG);
				Dialog->SetPromptL(tmpBufff);
				if(Dialog->RunLD())
				{
					iLightTimetxt.Num(iLightTime);
				}
				
				LoadSettingsL();
				ListBox()->DrawNow();
			}
			else if(ListBox()->CurrentItemIndex() == 6)// Font
			{
				StoreSettingsL();
				
				SelectFontL();
				
				LoadSettingsL();
				ListBox()->DrawNow();
			}
			else if(ListBox()->CurrentItemIndex() == 7)// Font color
			{
				StoreSettingsL();
				
				StringLoader::Load(tmpBufff,R_SH_STR_FONTCOLOR);	
				
				if(SelectFontBGColorL(iFontColorRgb,tmpBufff)){
					GetColorName(iFontColorRgb,iFontColor);
				}
				
				LoadSettingsL();
				ListBox()->DrawNow();				
			}
			else if(ListBox()->CurrentItemIndex() == 8)// Background color
			{
				StoreSettingsL();
				
				StringLoader::Load(tmpBufff,R_SH_STR_BACKCOLOR);	
				
				if(SelectFontBGColorL(iBgColorRgb,tmpBufff)){
					GetColorName(iBgColorRgb,iBgColor);
				}	
				
				LoadSettingsL();
				ListBox()->DrawNow();
			}		
			else if(ListBox()->CurrentItemIndex() == 11)// Key lock time
			{
				StoreSettingsL();
			
				StringLoader::Load(tmpBufff,R_SH_STR_LOCKTIM);	
				
				CAknNumberQueryDialog* Dialog = CAknNumberQueryDialog::NewL(iLockTime,CAknQueryDialog::ENoTone);
				Dialog->PrepareLC(R_BIG_DIALOG);
				Dialog->SetPromptL(tmpBufff);
				if(Dialog->RunLD())
				{
					iLockTimetxt.Num(iLockTime);
				}
				
				
				LoadSettingsL();
				ListBox()->DrawNow();
			}
			else // time
			{		
				CAknSettingItemList::HandleListBoxEventL(aListBox,aEventType);
			}
		}
		else // time
		{		
			CAknSettingItemList::HandleListBoxEventL(aListBox,aEventType);
		}
	}
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
TBool CAknSettingsListListbox::SelectFontBGColorL(TRgb& aColor, const TDesC& aTitle)
{
	TBool Ret(EFalse);
	
    CAknSinglePopupMenuStyleListBox* list = new(ELeave) CAknSinglePopupMenuStyleListBox;
	CleanupStack::PushL(list);

    CAknPopupList* popupList = CAknPopupList::NewL(list, R_AVKON_SOFTKEYS_SELECT_BACK,AknPopupLayouts::EMenuWindow);  
    CleanupStack::PushL(popupList);

    list->ConstructL(popupList, 0);
    list->CreateScrollBarFrameL(ETrue);
    list->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,CEikScrollBarFrame::EAuto);
    
	CDesCArrayFlat* AnimList = new(ELeave)CDesCArrayFlat(10);
	CleanupStack::PushL(AnimList);
	
	TBuf<60> Bufff;
	StringLoader::Load(Bufff,R_SH_STR_CURRTHEM);
	
	AnimList->AppendL(Bufff);
	
	Bufff.Zero();
	StringLoader::Load(Bufff,R_SH_STR_SELCOLOR);
	AnimList->AppendL(Bufff);


	CleanupStack::Pop(AnimList);
    list->Model()->SetItemTextArray(AnimList);
    list->Model()->SetOwnershipType(ELbmOwnsItemArray);

	popupList->SetTitleL(aTitle);
	
	if (popupList->ExecuteLD())
    {
		TInt Selected = list->CurrentItemIndex();
	
		if(Selected == 0)
		{
			aColor = TRgb(0x5ffffff);
			Ret = ETrue;
		}
		else if(Selected == 1)
		{
			Ret = SelectColorL(aColor);
		}

	}

  	CleanupStack::Pop();             // popuplist
	CleanupStack::PopAndDestroy(1);  // list

	
	return Ret;
}
/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
TBool CAknSettingsListListbox::SelectColorL(TRgb& aColor)
{
	TBool ret(EFalse);
	TBool noneExist = EFalse;
 
	TBool noneChosen = EFalse;
 
	TRgb colour = KRgbDarkGray; 
 
	CArrayFixFlat<TRgb>* colours = new(ELeave) CArrayFixFlat<TRgb>(16);
 
	colours->AppendL(KRgbBlack);
	colours->AppendL(KRgbDarkGray);
	colours->AppendL(KRgbDarkRed);
	colours->AppendL(KRgbDarkGreen);
	colours->AppendL(KRgbDarkYellow);
	colours->AppendL(KRgbDarkBlue);
	colours->AppendL(KRgbDarkMagenta);
	colours->AppendL(KRgbDarkCyan);
	colours->AppendL(KRgbRed);
	colours->AppendL(KRgbGreen);
	colours->AppendL(KRgbYellow);
	colours->AppendL(KRgbBlue);
	colours->AppendL(KRgbMagenta);
	colours->AppendL(KRgbCyan);
	colours->AppendL(KRgbGray);
	colours->AppendL(KRgbWhite);
  
	CAknColourSelectionGrid *d = CAknColourSelectionGrid::NewL( colours, noneExist, noneChosen, colour);
 
	if(d->ExecuteLD())
	{
		ret = ETrue;
		aColor = colour;
	}
	
	return ret;
}

/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
void CAknSettingsListListbox::GetColorName(const TRgb& aColor, TDes& aName)
{
	aName.Zero();

	if(KRgbBlack == aColor){
		StringLoader::Load(aName,R_SH_STR_BLACK);
	}else if(KRgbDarkGray == aColor){
		StringLoader::Load(aName,R_SH_STR_DARKGRAY);
	}else if(KRgbDarkRed == aColor){
		StringLoader::Load(aName,R_SH_STR_DARKRED);
	}else if(KRgbDarkGreen == aColor){
		StringLoader::Load(aName,R_SH_STR_DARKGREEN);
	}else if(KRgbDarkYellow == aColor){
		StringLoader::Load(aName,R_SH_STR_DARKYELLOW);
	}else if(KRgbDarkBlue == aColor){
		StringLoader::Load(aName,R_SH_STR_DARKBLUE);
	}else if(KRgbDarkMagenta == aColor){
		StringLoader::Load(aName,R_SH_STR_DARKMAGENDA);
	}else if(KRgbDarkCyan == aColor){
		StringLoader::Load(aName,R_SH_STR_DARKCYAN);
	}else if(KRgbRed == aColor){
		StringLoader::Load(aName,R_SH_STR_RED);
	}else if(KRgbGreen == aColor){
		StringLoader::Load(aName,R_SH_STR_GREEN);
	}else if(KRgbYellow == aColor){
		StringLoader::Load(aName,R_SH_STR_YELLOW);
	}else if(KRgbBlue == aColor){
		StringLoader::Load(aName,R_SH_STR_BLUE);
	}else if(KRgbMagenta == aColor){
		StringLoader::Load(aName,R_SH_STR_MAGENDA);
	}else if(KRgbCyan == aColor){
		StringLoader::Load(aName,R_SH_STR_CYAN);
	}else if(KRgbGray == aColor){
		StringLoader::Load(aName,R_SH_STR_GRAY);
	}else if(0x5ffffff == aColor.Value()){
		StringLoader::Load(aName,R_SH_STR_CURRTHEM);
	}else if(KRgbWhite == aColor){
		StringLoader::Load(aName,R_SH_STR_WHITE);
	}else{
		aName.Num(aColor.Value(),EHex);
	}
}

/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
void CAknSettingsListListbox::SelectFontL(void)
{
	TInt NumTfaces = CEikonEnv::Static()->ScreenDevice()->NumTypefaces();
	if(NumTfaces > 0)
	{
		CAknSinglePopupMenuStyleListBox* list = new(ELeave) CAknSinglePopupMenuStyleListBox;
	    CleanupStack::PushL(list);

		CAknPopupList* popupList = CAknPopupList::NewL(list,R_AVKON_SOFTKEYS_OK_CANCEL);
	    CleanupStack::PushL(popupList);
		
	    list->ConstructL(popupList, CEikListBox::ELeftDownInViewRect);
		list->CreateScrollBarFrameL(ETrue);
	    list->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,CEikScrollBarFrame::EAuto);
	    
	    CDesCArrayFlat* Ittems = new(ELeave)CDesCArrayFlat(5);
	    CleanupStack::PushL(Ittems);
	    
	    TTypefaceSupport tfs;
	    
	    for(TInt i=0; i < NumTfaces; i++)
	    {
	    	CEikonEnv::Static()->ScreenDevice()->TypefaceSupport(tfs, i);
			Ittems->AppendL(tfs.iTypeface.iName);
	    }
		
	    CleanupStack::Pop(Ittems);
	    list->Model()->SetItemTextArray(Ittems);
		list->Model()->SetOwnershipType(ELbmOwnsItemArray);
		
		TBuf<60> tmpBufff;
		StringLoader::Load(tmpBufff,R_SH_STR_FONT);	
		
		popupList->SetTitleL(tmpBufff);
		
		if (popupList->ExecuteLD())
	    {
			TInt Ret = list->CurrentItemIndex();
			if(Ret >= 0 && Ret < NumTfaces)
			{
				CEikonEnv::Static()->ScreenDevice()->TypefaceSupport(tfs, Ret);
				iFont.Copy(tfs.iTypeface.iName);	
			}
	    }
	    
	    CleanupStack::Pop();            // popuplist
	    CleanupStack::PopAndDestroy();  // list
	}
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CAknSettingsListListbox::SizeChanged()
    {
    if (ListBox()) 
        {
        ListBox()->SetRect(Rect());
        }
    }

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

	delete iListBox;
	iListBox = NULL;

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

	MakeListboxL(-1);
	SetMenuL();
	ActivateL();
	
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
void CMainContainer::MakeListboxL(TInt aSelected)
{			
    delete iListBox;
    iListBox = NULL;
    iListBox = new (ELeave) CAknSettingsListListbox();
    iListBox->SetMopParent(this); 
    
    GetValuesFromStoreL();
	
	iListBox->ConstructFromResourceL(R_PROF1_SETTING);

	iListBox->MakeVisible(ETrue);
    iListBox->SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());

	if(aSelected >= 0 && aSelected < 2)
	{
		iListBox->ListBox()->SetCurrentItemIndex(aSelected);
	}
    iListBox->ActivateL();
	iListBox->LoadSettingsL();
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::SizeChanged()
{
	if(iListBox)
	{
		iListBox->SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
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
	TRect rect;
	TBool SetR(EFalse);
	
    if ( aType==KEikDynamicLayoutVariantSwitch )
    {  	
    	SetR = ETrue;
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, rect);
    } 
 
 	if(SetR)
 	{
	    
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
	MakeListboxL(-1);
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
	else if(iListBox && iAddIsValid)
	{
		Ret = iListBox->OfferKeyEventL(aKeyEvent,aEventCode);
	}

	return Ret;
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
	if(iMyHelpContainer)
	{
		return iMyHelpContainer;		
	}
	else 
	{
		return iListBox;
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TInt CMainContainer::CountComponentControls() const
{
	if(iMyHelpContainer)
		return 1;
	else if(iListBox)
		return 1;
	else
		return 0;
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
#ifdef SONE_VERSION
    if(!KAppIsTrial){
        aMenuPane->SetItemDimmed(EVisitDrJukka,ETrue);
    } 
#else

#endif  
	}
}


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::GetValuesFromStoreL(void)
{	
	if(!iListBox)
		return;
	
	if(!iAddIsValid)
	{
		iListBox->iOn = EFalse;
		iListBox->iLockEnabled = EFalse;
		iListBox->iAddIsValid = EFalse;
	}
	else
	{
		iListBox->iOrienta = 0;
		iListBox->iAddIsValid = ETrue;
	
		TFindFile AufFolder(CCoeEnv::Static()->FsSession());
		if(KErrNone == AufFolder.FindByDir(KtxMonFile, KNullDesC))
		{
			CDictionaryFileStore* MyDStore = CDictionaryFileStore::OpenLC(CCoeEnv::Static()->FsSession(),AufFolder.File(), TUid::Uid(0x102013AD));
				
			iListBox->iOn = EFalse;
			iListBox->iLockEnabled = EFalse;
	
			
			TUint32 TmpValue(0);
			GetValuesL(MyDStore,0x5555,TmpValue);
			
			if(TmpValue > 50)
				iListBox->iOn = ETrue;
			else// if(TmpValue > 10)
				iListBox->iOn = EFalse;
			
			TmpValue = 0;
			GetValuesL(MyDStore,0x7777,TmpValue);
			
			if(TmpValue > 50)
				iListBox->iLockEnabled = ETrue;
			else 
				iListBox->iLockEnabled  = EFalse;
			
			TmpValue = 0;
			GetValuesL(MyDStore,0x9999,TmpValue);
			
			if(TmpValue < 10)
				iListBox->iLockTime = 10;
			else if(TmpValue > 3599)
				iListBox->iLockTime = 3599;
			else
				iListBox->iLockTime = TmpValue;
	
			iListBox->iLockTimetxt.Num(iListBox->iLockTime);
			
			TmpValue = 0;
			GetValuesL(MyDStore,0x5678,TmpValue);
			
			if(TmpValue < 1)
				iListBox->iLightTime = 1;
			else if(TmpValue > 3599)
				iListBox->iLightTime = 3599;
			else
				iListBox->iLightTime = TmpValue;
			
			iListBox->iLightTimetxt.Num(iListBox->iLightTime);
			
			TmpValue = 0;
			GetValuesL(MyDStore,0x8888,TmpValue);
			
			if(TmpValue > 2)
				iListBox->iLightOn = 0;
			else 
				iListBox->iLightOn  = TmpValue;
	
			TmpValue = 0;
			GetValuesL(MyDStore,0x7676,TmpValue);
			
			if(TmpValue > 2)
				iListBox->iShowDate = 0;
			else 
				iListBox->iShowDate  = TmpValue;		
			
			
			TmpValue = 0;
			GetValuesL(MyDStore,0x6666,TmpValue);
			
			if(TmpValue < 0)
				iListBox->iLightVal = 0;
			else if(TmpValue > 100)
				iListBox->iLightVal = 100;
			else
				iListBox->iLightVal = TmpValue;
			
			
			TmpValue = 0;
			GetValuesL(MyDStore,0x6667,TmpValue);
			
			if(TmpValue < 0)
				iListBox->iLightVal2 = 0;
			else if(TmpValue > 100)
				iListBox->iLightVal2 = 100;
			else
				iListBox->iLightVal2 = TmpValue;		
				
			GetValuesL(MyDStore,0x1234,iListBox->iFont);
			
			TmpValue = 0;
			GetValuesL(MyDStore,0x1001,TmpValue);
			iListBox->iBgColorRgb = TRgb(TmpValue);
			
			iListBox->GetColorName(iListBox->iBgColorRgb,iListBox->iBgColor);
			
			TmpValue = 0;
			GetValuesL(MyDStore,0x2002,TmpValue);
			iListBox->iFontColorRgb = TRgb(TmpValue);
			
			iListBox->GetColorName(iListBox->iFontColorRgb,iListBox->iFontColor);
			
			TmpValue = 0;
			GetValuesL(MyDStore,0x2772,TmpValue);
			
			if(TmpValue > 2){
				iListBox->iOrienta = 0;
			}else{
				iListBox->iOrienta = TmpValue;
			}
			
			CleanupStack::PopAndDestroy(1);// Store
		}
	}
}
		
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::GetValuesL(CDictionaryFileStore* aDStore,TInt aUID,TDes& aValue)
{
	aValue.Zero();
	
	if(aDStore == NULL)
		return;
		
	TUid FileUid = {0x10};
	FileUid.iUid = aUID;

	if(aDStore->IsPresentL(FileUid))
	{
		RDictionaryReadStream in;
		in.OpenLC(*aDStore,FileUid);
		in >> aValue;
		CleanupStack::PopAndDestroy(1);// in
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::GetValuesL(CDictionaryFileStore* aDStore,TInt aUID,TUint32& aValue)
{
	if(aDStore == NULL)
		return;
		
	TUid FileUid = {0x10};
	FileUid.iUid = aUID;

	if(aDStore->IsPresentL(FileUid))
	{
		RDictionaryReadStream in;
		in.OpenLC(*aDStore,FileUid);
		aValue = in.ReadInt32L();
		CleanupStack::PopAndDestroy(1);// in
	}
	else
		aValue = 0;
}

/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
void CMainContainer::SaveValuesL(void)
{
	if(!iListBox)
		return;

	iListBox->StoreSettingsL();

	TFindFile AufFolder(CCoeEnv::Static()->FsSession());
	if(KErrNone == AufFolder.FindByDir(KtxMonFile, KNullDesC))
	{
		User::LeaveIfError(CCoeEnv::Static()->FsSession().Delete(AufFolder.File()));

		CDictionaryFileStore* MyDStore = CDictionaryFileStore::OpenLC(CCoeEnv::Static()->FsSession(),AufFolder.File(), TUid::Uid(0x102013AD));

		if(iListBox->iOn)
			SaveValuesL(MyDStore,0x5555,0x100);	
		else
			SaveValuesL(MyDStore,0x5555,0x000);

		if(iListBox->iLockEnabled)
			SaveValuesL(MyDStore,0x7777,0x100);	
		else
			SaveValuesL(MyDStore,0x7777,0x000);
		
		SaveValuesL(MyDStore,0x9999,iListBox->iLockTime);
		SaveValuesL(MyDStore,0x5678,iListBox->iLightTime);		
		SaveValuesL(MyDStore,0x8888,iListBox->iLightOn);
		SaveValuesL(MyDStore,0x7676,iListBox->iShowDate);
		
		SaveValuesL(MyDStore,0x1234,iListBox->iFont);
		
		SaveValuesL(MyDStore,0x1001,iListBox->iBgColorRgb.Value());
		SaveValuesL(MyDStore,0x2002,iListBox->iFontColorRgb.Value());
		
		SaveValuesL(MyDStore,0x6666,iListBox->iLightVal);
		SaveValuesL(MyDStore,0x6667,iListBox->iLightVal2);
		
		SaveValuesL(MyDStore,0x2772,iListBox->iOrienta);						
				
		MyDStore->CommitL();
		CleanupStack::PopAndDestroy(1);// Store
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::SaveValuesL(CDictionaryFileStore* aDStore,TInt aUID, const TDesC& aValue)
{
	if(aDStore == NULL)
		return;
		
	TUid FileUid = {0x10};
	FileUid.iUid = aUID;

	if(aDStore->IsPresentL(FileUid))
	{
		aDStore->Remove(FileUid);
		aDStore->CommitL();
	}
			
	RDictionaryWriteStream out2;
	out2.AssignLC(*aDStore,FileUid);
	out2 << aValue;
	out2.CommitL(); 	
	CleanupStack::PopAndDestroy(1);// out2
}


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::SaveValuesL(CDictionaryFileStore* aDStore,TInt aUID, TUint32 aValue)
{
	if(aDStore == NULL)
		return;
		
	TUid FileUid = {0x10};
	FileUid.iUid = aUID;

	if(aDStore->IsPresentL(FileUid))
	{
		aDStore->Remove(FileUid);
		aDStore->CommitL();
	}
			
	RDictionaryWriteStream out2;
	out2.AssignLC(*aDStore,FileUid);
	out2.WriteInt32L(aValue);
	out2.CommitL(); 	
	CleanupStack::PopAndDestroy(1);// out2
}

