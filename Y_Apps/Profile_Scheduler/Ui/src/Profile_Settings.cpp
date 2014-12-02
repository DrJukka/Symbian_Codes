/* Copyright (c) 2001 - 2008, Dr. Jukka Silvennoinen. All rights reserved */

#include "Profile_Settings.h"                     // own definitions
#include "YApp_E8876005.hrh"                   // own resource header

#include "Common.h"

#ifdef SONE_VERSION
	#include <YApp_2002884E.rsg>
#else
	#ifdef LAL_VERSION
		#include <YApp_20022080.rsg>
	#else
		#include <YApp_E8876005.rsg>
	#endif
#endif

#include "YApp_E8876005.h"

#include <AknQueryDialog.h>
#include <stringloader.h> 
#include <eikappui.h> 
#include <bautils.h>
#include <AknLists.h>
#include <AknPopup.h>



// Settings
//,
	
CAknSettingItem* CAknSettingsListListbox ::CreateSettingItemL(TInt aIdentifier)
{
	CAknSettingItem* settingItem = NULL;

	switch(aIdentifier)
	{
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
void CAknSettingsListListbox::HandleListBoxEventL(CEikListBox* aListBox,TListBoxEvent aEventType)
{
	if(ListBox())
	{
	
		if (aEventType == EEventEnterKeyPressed || aEventType == EEventItemDoubleClicked)
		{
			if(ListBox()->CurrentItemIndex() == 0)// profile
			{
				StoreSettingsL();
				
				SelectProfileL(iSelProfile,iProfileId);
				
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
		else // time
		{		
			CAknSettingItemList::HandleListBoxEventL(aListBox,aEventType);
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
		
		StringLoader::Load(Hjelppper,R_SH_STR_RANDOM);	
		
		Array->AppendL(Hjelppper);
		
		for (TInt i=0; i < Arrr->MdcaCount(); i++)
		{
			Hjelppper.Zero();
			
			if(Arrr->ProfileName (i))
			{
				Hjelppper.Copy(Arrr->ProfileName (i)->ShortName());
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
			
			Selected = Selected - 1;
			
			if(Selected < 0)
			{
				aProfId = -1;
				StringLoader::Load(aProfile,R_SH_STR_RANDOM);	
			}
			else if(Selected >= 0 && Selected < Arrr->MdcaCount())
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
CProfileSettings::CProfileSettings(void):iIndex(-1),iEnabled(ETrue)
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
void CProfileSettings::ConstructL()
{
    CreateWindowL();
	SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
	
	MakeListboxL();

	ActivateL();
	DrawNow();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CProfileSettings::SetDataL(TInt aProfile, TInt aRepeat, TTime aTime,TInt aIndex,const TBool& aEnabled)
{
	iEnabled = aEnabled;

	if(iListBox)
	{
		iListBox->iTypeId = aRepeat;
		STATIC_CAST(CMgAppUi*,CEikonEnv::Static()->EikAppUi())->GetRepeatTypeName(iListBox->iTypeId,iListBox->iType);
	
		iListBox->iProfileId = aProfile;
		STATIC_CAST(CMgAppUi*,CEikonEnv::Static()->EikAppUi())->GetProfileNameL(iListBox->iProfileId,iListBox->iSelProfile);
		
		iListBox->iAlrmTime  = aTime;
	
		iListBox->LoadSettingsL();
    	iListBox->DrawNow();
	}
	
	iIndex = aIndex;
}
/*
-----------------------------------------------------------------------------			
-----------------------------------------------------------------------------
*/
void CProfileSettings::MakeListboxL(void)
{
    iListBox = new (ELeave) CAknSettingsListListbox;
	iListBox->ConstructFromResourceL(R_PROF_SETTING);
	
	// default settings data
	iListBox->iTypeId = 0;
	STATIC_CAST(CMgAppUi*,CEikonEnv::Static()->EikAppUi())->GetRepeatTypeName(iListBox->iTypeId,iListBox->iType);

	iListBox->iProfileId = -1;
	STATIC_CAST(CMgAppUi*,CEikonEnv::Static()->EikAppUi())->GetProfileNameL(iListBox->iProfileId,iListBox->iSelProfile);
	
	iListBox->iAlrmTime.HomeTime();

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
void CProfileSettings::GetValuesL(TInt& aProfile, TInt& aRepeat, TTime& aTime,TInt& aIndex,TBool& aEnabled)
{
	aEnabled = iEnabled;

	if(iListBox)
	{
		iListBox->StoreSettingsL();
		
		aProfile= iListBox->iProfileId;
		aRepeat	= iListBox->iTypeId;
		aTime 	= iListBox->iAlrmTime;
		
		aIndex	= iIndex;
	}
}
