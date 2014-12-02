/* Copyright (c) 2001 - 2008, Dr. Jukka Silvennoinen. All rights reserved */

#include "Main_Container.h"                     // own definitions

#include <CPbkContactEngine.h>
#include <CPbkSingleEntryFetchDlg.h>
#include <rpbkviewresourcefile.h>
#include <CNTFLDST.H>
#include <AknGlobalNote.h>

#include "Profile_Settings.h"                     // own definitions
#include "YApps_E8876008.hrh"                   // own resource header

#include "Common.h"
#ifdef SONE_VERSION
	#include <YApps_20028851.rsg>
#else
	#ifdef LAL_VERSION
		#include <YApps_20022085.rsg>
	#else
		#include <YApps_E8876008.rsg>
	#endif
#endif

#include "YApps_E8876008.h"
#include "MsgDataBase.h" 

#include <AknQueryDialog.h>

#include <eikappui.h> 
#include <bautils.h>
#include <AknLists.h>
#include <AknPopup.h>

#include "SMS_Container.h"
#include "Messages_Container.h" 

// Settings
//,
	
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CAknSettingsListListbox::CAknSettingsListListbox(MGetMsgCallback& aCallBack):iCallBack(aCallBack)
{	
}


CAknSettingItem* CAknSettingsListListbox ::CreateSettingItemL(TInt aIdentifier)
{
	CAknSettingItem* settingItem = NULL;

	switch(aIdentifier)
	{
	case ESenderName:
		settingItem = new (ELeave) CAknTextSettingItem(aIdentifier,iSendName);
		break;
	case ESenderNumber:
		settingItem = new (ELeave) CAknTextSettingItem(aIdentifier,iSendNumber);
		break;
	case ESMSMessage:
		settingItem = new (ELeave) CAknTextSettingItem(aIdentifier,iSendMessage);
		break;
	case ESendTime:
		settingItem = new (ELeave) CAknTimeOrDateSettingItem(aIdentifier,CAknTimeOrDateSettingItem::ETime,iAlrmTime);
		break;
	case ESendDate:
		settingItem = new (ELeave) CAknTimeOrDateSettingItem(aIdentifier,CAknTimeOrDateSettingItem::EDate,iAlrmDate);
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
		/*if(ListBox()->CurrentItemIndex() == 2)// message
		{
			StoreSettingsL();
			
			iCallBack.GetMsgNowL();
			
			LoadSettingsL();
			ListBox()->DrawNow();
		}
		else // */
		{		
			CAknSettingItemList::HandleListBoxEventL(aListBox,aEventType);
		}
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
CProfileSettings::CProfileSettings(CEikButtonGroupContainer* aCba)
:iCba(aCba),iIndex(-1)
{	
}

CProfileSettings::~CProfileSettings()
{	
	delete iSMSContainer;
	delete iContacsContainer;
	delete iMessagesContainer;
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
void CProfileSettings::SetDataL(CMsgSched *aData)
{
	iIndex = -1;

	if(iListBox && aData)
	{
		if(aData->iName)
			iListBox->iSendName.Copy(*aData->iName);
		else
			iListBox->iSendName.Zero();
		
		if(aData->iNunmber)
			iListBox->iSendNumber.Copy(*aData->iNunmber);
		else
			iListBox->iSendNumber.Zero();
		
		if(aData->iMessage)
			iListBox->iSendMessage.Copy(*aData->iMessage);
		else
			iListBox->iSendMessage.Zero();
		
		iListBox->iAlrmTime  = aData->iTime;
		iListBox->iAlrmDate  = aData->iTime;
		iIndex = aData->iIndex;
	
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
    iListBox = new (ELeave) CAknSettingsListListbox(*this);
	iListBox->ConstructFromResourceL(R_PROF_SETTING);
	
	iListBox->iAlrmTime.HomeTime();
	iListBox->iAlrmDate.HomeTime();

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
	
	if(iMessagesContainer)
	{
		iMessagesContainer->SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
	}
	
	if(iContacsContainer)
	{
		iContacsContainer->SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
	}

	if(iSMSContainer)
	{
		iSMSContainer->SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CProfileSettings::GetMsgNowL(void)
{
	delete iSMSContainer;
	iSMSContainer = NULL;
	iSMSContainer = new(ELeave)CSMSContainer();
    iSMSContainer->ConstructL();
    
    if(iListBox){
		iSMSContainer->AddTextToEditorL(iListBox->iSendMessage);
    }
    
    SetMenuL();
    DrawNow();
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
TKeyResponse CProfileSettings::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
	TKeyResponse Ret = EKeyWasNotConsumed;
	

	if(iMessagesContainer)
	{
		Ret = iMessagesContainer->OfferKeyEventL(aKeyEvent,aType);
	}
	else if(iContacsContainer)
	{
		Ret = iContacsContainer->OfferKeyEventL(aKeyEvent,aType);
	}
	else if(iSMSContainer)
	{
		Ret = iSMSContainer->OfferKeyEventL(aKeyEvent,aType);
	}
	else if(iListBox)
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
	if(iMessagesContainer)
		return 1;
	else if(iContacsContainer)
		return 1;
	else if(iSMSContainer)
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
CCoeControl* CProfileSettings::ComponentControl( TInt aIndex) const
{
	if(iMessagesContainer)
		return iMessagesContainer;
	else if(iContacsContainer)
		return iContacsContainer;
	else if(iSMSContainer)
		return iSMSContainer;
	else 
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
void CProfileSettings::GetValuesL(CMsgSched *aData)
{
	if(iListBox && aData)
	{
		iListBox->StoreSettingsL();
		
		if(iListBox->iSendName.Length())
			aData->iName	= iListBox->iSendName.AllocL();
		
		if(iListBox->iSendNumber.Length())
			aData->iNunmber	= iListBox->iSendNumber.AllocL();
		
		if(iListBox->iSendMessage.Length())
			aData->iMessage	= iListBox->iSendMessage.AllocL();
		
		//TInt Msg  = iListBox->iSendMessage.Length();
		
		aData->iTime = GetAlarmTimeL(iListBox->iAlrmTime, iListBox->iAlrmDate);
		
		aData->iIndex	= iIndex;
	}
}

/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
TTime CProfileSettings::GetAlarmTimeL(TTime aTime, TTime aDate)
{	
	TDateTime NewAlarm(aDate.DateTime().Year(),aDate.DateTime().Month(),aDate.DateTime().Day(),aTime.DateTime().Hour(),aTime.DateTime().Minute(), aTime.DateTime().Second(),0);
    
	TTime RetAlarm(NewAlarm);

    return RetAlarm;
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CProfileSettings::ShowNoteL(const TDesC& aMessage)
{
	CAknGlobalNote* dialog = CAknGlobalNote::NewLC();
	TInt NoteId = dialog->ShowNoteL(EAknGlobalInformationNote,aMessage);
	User::After(2000000);
	dialog->CancelNoteL(NoteId);
	CleanupStack::PopAndDestroy(dialog);;
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/

void CProfileSettings::ContactClikkedL(void)
{
	HandleViewCommandL(EContDone);
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CProfileSettings::HandleViewCommandL(TInt aCommand)
{
	switch(aCommand)
	{
	case EMsgStoreOk:
		if(iMessagesContainer && iListBox)
		{
			iMessagesContainer->GetMessageL(iListBox->iSendMessage);
			iListBox->LoadSettingsL();
			
		}
	case EMsgStoreCancel:
		{
			delete iMessagesContainer;
			iMessagesContainer = NULL;
			
			if(iListBox)
			{
				iListBox->DrawNow();
			}
		}
		SetMenuL();
		DrawNow();
		break;
	case EMsgStore:
		{
			iMessagesContainer = new(ELeave)CMessagesContainer(iCba);
			iMessagesContainer->ConstructL();
		}
		SetMenuL();
		DrawNow();
		break;
	case EContDone:
		if(iContacsContainer && iListBox)
		{
			iContacsContainer->GetSelectedNumberL(iListBox->iSendName,iListBox->iSendNumber);
			iListBox->LoadSettingsL();
		}
	case EContCancel:
		{
			delete iContacsContainer;
			iContacsContainer = NULL;
			
			if(iListBox)
			{
				iListBox->DrawNow();
			}
		}
		SetMenuL();
		DrawNow();
		break;
	case ESenderContact:
		{
			RPointerArray<CMyCItem> ContArray;
			STATIC_CAST(CMgAppUi*,CEikonEnv::Static()->EikAppUi())->GetContactsArrayL(ContArray);
				
			delete iContacsContainer;
			iContacsContainer = NULL;
			iContacsContainer = new(ELeave)CContacsContainer(ContArray,*this);
			iContacsContainer->ConstructL();		
		}
		SetMenuL();
		DrawNow();
		break;
	case EMsgDone:
		if(iSMSContainer && iListBox)
		{
			HBufC* msg = iSMSContainer->GetSMSMessageL();
			if(msg)
			{
				CleanupStack::PushL(msg);
				
				iListBox->iSendMessage.Copy(*msg);
				iListBox->LoadSettingsL();
				iListBox->DrawNow();
				
				CleanupStack::PopAndDestroy(msg);
			}
		}
	case EMsgCancel:
		{
			delete iSMSContainer;
			iSMSContainer = NULL;
		}
		SetMenuL();
		DrawNow();
		break;
	default:
		if(iMessagesContainer)
		{
			iMessagesContainer->HandleViewCommandL(aCommand);
		}
		break;
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/

void CProfileSettings::SetMenuL(void)
{	
	if(iMessagesContainer)
	{
		iMessagesContainer->SetMenuL();
	}
	else
	{
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->StopDisplayingMenuBar();
	
		TInt MenuRes(R_MAIN_MENUBAR);
		TInt ButtomRes(R_MAIN_CBA);
	
		if(iMessagesContainer)
		{
			ButtomRes = R_MSGS_CBA;
			MenuRes = R_MSGS_MENUBAR;
		}
		else if(iContacsContainer)
		{
			ButtomRes = R_CONTTT_CBA;
		}
		else if(iSMSContainer)
		{
			ButtomRes = R_MYMSG_CBA;
		}
		else
		{
			MenuRes = R_SETT_MENUBAR;
			ButtomRes = R_SETT_CBA;
		}
		
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
void CProfileSettings::InitMenuPanelL(TInt aResourceId, CEikMenuPane* aMenuPane)
{
	if(iMessagesContainer)
	{
		iMessagesContainer->InitMenuPanelL(aResourceId,aMenuPane);
	}
}
