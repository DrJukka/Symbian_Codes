/* Copyright (c) 2001 - 2006 , J.P. Silvennoinen. All rights reserved */

#include "CommandSC_Settings.h"            // own definitions


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

#include <eikclbd.h>
#include <apgcli.h>
#include <APMREC.H>
#include <hal.h> 
#include <aknmfnesettingpage.h> 
#include <aknmfnesettingpage.h>
#include <AknQueryDialog.h>
#include <AknSettingItemList.h>
#include <AknQueryDialog.h>
#include <BAUTILS.H>
#include <eikappui.h> 
#include <eikapp.h>
#include <s32file.h>
#include <aknlists.h> 
#include <AknIconArray.h>
#include "YuccaBrowser_Container.h"
#include <stringloader.h> 

CAknSettingsListListbox ::CAknSettingsListListbox(CGeneralSettings& aGeneralSettings)
:iGeneralSettings(aGeneralSettings)
{
	
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
CAknSettingsListListbox::~CAknSettingsListListbox()
{		

}				

// Settings
//
CAknSettingItem* CAknSettingsListListbox ::CreateSettingItemL(TInt aIdentifier)
{
	CAknSettingItem* settingItem = NULL;

		
	
	switch(aIdentifier)
	{
    case ECommandSCTE1:
        settingItem = new (ELeave) CAknTextSettingItem(aIdentifier,iCommandTxtTE1);
        break;
    case ECommandSCTE2:
        settingItem = new (ELeave) CAknTextSettingItem(aIdentifier,iCommandTxtTE2);
        break;
    case ECommandSCTE3:
        settingItem = new (ELeave) CAknTextSettingItem(aIdentifier,iCommandTxtTE3);
        break;
	case ECommandSC0:
    	settingItem = new (ELeave) CAknTextSettingItem(aIdentifier,iCommandTxt0);
		break;
	case ECommandSC1:
    	settingItem = new (ELeave) CAknTextSettingItem(aIdentifier,iCommandTxt1);
		break;
	case ECommandSC2:
    	settingItem = new (ELeave) CAknTextSettingItem(aIdentifier,iCommandTxt2);
		break;
	case ECommandSC3:
    	settingItem = new (ELeave) CAknTextSettingItem(aIdentifier,iCommandTxt3);
		break;
	case ECommandSC4:
    	settingItem = new (ELeave) CAknTextSettingItem(aIdentifier,iCommandTxt4);
		break;
	case ECommandSC5:
    	settingItem = new (ELeave) CAknTextSettingItem(aIdentifier,iCommandTxt5);
		break;
	case ECommandSC6:
    	settingItem = new (ELeave) CAknTextSettingItem(aIdentifier,iCommandTxt6);
		break;
	case ECommandSC7:
    	settingItem = new (ELeave) CAknTextSettingItem(aIdentifier,iCommandTxt7);
		break;
	case ECommandSC8:
    	settingItem = new (ELeave) CAknTextSettingItem(aIdentifier,iCommandTxt8);
		break;
	case ECommandSC9:
    	settingItem = new (ELeave) CAknTextSettingItem(aIdentifier,iCommandTxt9);
		break;
	}	
	
	return settingItem;
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
----------------------------------------------------------------------------
*/
void CAknSettingsListListbox::HandleListBoxEventL(CEikListBox* aListBox,TListBoxEvent aEventType)
{
	if(ListBox())
	{		
		if (aEventType == EEventEnterKeyPressed || aEventType == EEventItemDoubleClicked)
		{
			if(ListBox()->CurrentItemIndex() >= 0)
			{
				 SelectCommandSCL(ListBox()->CurrentItemIndex());
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
----------------------------------------------------------------------------
*/
void CAknSettingsListListbox::SelectCommandSCL(TInt aSC)
{
    CAknSinglePopupMenuStyleListBox * list = new(ELeave) CAknSinglePopupMenuStyleListBox;
	CleanupStack::PushL(list);

    CAknPopupList* popupList = CAknPopupList::NewL(list, R_AVKON_SOFTKEYS_SELECT_BACK);  
    CleanupStack::PushL(popupList);

    list->ConstructL(popupList, CEikListBox::ELeftDownInViewRect);
    list->CreateScrollBarFrameL(ETrue);
    list->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,CEikScrollBarFrame::EAuto);
    
    
	CDesCArrayFlat* SelectArrray = new(ELeave)CDesCArrayFlat(20);
	CleanupStack::PushL(SelectArrray);
	
	TBuf<60> Hjelpper;
	
	iGeneralSettings.GetCommandText(Hjelpper,EEditCopy); 
	SelectArrray->AppendL(Hjelpper);
                
	iGeneralSettings.GetCommandText(Hjelpper,EEditCut); 
	SelectArrray->AppendL(Hjelpper);
	                
	iGeneralSettings.GetCommandText(Hjelpper,EEditPaste); 
	SelectArrray->AppendL(Hjelpper);

    iGeneralSettings.GetCommandText(Hjelpper,EFileDelete); 
	SelectArrray->AppendL(Hjelpper);	                           

	iGeneralSettings.GetCommandText(Hjelpper,EFileRename); 
	SelectArrray->AppendL(Hjelpper);	            

    iGeneralSettings.GetCommandText(Hjelpper,EFileDetails); 
	SelectArrray->AppendL(Hjelpper);	            
	
	iGeneralSettings.GetCommandText(Hjelpper,EFileAttributes); 
	SelectArrray->AppendL(Hjelpper);
	            
	iGeneralSettings.GetCommandText(Hjelpper,EFileShowName); 
	SelectArrray->AppendL(Hjelpper);

    iGeneralSettings.GetCommandText(Hjelpper,ESendFiles); 
	SelectArrray->AppendL(Hjelpper);

    iGeneralSettings.GetCommandText(Hjelpper,EFileNewFile); 
	SelectArrray->AppendL(Hjelpper);
	
    iGeneralSettings.GetCommandText(Hjelpper,EFileNewFolder); 
	SelectArrray->AppendL(Hjelpper);
	
    iGeneralSettings.GetCommandText(Hjelpper,ESettings); 
	SelectArrray->AppendL(Hjelpper);
	
    iGeneralSettings.GetCommandText(Hjelpper,EExtrasShowShortCut); 
	SelectArrray->AppendL(Hjelpper);
	
    iGeneralSettings.GetCommandText(Hjelpper,EExtrasSetShortCut); 
	SelectArrray->AppendL(Hjelpper);
	
    iGeneralSettings.GetCommandText(Hjelpper,EExtrasSearchOn); 
	SelectArrray->AppendL(Hjelpper);
	
    iGeneralSettings.GetCommandText(Hjelpper,EExtrasRefresh); 
	SelectArrray->AppendL(Hjelpper);
	
	iGeneralSettings.GetCommandText(Hjelpper,EFolderViewMarkTogle); 
	SelectArrray->AppendL(Hjelpper);
	
    iGeneralSettings.GetCommandText(Hjelpper,EFolderViewMarkAll); 
	SelectArrray->AppendL(Hjelpper);
	
    iGeneralSettings.GetCommandText(Hjelpper,EFolderViewUnMarkAll); 
	SelectArrray->AppendL(Hjelpper);
	
    iGeneralSettings.GetCommandText(Hjelpper,EVBOpenFile); 
	SelectArrray->AppendL(Hjelpper);
	
    iGeneralSettings.GetCommandText(Hjelpper,EOpenWithViewer); 
	SelectArrray->AppendL(Hjelpper);
	
    iGeneralSettings.GetCommandText(Hjelpper,EEditShowPaste); 
	SelectArrray->AppendL(Hjelpper);
	
    iGeneralSettings.GetCommandText(Hjelpper,EEditClearPaste); 
	SelectArrray->AppendL(Hjelpper);
	
    iGeneralSettings.GetCommandText(Hjelpper,ESearchFiles); 
	SelectArrray->AppendL(Hjelpper);
	
    iGeneralSettings.GetCommandText(Hjelpper,EMovePageUp); 
	SelectArrray->AppendL(Hjelpper);
	
    iGeneralSettings.GetCommandText(Hjelpper,EMovePageDown); 
	SelectArrray->AppendL(Hjelpper);

    iGeneralSettings.GetCommandText(Hjelpper,EBackOneLevel); 
    SelectArrray->AppendL(Hjelpper);	

    iGeneralSettings.GetCommandText(Hjelpper,EChangeExtraInfo); 
    SelectArrray->AppendL(Hjelpper);    
    
	CleanupStack::Pop();//SelectArrray
    list->Model()->SetItemTextArray(SelectArrray);
    list->Model()->SetOwnershipType(ELbmOwnsItemArray);

	StringLoader::Load(Hjelpper,R_MNU_SELCOMMAND);
	popupList->SetTitleL(Hjelpper);
	if (popupList->ExecuteLD())
    {
		TInt Selected = list->CurrentItemIndex();
		
		TInt SelCommand(-1);
		TBuf<30> SelComBuf;
		switch(Selected)
		{
		case 0:
			SelCommand = EEditCopy;
			break;
		case 1:
			SelCommand = EEditCut;					
			break;
		case 2:
			SelCommand = EEditPaste;					
			break;
		case 3:
			SelCommand = EFileDelete;				
			break;
		case 4:
			SelCommand = EFileRename;				
			break;
		case 5:
			SelCommand = EFileDetails;				
			break;
		case 6:
			SelCommand = EFileAttributes;			
			break;
		case 7:
			SelCommand = EFileShowName;				
			break;
		case 8:
			SelCommand = ESendFiles;					
			break;
		case 9:
			SelCommand = EFileNewFile;				
			break;
		case 10:
			SelCommand = EFileNewFolder;				
			break;
		case 11:
			SelCommand = ESettings;					
			break;
		case 12:
			SelCommand = EExtrasShowShortCut;		
			break;
		case 13:
			SelCommand = EExtrasSetShortCut;			
			break;
		case 14:
			SelCommand = EExtrasSearchOn;			
			break;
		case 15:
			SelCommand = EExtrasRefresh;				
			break;
        case 16:
            SelCommand = EFolderViewMarkTogle;            
            break;			
		case 17:
			SelCommand = EFolderViewMarkAll;			
			break;
		case 18:
			SelCommand = EFolderViewUnMarkAll;		
			break;
		case 19:
			SelCommand = EVBOpenFile;				
			break;
		case 20:
			SelCommand = EOpenWithViewer;				
			break;
		case 21:
			SelCommand = EEditShowPaste;				
			break;	
		case 22:
			SelCommand = EEditClearPaste;				
			break;
		case 23:
			SelCommand = ESearchFiles;				
			break;
		case 24:
			SelCommand = EMovePageUp;				
			break;
		case 25:
			SelCommand = EMovePageDown;				
			break;	
        case 26:
            SelCommand = EBackOneLevel;             
            break;			
        case 27:
            SelCommand = EChangeExtraInfo;             
            break;			
		default:
			break;
		}
		
		iGeneralSettings.GetCommandText(SelComBuf,SelCommand);
		
		if(SelCommand > 0)
		{
			switch(aSC)
			{
            case 0:
                iCommandTE1 = SelCommand;
                iCommandTxtTE1.Copy(SelComBuf);
                break;
            case 1:
                iCommandTE2 = SelCommand;
                iCommandTxtTE2.Copy(SelComBuf);
                break;
            case 2:
                iCommandTE3 = SelCommand;
                iCommandTxtTE3.Copy(SelComBuf);
                break;			    
            case 3:
				iCommand0 = SelCommand;
				iCommandTxt0.Copy(SelComBuf);
				break;
			case 4:
				iCommand1 = SelCommand;
				iCommandTxt1.Copy(SelComBuf);
				break;
			case 5:
				iCommand2 = SelCommand;
				iCommandTxt2.Copy(SelComBuf);
				break;
			case 6:
				iCommand3 = SelCommand;
				iCommandTxt3.Copy(SelComBuf);
				break;
			case 7:
				iCommand4 = SelCommand;
				iCommandTxt4.Copy(SelComBuf);
				break;
			case 8:
				iCommand5 = SelCommand;
				iCommandTxt5.Copy(SelComBuf);
				break;
			case 9:
				iCommand6 = SelCommand;
				iCommandTxt6.Copy(SelComBuf);
				break;
			case 10:
				iCommand7 = SelCommand;
				iCommandTxt7.Copy(SelComBuf);
				break;
			case 11:
				iCommand8 = SelCommand;
				iCommandTxt8.Copy(SelComBuf);
				break;
			case 12:
				iCommand9 = SelCommand;
				iCommandTxt9.Copy(SelComBuf);
				break;
			default:
				break;
			}
		}

		LoadSettingsL();
		ListBox()->DrawNow();
	}
    
  	CleanupStack::Pop();             // popuplist
	CleanupStack::PopAndDestroy(1);  // list
}

			
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CCommandSCSettings::CCommandSCSettings(CGeneralSettings& aGeneralSettings)
:iListBox(NULL),iGeneralSettings(aGeneralSettings)
{	
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
CCommandSCSettings::~CCommandSCSettings()
{		
	delete iListBox;
	iListBox = NULL;
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CCommandSCSettings::ConstructL(void)
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
void CCommandSCSettings::MakeListboxL(void)
{			
    delete iListBox;
    iListBox = NULL;
    iListBox = new (ELeave) CAknSettingsListListbox(iGeneralSettings);
    iListBox->SetMopParent(this); 
    
    GetValuesFromStoreL();
	
	iListBox->ConstructFromResourceL(R_COMMANDSC_SETTING);

	iListBox->MakeVisible(ETrue);
    iListBox->SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());

	
    iListBox->ActivateL();
	iListBox->LoadSettingsL();
	DrawNow();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CCommandSCSettings::SizeChanged()
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
TKeyResponse CCommandSCSettings::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
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
TInt CCommandSCSettings::CountComponentControls() const
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
CCoeControl* CCommandSCSettings::ComponentControl( TInt aIndex) const
{
	return iListBox; 
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CCommandSCSettings::Draw(const TRect& aRect) const
    {
		CWindowGc& gc = SystemGc();
		gc.Clear(aRect); 
    }
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CCommandSCSettings::GetValuesFromStoreL(void)
{	
	if(!iListBox)
		return;
		
	TFindFile AufFolder(CCoeEnv::Static()->FsSession());
	if(KErrNone == AufFolder.FindByDir(KtxCommandSCFileName, KNullDesC))
	{
		CDictionaryFileStore* MyDStore = CDictionaryFileStore::OpenLC(CCoeEnv::Static()->FsSession(),AufFolder.File(), TUid::Uid(0x102013AD));

		TInt CommandSc(0);
		GetValuesL(MyDStore,0x5000,CommandSc);
		if(CommandSc > 0)
		{
			iListBox->iCommand0 = CommandSc;
			iGeneralSettings.GetCommandText(iListBox->iCommandTxt0,CommandSc);
		}
		else
		{
			StringLoader::Load(iListBox->iCommandTxt0,R_MNU_NOTSET);
		}
		
		CommandSc =0;
		GetValuesL(MyDStore,0x5001,CommandSc);
		if(CommandSc > 0)
		{
			iListBox->iCommand1 = CommandSc;
			iGeneralSettings.GetCommandText(iListBox->iCommandTxt1,CommandSc);
		}
		else
		{
			StringLoader::Load(iListBox->iCommandTxt1,R_MNU_NOTSET);
		}
		
		CommandSc =0;
		GetValuesL(MyDStore,0x5002,CommandSc);
		if(CommandSc > 0)
		{
			iListBox->iCommand2 = CommandSc;
			iGeneralSettings.GetCommandText(iListBox->iCommandTxt2,CommandSc);
		}
		else
		{
			StringLoader::Load(iListBox->iCommandTxt2,R_MNU_NOTSET);
		}
		
		CommandSc =0;
		GetValuesL(MyDStore,0x5003,CommandSc);
		if(CommandSc > 0)
		{
			iListBox->iCommand3 = CommandSc;
			iGeneralSettings.GetCommandText(iListBox->iCommandTxt3,CommandSc);
		}
		else
		{
			StringLoader::Load(iListBox->iCommandTxt3,R_MNU_NOTSET);
		}
		
		CommandSc =0;
		GetValuesL(MyDStore,0x5004,CommandSc);
		if(CommandSc > 0)
		{
			iListBox->iCommand4 = CommandSc;
			iGeneralSettings.GetCommandText(iListBox->iCommandTxt4,CommandSc);
		}
		else
		{
			StringLoader::Load(iListBox->iCommandTxt4,R_MNU_NOTSET);
		}

		CommandSc =0;
		GetValuesL(MyDStore,0x5005,CommandSc);
		if(CommandSc > 0)
		{
			iListBox->iCommand5 = CommandSc;
			iGeneralSettings.GetCommandText(iListBox->iCommandTxt5,CommandSc);
		}
		else
		{
			StringLoader::Load(iListBox->iCommandTxt5,R_MNU_NOTSET);
		}
		
		CommandSc =0;
		GetValuesL(MyDStore,0x5006,CommandSc);
		if(CommandSc > 0)
		{
			iListBox->iCommand6 = CommandSc;
			iGeneralSettings.GetCommandText(iListBox->iCommandTxt6,CommandSc);
		}
		else
		{
			StringLoader::Load(iListBox->iCommandTxt6,R_MNU_NOTSET);
		}
		
		CommandSc =0;
		GetValuesL(MyDStore,0x5007,CommandSc);
		if(CommandSc > 0)
		{
			iListBox->iCommand7 = CommandSc;
			iGeneralSettings.GetCommandText(iListBox->iCommandTxt7,CommandSc);
		}
		else
		{
			StringLoader::Load(iListBox->iCommandTxt7,R_MNU_NOTSET);
		}
		
		CommandSc =0;
		GetValuesL(MyDStore,0x5008,CommandSc);
		if(CommandSc > 0)
		{
			iListBox->iCommand8 = CommandSc;
			iGeneralSettings.GetCommandText(iListBox->iCommandTxt8,CommandSc);
		}
		else
		{
			StringLoader::Load(iListBox->iCommandTxt8,R_MNU_NOTSET);
		}
		
		CommandSc =0;
		GetValuesL(MyDStore,0x5009,CommandSc);
		if(CommandSc > 0)
		{
			iListBox->iCommand9 = CommandSc;
			iGeneralSettings.GetCommandText(iListBox->iCommandTxt9,CommandSc);
		}
		else
		{
			StringLoader::Load(iListBox->iCommandTxt9,R_MNU_NOTSET);
		}
		    
        CommandSc =0;
        GetValuesL(MyDStore,0x500A,CommandSc);
        if(CommandSc < 0)
        {
            CommandSc = EBackOneLevel;
        }
        
        iListBox->iCommandTE1 = CommandSc;
        iGeneralSettings.GetCommandText(iListBox->iCommandTxtTE1,CommandSc);
       	
        
        CommandSc =0;
        GetValuesL(MyDStore,0x500B,CommandSc);
        if(CommandSc < 0)
        {
            CommandSc = EFolderViewMarkTogle;
        }
        
        iListBox->iCommandTE2 = CommandSc;
        iGeneralSettings.GetCommandText(iListBox->iCommandTxtTE2,CommandSc);

        
        CommandSc =0;
        GetValuesL(MyDStore,0x500C,CommandSc);
        if(CommandSc < 0)
        {
            CommandSc = EChangeExtraInfo;
        }
        
        iListBox->iCommandTE3 = CommandSc;
        iGeneralSettings.GetCommandText(iListBox->iCommandTxtTE3,CommandSc);         

		CleanupStack::PopAndDestroy(1);// Store
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CCommandSCSettings::GetValuesL(CDictionaryFileStore* aDStore,TInt aUID,TInt& aValue)
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
		aValue = -1;
}
	
/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
void CCommandSCSettings::SaveValuesL(void)
{
	if(!iListBox)
		return;

	iListBox->StoreSettingsL();

	TFindFile AufFolder(CCoeEnv::Static()->FsSession());
	if(KErrNone == AufFolder.FindByDir(KtxCommandSCFileName, KNullDesC))
	{
		User::LeaveIfError(CCoeEnv::Static()->FsSession().Delete(AufFolder.File()));

		CDictionaryFileStore* MyDStore = CDictionaryFileStore::OpenLC(CCoeEnv::Static()->FsSession(),AufFolder.File(), TUid::Uid(0x102013AD));
		
		SaveValuesL(MyDStore,0x5000,iListBox->iCommand0);
		SaveValuesL(MyDStore,0x5001,iListBox->iCommand1);
		SaveValuesL(MyDStore,0x5002,iListBox->iCommand2);
		SaveValuesL(MyDStore,0x5003,iListBox->iCommand3);
		SaveValuesL(MyDStore,0x5004,iListBox->iCommand4);
		SaveValuesL(MyDStore,0x5005,iListBox->iCommand5);
		SaveValuesL(MyDStore,0x5006,iListBox->iCommand6);
		SaveValuesL(MyDStore,0x5007,iListBox->iCommand7);
		SaveValuesL(MyDStore,0x5008,iListBox->iCommand8);
		SaveValuesL(MyDStore,0x5009,iListBox->iCommand9);
		
		SaveValuesL(MyDStore,0x500A,iListBox->iCommandTE1);
	    SaveValuesL(MyDStore,0x500B,iListBox->iCommandTE2);
	    SaveValuesL(MyDStore,0x500C,iListBox->iCommandTE3);

		MyDStore->CommitL();
		CleanupStack::PopAndDestroy(1);// Store
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CCommandSCSettings::SaveValuesL(CDictionaryFileStore* aDStore,TInt aUID, TInt aValue)
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
