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
#include <EIKFUTIL.H>
#include <mgfetch.h> 

#include "Default_settings.h"                     // own definitions
#include "Common.h"

#include "YApps_E8876015.hrh"                   // own resource header
#include "YApps_E8876015.h"
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

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/

CAknSettingItem* CDefaultSettingsListListbox ::CreateSettingItemL(TInt aIdentifier)
{
	CAknSettingItem* settingItem = NULL;

	switch(aIdentifier)
	{	
	case EOperSel:
		settingItem = new (ELeave) CAknTextSettingItem(aIdentifier,iOperator);
		break;	
	case ESundFile:
		settingItem = new (ELeave) CAknTextSettingItem(aIdentifier,iSoundFile);
		break;				
	case ESnozzeTimme:  
		settingItem = new (ELeave) CAknTextSettingItem(aIdentifier,iSnoozeTimeText);
        break;	
	case EMaxTimme:  
		settingItem = new (ELeave) CAknTextSettingItem(aIdentifier,iMaxTimeText);
        break;	 
	case ESnozLabel:
		settingItem = new (ELeave) CAknTextSettingItem(aIdentifier,iSnoozeLabel);
		break;	        
	}	
	
	return settingItem;
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CDefaultSettingsListListbox::HandleListBoxEventL(CEikListBox* aListBox,TListBoxEvent aEventType)
{
	if(ListBox())
	{
		TFileName tmpBufff;
		if (aEventType == EEventEnterKeyPressed || aEventType == EEventItemDoubleClicked)
		{
			if(ListBox()->CurrentItemIndex() == 3)// iMaxTime
			{
				StoreSettingsL();
					
				StringLoader::Load(tmpBufff,R_SH_STR_MAXTIME);	
						
				CAknNumberQueryDialog* Dialog = CAknNumberQueryDialog::NewL(iMaxTime,CAknQueryDialog::ENoTone);
				Dialog->PrepareLC(R_BIG_DIALOG);
				Dialog->SetPromptL(tmpBufff);
				if(Dialog->RunLD())
				{
					iMaxTimeText.Num(iMaxTime);
				}
						
				LoadSettingsL();
				ListBox()->DrawNow();
			}
			else if(ListBox()->CurrentItemIndex() == 2)// liSnoozeTime
			{
				StoreSettingsL();
			
				StringLoader::Load(tmpBufff,R_SH_STR_SNOOZETIME);	
				
				CAknNumberQueryDialog* Dialog = CAknNumberQueryDialog::NewL(iSnoozeTime,CAknQueryDialog::ENoTone);
				Dialog->PrepareLC(R_BIG_DIALOG);
				Dialog->SetPromptL(tmpBufff);
				if(Dialog->RunLD())
				{
					iSnoozeTimeText.Num(iSnoozeTime);
				}
				
				LoadSettingsL();
				ListBox()->DrawNow();
			}
			else if(ListBox()->CurrentItemIndex() == 1)// iSoundFile
			{
				StoreSettingsL();
				
				CDesCArrayFlat* SelectedFiles = new(ELeave)CDesCArrayFlat(1);
				CleanupStack::PushL(SelectedFiles);
							
				if(MGFetch::RunL(*SelectedFiles,EAudioFile, EFalse)) 
				{
					if(SelectedFiles->Count())
					{		
						tmpBufff.Copy(SelectedFiles->MdcaPoint(0));
						if(EikFileUtils::Parse(tmpBufff) == KErrNone){
										
							iFullFileName.Copy(tmpBufff);
							TParsePtrC Hjepl(iFullFileName);
							iSoundFile.Copy(Hjepl.NameAndExt());
						}
					}
				}
				CleanupStack::PopAndDestroy(SelectedFiles);
				
				LoadSettingsL();
				ListBox()->DrawNow();
			}
			else // iOperator, iSnoozeLabel
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
void CDefaultSettingsListListbox::SizeChanged()
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
CDefaultSettings::CDefaultSettings(CEikButtonGroupContainer* aCba)
:iCba(aCba)
{		

}

	/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CDefaultSettings::~CDefaultSettings()
{  
	delete iMyHelpContainer;
	iMyHelpContainer = NULL;

	delete iListBox;
	iListBox = NULL; 	
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CDefaultSettings::ConstructL(void)
{
	CreateWindowL();

	SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());

	MakeListboxL(-1);
	SetMenuL();
	ActivateL();
	
	DrawNow();
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CDefaultSettings::MakeListboxL(TInt aSelected)
{			
    delete iListBox;
    iListBox = NULL;
    iListBox = new (ELeave) CDefaultSettingsListListbox();
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
void CDefaultSettings::SizeChanged()
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
void CDefaultSettings::HandleResourceChange(TInt aType)
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
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CDefaultSettings::Draw(const TRect& /*aRect*/) const
{
	CWindowGc& gc = SystemGc();

	gc.Clear(Rect());
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
TKeyResponse CDefaultSettings::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aEventCode)
{
	TKeyResponse Ret = EKeyWasNotConsumed;	
	
	if(iMyHelpContainer)
	{
		Ret = iMyHelpContainer->OfferKeyEventL(aKeyEvent,aEventCode);
	}
	else if(iListBox)
	{
		Ret = iListBox->OfferKeyEventL(aKeyEvent,aEventCode);
	}

	return Ret;
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CDefaultSettings::HandleViewCommandL(TInt aCommand)
{
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
			iMyHelpContainer = CMyHelpContainer::NewL(3);
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
CCoeControl* CDefaultSettings::ComponentControl( TInt /*aIndex*/) const
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
TInt CDefaultSettings::CountComponentControls() const
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
void CDefaultSettings::SetMenuL(void)
{
	CEikonEnv::Static()->AppUiFactory()->MenuBar()->StopDisplayingMenuBar();

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
		TInt ButtomRes(R_DEFAULT_CBA);
		TInt MenuRes(R_DEFAULT_MENUBAR);
		
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
void CDefaultSettings::InitMenuPanelL(TInt aResourceId, CEikMenuPane* aMenuPane)
{
	if(aResourceId == R_MAIN_MENU)
	{

	}
}


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CDefaultSettings::GetValuesFromStoreL(void)
{	
	if(!iListBox)
		return;
	
	{
		TFindFile AufFolder(CCoeEnv::Static()->FsSession());
		if(KErrNone == AufFolder.FindByDir(KtxDefSettingsFile, KNullDesC))
		{
			CDictionaryFileStore* MyDStore = CDictionaryFileStore::OpenLC(CCoeEnv::Static()->FsSession(),AufFolder.File(), TUid::Uid(0x102013AD));
				
			GetValuesL(MyDStore,0x0110,iListBox->iOperator);
			GetValuesL(MyDStore,0x1234,iListBox->iSnoozeLabel);

			TFileName hljBuffer;
			GetValuesL(MyDStore,0x1100,hljBuffer);
			
			if(EikFileUtils::Parse(hljBuffer) == KErrNone){
				
				iListBox->iFullFileName.Copy(hljBuffer);
			
				TParsePtrC Hjepl(iListBox->iFullFileName);
				iListBox->iSoundFile.Copy(Hjepl.NameAndExt());
			}
			
			TUint32 TmpValue(0);
			GetValuesL(MyDStore,0x0101,TmpValue);
			
			iListBox->iSnoozeTime = TmpValue;
			iListBox->iSnoozeTimeText.Num(iListBox->iSnoozeTime);
			
			TmpValue = 0;
			GetValuesL(MyDStore,0x0111,TmpValue);
						
			iListBox->iMaxTime = TmpValue;
			iListBox->iMaxTimeText.Num(iListBox->iMaxTime);
			
			CleanupStack::PopAndDestroy(1);// Store
		}
	}
}
		
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CDefaultSettings::GetValuesL(CDictionaryFileStore* aDStore,TInt aUID,TDes& aValue)
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
void CDefaultSettings::GetValuesL(CDictionaryFileStore* aDStore,TInt aUID,TUint32& aValue)
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
		aValue = 30;
}

/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
void CDefaultSettings::SaveValuesL(void)
{
	if(!iListBox)
		return;

	iListBox->StoreSettingsL();

	TFindFile AufFolder(CCoeEnv::Static()->FsSession());
	if(KErrNone == AufFolder.FindByDir(KtxDefSettingsFile, KNullDesC))
	{
		User::LeaveIfError(CCoeEnv::Static()->FsSession().Delete(AufFolder.File()));

		CDictionaryFileStore* MyDStore = CDictionaryFileStore::OpenLC(CCoeEnv::Static()->FsSession(),AufFolder.File(), TUid::Uid(0x102013AD));

		SaveValuesL(MyDStore,0x0110,iListBox->iOperator);
		SaveValuesL(MyDStore,0x1234,iListBox->iSnoozeLabel);
		
		TFileName hljBuffer;
		SaveValuesL(MyDStore,0x1100,iListBox->iFullFileName);

		TUint32 TmpValue(iListBox->iSnoozeTime);
		SaveValuesL(MyDStore,0x0101,TmpValue);

		TmpValue = iListBox->iMaxTime;
		GetValuesL(MyDStore,0x0111,TmpValue);
			
		MyDStore->CommitL();
		CleanupStack::PopAndDestroy(1);// Store
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CDefaultSettings::SaveValuesL(CDictionaryFileStore* aDStore,TInt aUID, const TDesC& aValue)
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
void CDefaultSettings::SaveValuesL(CDictionaryFileStore* aDStore,TInt aUID, TUint32 aValue)
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

