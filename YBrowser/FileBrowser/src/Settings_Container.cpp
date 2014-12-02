/* 
	Copyright (c) 2006, 
	Jukka Silvennoinen
	All rights reserved 
*/
#include <coemain.h>
#include <aknnotewrappers.h> 
#include <aknutils.h>
#include <EIKPROGI.H>
#include <AknIconArray.h>
#include <eikapp.h>
#include <eikclbd.h>
#include <EIKLBV.H>
#include <eikmenup.h>
#include <eikmenub.h>
#include <APGCLI.H>
#include <f32file.h>
#include <AknGlobalNote.h>
#include <S32FILE.H>
#include <EIKCLB.H>
#include <akncommondialogs.h> 
#include <miutset.h>
#include <MIUTMSG.H>
#include <EIKBTGPC.H>
#include <aknnavi.h> 
#include <aknnavide.h> 
#include <akntabgrp.h> 
#include <stringloader.h>
#include <akntitle.h>

#include "IconHandler.h"
#include "Settings_Container.h"
#include "CommandSC_Settings.h"
#include "TypeAssociations.h"
#include "PluginSettings.h"

#include "YuccaBrowser.h"
#include "YuccaBrowser.hrh"

#include "Common.h"
#ifdef SONE_VERSION
	#include <YFB_2002B0AA.rsg>
#else
	#ifdef LAL_VERSION

	#else
		#ifdef __YTOOLS_SIGNED
			#include <YuccaBrowser_2000713D.rsg>
		#else
			#include <YuccaBrowser.rsg>
		#endif
	#endif
#endif
 
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
CSettingsContainer* CSettingsContainer::NewL(CEikButtonGroupContainer* aCba,TSettingsItem aSettings,CIconHandler* aIconHandler,CGeneralSettings& aGeneralSettings)
    {
    CSettingsContainer* self = CSettingsContainer::NewLC(aCba,aSettings,aIconHandler,aGeneralSettings);
    CleanupStack::Pop(self);
    return self;
    }
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
CSettingsContainer* CSettingsContainer::NewLC(CEikButtonGroupContainer* aCba,TSettingsItem aSettings,CIconHandler* aIconHandler,CGeneralSettings& aGeneralSettings)
    {
    CSettingsContainer* self = new (ELeave) CSettingsContainer(aCba,aSettings,aIconHandler,aGeneralSettings);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
CSettingsContainer::CSettingsContainer(CEikButtonGroupContainer* aCba,TSettingsItem aSettings,CIconHandler* aIconHandler,CGeneralSettings& aGeneralSettings)
:iCba(aCba),iSettings(aSettings),iIconHandler(aIconHandler),iGeneralSettings(aGeneralSettings)
    {
	
    }
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
CSettingsContainer::~CSettingsContainer()
{
	delete iCommandSCSettings;
	iCommandSCSettings = NULL;
	
	delete iTypesContainer;
	iTypesContainer = NULL;
	
	delete iPluInContainer;
	iPluInContainer = NULL;
	
	delete iYuccaSettings;
	iYuccaSettings = NULL;

	delete iSelectionBox;
	iSelectionBox = NULL;
	delete iListArray;
	iListArray = NULL;
	
	delete iBgContext;
	iBgContext = NULL;
}

			
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CSettingsContainer::ConstructL()
{
    CreateWindowL();
    
    CEikStatusPane*		sp = ((CAknAppUi*)iEikonEnv->EikAppUi())->StatusPane();
	if(sp)
	{		
		if(sp->CurrentLayoutResId() != R_AVKON_STATUS_PANE_LAYOUT_USUAL)//R_AVKON_STATUS_PANE_LAYOUT_SMALL_WITH_SIGNAL_PANE)
		{
			sp->SwitchLayoutL(R_AVKON_STATUS_PANE_LAYOUT_USUAL);
		}
		
		HBufC* Buff= StringLoader::LoadLC(R_MNU_SETTINGS);
		
		CAknTitlePane* TitlePane = STATIC_CAST(CAknTitlePane*,sp->ControlL(TUid::Uid(EEikStatusPaneUidTitle)));
		TitlePane->SetTextL(*Buff);
		
		CleanupStack::PopAndDestroy(Buff);

/*		CAknContextPane* 	ContextPane = (CAknContextPane *)sp->ControlL(TUid::Uid(EEikStatusPaneUidContext));
		if(ContextPane)
		{
			TFindFile AppFile(CCoeEnv::Static()->FsSession());
			if(KErrNone == AppFile.FindByDir(KtxAppIconFileName, KNullDesC))
			{
				ContextPane->SetPictureFromFileL(AppFile.File(),EMbmIconsB,EMbmIconsB_m);  
			}
		}*/
	}
	
	TRect myRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
	
    iBgContext = CAknsBasicBackgroundControlContext::NewL(KAknsIIDQsnBgAreaMain,TRect(0,0,myRect.Width(),myRect.Height()), ETrue);
	iBgContext->SetParentPos(myRect.iTl);
	
    SetRect(myRect);
    
  	ActivateL();	
	SetMenuL();
}

/*
-----------------------------------------------------------------------
CEikFormattedCellListBox 
-----------------------------------------------------------------------
*/
void CSettingsContainer::DisplayListBoxL(void)
{	
	TInt curIndex(-1);
	
	if(iSelectionBox)
	{
		curIndex = iSelectionBox->CurrentItemIndex();
	}
	
	delete iSelectionBox;
	iSelectionBox = NULL;
	iSelectionBox   = new( ELeave ) CAknSingleLargeStyleListBox();
	iSelectionBox->ConstructL(this,EAknListBoxMarkableList);
		
	if(!iListArray)
	{
		iListArray = new(ELeave)CDesCArrayFlat(10);
		
		TBuf<60> hjelpper,hjelpperBuf;	
		StringLoader::Load(hjelpper,R_MNU_GENERAL);
		hjelpperBuf.Copy(_L("0\t"));
		hjelpperBuf.Append(hjelpper);
		iListArray->AppendL(hjelpperBuf);
		
		StringLoader::Load(hjelpper,R_MNU_COMMANDS);
		hjelpperBuf.Copy(_L("0\t"));
		hjelpperBuf.Append(hjelpper);
		iListArray->AppendL(hjelpperBuf);
		
		StringLoader::Load(hjelpper,R_MNU_FILETYPES);
		hjelpperBuf.Copy(_L("0\t"));
		hjelpperBuf.Append(hjelpper);
		iListArray->AppendL(hjelpperBuf);
		
		StringLoader::Load(hjelpper,R_MNU_PLUGINS);
		hjelpperBuf.Copy(_L("0\t"));
		hjelpperBuf.Append(hjelpper);
		iListArray->AppendL(hjelpperBuf);
	}
	
	iSelectionBox->Model()->SetItemTextArray(iListArray);
    
    iSelectionBox->Model()->SetOwnershipType(ELbmDoesNotOwnItemArray);
	iSelectionBox->CreateScrollBarFrameL( ETrue );
    iSelectionBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
	iSelectionBox->SetRect(Rect());	
	
	iSelectionBox->ItemDrawer()->ColumnData()->SetIconArray(GetIconArrayL());
	iSelectionBox->ActivateL();
	iSelectionBox->ItemDrawer()->ColumnData()->EnableMarqueeL(ETrue);
	
	if(iSelectionBox->Model()->ItemTextArray()->MdcaCount() > 0)
	{
		if(curIndex >=0 && curIndex < iSelectionBox->Model()->ItemTextArray()->MdcaCount())
			iSelectionBox->SetCurrentItemIndex(curIndex);
		else
			iSelectionBox->SetCurrentItemIndex(0);
	}
	
	UpdateScrollBar(iSelectionBox);
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
CArrayPtr<CGulIcon>* CSettingsContainer::GetIconArrayL(void)
{
	CArrayPtr<CGulIcon>* icons =new( ELeave ) CAknIconArray(1);
	CleanupStack::PushL(icons);	
	

 	CFbsBitmap* BgPic(NULL);
	CFbsBitmap* BgMsk(NULL);
	
	MAknsSkinInstance* skin = AknsUtils::SkinInstance();
  	
    AknsUtils::CreateAppIconLC(skin,KUidYBrowserApp,  EAknsAppIconTypeContext,BgPic,BgMsk);
	CleanupStack::Pop(2);// BgPic,BgMsk

	icons->AppendL(CGulIcon::NewL(BgPic,BgMsk));

	CleanupStack::Pop(icons);
	return icons;
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CSettingsContainer::UpdateScrollBar(CEikListBox* aListBox)
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
-------------------------------------------------------------------------------
case EFilTypOpen:
case EExtrasCommandSC:
        			
-------------------------------------------------------------------------------
*/
void CSettingsContainer::HandleViewCommandL(TInt aCommand)
    {
	switch(aCommand)
        {
        case EPluginBack:
        	{
        		if(iPluInContainer)
				{
					iPluInContainer->CheckReFresh();
				}
	
        		SwitchToSettigL(-1);
        	}
        	SetMenuL();
        	DrawNow();
        	break;
        case ESettingsSave:
			if(iYuccaSettings)
			{
				iYuccaSettings->GetValuesL(iSettings);
				iGenChanged = ETrue;
			}
		case ESettingsCancel:
			{
				SwitchToSettigL(-1);	
			}
			SetMenuL();
			DrawNow();
			break;
		case EFilTypBack:
			{				
				SwitchToSettigL(-1);
			}
			SetMenuL();
			DrawNow();
			break; 
		case EExtrasCommandSCSave:
        	if(iCommandSCSettings)
        	{
        		iCommandSCSettings->SaveValuesL();
        	}
        case EExtrasCommandSCBack:
			{
        		SwitchToSettigL(-1);
        	}
        	SetMenuL();
        	DrawNow();
        	break;
		case ESettingsSelect:
			{
				SwitchToSettigL(GetSelectedIndexL());
			}
			SetMenuL();
			DrawNow();
			break;
        default:
        	if(iTypesContainer)
	        {
	        	iTypesContainer->HandleViewCommandL(aCommand);
	        }
	        else if(iPluInContainer)
	        {
	        	iPluInContainer->HandleViewCommandL(aCommand);
	        }
            break;
        }
  }

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CSettingsContainer::SwitchToSettigL(TInt aSelected)
{
	delete iYuccaSettings;
	iYuccaSettings = NULL;
	
	TBool ReFreshPlugIn(EFalse);
	
	if(iTypesContainer)
	{
		ReFreshPlugIn = iTypesContainer->CheckReFresh();
	}

	if(iPluInContainer)
	{
		ReFreshPlugIn = iPluInContainer->CheckReFresh();
	}
	
	delete iTypesContainer;
	iTypesContainer = NULL;
	delete iPluInContainer;
	iPluInContainer = NULL;
	
	delete iCommandSCSettings;
	iCommandSCSettings = NULL;
	
	
	if(ReFreshPlugIn)
	{
		TRAPD(Err,iIconHandler->ReFreshPlugInsL());	
	}
	
	TBuf<60> hjelpper;

	switch(aSelected)
	{
	case 0:
		{
		iYuccaSettings = new(ELeave)CYuccaSettings();
		iYuccaSettings->ConstructL(iSettings);
		StringLoader::Load(hjelpper,R_MNU_GENERAL);
		}
		break;
	case 1:
		{
		iCommandSCSettings = new(ELeave)CCommandSCSettings(iGeneralSettings);
		iCommandSCSettings->ConstructL();
		StringLoader::Load(hjelpper,R_MNU_COMMANDS);
		}
		break;
	case 2:
		{
		iTypesContainer = new(ELeave)CTypesContainer();
		iTypesContainer->ConstructL(iCba,iIconHandler);
		StringLoader::Load(hjelpper,R_MNU_FILETYPES);
		}
		break;
	case 3:
		{
		iPluInContainer = new(ELeave)CPluInContainer(iCba,iIconHandler);
		iPluInContainer->ConstructL();
		StringLoader::Load(hjelpper,R_MNU_PLUGINS);
		}
		break;
	default:
		StringLoader::Load(hjelpper,R_MNU_SETTINGS);	
		break;
	};
	
	CEikStatusPane*		sp = ((CAknAppUi*)iEikonEnv->EikAppUi())->StatusPane();
	if(sp)
	{		
		CAknTitlePane* TitlePane = STATIC_CAST(CAknTitlePane*,sp->ControlL(TUid::Uid(EEikStatusPaneUidTitle)));
		TitlePane->SetTextL(hjelpper);
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TBool CSettingsContainer::GetGeneralSettings(TSettingsItem& Settings)
{
	Settings = iSettings;
	return iGenChanged;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CSettingsContainer::SizeChanged()
{
	if ( iBgContext )
	{
		iBgContext->SetRect(Rect());

		if ( &Window() )
		{
			iBgContext->SetParentPos( PositionRelativeToScreen() );
		}
	}
	
	DisplayListBoxL();
	
	if(iCommandSCSettings)
	{
		iCommandSCSettings->SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
	}

	if(iTypesContainer)
 	{
 		iTypesContainer->SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
 	}
 	
 	if(iPluInContainer)
 	{
 		iPluInContainer->SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
 	}

	if(iYuccaSettings)
	{
		iYuccaSettings->SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
	}
}
/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
TTypeUid::Ptr CSettingsContainer::MopSupplyObject(TTypeUid aId)
{	
	if (iBgContext)
	{
		return MAknsControlContext::SupplyMopObject(aId, iBgContext );
	}
	
	return CCoeControl::MopSupplyObject(aId);
}

/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
TKeyResponse CSettingsContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
	TKeyResponse Ret = EKeyWasNotConsumed;

	if(iYuccaSettings)
	{
		Ret = iYuccaSettings->OfferKeyEventL(aKeyEvent,aType);
	}
	else if(iTypesContainer)
	{
		Ret = iTypesContainer->OfferKeyEventL(aKeyEvent,aType);
	}
	else if (iPluInContainer)
	{
		Ret = iPluInContainer->OfferKeyEventL(aKeyEvent,aType);
	}
	else if(iCommandSCSettings)
	{
		Ret = iCommandSCSettings->OfferKeyEventL(aKeyEvent,aType);
	}
	else 
	{		
		switch (aKeyEvent.iCode)
	    {
	    case EKeyDevice3:
	    	HandleViewCommandL(ESettingsSelect);
			break;
		case EKeyRightArrow:
		case EKeyLeftArrow:
		case EKeyDownArrow:
		case EKeyUpArrow:
		default:
			if(iSelectionBox)
			{
				Ret = iSelectionBox->OfferKeyEventL(aKeyEvent,aType);
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
TInt CSettingsContainer::GetSelectedIndexL(void)
{
	TInt Ret(-1);

	if(iSelectionBox)
	{
		Ret = iSelectionBox->CurrentItemIndex();
	}
	
	return Ret;
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CSettingsContainer::SetMenuL(void)
{	
	
	if(iCba)
	{
		if(iYuccaSettings)
		{
			iCba->SetCommandSetL(R_SETTINGS_CBA);
		}
		else if(iTypesContainer)
		{
			iTypesContainer->SetMenuL();
		}
		else if(iPluInContainer)
		{
			iPluInContainer->SetMenuL();
		}
		else if(iCommandSCSettings)
		{
			iCba->SetCommandSetL(R_COMMANDSC_CBA);
		}
		else
		{		
			iCba->SetCommandSetL(R_SETTALL_CBA);				
		}
		
		iCba->DrawDeferred();
	}
}

/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
void CSettingsContainer::InitMenuPanelL(TInt aResourceId, CEikMenuPane* aMenuPane)
{			
	if(iTypesContainer)
	{
		iTypesContainer->InitMenuPanelL(aResourceId,aMenuPane);
	}
	
	if(iPluInContainer)
	{
		iPluInContainer->InitMenuPanelL(aResourceId,aMenuPane);
	}
}
/*
-----------------------------------------------------------------------

-----------------------------------------------------------------------
*/
TInt CSettingsContainer::CountComponentControls() const
{
	if(iYuccaSettings)
	{
		return 1;
	}
	else if(iTypesContainer)
	{
		return 1;
	}
	else if(iPluInContainer)
	{
		return 1;
	}
	else if(iCommandSCSettings)
	{
		return 1;
	}
	else if(iSelectionBox)
	{
		return 1;
	}
	else
		return 0;
}

/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
CCoeControl* CSettingsContainer::ComponentControl(TInt /*aIndex*/) const
{	
	if(iYuccaSettings)
	{
		return iYuccaSettings;
			
	}
	else if(iTypesContainer)
	{
		return iTypesContainer;
	}
	else if(iPluInContainer)
	{
		return iPluInContainer;
	}
	else if(iCommandSCSettings)
	{
		return iCommandSCSettings;
	}
	else
	{
		return iSelectionBox;
	}
}

/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CSettingsContainer::Draw(const TRect& /*aRect*/) const
{
	if(iBgContext)
	{
		CWindowGc& gc = SystemGc();
 		// draw background skin first.
  		MAknsSkinInstance* skin = AknsUtils::SkinInstance();
		AknsDrawUtils::Background( skin, iBgContext, this, gc, Rect() );
	}
}


