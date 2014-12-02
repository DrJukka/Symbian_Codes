/* Copyright (c) 2001 - 2008, Dr. Jukka Silvennoinen. All rights reserved */

#include "Main_Container.h"                     // own definitions

#include <CPbkContactEngine.h>
#include <CPbkSingleEntryFetchDlg.h>
#include <rpbkviewresourcefile.h>
#include <CNTFLDST.H>
#include <AknGlobalNote.h>
#include <StringLoader.h>

#include "Profile_Settings.h"                     // own definitions
#include "YApps_E8876015.hrh"                   // own resource header
#include "YApps_E8876015.h"
#include "MsgDataBase.h" 

#include <AknQueryDialog.h>

#include <eikappui.h> 
#include <bautils.h>
#include <AknLists.h>
#include <AknPopup.h>

#include "Help_Container.h"

#include "Common.h"
#ifdef SONE_VERSION
	#include <YApps_2002B0B1.rsg>
#else
	#ifdef LAL_VERSION
		#include <YApps_E8876015.rsg>
	#else
		#include <YApps_E8876015.rsg>
	#endif
#endif

// Settings
//,
	
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CAknSettingsListListbox::CAknSettingsListListbox(CProfileSettings* aSettings):iSettings(aSettings)
{	
}


CAknSettingItem* CAknSettingsListListbox ::CreateSettingItemL(TInt aIdentifier)
{
	CAknSettingItem* settingItem = NULL;

	switch(aIdentifier)
	{
	case ESenderNumber:
		settingItem = new (ELeave) CAknTextSettingItem(aIdentifier,iSendNumber);
		break;
	case ERepeatType:
		settingItem = new (ELeave) CAknTextSettingItem(aIdentifier,iRepeat);
		break;		
	case ESendTime:
		settingItem = new (ELeave) CAknTimeOrDateSettingItem(aIdentifier,CAknTimeOrDateSettingItem::ETime,iAlrmTime);
		break;
	case ESendDate:
		settingItem = new (ELeave) CAknTimeOrDateSettingItem(aIdentifier,CAknTimeOrDateSettingItem::EDate,iAlrmDate);
		break;
	case ECallDelay:
		settingItem = new (ELeave) CAknTextSettingItem(aIdentifier,iDelayBuf);
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
			TBuf<60> tmpBufff;
			
			if(ListBox()->CurrentItemIndex() == 0)
			{
				iSettings->HandleViewCommandL(ESenderContact);
			}
			else if(ListBox()->CurrentItemIndex() == 1)// ERepeatType
			{
				StoreSettingsL();
				
				if(iSchedule){				
					SelectRepeatL(iRepeat,iRepeatId);
				}else{
					StringLoader::Load(tmpBufff,R_SH_STR_DELAY);	
					CAknNumberQueryDialog* Dialog = CAknNumberQueryDialog::NewL(iDelay,CAknQueryDialog::ENoTone);
					Dialog->PrepareLC(R_BIG_DIALOG);
					Dialog->SetPromptL(tmpBufff);
					if(Dialog->RunLD())
					{
						iDelayBuf.Num(iDelay);
					}
				}
				LoadSettingsL();
				ListBox()->DrawNow();
			}
			else 
			{		
				CAknSettingItemList::HandleListBoxEventL(aListBox,aEventType);
			}
		}
		else 
		{		
			CAknSettingItemList::HandleListBoxEventL(aListBox,aEventType);
		}
	}
}


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CAknSettingsListListbox::SelectRepeatL(TDes& aSetOn, TInt & aTypeId)
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
	for(TInt i=0; i < 11 ; i++)
	{
		STATIC_CAST(CMgAppUi*,CEikonEnv::Static()->EikAppUi())->GetRepeatTypeName(i,Hjelpper);
		Array->AppendL(Hjelpper);
	}

	list->Model()->SetItemTextArray(Array);
	CleanupStack::Pop();//Array
	list->Model()->SetOwnershipType(ELbmOwnsItemArray);

	
	Hjelpper.Zero();
	StringLoader::Load(Hjelpper,R_SH_STR_REPEATS);
	
	popupList->SetTitleL(Hjelpper);
	if (popupList->ExecuteLD())
    {
    	TInt Selected = list->CurrentItemIndex();
		if(Selected > 0)
		{
			aTypeId = Selected;
			aSetOn.Copy(list->Model()->ItemText(Selected));
		}
		else if(iDateFormatString)
		{
			Hjelpper.Zero();
			StringLoader::Load(Hjelpper,R_SH_STR_SNDDATE);
			
			CAknTimeQueryDialog* Dialog = CAknTimeQueryDialog::NewL(iAlrmDate,CAknQueryDialog::ENoTone);
			Dialog->PrepareLC(R_MY_DATE_QUERY);
			Dialog->SetPromptL(Hjelpper);
			if(Dialog->RunLD())
			{
				aTypeId = 0;
				TRAPD(ErrNumm, iAlrmDate.FormatL(aSetOn, *iDateFormatString));
			}
		}
	}

	CleanupStack::Pop();            // popuplist
	CleanupStack::PopAndDestroy(1); // list
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
CProfileSettings::CProfileSettings(CEikButtonGroupContainer* aCba,const TBool& aSchedule)
:iCba(aCba),iIndex(-1),iSchedule(aSchedule)
{	
}

CProfileSettings::~CProfileSettings()
{
	delete iMyHelpContainer;
	iMyHelpContainer = NULL;
	
	delete iContacsContainer;
	delete iListBox;
	delete iDateFormatString;
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
		if(aData->iNunmber)
			iListBox->iSendNumber.Copy(*aData->iNunmber);
		else
			iListBox->iSendNumber.Zero();

		if(aData->iName)
			iListBox->iSenderName.Copy(*aData->iName);
		else
			iListBox->iSenderName.Zero();
				
		iListBox->iEnabled	= aData->iEnabled;
		iListBox->iRepeatId = aData->iRepeat;
		
		iListBox->iAlrmTime  = aData->iTime;
		iListBox->iAlrmDate  = aData->iTime;
		iIndex = aData->iIndex;
	
		if(iListBox->iRepeatId != 0)
		{
			STATIC_CAST(CMgAppUi*,CEikonEnv::Static()->EikAppUi())->GetRepeatTypeName(iListBox->iRepeatId,iListBox->iRepeat);
		}
		else if(iDateFormatString)
		{
			TRAPD(ErrNumm, iListBox->iAlrmDate.FormatL(iListBox->iRepeat, *iDateFormatString));
		}
		
		iListBox->LoadSettingsL();
    	iListBox->DrawNow();
	}
}
/*
-----------------------------------------------------------------------------			
-----------------------------------------------------------------------------
*/
void CProfileSettings::SetDataL(CKeyCapcap *aData)
{
	if(iListBox && aData)
	{
		if(aData->iNunmber)
			iListBox->iSendNumber.Copy(*aData->iNunmber);
		else
			iListBox->iSendNumber.Zero();

		if(aData->iName)
			iListBox->iSenderName.Copy(*aData->iName);
		else
			iListBox->iSenderName.Zero();
		
		iListBox->iDelay	= aData->iDelay;
		iListBox->iDelayBuf.Num(iListBox->iDelay);
		
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
    iListBox = new (ELeave) CAknSettingsListListbox(this);
    
    if(iSchedule){
		iListBox->ConstructFromResourceL(R_PROF_SETTING);
    }else{
		iListBox->ConstructFromResourceL(R_PROF_SETTING2);
    }
    
    iListBox->iSchedule = iSchedule;
    
	iListBox->SetIncludeHiddenInOrdinal(EFalse);
	
	delete iDateFormatString;
	iDateFormatString = NULL;
	iDateFormatString = CEikonEnv::Static()->AllocReadResourceL(R_QTN_DATE_USUAL);
	iListBox->iDateFormatString = iDateFormatString;
	
	iListBox->iAlrmTime.HomeTime();
	iListBox->iAlrmDate.HomeTime();
	iListBox->iEnabled = ETrue;
	iListBox->iRepeatId = 0;
	iListBox->iDelay = 0;
	iListBox->iDelayBuf.Num(iListBox->iDelay);	
	
	if(iDateFormatString)
	{
		TRAPD(ErrNumm, iListBox->iAlrmDate.FormatL(iListBox->iRepeat, *iDateFormatString));
	}
	
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
	
	if(iContacsContainer)
	{
		iContacsContainer->SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
	}
	
	if(iMyHelpContainer)
	{
		iMyHelpContainer->SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
	}	
}


/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
TKeyResponse CProfileSettings::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
	TKeyResponse Ret = EKeyWasNotConsumed;
	

	if(iMyHelpContainer)
	{
		Ret = iMyHelpContainer->OfferKeyEventL(aKeyEvent,aType);
	}
	else if(iContacsContainer)
	{
		Ret = iContacsContainer->OfferKeyEventL(aKeyEvent,aType);
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
	if(iMyHelpContainer)
		return 1;
	else if(iContacsContainer)
		return iContacsContainer->CountComponentControls ();
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
	if(iMyHelpContainer)
		return iMyHelpContainer;		
	else if(iContacsContainer)
		return iContacsContainer->ComponentControl (aIndex);
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
void CProfileSettings::GetValuesL(CKeyCapcap *aData)
{
	if(iListBox && aData)
	{
		delete aData->iName;
		aData->iName = NULL;
		
		delete aData->iNunmber;
		aData->iNunmber = NULL;
		
		iListBox->StoreSettingsL();
			
		if(iListBox->iSendNumber.Length())
			aData->iNunmber	= iListBox->iSendNumber.AllocL();

		if(iListBox->iSenderName.Length())
			aData->iName	= iListBox->iSenderName.AllocL();

		aData->iDelay = iListBox->iDelay;		
	}
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
		
		if(iListBox->iSendNumber.Length())
			aData->iNunmber	= iListBox->iSendNumber.AllocL();

		if(iListBox->iSenderName.Length())
			aData->iName	= iListBox->iSenderName.AllocL();
		
		//TInt Msg  = iListBox->iSendName.Length();
		
		aData->iEnabled	= iListBox->iEnabled;
		aData->iRepeat 	= iListBox->iRepeatId;
		aData->iTime 	= GetAlarmTimeL(iListBox->iAlrmTime, iListBox->iAlrmDate);
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
void CProfileSettings::HandleViewCommandL(TInt aCommand)
{
	TBuf<100> dummy;

	switch(aCommand)
	{
	case EAppHelpBack2:
		{
			delete iMyHelpContainer;
			iMyHelpContainer = NULL;
		}
		SetMenuL();
		DrawNow();
		break;
	case EAppHelp2:
		{
			delete iMyHelpContainer;
			iMyHelpContainer = NULL;    
			
			if(iSchedule)
				iMyHelpContainer = CMyHelpContainer::NewL(1);
			else
				iMyHelpContainer = CMyHelpContainer::NewL(2);
		}
		SetMenuL();
		DrawNow();    		
		break;
	case EContDone:
		if(iContacsContainer && iListBox)
		{		
			iContacsContainer->GetSelectedNumberL(iListBox->iSenderName,iListBox->iSendNumber);
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
	case ENewNumber:
		if(iListBox)
		{
			StringLoader::Load(dummy,R_STR_NEWCONTACT);
			TBuf<100> Hjelpper(iListBox->iSendNumber);
			CAknTextQueryDialog* Dialog = CAknTextQueryDialog::NewL(Hjelpper,CAknQueryDialog::ENoTone);
			Dialog->PrepareLC(R_ASK_NAME_DIALOG);
			Dialog->SetPromptL(dummy);
			if(Dialog->RunLD())
			{
				iListBox->iSendNumber.Copy(Hjelpper);
				iListBox->LoadSettingsL();
			}
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

void CProfileSettings::ContactClikkedL(void)
{
	HandleViewCommandL(EContDone);
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/

void CProfileSettings::SetMenuL(void)
{	
	if(iMyHelpContainer)
	{
		if(iCba)
		{
			iCba->SetCommandSetL(R_APPHELP_CBA2);
			iCba->DrawDeferred();
		}
	}
	else 
	{
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->StopDisplayingMenuBar();
	
		TInt MenuRes(R_MAIN_MENUBAR);
		TInt ButtomRes(R_MAIN_CBA);
	
		if(iContacsContainer)
		{
			ButtomRes = R_CONTTT_CBA;
		}
		else if(iSchedule)
		{
			MenuRes = R_SETT_MENUBAR;
			ButtomRes = R_SETT_CBA;
		}
		else
		{
			MenuRes = R_SETT_MENUBAR;
			ButtomRes = R_SETT_CBA2;
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

}
