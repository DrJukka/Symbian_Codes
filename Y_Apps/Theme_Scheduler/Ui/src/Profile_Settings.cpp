/* Copyright (c) 2001 - 2008 , Dr Jukka Silvennoinen. All rights reserved */


#include "Definitions.h"

#include "Profile_Settings.h"                     // own definitions
#include "YApp_E8876002.hrh"                   // own resource header
#include "Common.h"
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

#include <AknQueryDialog.h>
#include <stringloader.h> 
#include <eikappui.h> 
#include <bautils.h>
#include <AknLists.h>
#include <AknPopup.h>
#include <mgfetch.h> 
#include <EIKFUTIL.H>


// Settings
//,
	
CAknSettingItem* CAknSettingsListListbox ::CreateSettingItemL(TInt aIdentifier)
{
	CAknSettingItem* settingItem = NULL;

	switch(aIdentifier)
	{
	case EThemeSel:
		settingItem = new (ELeave) CAknTextSettingItem(aIdentifier,iSelTheme);
		break;
	case EProfileSel:
		settingItem = new (ELeave) CAknTextSettingItem(aIdentifier,iSelProfile);
		break;
	case EProfType:
		settingItem = new (ELeave) CAknTextSettingItem(aIdentifier,iType);
		break;
	case EProfTime:
		settingItem = new (ELeave) CAknTimeOrDateSettingItem(aIdentifier,CAknTimeOrDateSettingItem::ETime,iAlrmTime);
		break;
	}

	return settingItem;
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CAknSettingsListListbox::SetProfileSettings(CProfileSettings* aSettings)
{
	iSettings = aSettings;
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CAknSettingsListListbox::HandleListBoxEventL(CEikListBox* aListBox,TListBoxEvent aEventType)
{
	if(ListBox())
	{
		if (aEventType == EEventEnterKeyPressed || aEventType == EEventItemDoubleClicked)
		{
			if(iProfile)
			{
				if(ListBox()->CurrentItemIndex() == 0)// Theme
				{
					StoreSettingsL();
					SelectThemeL(iSelTheme,iThemeId);
					
					LoadSettingsL();
					ListBox()->DrawNow();
				}
				else if(ListBox()->CurrentItemIndex() == 1)// profile
				{
					StoreSettingsL();
					SelectProfileL(iSelProfile,iProfileId);
					
					LoadSettingsL();
					ListBox()->DrawNow();
				}
				else // time
				{		
					CAknSettingItemList::HandleListBoxEventL(aListBox,aEventType);
				}			
			}
			else
			{
				if(ListBox()->CurrentItemIndex() == 0)// Theme
				{	
					StoreSettingsL();
					SelectThemeL(iSelTheme,iThemeId);
					
					LoadSettingsL();
					ListBox()->DrawNow();
				}
				else if(ListBox()->CurrentItemIndex() == 1)// type
				{
					StoreSettingsL();
					SelectSetOnL(iType,iTypeId);
					LoadSettingsL();
					ListBox()->DrawNow();
				}
				else // time
				{		
					CAknSettingItemList::HandleListBoxEventL(aListBox,aEventType);
				}
			}
		}
		else // time
		{		
			CAknSettingItemList::HandleListBoxEventL(aListBox,aEventType);
		}
		if(iSettings)
		{
			iSettings->SetMenuL();
		}
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CAknSettingsListListbox::SelectSetOnL(TDes& aSetOn, TInt & aTypeId)
{
	CAknSinglePopupMenuStyleListBox* list = new(ELeave) CAknSinglePopupMenuStyleListBox;
	CleanupStack::PushL(list);

	CAknPopupList* popupList = CAknPopupList::NewL(list,R_AVKON_SOFTKEYS_OK_CANCEL);
	CleanupStack::PushL(popupList);

	list->ConstructL(popupList, CEikListBox::ELeftDownInViewRect);
	list->CreateScrollBarFrameL(ETrue);
	list->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,CEikScrollBarFrame::EAuto);

	CDesCArrayFlat* Array = new (ELeave) CDesCArrayFlat(1);
	CleanupStack::PushL(Array);

	TBuf<150> Hjelpper;
	for(TInt i=0; i < 10 ; i++)
	{
		STATIC_CAST(CMgAppUi*,CEikonEnv::Static()->EikAppUi())->GetRepeatTypeName(i,Hjelpper);
		Array->AppendL(Hjelpper);
	}

	list->Model()->SetItemTextArray(Array);
	CleanupStack::Pop();//Array
	list->Model()->SetOwnershipType(ELbmOwnsItemArray);

	Hjelpper.Zero();
	StringLoader::Load(Hjelpper,R_SH_STR_ACTIVEON);	
	popupList->SetTitleL(Hjelpper);
	if (popupList->ExecuteLD())
    {
    	TInt Selected = list->CurrentItemIndex();
		if(Selected >= 0)
		{
			aTypeId = Selected;
			aSetOn.Copy(list->Model()->ItemText(Selected));
		}
	}

	CleanupStack::Pop();            // popuplist
	CleanupStack::PopAndDestroy(1); // list
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CAknSettingsListListbox::SelectThemeL(TDes& aTheme,TUint32& aThemeId)
{
	RAknsSrvSession AknsSrvSession = STATIC_CAST(CMgAppUi*,CEikonEnv::Static()->EikAppUi())->AknsSrvSession();
	
	CArrayPtr< CAknsSrvSkinInformationPkg >* skinInfoArray = AknsSrvSession.EnumerateSkinPackagesL(EAknsSrvAll);
	CleanupStack::PushL( skinInfoArray );

	CAknSinglePopupMenuStyleListBox* list = new(ELeave) CAknSinglePopupMenuStyleListBox;
	CleanupStack::PushL(list);

	CAknPopupList* popupList = CAknPopupList::NewL(list,R_AVKON_SOFTKEYS_OK_CANCEL);
	CleanupStack::PushL(popupList);

	list->ConstructL(popupList, CEikListBox::ELeftDownInViewRect);
	list->CreateScrollBarFrameL(ETrue);
	list->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,CEikScrollBarFrame::EAuto);

	CDesCArrayFlat* Array = new (ELeave) CDesCArrayFlat(1);
	CleanupStack::PushL(Array);

	TBuf<255> Hjelpper;
	StringLoader::Load(Hjelpper,R_SH_STR_BGIMAGE);	
	Array->AppendL(Hjelpper);
	
	Hjelpper.Zero();
	StringLoader::Load(Hjelpper,R_SH_STR_RANDOM);	
	Array->AppendL(Hjelpper);
	
    for(TInt i=0; i < skinInfoArray->Count(); i++)
    {
		Array->AppendL(skinInfoArray->At( i )->Name());
	}
	
	list->Model()->SetItemTextArray(Array);
	CleanupStack::Pop();//Array
	list->Model()->SetOwnershipType(ELbmOwnsItemArray);

	Hjelpper.Zero();
	StringLoader::Load(Hjelpper,R_SH_STR_SELECTTHEME);	
	
	popupList->SetTitleL(Hjelpper);
	if (popupList->ExecuteLD())
    {
		TInt Selected = list->CurrentItemIndex();
		if(Selected == 0)
		{
			
			CDesCArrayFlat* SelectedFiles = new(ELeave)CDesCArrayFlat(1);
			CleanupStack::PushL(SelectedFiles);
			
			if(MGFetch::RunL(*SelectedFiles,EImageFile, EFalse)) 
			{
				if(SelectedFiles->Count())
				{		
					Hjelpper.Copy(SelectedFiles->MdcaPoint(0));
					if(EikFileUtils::Parse(Hjelpper) == KErrNone){
						
						aThemeId = 1; // is this safe ?
						iImageName.Copy(Hjelpper);
						
						TParsePtrC Hjepl(iImageName);
						aTheme.Copy(Hjepl.NameAndExt());
					}
		
				}
			}
			
			CleanupStack::PopAndDestroy(SelectedFiles);
		}
		else if(Selected == 1)
		{
			aThemeId = 0;
			StringLoader::Load(aTheme,R_SH_STR_RANDOM);
		}
		else
		{
			Selected = Selected - 2;
			
			if(Selected >= 0 && Selected < skinInfoArray->Count())
			{
				aThemeId = (TUint32)(skinInfoArray->At(Selected)->PID().Uid ().iUid);
       			aTheme.Copy(skinInfoArray->At(Selected)->Name());
			}
		}
	}

	CleanupStack::Pop();            // popuplist
	CleanupStack::PopAndDestroy(1);  // list,skinInfoArray
	
	skinInfoArray->ResetAndDestroy();        
	CleanupStack::PopAndDestroy(1); //skinInfoArray
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CAknSettingsListListbox::SelectProfileL(TDes& aProfile,TInt& aProfId)
{
	MProfileEngine* ProfileEngine = STATIC_CAST(CMgAppUi*,CEikonEnv::Static()->EikAppUi())->ProfileEngine();
	
	if(ProfileEngine)
	{
		MProfilesNamesArray* Arrr = ProfileEngine->ProfilesNamesArrayLC();
			
		CAknSinglePopupMenuStyleListBox* list = new(ELeave) CAknSinglePopupMenuStyleListBox;
		CleanupStack::PushL(list);

		CAknPopupList* popupList = CAknPopupList::NewL(list,R_AVKON_SOFTKEYS_OK_CANCEL);
		CleanupStack::PushL(popupList);

		list->ConstructL(popupList, CEikListBox::ELeftDownInViewRect);
		list->CreateScrollBarFrameL(ETrue);
		list->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,CEikScrollBarFrame::EAuto);

		CDesCArrayFlat* Array = new (ELeave) CDesCArrayFlat(1);
		CleanupStack::PushL(Array);

		TBuf<255> Hjelppper;
		
		for (TInt i=0; i < Arrr->MdcaCount(); i++)
		{
			Hjelppper.Zero();
			
			if(Arrr->ProfileName (i))
			{
				Hjelppper.Copy(Arrr->ProfileName (i)->ShortName());
			/*	Hjelppper.Append(_L(", "));
				Hjelppper.AppendNum(Arrr->ProfileName(i)->Id());//DBG*/
			}
			else
			{
				StringLoader::Load(Hjelppper,R_SH_STR_NONAME);	
			}
		
			Array->AppendL(Hjelppper);	
		}
		
		CleanupStack::Pop();//Array
		list->Model()->SetItemTextArray(Array);
		list->Model()->SetOwnershipType(ELbmOwnsItemArray);

		Hjelppper.Zero();
		StringLoader::Load(Hjelppper,R_SH_STR_SELECTPROF);	
		
		popupList->SetTitleL(Hjelppper);
		if (popupList->ExecuteLD())
	    {
			TInt Selected = list->CurrentItemIndex();
			
			if(Selected >= 0 && Selected < Arrr->MdcaCount())
			{
				if(Arrr->ProfileName(Selected))
				{	
					aProfile.Copy(Arrr->ProfileName(Selected)->ShortName());
					aProfId = Arrr->ProfileName(Selected)->Id();
				} 
			}
		}

		CleanupStack::Pop();            // popuplist
		CleanupStack::PopAndDestroy(1);  // list
		CleanupStack::PopAndDestroy(1);  // Arrr
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
CProfileSettings::CProfileSettings(CEikButtonGroupContainer* aCba,TBool aProfile)
:iProfile(aProfile),iEnabled(ETrue),iCba(aCba),iIndex(-1)
{	
}

CProfileSettings::~CProfileSettings()
{	
	delete iListBox;
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CProfileSettings::ConstructL(void)
{
    CreateWindowL();
	SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
	
	MakeListboxL();

	ActivateL();
	SetMenuL();
	DrawNow();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CProfileSettings::SetDataL(const TInt& aProfile,const TUint32& aTheme,const TInt& aRepeat,const TTime& aTime,const TInt& aIndex,const TBool& aEnabled, const TDesC& aFilename)
{
	iIndex = aIndex;
	iEnabled = aEnabled;
	
	if(iListBox)
	{
		iListBox->iTypeId = aRepeat;
		STATIC_CAST(CMgAppUi*,CEikonEnv::Static()->EikAppUi())->GetRepeatTypeName(iListBox->iTypeId,iListBox->iType);
	
		iListBox->iProfileId = aProfile;
		STATIC_CAST(CMgAppUi*,CEikonEnv::Static()->EikAppUi())->GetProfileNameL(iListBox->iProfileId,iListBox->iSelProfile);
		
		iListBox->iThemeId	 = aTheme;
		STATIC_CAST(CMgAppUi*,CEikonEnv::Static()->EikAppUi())->GetThemeNameL(iListBox->iThemeId,iListBox->iSelTheme,iIndex);
		
		iListBox->iAlrmTime  = aTime;
		iListBox->iImageName.Copy(aFilename);
		
		iListBox->LoadSettingsL();
    	iListBox->DrawNow();
	}
	
	
}

/*
-----------------------------------------------------------------------------			
-----------------------------------------------------------------------------
*/
void CProfileSettings::MakeListboxL(void)
{
    iListBox = new (ELeave) CAknSettingsListListbox;
		
	iListBox->SetProfileSettings(this);	
	
	iListBox->iAlrmTime.HomeTime();
	StringLoader::Load(iListBox->iType,R_SH_STR_EVERYDAY);
		
//	iListBox->iSelProfile.Copy();
//	iListBox->iSelTheme.Copy();

	iListBox->iProfileId = 0;
	iListBox->iProfile = iProfile;

	if(iProfile)
		iListBox->ConstructFromResourceL(R_PROF2_SETTING);	
	else
		iListBox->ConstructFromResourceL(R_PROF1_SETTING);	
	
	iListBox->MakeVisible(ETrue);
    iListBox->SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
    iListBox->ActivateL();
	iListBox->LoadSettingsL();
    iListBox->DrawNow();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CProfileSettings::SizeChanged()
{
	if(iListBox)
	{
		iListBox->SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
	}
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
TKeyResponse CProfileSettings::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
	TKeyResponse Ret = EKeyWasNotConsumed;
	
	if(iListBox)
	{
		Ret = iListBox->OfferKeyEventL(aKeyEvent,aType);
	}
	
	return Ret;
}


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TInt CProfileSettings::CountComponentControls() const
{
	if(iListBox)
		return 1;
	else
		return 0;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CCoeControl* CProfileSettings::ComponentControl( TInt aIndex) const
    {
		return iListBox; 
    }

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CProfileSettings::Draw(const TRect& aRect) const
    {
		CWindowGc& gc = SystemGc();
		gc.Clear(aRect); 
    }
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CProfileSettings::SetMenuL(void)
{
	if(iListBox && iCba)
	{
		TInt ButtomRes(R_SETT2_CBA);
		
		if(iProfile)
		{
			if(iListBox->iSelTheme.Length()
			&& iListBox->iSelProfile.Length())
			{
				ButtomRes = R_SETT1_CBA;
			}
		}
		else
		{
			if(iListBox->iSelTheme.Length())
			{
				ButtomRes = R_SETT1_CBA;
			}
		}
	
		iCba->SetCommandSetL(ButtomRes);
		iCba->DrawDeferred();
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CProfileSettings::GetValuesL(TBool& aType, TInt& aProfile, TUint32& aTheme,TInt& aRepeat, TTime& aTime,TInt& aIndex,TBool& aEnabled, TDes& aFilename)
{
	if(iListBox)
	{
		iListBox->StoreSettingsL();
		
		aProfile= iListBox->iProfileId;
		aTheme	= iListBox->iThemeId;
		aRepeat	= iListBox->iTypeId;
		aTime 	= iListBox->iAlrmTime;
		
		aFilename.Copy(iListBox->iImageName);
		
		aType 	= iProfile;
		aIndex	= iIndex;
		aEnabled = iEnabled;
	}
}
