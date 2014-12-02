/* 
	Copyright (c) 2006, 
	Jukka Silvennoinen
	All rights reserved 
*/
#include "YuccaBrowser.h"
#include "YuccaBrowser.hrh"
#ifdef __YTOOLS_SIGNED
	#include <YuccaBrowser_2000713D.rsg>
#else
	#include <YuccaBrowser.rsg>
#endif
#include "Yucca_Settings.h"

// 
//
CAknSettingItem* CAknYuccaSettingsListListbox::CreateSettingItemL(TInt aIdentifier)
{
	CAknSettingItem* settingItem = NULL;

	switch(aIdentifier)
	{
	case EAttrTime:
		settingItem = new (ELeave) CAknTimeOrDateSettingItem(aIdentifier,CAknTimeOrDateSettingItem::ETime,iFileTime);
		break;	
	case EAttrDate:
		settingItem = new (ELeave) CAknTimeOrDateSettingItem(aIdentifier,CAknTimeOrDateSettingItem::EDate,iFileDate);
		break;	
	case EEYSetSortBy:
		settingItem = new (ELeave) CAknEnumeratedTextPopupSettingItem(aIdentifier,iSettings.iOrder);
		break;
	case EEYSetFoldersFirst:
		settingItem = new (ELeave) CAknBinaryPopupSettingItem(aIdentifier,iSettings.iFolders);
		break;
	case EEYSetSortOrder:
		settingItem = new (ELeave) CAknBinaryPopupSettingItem(aIdentifier,iSettings.iSort);
		break;
	case EEYSetShowAll:
		settingItem = new (ELeave) CAknBinaryPopupSettingItem(aIdentifier,iSettings.iShow);
		break;
	case EAttrSystem:
		settingItem = new (ELeave) CAknBinaryPopupSettingItem(aIdentifier,iSystem);
		break;
	case EAttrHidden:
		settingItem = new (ELeave) CAknBinaryPopupSettingItem(aIdentifier,iHidden);
		break;
	case EAttrReadOnly:
		settingItem = new (ELeave) CAknBinaryPopupSettingItem(aIdentifier,iReadOnly);
		break;
	case EEYSetSavePath:
		settingItem = new (ELeave) CAknBinaryPopupSettingItem(aIdentifier,iSettings.iSavePath);
		break;
	case EEYConfirmExit:
		settingItem = new (ELeave) CAknBinaryPopupSettingItem(aIdentifier,iSettings.iConfirmExit);
		break;
	case EEYSetOptmize:
		settingItem = new (ELeave) CAknBinaryPopupSettingItem(aIdentifier,iSettings.iOptimize);
		break;
	case EEYDoRecocnize:
		settingItem = new (ELeave) CAknBinaryPopupSettingItem(aIdentifier,iSettings.iDoRecocnize);
		break;
	case EEYSetFullScreen:
		settingItem = new (ELeave) CAknBinaryPopupSettingItem(aIdentifier,iSettings.iFullScreen);
		break;
	}
	
	
	
	return settingItem;
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CAknYuccaSettingsListListbox::SizeChanged()
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
CYuccaSettings::CYuccaSettings(void):iListBox(NULL)
{	
}

CYuccaSettings::~CYuccaSettings()
{	
	delete iListBox;
}


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CYuccaSettings::ConstructL(TSettingsItem aSettings)
{
    CreateWindowL();
    
	iFileDate.HomeTime();
	iAttr = EFalse;
	iSettings = aSettings;
	
	SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());

	ActivateL();
	DrawNow();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CYuccaSettings::ConstructL(TBool aReadOnly,TBool aHidden,TBool aSystem,TTime aTime)
{
    CreateWindowL();
	
	iFileDate 	= aTime;
	iReadOnly	= aReadOnly;
	iHidden		= aHidden;
	iSystem		= aSystem;
	iAttr 		= ETrue;
	SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());

	ActivateL();
	DrawNow();
}
				
				
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CYuccaSettings::SizeChanged()
{
	if(iAttr)
	{
		TInt Hours(0);
		TInt Minutes(0);
		TInt Seconds(0);
					
		GetValuesL(iReadOnly,iHidden,iSystem,iFileDate,Hours,Minutes,Seconds);
	
		iFileDate.DateTime().SetHour(Hours);
		iFileDate.DateTime().SetMinute(Minutes);
		iFileDate.DateTime().SetSecond(Seconds);
	}
	else
	{
		GetValuesL(iSettings);
	}
	
	CreateListBoxL(iSettings);
	
	DrawNow();
}
// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
void CYuccaSettings::CreateListBoxL(TSettingsItem aSettings)
{
    delete iListBox;
    iListBox = NULL;

    iListBox = new (ELeave) CAknYuccaSettingsListListbox;
    
    if(iAttr)
    {
    	iListBox->iReadOnly	= iReadOnly;
    	iListBox->iHidden	= iHidden;
    	iListBox->iSystem	= iSystem;
    	iListBox->iFileDate	= iFileDate;
    	iListBox->iFileTime = iFileDate;
    
    	iListBox->ConstructFromResourceL(R_YUCCA_ATTR);
    }
    else
    {
	    iListBox->iSettings.iSort 	= aSettings.iSort;
		iListBox->iSettings.iFolders= aSettings.iFolders;
		iListBox->iSettings.iShow	= aSettings.iShow; 	
		iListBox->iSettings.iOrder	= aSettings.iOrder;
    	iListBox->iSettings.iDoRecocnize= aSettings.iDoRecocnize;
    	iListBox->iSettings.iSavePath	 = aSettings.iSavePath;
    	iListBox->iSettings.iConfirmExit = aSettings.iConfirmExit;
    	iListBox->iSettings.iOptimize	 = aSettings.iOptimize; 
    	iListBox->iSettings.iFullScreen	 = aSettings.iFullScreen; 
    	
    	iListBox->ConstructFromResourceL(R_YUCCA_SETTING);
    }
    
	iListBox->MakeVisible(ETrue);
    iListBox->SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
    iListBox->ActivateL();
	
	iListBox->LoadSettingsL();
    iListBox->DrawNow();
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
TKeyResponse CYuccaSettings::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
	TKeyResponse Ret = EKeyWasNotConsumed;

	if(iListBox)
		Ret = iListBox->OfferKeyEventL(aKeyEvent,aType);

	return Ret;
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TInt CYuccaSettings::CountComponentControls() const
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
CCoeControl* CYuccaSettings::ComponentControl( TInt aIndex) const
{
	if(iListBox)
		return iListBox->ComponentControl(aIndex); 
	return NULL;
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CYuccaSettings::GetValuesL(TBool& aReadOnly,TBool& aHidden,TBool& aSystem,TTime& aTime,
TInt& aHours,TInt& aMinutes,TInt& aSeconds)
{
	if(iListBox)
	{
		iListBox->StoreSettingsL();
		aReadOnly 	= iListBox->iReadOnly;
		aHidden		= iListBox->iHidden;
		aSystem		= iListBox->iSystem;
		
		aTime = iListBox->iFileDate;
		
		aHours = iListBox->iFileTime.DateTime().Hour();
		aMinutes = iListBox->iFileTime.DateTime().Minute();
		aSeconds = iListBox->iFileTime.DateTime().Second();
	}
	else
	{
		aReadOnly 	= iReadOnly;
		aHidden		= iHidden;
		aSystem		= iSystem;
		aTime 		= iFileDate;	
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CYuccaSettings::GetValuesL(TSettingsItem& aSettings)
{
	if(iListBox)
	{
		iListBox->StoreSettingsL();
		aSettings.iFullScreen = iListBox->iSettings.iFullScreen;
		aSettings.iSort 	= iListBox->iSettings.iSort;
		aSettings.iFolders 	= iListBox->iSettings.iFolders;
		aSettings.iShow 	= iListBox->iSettings.iShow; 	
		aSettings.iOrder 	= iListBox->iSettings.iOrder; 	
		aSettings.iSavePath 	= iListBox->iSettings.iSavePath;
		aSettings.iConfirmExit	= iListBox->iSettings.iConfirmExit;
		aSettings.iOptimize 	= iListBox->iSettings.iOptimize;
		aSettings.iDoRecocnize = iListBox->iSettings.iDoRecocnize;
	}
	else
	{
		aSettings.iFullScreen = iSettings.iFullScreen;
		aSettings.iSort 	= iSettings.iSort;
		aSettings.iFolders 	= iSettings.iFolders;
		aSettings.iShow 	= iSettings.iShow; 	
		aSettings.iOrder 	= iSettings.iOrder;
		aSettings.iSavePath = iSettings.iSavePath;
		aSettings.iConfirmExit 	= iSettings.iConfirmExit;
		aSettings.iOptimize 	= iSettings.iOptimize;
		aSettings.iDoRecocnize 	= iSettings.iDoRecocnize;
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CYuccaSettings::Draw(const TRect& aRect) const
    {
		CWindowGc& gc = SystemGc();
		gc.Clear(aRect); 
    }

