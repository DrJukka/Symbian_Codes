/* Copyright (c) 2001 - 2006 , J.P. Silvennoinen. All rights reserved */

#include "CommandSC_Settings.h"            // own definitions


#include "YuccaBrowser.h"
#include "YuccaBrowser.hrh"
#ifdef __YTOOLS_SIGNED
	#include <YuccaBrowser_2000713D.rsg>
#else
	#include <YuccaBrowser.rsg>
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

CAknSettingsListListbox ::CAknSettingsListListbox()
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
		if(ListBox()->CurrentItemIndex() >= 0)// alarms
		{
			SelectCommandSCL(ListBox()->CurrentItemIndex());
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
	StringLoader::Load(Hjelpper,R_MNU_COPY);
	SelectArrray->AppendL(Hjelpper);
	
	StringLoader::Load(Hjelpper,R_MNU_CUT);
	SelectArrray->AppendL(Hjelpper);
	StringLoader::Load(Hjelpper,R_MNU_PASTE);
	SelectArrray->AppendL(Hjelpper);
	StringLoader::Load(Hjelpper,R_MNU_DELETE);
	SelectArrray->AppendL(Hjelpper);
	StringLoader::Load(Hjelpper,R_MNU_RENAME);
	SelectArrray->AppendL(Hjelpper);
	StringLoader::Load(Hjelpper,R_MNU_PROPERTIES);
	SelectArrray->AppendL(Hjelpper);
	StringLoader::Load(Hjelpper,R_MNU_ATTRIBUTES);
	SelectArrray->AppendL(Hjelpper);
	StringLoader::Load(Hjelpper,R_MNU_SHOWNAME);
	SelectArrray->AppendL(Hjelpper);
	StringLoader::Load(Hjelpper,R_MNU_SEND);
	SelectArrray->AppendL(Hjelpper);
	StringLoader::Load(Hjelpper,R_MNU_NEWFILE);
	SelectArrray->AppendL(Hjelpper);
	StringLoader::Load(Hjelpper,R_MNU_NEWFOLDER);
	SelectArrray->AppendL(Hjelpper);
	StringLoader::Load(Hjelpper,R_MNU_SETTINGS);
	SelectArrray->AppendL(Hjelpper);
	StringLoader::Load(Hjelpper,R_MNU_SHOWSC);
	SelectArrray->AppendL(Hjelpper);
	StringLoader::Load(Hjelpper,R_MNU_SETSC);
	SelectArrray->AppendL(Hjelpper);
	StringLoader::Load(Hjelpper,R_MNU_SETSEARCHON);
	SelectArrray->AppendL(Hjelpper);
	StringLoader::Load(Hjelpper,R_MNU_REFRESH);
	SelectArrray->AppendL(Hjelpper);
	StringLoader::Load(Hjelpper,R_MNU_MARKALL);
	SelectArrray->AppendL(Hjelpper);
	StringLoader::Load(Hjelpper,R_MNU_UNMARKALL);
	SelectArrray->AppendL(Hjelpper);
	StringLoader::Load(Hjelpper,R_MNU_OPEN);
	SelectArrray->AppendL(Hjelpper);
	StringLoader::Load(Hjelpper,R_MNU_OPENWITH);
	SelectArrray->AppendL(Hjelpper);
	StringLoader::Load(Hjelpper,R_MNU_PASTESHOW);
	SelectArrray->AppendL(Hjelpper);
	StringLoader::Load(Hjelpper,R_MNU_CLEARPASTE);
	SelectArrray->AppendL(Hjelpper);
	StringLoader::Load(Hjelpper,R_MNU_SEARCHFLES);
	SelectArrray->AppendL(Hjelpper);
	StringLoader::Load(Hjelpper,R_MNU_PAGEUP);
    SelectArrray->AppendL(Hjelpper);
    StringLoader::Load(Hjelpper,R_MNU_PAGEDOWN);
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
			StringLoader::Load(SelComBuf,R_MNU_COPY);
			break;
		case 1:
			SelCommand = EEditCut;					
			StringLoader::Load(SelComBuf,R_MNU_CUT);
			break;
		case 2:
			SelCommand = EEditPaste;					
			StringLoader::Load(SelComBuf,R_MNU_PASTE);
			break;
		case 3:
			SelCommand = EFileDelete;				
			StringLoader::Load(SelComBuf,R_MNU_DELETE);	
			break;
		case 4:
			SelCommand = EFileRename;				
			StringLoader::Load(SelComBuf,R_MNU_RENAME);	
			break;
		case 5:
			SelCommand = EFileDetails;				
			StringLoader::Load(SelComBuf,R_MNU_PROPERTIES);
			break;
		case 6:
			SelCommand = EFileAttributes;			
			StringLoader::Load(SelComBuf,R_MNU_ATTRIBUTES);
			break;
		case 7:
			SelCommand = EFileShowName;				
			StringLoader::Load(SelComBuf,R_MNU_SHOWNAME);
			break;
		case 8:
			SelCommand = ESendFiles;					
			StringLoader::Load(SelComBuf,R_MNU_SEND);
			break;
		case 9:
			SelCommand = EFileNewFile;				
			StringLoader::Load(SelComBuf,R_MNU_NEWFILE);
			break;
		case 10:
			SelCommand = EFileNewFolder;				
			StringLoader::Load(SelComBuf,R_MNU_NEWFOLDER);
			break;
		case 11:
			SelCommand = ESettings;					
			StringLoader::Load(SelComBuf,R_MNU_SETTINGS);
			break;
		case 12:
			SelCommand = EExtrasShowShortCut;		
			StringLoader::Load(SelComBuf,R_MNU_SHOWSC);
			break;
		case 13:
			SelCommand = EExtrasSetShortCut;			
			StringLoader::Load(SelComBuf,R_MNU_SETSC);
			break;
		case 14:
			SelCommand = EExtrasSearchOn;			
			StringLoader::Load(SelComBuf,R_MNU_SETSEARCHON);
			break;
		case 15:
			SelCommand = EExtrasRefresh;				
			StringLoader::Load(SelComBuf,R_MNU_REFRESH);
			break;
		case 16:
			SelCommand = EFolderViewMarkAll;			
			StringLoader::Load(SelComBuf,R_MNU_MARKALL);
			break;
		case 17:
			SelCommand = EFolderViewUnMarkAll;		
			StringLoader::Load(SelComBuf,R_MNU_UNMARKALL);
			break;
		case 18:
			SelCommand = EVBOpenFile;				
			StringLoader::Load(SelComBuf,R_MNU_OPEN);
			break;
		case 19:
			SelCommand = EOpenWithViewer;				
			StringLoader::Load(SelComBuf,R_MNU_OPENWITH);
			break;
		case 20:
			SelCommand = EEditShowPaste;				
			StringLoader::Load(SelComBuf,R_MNU_PASTESHOW);
			break;	
		case 21:
			SelCommand = EEditClearPaste;				
			StringLoader::Load(SelComBuf,R_MNU_CLEARPASTE);
			break;
		case 22:
			SelCommand = ESearchFiles;				
			StringLoader::Load(SelComBuf,R_MNU_SEARCHFLES);
			break;
		case 23:
			SelCommand = EMovePageUp;				
			StringLoader::Load(SelComBuf,R_MNU_PAGEUP);
			break;
		case 24:
			SelCommand = EMovePageDown;				
			StringLoader::Load(SelComBuf,R_MNU_PAGEDOWN);
			break;	
		default:
			break;
		}
		
		if(SelCommand > 0)
		{
			switch(aSC)
			{
			case 0:
				iCommand0 = SelCommand;
				iCommandTxt0.Copy(SelComBuf);
				break;
			case 1:
				iCommand1 = SelCommand;
				iCommandTxt1.Copy(SelComBuf);
				break;
			case 2:
				iCommand2 = SelCommand;
				iCommandTxt2.Copy(SelComBuf);
				break;
			case 3:
				iCommand3 = SelCommand;
				iCommandTxt3.Copy(SelComBuf);
				break;
			case 4:
				iCommand4 = SelCommand;
				iCommandTxt4.Copy(SelComBuf);
				break;
			case 5:
				iCommand5 = SelCommand;
				iCommandTxt5.Copy(SelComBuf);
				break;
			case 6:
				iCommand6 = SelCommand;
				iCommandTxt6.Copy(SelComBuf);
				break;
			case 7:
				iCommand7 = SelCommand;
				iCommandTxt7.Copy(SelComBuf);
				break;
			case 8:
				iCommand8 = SelCommand;
				iCommandTxt8.Copy(SelComBuf);
				break;
			case 9:
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
CCommandSCSettings::CCommandSCSettings(void)
:iListBox(NULL)
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
    iListBox = new (ELeave) CAknSettingsListListbox();
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
	MakeListboxL();	
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
		return iListBox->CountComponentControls ();
	else
		return 0;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CCoeControl* CCommandSCSettings::ComponentControl( TInt aIndex) const
{
	return iListBox->ComponentControl(aIndex); 
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
			GetCommandText(iListBox->iCommandTxt0,CommandSc);
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
			GetCommandText(iListBox->iCommandTxt1,CommandSc);
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
			GetCommandText(iListBox->iCommandTxt2,CommandSc);
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
			GetCommandText(iListBox->iCommandTxt3,CommandSc);
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
			GetCommandText(iListBox->iCommandTxt4,CommandSc);
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
			GetCommandText(iListBox->iCommandTxt5,CommandSc);
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
			GetCommandText(iListBox->iCommandTxt6,CommandSc);
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
			GetCommandText(iListBox->iCommandTxt7,CommandSc);
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
			GetCommandText(iListBox->iCommandTxt8,CommandSc);
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
			GetCommandText(iListBox->iCommandTxt9,CommandSc);
		}
		else
		{
			StringLoader::Load(iListBox->iCommandTxt9,R_MNU_NOTSET);
		}

		CleanupStack::PopAndDestroy(1);// Store
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CCommandSCSettings::GetCommandText(TDes& aTxtx,TInt aSC)
{
	switch(aSC)
	{
	case EEditCopy:
		StringLoader::Load(aTxtx,R_MNU_COPY);					
		break;
	case EEditCut:				
		StringLoader::Load(aTxtx,R_MNU_CUT);	
		break;
	case EEditPaste:			
		StringLoader::Load(aTxtx,R_MNU_PASTE);		
		break;
	case EFileDelete:			
		StringLoader::Load(aTxtx,R_MNU_DELETE);	
		break;
	case EFileRename:			
		StringLoader::Load(aTxtx,R_MNU_RENAME);	
		break;
	case EFileDetails:			
		StringLoader::Load(aTxtx,R_MNU_PROPERTIES);	
		break;
	case EFileAttributes:		
		StringLoader::Load(aTxtx,R_MNU_ATTRIBUTES);	
		break;
	case EFileShowName:			
		StringLoader::Load(aTxtx,R_MNU_SHOWNAME);	
		break;
	case ESendFiles:			
		StringLoader::Load(aTxtx,R_MNU_SEND);
		break;
	case EFileNewFile:			
		StringLoader::Load(aTxtx,R_MNU_NEWFILE);	
		break;
	case EFileNewFolder:		
		StringLoader::Load(aTxtx,R_MNU_NEWFOLDER);
		break;
	case ESettings:				
		StringLoader::Load(aTxtx,R_MNU_SETTINGS);	
		break;
	case EExtrasShowShortCut:	
		StringLoader::Load(aTxtx,R_MNU_SHOWSC);	
		break;
	case EExtrasSetShortCut:	
		StringLoader::Load(aTxtx,R_MNU_SETSC);	
		break;
	case EExtrasSearchOn:		
		StringLoader::Load(aTxtx,R_MNU_SETSEARCHON);
		break;
	case EExtrasRefresh:		
		StringLoader::Load(aTxtx,R_MNU_REFRESH);		
		break;
	case EFolderViewMarkAll:	
		StringLoader::Load(aTxtx,R_MNU_MARKALL);
		break;
	case EFolderViewUnMarkAll:	
		StringLoader::Load(aTxtx,R_MNU_UNMARKALL);	
		break;
	case EVBOpenFile:
		StringLoader::Load(aTxtx,R_MNU_OPEN);
		break;
	case EOpenWithViewer:
		StringLoader::Load(aTxtx,R_MNU_OPENWITH);
		break;
	case EEditShowPaste:		
		StringLoader::Load(aTxtx,R_MNU_PASTESHOW);		
		break;
	case EEditClearPaste:
		StringLoader::Load(aTxtx,R_MNU_CLEARPASTE);
		break;
	case ESearchFiles:
		StringLoader::Load(aTxtx,R_MNU_SEARCHFLES);
		break;
	case EMovePageUp:
		StringLoader::Load(aTxtx,R_MNU_PAGEUP);
		break;
	case EMovePageDown:
		StringLoader::Load(aTxtx,R_MNU_PAGEDOWN);
		break;	
	default:
		break;
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
