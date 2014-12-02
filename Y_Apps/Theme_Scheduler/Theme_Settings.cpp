/* Copyright (c) 2001 - 2005 , J.P. Silvennoinen. All rights reserved */

#include "Theme_Settings.h"                     // own definitions

#include "S125AniLauncher.h"                     // own definitions
#include "S125AniLauncher.hrh"                   // own resource header
#include <S125AniLauncher_200027A9.rsg>


#include <AknQueryDialog.h>
#include <StringLoader.h>
#include <eikappui.h> 
#include <bautils.h>
#include <AknLists.h>
#include <AknPopup.h>



// Settings
//,
	
CAknSettingItem* CThemeSettingsListListbox ::CreateSettingItemL(TInt aIdentifier)
{
	CAknSettingItem* settingItem = NULL;

	switch(aIdentifier)
	{
	case EThemeSel:
		settingItem = new (ELeave) CAknTextSettingItem(aIdentifier,iSelTheme);
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
void CThemeSettingsListListbox::SetProfileSettings(CThemeSettings* aSettings)
{
	iSettings = aSettings;
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CThemeSettingsListListbox::HandleListBoxEventL(CEikListBox* aListBox,TListBoxEvent aEventType)
{
	if(ListBox())
	{
		if(ListBox()->CurrentItemIndex() == 0)// Theme
		{
			StoreSettingsL();
			
			SelectThemeL(iSelTheme,iThemeId);
			
			LoadSettingsL();
			ListBox()->DrawNow();
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
void CThemeSettingsListListbox::SelectThemeL(TDes& aTheme,TUint32& aThemeId)
{
	CArrayPtr< CAknsSrvSkinInformationPkg >* skinInfoArray = STATIC_CAST(CMgAppUi*,CEikonEnv::Static()->EikAppUi())->SkinInfoArray();

	CAknSinglePopupMenuStyleListBox* list = new(ELeave) CAknSinglePopupMenuStyleListBox;
	CleanupStack::PushL(list);

	CAknPopupList* popupList = CAknPopupList::NewL(list,R_AVKON_SOFTKEYS_OK_CANCEL);
	CleanupStack::PushL(popupList);

	list->ConstructL(popupList, CEikListBox::ELeftDownInViewRect);
	list->CreateScrollBarFrameL(ETrue);
	list->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,CEikScrollBarFrame::EAuto);

	CDesCArrayFlat* Array = new (ELeave) CDesCArrayFlat(1);
	CleanupStack::PushL(Array);

	
    for(TInt i=0; i < skinInfoArray->Count(); i++)
    {
		Array->AppendL(skinInfoArray->At( i )->Name());
	}
	
	list->Model()->SetItemTextArray(Array);
	CleanupStack::Pop();//Array
	list->Model()->SetOwnershipType(ELbmOwnsItemArray);

	TBuf<60> TmpBuffer;
	StringLoader::Load(TmpBuffer,R_SH_STR_SELTHEME);
	
	popupList->SetTitleL(TmpBuffer);
	if (popupList->ExecuteLD())
    {
		TInt Selected = list->CurrentItemIndex();
		if(Selected >= 0 && Selected < skinInfoArray->Count())
		{
			aThemeId = skinInfoArray->At(Selected)->PID().Uid ().iUid;
       		aTheme.Copy(skinInfoArray->At(Selected)->Name());
		}
	}

	CleanupStack::Pop();            // popuplist
	CleanupStack::PopAndDestroy(1);  // list
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CThemeSettingsListListbox::SizeChanged()
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
CThemeSettings::CThemeSettings(CEikButtonGroupContainer* aCba)
:iCba(aCba),iIndex(-1)
{	
}

CThemeSettings::~CThemeSettings()
{	
	delete iListBox;
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CThemeSettings::ConstructL(void)
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
void CThemeSettings::SetDataL(TUint32 aTheme,TTime aTime,TInt aIndex)
{
	if(iListBox)
	{
		iListBox->iThemeId	 = aTheme;
		STATIC_CAST(CMgAppUi*,CEikonEnv::Static()->EikAppUi())->GetThemeNameL(iListBox->iThemeId,iListBox->iSelTheme);
		
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
void CThemeSettings::MakeListboxL(void)
{
    iListBox = new (ELeave) CThemeSettingsListListbox;
		
	iListBox->SetProfileSettings(this);	
	
	iListBox->iAlrmTime.HomeTime();

//	iListBox->iSelProfile.Copy();
//	iListBox->iSelTheme.Copy();

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
void CThemeSettings::SizeChanged()
{
	if(iListBox)
	{
		iListBox->SetRect(Rect());
	}
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
TKeyResponse CThemeSettings::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
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
TInt CThemeSettings::CountComponentControls() const
{
	if(iListBox)
		return iListBox->CountComponentControls ();
	else
		return 0;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CCoeControl* CThemeSettings::ComponentControl( TInt aIndex) const
    {
		return iListBox->ComponentControl(aIndex); 
    }

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CThemeSettings::Draw(const TRect& aRect) const
    {
		CWindowGc& gc = SystemGc();
		gc.Clear(aRect); 
    }
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CThemeSettings::SetMenuL(void)
{
	if(iListBox && iCba)
	{
		TInt ButtomRes(R_AUTO_SET2_CBA);
		
		if(iListBox->iSelTheme.Length())
		{
			ButtomRes = R_AUTO_SET1_CBA;
		}
	
		iCba->SetCommandSetL(ButtomRes);
		iCba->DrawDeferred();
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CThemeSettings::GetValuesL(TUint32& aTheme,TTime& aTime,TInt& aIndex)
{
	if(iListBox)
	{
		iListBox->StoreSettingsL();
		
		aTheme	= iListBox->iThemeId;
		aTime 	= iListBox->iAlrmTime;
		
		aIndex	= iIndex;
	}
}
