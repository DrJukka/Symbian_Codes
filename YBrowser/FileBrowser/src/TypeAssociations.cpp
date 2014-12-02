/*
	Copyright (C) 2006 Jukka Silvennoinen
	All right reserved
*/

#include <GULUTIL.H>
#include <APGCLI.H>
#include <APMREC.H>
#include <AknIconArray.h>
#include <AknSelectionList.h>
#include <StringLoader.h>
#include <AknQueryDialog.h>
#include <aknglobalnote.h> 
#include <eikclbd.h>
#include <BAUTILS.H>
#include <aknnavide.h>
#include <akntabgrp.h>
#include "IconHandler.h"

#include "TypeAssociations.h"
#include "ShowString.h"
#include "FileTypesDataBase.h"

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
	

CTypesListbox ::CTypesListbox()
{
	
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
CTypesListbox::~CTypesListbox()
{		
	delete iExtensions;
	iExtensions = NULL;
}				

CAknSettingItem* CTypesListbox ::CreateSettingItemL(TInt aIdentifier)
{
	CAknSettingItem* settingItem = NULL;

	switch(aIdentifier)
	{
	case ETypesFileType:
    	settingItem = new (ELeave) CAknTextSettingItem(aIdentifier,iFileType);
		break;
	case ETypesExtensions:
    	settingItem = new (ELeave) CAknTextSettingItem(aIdentifier,iExtensionCount);
		break;
	case ETypesHandler:
    	settingItem = new (ELeave) CAknTextSettingItem(aIdentifier,iHandlerApp);
		break;
	}	
	
	return settingItem;
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CTypesListbox::SizeChanged()
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
void CTypesListbox::HandleListBoxEventL(CEikListBox* aListBox,TListBoxEvent aEventType)
{
	if(ListBox())
	{		
		if(ListBox()->CurrentItemIndex() == 0)// file type
		{
			if(iOwn)
			{
				CAknSettingItemList::HandleListBoxEventL(aListBox,aEventType);
			}
			// not modifying system file types, only own
		}
		else if(ListBox()->CurrentItemIndex() == 1)// handler
		{
			
		}
		else // extension.
		{	
			
		}
	}
}

/*
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/
CTypesContainer::~CTypesContainer()
{
	RemoveNavipanel();

	delete iMyDBClass;
	iMyDBClass = NULL;
	
	delete iTypesListbox;
	iTypesListbox = NULL;
	
	delete iShowString;
	iShowString = NULL;
	
	delete iSysListBox;
	iSysListBox = NULL;
	
	delete iSysFilTypArray;
	iSysFilTypArray = NULL;
	
	delete iOwnListBox;
	delete iOwnFilTypArray;
	
	delete iBgContext;
	iBgContext = NULL;
}
	
/*
-------------------------------------------------------------------------
This will be called when this file shandler is preraped to be used for, 
opening or constructing a file. The open/new etc. functions are called
strait after calling this function.
-------------------------------------------------------------------------
*/
void CTypesContainer::ConstructL(CEikButtonGroupContainer* aCba,CIconHandler* aIconHandler)
{
	iCba = aCba;
	iIconHandler = aIconHandler;
	
 	CreateWindowL();
 	
 	MakeNavipanelL();
 	
 	iMyDBClass = new(ELeave)CMyDBClass();
	iMyDBClass->ConstructL();
	
	ReReadMyTypesL();
	
 	iSysFilTypArray = new(ELeave)CDesCArrayFlat(10);
	
	RApaLsSession ls;
	User::LeaveIfError(ls.Connect());
	CleanupClosePushL(ls);
	
	CDataTypeArray* allDataTypes = new(ELeave)CDataTypeArray(50);
	CleanupStack::PushL(allDataTypes);
	ls.GetSupportedDataTypesL(*allDataTypes);
	
	TKeyArrayFix SortKey(0, ECmpNormal);	
	allDataTypes->Sort(SortKey);

	TFileName HelpFn;
	
	for(TInt i=(allDataTypes->Count()- 1); i >=0 ; i--)
	{
		if(allDataTypes->At(i).Des().Length())
		{			
			HelpFn.Copy(_L("\t"));
			HelpFn.Append(allDataTypes->At(i).Des());
			
			iSysFilTypArray->AppendL(HelpFn);	
		}
	}
		
	CleanupStack::PopAndDestroy(2);//ls,allDataTypes
	
 	iBgContext = CAknsBasicBackgroundControlContext::NewL(KAknsIIDQsnBgAreaMain,TRect(0,0,1,1), ETrue);
	
	SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect()); 	
		
	ActivateL();
	SetMenuL();
	DrawNow();	
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CAknSingleStyleListBox* CTypesContainer::ConstructListBoxL(TInt aIndex,CDesCArray* aArray)
{
	TInt MySetIndex(aIndex);

	CAknSingleStyleListBox* ListBox   = new( ELeave )CAknSingleStyleListBox();	
	ListBox->ConstructL(this,EAknListBoxMarkableList);	
	
	
	ListBox->Model()->SetItemTextArray(aArray);
    ListBox->Model()->SetOwnershipType(ELbmDoesNotOwnItemArray);
    
	ListBox->CreateScrollBarFrameL( ETrue );
    ListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
	ListBox->SetRect(Rect());
	ListBox->ItemDrawer()->ColumnData()->EnableMarqueeL(ETrue);
	
	ListBox->ActivateL();

	TInt ItemsCount = ListBox->Model()->ItemTextArray()->MdcaCount();
	
	if(ItemsCount > MySetIndex && MySetIndex >= 0)
		ListBox->SetCurrentItemIndex(MySetIndex);
	else if(ItemsCount > 0)
		ListBox->SetCurrentItemIndex(0);
	
	UpdateScrollBar(ListBox);
	ListBox->DrawNow();
	
	return ListBox;
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CTypesContainer::UpdateScrollBar(CEikListBox* aListBox)
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
-----------------------------------------------------------------------------
Y-Browser will call setsize for client rect when lay-out or size changes,
This will be then called by the framework
-----------------------------------------------------------------------------
*/
void CTypesContainer::SizeChanged()
{
	if ( iBgContext )
	{
		iBgContext->SetRect(Rect());

		if ( &Window() )
		{
			iBgContext->SetParentPos( PositionRelativeToScreen() );
		}
	}
	
	if(iOwnFilTypArray)
	{
		TInt SelInd(-1);
		
		if(iOwnListBox)
		{
			SelInd = iOwnListBox->CurrentItemIndex();
		}
		
		delete iOwnListBox;
		iOwnListBox = NULL;
	 	iOwnListBox = ConstructListBoxL(SelInd,iOwnFilTypArray);
	}	
	
	if(iSysFilTypArray)
	{
		TInt SelInd(-1);
		
		if(iSysListBox)
		{
			SelInd = iSysListBox->CurrentItemIndex();
		}
		
		delete iSysListBox;
		iSysListBox = NULL;
	 	iSysListBox = ConstructListBoxL(SelInd,iSysFilTypArray);
	}
	
	if(iTypesListbox)
	{
		iTypesListbox->SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
	}
	
	if(iShowString)
	{
		iShowString->UpDateSizeL();
	}
	 	
}

/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
TTypeUid::Ptr CTypesContainer::MopSupplyObject(TTypeUid aId)
{	
	if (iBgContext)
	{
		return MAknsControlContext::SupplyMopObject(aId, iBgContext );
	}
	
	return CCoeControl::MopSupplyObject(aId);
}

/*
-------------------------------------------------------------------------

-------------------------------------------------------------------------
*/
void CTypesContainer::SetMenuL(void)
{
	if(iTypesListbox)
	{	
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->SetMenuTitleResourceId(R_FILTYPMOD_MENUBAR);
		iCba->SetCommandSetL(R_FILTYPMOD_CBA);
		iCba->DrawDeferred();
	}
	else if(iShowString)
	{
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->SetMenuTitleResourceId(R_SHOWSTRING_MENUBAR);	
		iCba->SetCommandSetL(R_SHOWSTRING2_CBA);				
		iCba->DrawDeferred();
	}
	else
	{
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->SetMenuTitleResourceId(R_FILTYPASS_MENUBAR);				
		iCba->SetCommandSetL(R_FILTYPASS_CBA);
		iCba->DrawDeferred();
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CTypesContainer::InitMenuPanelL(TInt aResourceId, CEikMenuPane* aMenuPane)
{
	if(aResourceId == R_FILTYPASS_MENU)
	{
		TBool Dimopen(ETrue),DimMod(ETrue);
		
		if(iTabGroup)
		{
			if(iTabGroup->ActiveTabIndex() == 1)
			{
				DimMod = EFalse;
			}
		}
		
		if(CurrentListBox())
		{
			TInt CurrInd = CurrentListBox()->CurrentItemIndex();
			if(CurrInd >= 0)
			{
				Dimopen = EFalse;		
			}
		}
		
		if(Dimopen)
		{
			aMenuPane->SetItemDimmed(EFileShowFt,ETrue);
			aMenuPane->SetItemDimmed(EFilTypModify,ETrue);
			aMenuPane->SetItemDimmed(EFilTypRemNew,ETrue);
		}
		
		if(DimMod)
		{
			aMenuPane->SetItemDimmed(EFilTypAddNew,ETrue);
			aMenuPane->SetItemDimmed(EFilTypRemNew,ETrue);
		}
	}
	if(aResourceId == R_FILTYPMOD_MENU)
	{
		TBool DimRem(ETrue),DimName(ETrue);
		
		if(iTypesListbox)
		{
			if(iTypesListbox->iOwn)
			{
				DimName = EFalse;
			}
			
			if(iTypesListbox)
			{
				if(iTypesListbox->iExtensions)
				{
					if(iTypesListbox->iExtensions->MdcaCount())
					{
						DimRem = EFalse;
					}
				}
			}
		}
	
		if(DimName)
		{
			aMenuPane->SetItemDimmed(EFilTypModChangeName,ETrue);
		}
		
		if(DimRem)
		{
			aMenuPane->SetItemDimmed(EFilTypModExtRem,ETrue);
		}	
	}
	
}


/*
-------------------------------------------------------------------------

-------------------------------------------------------------------------
*/
void CTypesContainer::RemoveNavipanel(void) 
{
	iTabGroup = NULL;
	
	if (!iNaviPane)
    {
        CEikStatusPane *sp = ((CAknAppUi*)iEikonEnv->EikAppUi())->StatusPane();
        iNaviPane = (CAknNavigationControlContainer *)sp->ControlL(TUid::Uid(EEikStatusPaneUidNavi));
    }

	if(iNaviPane)
	    iNaviPane->Pop(NULL); 
	
	iNaviPane = NULL;
	
    delete iNaviDecoratorForTabs;
    iNaviDecoratorForTabs = NULL;
    delete iNaviDecorator;
    iNaviDecorator = NULL;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CTypesContainer::MakeNavipanelL(void)
{
	CEikStatusPane *sp = ((CAknAppUi*)iEikonEnv->EikAppUi())->StatusPane();
	iNaviPane = (CAknNavigationControlContainer *)sp->ControlL(TUid::Uid(EEikStatusPaneUidNavi));

	CAknNavigationDecorator* iNaviDecoratorForTabsTemp = iNaviPane->CreateTabGroupL();
	delete iNaviDecoratorForTabs;
    iNaviDecoratorForTabs = NULL;
	iNaviDecoratorForTabs = iNaviDecoratorForTabsTemp;
   	iTabGroup = STATIC_CAST(CAknTabGroup*, iNaviDecoratorForTabs->DecoratedControl());
	iTabGroup->SetTabFixedWidthL(EAknTabWidthWithTwoTabs);//EAknTabWidthWithOneTab);

	TBuf<60> smallHjelp;

	StringLoader::Load(smallHjelp,R_SH_STR_SYSTEM2);
	iTabGroup->AddTabL(0,smallHjelp);
	
	StringLoader::Load(smallHjelp,R_SH_STR_OWN);
	iTabGroup->AddTabL(1,smallHjelp);
	
	if(iShowOwn)
		iTabGroup->SetActiveTabByIndex(1);
	else
		iTabGroup->SetActiveTabByIndex(0);
	
	iNaviPane->PushL(*iNaviDecoratorForTabs);
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CTypesContainer::ShowNoteL(const TDesC&  aMessage, TBool aError)
{
	CAknGlobalNote* dialog = CAknGlobalNote::NewLC();
		
	TInt NoteId(0); 
	
	if(aError)
		NoteId = dialog->ShowNoteL(EAknGlobalErrorNote,aMessage);
	else
		NoteId = dialog->ShowNoteL(EAknGlobalInformationNote,aMessage);
	
	User::After(1200000);
	dialog->CancelNoteL(NoteId);
	CleanupStack::PopAndDestroy(dialog);
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CTypesContainer::SelectApplicationL(void) 
{
	RApaLsSession ls;
	User::LeaveIfError(ls.Connect());
	CleanupClosePushL(ls);
	
	CDesCArrayFlat* AppNames = new(ELeave)CDesCArrayFlat(5);
    CleanupStack::PushL(AppNames);

    CDesCArrayFlat* Ittems = new(ELeave)CDesCArrayFlat(5);
    CleanupStack::PushL(Ittems);
	
	CArrayPtr<CGulIcon>* icons =new( ELeave ) CAknIconArray(10);
	CleanupStack::PushL(icons);
		
	User::LeaveIfError(ls.GetAllApps());

	TInt Errnono(KErrNone);
	TFileName HelpFn;
	TApaAppInfo AppInfo;
	
	MAknsSkinInstance* skin = AknsUtils::SkinInstance();
	
	do
	{
		Errnono = ls.GetNextApp(AppInfo);
		if(KErrNone == Errnono)
		{
			TRAPD(Err,
			CFbsBitmap*	AppIcon(NULL);
			CFbsBitmap*	AppIconMsk(NULL);
		    AknsUtils::CreateAppIconLC(skin,AppInfo.iUid,  EAknsAppIconTypeContext,AppIcon,AppIconMsk);
		
			if(AppIcon && AppIconMsk)
			{
				icons->AppendL(CGulIcon::NewL(AppIcon,AppIconMsk));
				CleanupStack::Pop(2);
						
				HelpFn.Num((icons->Count() - 1));
				HelpFn.Append(_L("\t"));
				HelpFn.Append(AppInfo.iCaption);
				HelpFn.TrimAll();
				
				Ittems->AppendL(HelpFn);	
				AppNames->AppendL(AppInfo.iFullName);					
			});
		}
		
	}while(KErrNone == Errnono);


	TInt OpenedItem(-1);

	CleanupStack::Pop(icons);

	CAknSelectionListDialog* dialog = CAknSelectionListDialog::NewL(OpenedItem,Ittems,0); 

	dialog->PrepareLC(R_SEL_APP_DIALOG);
	
	dialog->SetIconArrayL(icons); 
	if (dialog->RunLD () && iTypesListbox)
	{
		if(OpenedItem >= 0 && OpenedItem < AppNames->Count())
		{
			TPtrC ItName,Item;
			Item.Set(Ittems->MdcaPoint(OpenedItem));
			if(KErrNone == TextUtils::ColumnText(ItName,1,&Item))
			{
				iTypesListbox->iHasChanges = ETrue;
				iTypesListbox->iHandlerApp.Copy(ItName);
				iTypesListbox->iExecutable.Copy(AppNames->MdcaPoint(OpenedItem));
			}
		}
	}					
   
	CleanupStack::PopAndDestroy(3); // ls,AppNames,Ittems
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
TBool CTypesContainer::RemoveExtensionsL(CDesCArray& aArray)
{
	TBool Ret(EFalse);
	
	CArrayFixFlat<TInt>* SelectedItems = new(ELeave)CArrayFixFlat<TInt>(10);
	CleanupStack::PushL(SelectedItems);
	CDesCArrayFlat* SelectArrray = new(ELeave)CDesCArrayFlat(20);
	CleanupStack::PushL(SelectArrray);

	TBuf<255> hjelpp;
	for(TInt p=0; p < aArray.MdcaCount() ; p++)
	{
		hjelpp.Copy(_L("1\t"));
		hjelpp.Append(aArray.MdcaPoint(p));
		SelectedItems->AppendL(p);
		SelectArrray->AppendL(hjelpp);
	}

	CAknListQueryDialog* dialog = new(ELeave)CAknListQueryDialog(SelectedItems);

	dialog->PrepareLC(R_OWRTYPES_SELECTION_QUERY);

	dialog->SetItemTextArray(SelectArrray); 
	dialog->SetOwnershipType(ELbmDoesNotOwnItemArray); 
	dialog->SetIconArrayL(iIconHandler->GetSelectionIconArrayL()); 

	if (dialog->RunLD ())
	{
		Ret = ETrue;
		
		for(TInt i=(SelectedItems->Count()-1); i > -1 ; i--)
		{
			TInt selected = SelectedItems->At(i);
			
			if(selected >= 0 && selected < aArray.MdcaCount())
			{
				aArray.Delete(selected);
			}
		}
	}

	CleanupStack::PopAndDestroy(2);  //SelectedItems,SelectArrray					

	return Ret;
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/	
void CTypesContainer::HandleViewCommandL(TInt aCommand)
{
	switch(aCommand)
	{
	case EFilTypModChangeName:
		if(iTypesListbox)
		{
			HBufC* Buff= StringLoader::LoadLC(R_SH_STR_FILETYPE);

			CAknTextQueryDialog* Dialog = CAknTextQueryDialog::NewL(iTypesListbox->iFileType,CAknQueryDialog::ENoTone);
			Dialog->PrepareLC(R_ASK_NAME_DIALOG);
			Dialog->SetPromptL(*Buff);
			if(Dialog->RunLD())
			{
				iTypesListbox->iHasChanges = ETrue;
				iTypesListbox->LoadSettingsL();
			}

			CleanupStack::PopAndDestroy(Buff);
		}
		DrawNow();
		break;
	case EFilTypModChangeHand:
		if(iTypesListbox)
		{
			iTypesListbox->StoreSettingsL();
			SelectApplicationL();	
			iTypesListbox->LoadSettingsL();
		}
		DrawNow();
		break;
	case EFilTypModExtAdd:
		if(iTypesListbox)
		{
			iTypesListbox->StoreSettingsL();
			
			if(!iTypesListbox->iExtensions)
			{
				iTypesListbox->iExtensions = new(ELeave)CDesCArrayFlat(10);
			}
				
			TFileName Hjelpper;
			
			HBufC* Buff= StringLoader::LoadLC(R_SH_STR_FILEXTENSION);

			CAknTextQueryDialog* Dialog = CAknTextQueryDialog::NewL(Hjelpper,CAknQueryDialog::ENoTone);
			Dialog->PrepareLC(R_ASK_NAME_DIALOG);
			Dialog->SetPromptL(*Buff);
			if(Dialog->RunLD())
			{
				iTypesListbox->iExtensions->AppendL(Hjelpper);
				iTypesListbox->iHasChanges = ETrue;
			}

			CleanupStack::PopAndDestroy(Buff);
			
			if(iTypesListbox->iExtensions)
			{
				iTypesListbox->iExtensionCount.Num(iTypesListbox->iExtensions->MdcaCount());
				iTypesListbox->LoadSettingsL();
			}
		}
		DrawNow();
		break;
	case EFilTypModExtRem:
		if(iTypesListbox)
		{
			if(iTypesListbox->iExtensions)
			{
				iTypesListbox->StoreSettingsL();
			
				if(RemoveExtensionsL(*iTypesListbox->iExtensions))
				{
					iTypesListbox->iHasChanges = ETrue;
					iTypesListbox->iExtensionCount.Num(iTypesListbox->iExtensions->MdcaCount());
					iTypesListbox->LoadSettingsL();
				}
			}
		}
		DrawNow();
		break;
	case EFilTypModReset:
		if(iTypesListbox)
		{
			iTypesListbox->iHasChanges = ETrue;
			
			delete iTypesListbox->iExtensions;
			iTypesListbox->iExtensions = NULL;
			iTypesListbox->iExtensions = new(ELeave)CDesCArrayFlat(10);
		    
		    iTypesListbox->iExtensionCount.Num(0);
		    iTypesListbox->iExecutable.Zero();
			iTypesListbox->iHandlerApp.Zero();
			iTypesListbox->LoadSettingsL();
		}
		DrawNow();
		break;
	case EFilTypModSave:
	case EFilTypModCancel:
		{
			if(iTypesListbox && iMyDBClass)
			{
				if(iTypesListbox->iHasChanges)
				{
					iHasChanges = ETrue;
					iTypesListbox->StoreSettingsL();
					
					TInt Dummy(iTypesListbox->iIndex);		
					TBool HasExtensions(EFalse);
					
					if(iTypesListbox->iExtensions)
					{
						if(iTypesListbox->iExtensions->MdcaCount())
						{
							HasExtensions = ETrue;
						}
					}
					
					if(Dummy >= 0)
					{	
						if(iTypesListbox->iExecutable.Length() || HasExtensions)
							iMyDBClass->UpdateDatabaseL(iTypesListbox->iFileType,iTypesListbox->iExecutable,iTypesListbox->iExtensions,iTypesListbox->iOwn,Dummy);
						else
							iMyDBClass->DeleteFromDatabaseL(Dummy);
					}
					else if(iTypesListbox->iExecutable.Length() || HasExtensions)
					{
						iMyDBClass->SaveToDatabaseL(iTypesListbox->iFileType,iTypesListbox->iExecutable,iTypesListbox->iExtensions,iTypesListbox->iOwn,Dummy);
					}
					
					if(iTypesListbox->iOwn)
					{
						TInt SelInd(-1);
				
						if(iOwnListBox)
						{
							SelInd = iOwnListBox->CurrentItemIndex();
						}
						
						ReReadMyTypesL();
					
						delete iOwnListBox;
						iOwnListBox = NULL;
					 	iOwnListBox = ConstructListBoxL(SelInd,iOwnFilTypArray);
					}
				}
			}
			
			delete iTypesListbox;
			iTypesListbox = NULL;
		
			MakeNavipanelL();
		}
		SetMenuL();
		DrawNow();
		break;
	case EFilTypRemNew:
		if(iMyDBClass)
		{
			TFileName hJelp;
			GetSelFileType(hJelp);
				
			RPointerArray<CFtASSItem> myItemArray;
			
			iMyDBClass->ReadDbItemsL(myItemArray);
			
			for(TInt i=0; i < myItemArray.Count(); i++)
			{
				if(myItemArray[i])
				{
					if(myItemArray[i]->iTypeName && myItemArray[i]->iOwn)
					{
						if(hJelp == myItemArray[i]->iTypeName->Des())
						{
		        			HBufC* Buff= StringLoader::LoadLC(R_SH_STR_REMFILETYPE);
		        		
				        	CAknQueryDialog* dlg = CAknQueryDialog::NewL();
							if(dlg->ExecuteLD(R_QUERY,*Buff))
							{	
								iMyDBClass->DeleteFromDatabaseL(myItemArray[i]->iIndex);
								
								TInt SelInd(-1);
			
								if(iOwnListBox)
								{
									SelInd = iOwnListBox->CurrentItemIndex();
								}
								
								ReReadMyTypesL();
							
								delete iOwnListBox;
								iOwnListBox = NULL;
							 	iOwnListBox = ConstructListBoxL(SelInd,iOwnFilTypArray);
							}
							
							CleanupStack::PopAndDestroy(Buff);
						}
					}
				}
			}	
				
			myItemArray.ResetAndDestroy();	
		}
		SetMenuL();
		DrawNow();
		break;
	case EFilTypAddNew:
		{
			iShowOwn = EFalse;
			if(iTabGroup)
			{
				if(iTabGroup->ActiveTabIndex() == 1)
				{
					iShowOwn = ETrue;
				}
			}
			
			RemoveNavipanel();
			
			delete iTypesListbox;
		    iTypesListbox = NULL;

		    iTypesListbox = new (ELeave) CTypesListbox;
		    iTypesListbox->iIndex = -1;
		    iTypesListbox->iOwn = ETrue;
		    
		    iTypesListbox->ConstructFromResourceL(R_FILTYPES_SETTING);	    
			iTypesListbox->MakeVisible(ETrue);
		    iTypesListbox->SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
		    iTypesListbox->ActivateL();			
			iTypesListbox->LoadSettingsL();
		    iTypesListbox->DrawNow();
		}
		SetMenuL();
		DrawNow();
		break;
	case EFilTypModify:
		if(iMyDBClass)
		{
			iShowOwn = EFalse;
			if(iTabGroup)
			{
				if(iTabGroup->ActiveTabIndex() == 1)
				{
					iShowOwn = ETrue;
				}
			}
			
			delete iTypesListbox;
		    iTypesListbox = NULL;

		    iTypesListbox = new (ELeave) CTypesListbox;
		    iTypesListbox->iIndex = -1;
		    iTypesListbox->iExtensions = new(ELeave)CDesCArrayFlat(10);
		    
			GetSelFileType(iTypesListbox->iFileType);
			
			RemoveNavipanel();
				
			RPointerArray<CFtASSItem> myItemArray;
			
			iMyDBClass->ReadDbItemsL(myItemArray);
			
			for(TInt i=0; i < myItemArray.Count(); i++)
			{
				if(myItemArray[i])
				{
					if(myItemArray[i]->iTypeName)
					{
						if(iTypesListbox->iFileType == myItemArray[i]->iTypeName->Des())
						{
							if(myItemArray[i]->iExtensions)
							{
								for(TInt pp=0; pp < myItemArray[i]->iExtensions->MdcaCount(); pp++)
								{
									iTypesListbox->iExtensions->AppendL(myItemArray[i]->iExtensions->MdcaPoint(pp));		
								}
								
								
								iTypesListbox->iExtensionCount.Num(iTypesListbox->iExtensions->MdcaCount());
							}
							
							iTypesListbox->iIndex = myItemArray[i]->iIndex;
        					
        					if(myItemArray[i]->iHandler)
        						iTypesListbox->iExecutable.Copy(*myItemArray[i]->iHandler);
        					
        					iTypesListbox->iOwn = myItemArray[i]->iOwn;
        					
        					if(iTypesListbox->iExecutable.Length()
        					&& BaflUtils::Parse(iTypesListbox->iExecutable) == KErrNone)
        					{
        						TParsePtrC Hjelpper(iTypesListbox->iExecutable);
        						iTypesListbox->iHandlerApp.Copy(Hjelpper.NameAndExt());
        					
	        					RApaLsSession ls;
								if(ls.Connect() == KErrNone)
								{
									if(ls.GetAllApps() == KErrNone)
									{
										TInt Errnono(KErrNone);
										TApaAppInfo AppInfo;
							
										do
										{
											Errnono = ls.GetNextApp(AppInfo);
											if(AppInfo.iFullName == iTypesListbox->iExecutable)
											{
												iTypesListbox->iHandlerApp.Copy(AppInfo.iCaption);
												Errnono = 69696969;
											}
								
										}while(KErrNone == Errnono);
									}
									
							 	  	ls.Close();
								}
        					}
						}
					}
				}
			}	
				
			myItemArray.ResetAndDestroy();	
							
	    	iTypesListbox->ConstructFromResourceL(R_FILTYPES_SETTING);	    
			iTypesListbox->MakeVisible(ETrue);
		    iTypesListbox->SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
		    iTypesListbox->ActivateL();			
			iTypesListbox->LoadSettingsL();
		    iTypesListbox->DrawNow();
		}
		SetMenuL();
		DrawNow();
		break;
	case EFileShowFt:
		{
			TBuf<200> FileType;
			if(GetSelFileType(FileType))	
			{	
				delete iShowString;
				iShowString = NULL;
				iShowString = new(ELeave)CShowString();
				iShowString->ConstructL(FileType);
			}
		}
		SetMenuL();
		DrawNow();
		break;
	case ESSBack2:
		{
			delete iShowString;
			iShowString = NULL;
		}
		SetMenuL();
		DrawNow();
		break;
	default:
		if(iShowString)
    	{
	        iShowString->HandleViewCommandL(aCommand);
    	}
		break;
	}	
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CTypesContainer::ReReadMyTypesL(void) 
{
	delete iOwnFilTypArray;
	iOwnFilTypArray = NULL;
	iOwnFilTypArray = new(ELeave)CDesCArrayFlat(10);

	if(iMyDBClass)
	{
		RPointerArray<CFtASSItem> myItemArray;
				
		iMyDBClass->ReadDbItemsL(myItemArray);
		TFileName myHejlper;
		
		for(TInt i=0; i < myItemArray.Count(); i++)
		{
			if(myItemArray[i])
			{
				if(myItemArray[i]->iTypeName && myItemArray[i]->iOwn)
				{
					myHejlper.Copy(_L("\t"));
					myHejlper.Append(myItemArray[i]->iTypeName->Des());
				
					iOwnFilTypArray->AppendL(myHejlper);
				}
			}
		}	
			
		myItemArray.ResetAndDestroy();
	}
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
TBool CTypesContainer::GetSelFileType(TDes& aFileType) 
{
	TBool Ret(EFalse);
	
	if(CurrentListBox())
	{
		CDesCArray* itemArray = STATIC_CAST( CDesCArray* ,CurrentListBox()->Model()->ItemTextArray());
		TInt Curr = CurrentListBox()->CurrentItemIndex();	
		if(Curr >= 0 && itemArray)
		{
			if(itemArray->MdcaCount() > Curr)
			{
				TPtrC ItName,Item;
				Item.Set(itemArray->MdcaPoint(Curr));
				if(KErrNone == TextUtils::ColumnText(ItName,1,&Item))
				{				
					Ret = ETrue;
					aFileType.Copy(ItName);
				}
			}
		}
	}	
	
	return Ret;
}
/*
-----------------------------------------------------------------------

-----------------------------------------------------------------------
*/
CAknSingleStyleListBox* CTypesContainer::CurrentListBox(void) const
{
	CAknSingleStyleListBox* Ret = iSysListBox;
	if(iTabGroup)
	{
		if(iTabGroup->ActiveTabIndex() == 1)
		{
			Ret = iOwnListBox;
		}
	}
	
	return Ret;
}
				
/*
-----------------------------------------------------------------------

-----------------------------------------------------------------------
*/
TKeyResponse CTypesContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
	TKeyResponse Ret = EKeyWasNotConsumed;
	
	if(iTypesListbox)
	{
		switch (aKeyEvent.iCode)
	    {
		case EKeyDevice3:
			CEikonEnv::Static()->AppUiFactory()->MenuBar()->TryDisplayMenuBarL();
			break;
		default:
			if(CurrentListBox())
			{
				Ret = iTypesListbox->OfferKeyEventL(aKeyEvent,aType);
			}
			break;
		}
	}
	else if(iShowString)
	{
		Ret = iShowString->OfferKeyEventL(aKeyEvent,aType);
	}
	else
	{
		switch (aKeyEvent.iCode)
	    {
		case EKeyDevice3:
			CEikonEnv::Static()->AppUiFactory()->MenuBar()->TryDisplayMenuBarL();
			break;
		case '2':
			if(CurrentListBox())
			{
				CurrentListBox()->View()->MoveCursorL(CListBoxView::ECursorPreviousPage,CListBoxView::ENoSelection);
				CurrentListBox()->UpdateScrollBarsL();
				CurrentListBox()->DrawNow();
			}
			DrawNow();
			break;
		case '8':
			if(CurrentListBox())
			{
				CurrentListBox()->View()->MoveCursorL(CListBoxView::ECursorNextPage,CListBoxView::ENoSelection);
				CurrentListBox()->UpdateScrollBarsL();
				CurrentListBox()->DrawNow();
			}
			DrawNow();
			break;
		case EKeyRightArrow:
			if(iTabGroup)
			{
				if(iTabGroup->ActiveTabIndex() != 1)
				{
					iTabGroup->SetActiveTabByIndex(1);
				}
			}
			DrawNow();
			break;
		case EKeyLeftArrow:
			if(iTabGroup)
			{
				if(iTabGroup->ActiveTabIndex() != 0)
				{
					iTabGroup->SetActiveTabByIndex(0);
				}
			}
			DrawNow();
			break;
		case EKeyDownArrow:
		case EKeyUpArrow:
		default:
			if(CurrentListBox())
			{
				Ret = CurrentListBox()->OfferKeyEventL(aKeyEvent,aType);
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
void CTypesContainer::Draw(const TRect& /*aRect*/) const
{
	if(iBgContext)
	{
	 	CWindowGc& gc = SystemGc();
	  	MAknsSkinInstance* skin = AknsUtils::SkinInstance();
		AknsDrawUtils::Background( skin, iBgContext, this, gc, Rect() );
	}
}
/*
-------------------------------------------------------------------------

-------------------------------------------------------------------------
*/
TInt CTypesContainer::CountComponentControls() const
{
	if(iTypesListbox)
		return 1;
	if(iShowString && CurrentListBox())
		return 2;
	else if(CurrentListBox())
		return 1;
	else
		return 0;
}
/*
-------------------------------------------------------------------------

-------------------------------------------------------------------------
*/
CCoeControl* CTypesContainer::ComponentControl(TInt aIndex) const
{
	if(iTypesListbox)
		return iTypesListbox;
	if(iShowString && aIndex)
		return iShowString;
	else
		return CurrentListBox();
}

