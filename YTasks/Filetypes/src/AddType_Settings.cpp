/* Copyright (c) 2001 - 2005 , J.P. Silvennoinen. All rights reserved */

                     // own definitions
#include <AknQueryDialog.h>

#include <eikappui.h> 
#include <bautils.h>
#include <AknLists.h>
#include <AknIconArray.h>
#include <stringloader.h> 


#include "MainContainer.h"
#include "YTools_A0000F5D.hrh"
#include "YTools_A0000F5D.hrh"
#include "AddType_Settings.h"

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

//

CImeiSettingsListListbox ::CImeiSettingsListListbox(void)
{
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
CImeiSettingsListListbox ::~CImeiSettingsListListbox ()
{		
}				


// Settings
//
CAknSettingItem* CImeiSettingsListListbox ::CreateSettingItemL(TInt aIdentifier)
{
	CAknSettingItem* settingItem = NULL;

	switch(aIdentifier)
	{
    case ETypType:
    	settingItem = new (ELeave) CAknTextSettingItem(aIdentifier,iType);
		break;
	case ETypTypeId:
    	settingItem = new (ELeave) CAknTextSettingItem(aIdentifier,iTypeId);
		break;
	case ETypEntension:
    	settingItem = new (ELeave) CAknTextSettingItem(aIdentifier,iExtension);
		break;
	}

	return settingItem;
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CImeiSettingsListListbox::SizeChanged()
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
CImeiSettings::CImeiSettings()
{	
}

CImeiSettings::~CImeiSettings()
{	
	delete iListBox;
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CImeiSettings::ConstructL(const TDesC8& aType,const TDesC8& aData,const TDesC& aExtension,TInt aId)
{
    CreateWindowL();

	SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
	iId = aId;
	MakeListboxL(aType,aData,aExtension);

	ActivateL();
	DrawNow();
}
/*
-----------------------------------------------------------------------------			
-----------------------------------------------------------------------------
*/
void CImeiSettings::MakeListboxL(const TDesC8& aType,const TDesC8& aData,const TDesC& aExtension)
{
    iListBox = new (ELeave) CImeiSettingsListListbox();
	iListBox->ConstructFromResourceL(R_MYTYPE_SETTING);
	iListBox->MakeVisible(ETrue);
    iListBox->SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
    iListBox->ActivateL();

	TInt i =0;
				
	for(i = 0; i < aType.Length(); i++)
	{
		if(aType[i] == 47)
			break;
	}

	if(i < aType.Length())
	{
		iListBox->iType.Copy(aType.Left(i));
		iListBox->iTypeId.Copy(aType.Right(aType.Length() - (i+1)));
	}
	else
	{
		iListBox->iType.Copy(aType);
	}

	iListBox->iExtension.Copy(aExtension);

    iListBox->LoadSettingsL();
    iListBox->DrawNow();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CImeiSettings::SizeChanged()
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
TKeyResponse CImeiSettings::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
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
TInt CImeiSettings::CountComponentControls() const
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
CCoeControl* CImeiSettings::ComponentControl( TInt /*aIndex*/) const
    {
		return iListBox; 
    }

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CImeiSettings::Draw(const TRect& aRect) const
    {
		CWindowGc& gc = SystemGc();
		gc.Clear(aRect); 
    }

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CImeiSettings::GetDataL(TDes8& aType,TDes8& aData,TDes& aExtension,TInt& aId)
{
	aId = iId;
	aData.Zero();
	
	if(iListBox)
	{
		iListBox->StoreSettingsL();
		
		TFileName Hjelpper;
		Hjelpper.Copy(iListBox->iType);
		Hjelpper.Append(_L("/"));
		Hjelpper.Append(iListBox->iTypeId);
		
		aType.Copy(Hjelpper);
		aExtension.Copy(iListBox->iExtension);
	}
}





