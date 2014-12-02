/* Copyright (c) 2001 - 2006 , J.P. Silvennoinen. All rights reserved */

#include "Search_Settings.h"            // own definitions


#include "YuccaBrowser.h"
#include "YuccaBrowser.hrh"
#ifdef __YTOOLS_SIGNED
	#include <YuccaBrowser_2000713D.rsg>
#else
	#include <YuccaBrowser.rsg>
#endif

#include "IconHandler.h"
#include <aknselectionlist.h> 
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
#include <aknlistquerydialog.h> 
#include <stringloader.h> 
#include "Navipanel.h"
#include "Search_Container.h"
#include "YuccaBrowser_Container.h"




CAknSearhcListListbox ::CAknSearhcListListbox(CIconHandler& aIconHandler)
:iIconHandler(aIconHandler)
{
	
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
CAknSearhcListListbox::~CAknSearhcListListbox()
{		
	iSearchExludeTyp.ResetAndDestroy();
	iSearchIncludeTyp.ResetAndDestroy();
	iSearchIncludeTypOwn.ResetAndDestroy();
}				

// Settings
//
CAknSettingItem* CAknSearhcListListbox ::CreateSettingItemL(TInt aIdentifier)
{
	CAknSettingItem* settingItem = NULL;

	switch(aIdentifier)
	{
	case ESearchPath:
    	settingItem = new (ELeave) CAknTextSettingItem(aIdentifier,iSearchPath);
		break;
	case ESearchFname:
    	settingItem = new (ELeave) CAknTextSettingItem(aIdentifier,iSearchFname);
		break;
	case ESearchBigthan:
    	settingItem = new (ELeave) CAknTextSettingItem(aIdentifier,iSearchBigthantxt);
		break;
	case ESearchSmallThan:
    	settingItem = new (ELeave) CAknTextSettingItem(aIdentifier,iSearchSmallThantxt);
		break;
	case ESearchModBefore:
    	settingItem = new (ELeave) CAknTextSettingItem(aIdentifier,iSearchModBeforetxt);
		break;
	case ESearchModAfter:
    	settingItem = new (ELeave) CAknTextSettingItem(aIdentifier,iSearchModAftertxt);
		break;
	case ESearchCheckTypes:
    	settingItem = new (ELeave) CAknBinaryPopupSettingItem(aIdentifier,iSearchCheckTypes);
		break;
	case ESearchExludeTyp:
    	settingItem = new (ELeave) CAknTextSettingItem(aIdentifier,iSearchExludeTyptxt);
		break;
	case ESearchIncludeTyp:
    	settingItem = new (ELeave) CAknTextSettingItem(aIdentifier,iSearchIncludeTyptxt);
		break;
	case ESearchAttr:
    	settingItem = new (ELeave) CAknTextSettingItem(aIdentifier,iESearchAttrtxt);
		break;
	case ESearchString:
    	settingItem = new (ELeave) CAknTextSettingItem(aIdentifier,iSearchString);
		break;
	case ESearchUnicode:
    	settingItem = new (ELeave) CAknBinaryPopupSettingItem(aIdentifier,iSearchUnicode);
		break;
	case ESearchCaseSensitive:
    	settingItem = new (ELeave) CAknBinaryPopupSettingItem(aIdentifier,iSearchCaseSensitive);
		break;
	}	
	
	return settingItem;
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CAknSearhcListListbox::ConstructL(void)
{

}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CAknSearhcListListbox::SizeChanged()
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
void CAknSearhcListListbox::HandleListBoxEventL(CEikListBox* aListBox,TListBoxEvent aEventType)
{
	if(ListBox())
	{		
		if(ListBox()->CurrentItemIndex() == 6 // ESearchCheckTypes
		|| ListBox()->CurrentItemIndex() == 11 //ESearchCaseSensitive
		|| ListBox()->CurrentItemIndex() == 12)//ESearchUnicode
		{
			CAknSettingItemList::HandleListBoxEventL(aListBox,aEventType);
		}
		else
		{	
			TBuf<60> Hjelpper,Hjelpper2;
			StoreSettingsL();
			switch(ListBox()->CurrentItemIndex())
			{
			case 0:// ESearchPath
				{
					StringLoader::Load(Hjelpper,R_SH_STR_PATH);
					CAknTextQueryDialog* Dialog = CAknTextQueryDialog::NewL(iSearchPath,CAknQueryDialog::ENoTone);
					Dialog->PrepareLC(R_ASK_NAME2_DIALOG);
					Dialog->SetPromptL(Hjelpper);
					Dialog->RunLD();
				}
				break;
			case 1://ESearchFname
				{
					StringLoader::Load(Hjelpper,R_SH_STR_FILENAME);
					CAknTextQueryDialog* Dialog = CAknTextQueryDialog::NewL(iSearchFname,CAknQueryDialog::ENoTone);
					Dialog->PrepareLC(R_ASK_NAME_DIALOG);
					Dialog->SetPromptL(Hjelpper);
					Dialog->RunLD();
				}
				break;
			case 2://ESearchBigthan
				{
					StringLoader::Load(Hjelpper,R_SH_STR_BIGTHAN);
					CAknNumberQueryDialog* Dialog = CAknNumberQueryDialog::NewL(iSearchBigthan,CAknQueryDialog::ENoTone);
					Dialog->PrepareLC(R_BIG_DIALOG);
					Dialog->SetPromptL(Hjelpper);
					if(Dialog->RunLD())
					{
						GetSizeBuffer(iSearchBigthantxt,iSearchBigthan);
					}
				}
				break;
			case 3://ESearchSmallThan	
				{
					StringLoader::Load(Hjelpper,R_SH_STR_SMALTHAN);
					CAknNumberQueryDialog* Dialog = CAknNumberQueryDialog::NewL(iSearchSmallThan,CAknQueryDialog::ENoTone);
					Dialog->PrepareLC(R_BIG_DIALOG);
					Dialog->SetPromptL(Hjelpper);
					if(Dialog->RunLD())
					{
						GetSizeBuffer(iSearchSmallThantxt,iSearchSmallThan);
					}
				}
				break;
			case 4://ESearchModBefore
				{
					TBool TimeOk(EFalse);
					iSearchModBefore = GetTimeL(TimeOk,iSearchModBefore);
					if(TimeOk)
					{
						iSearchModBeforetxt.Zero();
						GetTimeBuffer(iSearchModBeforetxt,iSearchModBefore);
					}
				}
				break;
			case 5://ESearchModAfter
				{
					TBool TimeOk(EFalse);
					iSearchModAfter = GetTimeL(TimeOk,iSearchModAfter);
					if(TimeOk)
					{
						iSearchModAftertxt.Zero();
						GetTimeBuffer(iSearchModAftertxt,iSearchModAfter);
					}
				}
				break;
			case 7://ESearchExludeTyp
				{
					SelectFileTypesL(iSearchExludeTyp);
					
					iSearchExludeTyptxt.Zero();
					if(iSearchExludeTyp.Count())	
					{
						StringLoader::Load(Hjelpper,R_SH_STR_TYPES);
						iSearchExludeTyptxt.Num(iSearchExludeTyp.Count());
						iSearchExludeTyptxt.Append(Hjelpper);
					}
				}
				break;
			case 8://ESearchIncludeTyp, 
				{	
					SelectFileTypesL(iSearchIncludeTyp);
					iSearchIncludeTyptxt.Zero();
					
					if(iSearchIncludeTyp.Count())
					{
						StringLoader::Load(Hjelpper,R_SH_STR_TYPES);
						iSearchExludeTyptxt.Num(iSearchIncludeTyp.Count());
						iSearchExludeTyptxt.Append(Hjelpper);
					}
				}
				break;
			case 9://ESearchAttr//
				{
					CArrayFixFlat<TInt>* SelectedItems = new(ELeave)CArrayFixFlat<TInt>(10);
					CleanupStack::PushL(SelectedItems);
					CDesCArrayFlat* SelectArrray = new(ELeave)CDesCArrayFlat(20);
					CleanupStack::PushL(SelectArrray);
					
					if(iHidden)
					{
						SelectedItems->AppendL(0);
					}
					
					if(iSystem)
					{
						SelectedItems->AppendL(1);
					}
		
					
					if(iReadOnly)
					{
						SelectedItems->AppendL(2);
					}
					
					Hjelpper2.Copy(KtxOnePlusTab);
					StringLoader::Load(Hjelpper,R_SH_STR_ANDATRIBS);
					Hjelpper2.Append(Hjelpper);
					SelectArrray->AppendL(Hjelpper2);
					
					Hjelpper2.Copy(KtxOnePlusTab);
					StringLoader::Load(Hjelpper,R_SH_STR_SYSTEM);
					Hjelpper2.Append(Hjelpper);
					SelectArrray->AppendL(Hjelpper2);
					
					Hjelpper2.Copy(KtxOnePlusTab);
					StringLoader::Load(Hjelpper,R_SH_STR_HIDDEN);
					Hjelpper2.Append(Hjelpper);
					SelectArrray->AppendL(Hjelpper2);
					
					Hjelpper2.Copy(KtxOnePlusTab);
					StringLoader::Load(Hjelpper,R_SH_STR_READONLY);
					Hjelpper2.Append(Hjelpper);
					SelectArrray->AppendL(Hjelpper2);
	
					CAknListQueryDialog* dialog = new(ELeave)CAknListQueryDialog(SelectedItems);
					
					dialog->PrepareLC(R_ATTR_SELECTION_QUERY);
					
					dialog->SetItemTextArray(SelectArrray); 
					dialog->SetOwnershipType(ELbmDoesNotOwnItemArray); 
					dialog->SetIconArrayL(iIconHandler.GetSelectionIconArrayL()); 
				
					if (dialog->RunLD ())
					{
						iAndAttributes = iHidden = iSystem = iReadOnly = EFalse;
						for(TInt i=0; i < SelectedItems->Count(); i++)
						{
							switch(SelectedItems->At(i))
							{
							case 1:
								iSystem = ETrue;
								break;
							case 2:
								iHidden = ETrue;
								break;
							case 3:
								iReadOnly = ETrue;
								break;
							default:
								iAndAttributes = ETrue;
								break;
							}
						}
						
						GetAttribBuffer(iESearchAttrtxt,iHidden,iSystem,iReadOnly,iAndAttributes);
					}
				
					CleanupStack::PopAndDestroy(2);  //SelectedItems,SelectArrray					
				}
				break;
			case 10://ESearchString
				{
					StringLoader::Load(Hjelpper,R_SH_STR_SEARCFOR);
					CAknTextQueryDialog* Dialog = CAknTextQueryDialog::NewL(iSearchString,CAknQueryDialog::ENoTone);
					Dialog->PrepareLC(R_ASK_NAME_DIALOG);
					Dialog->SetPromptL(Hjelpper);
					Dialog->RunLD();
				}
				break;
			default:
				break;
			};
		
			LoadSettingsL();
		}
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CAknSearhcListListbox::ShowSelectedFileTypesL(void)
{
	if(ListBox()->CurrentItemIndex() == 7)
	{
		ShowTypesListL(iSearchExludeTyp);
	}
	else if(ListBox()->CurrentItemIndex() == 8)
	{
		ShowTypesListL(iSearchIncludeTyp);
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CAknSearhcListListbox::ShowTypesListL(RPointerArray<CFFileTypeItem>& aArray)
{
	if(aArray.Count())
	{
		CAknSingleGraphicPopupMenuStyleListBox* list = new(ELeave) CAknSingleGraphicPopupMenuStyleListBox;
	    CleanupStack::PushL(list);

		CAknPopupList* popupList = CAknPopupList::NewL(list,R_AVKON_SOFTKEYS_BACK);
	    CleanupStack::PushL(popupList);
		
	    list->ConstructL(popupList, CEikListBox::ELeftDownInViewRect);
		list->CreateScrollBarFrameL(ETrue);
	    list->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,CEikScrollBarFrame::EAuto);
	    
	    CDesCArrayFlat* Array = new (ELeave) CDesCArrayFlat(1);
		CleanupStack::PushL(Array);
		
//		RPointerArray<CFFileTypeItem> ItemTypeArray  = iIconHandler.GetItemTypeArray();
		
		TBuf<255> Hjelpper;
		for(TInt i=0; i < aArray.Count(); i++)
		{
			if(aArray[i])
			{
				if(aArray[i]->iIdString) 
				//&& aArray[i]->iName)
				{
					Hjelpper.Num(aArray[i]->iTypeId);
					Hjelpper.Append(_L("\t"));
					Hjelpper.Append(aArray[i]->iIdString->Des());
					
					Array->AppendL(Hjelpper);
				}
			}
		}
		
	    list->Model()->SetItemTextArray(Array);
	    CleanupStack::Pop();//Array
		list->Model()->SetOwnershipType(ELbmOwnsItemArray);
		
		list->ItemDrawer()->ColumnData()->SetIconArray(iIconHandler.GetIconArrayL());
		
		StringLoader::Load(Hjelpper,R_SH_STR_SELFILETYPES);

		popupList->SetTitleL(Hjelpper);
		popupList->ExecuteLD();
		
	    CleanupStack::Pop();            // popuplist
	    CleanupStack::PopAndDestroy();  // list
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CAknSearhcListListbox::ClearSelectedConditionL(void)
{
	StoreSettingsL();
	switch(ListBox()->CurrentItemIndex())
	{
	case 0://ESearchPath
		{
			iSearchPath.Copy(KtxAllDrives);
		}
		break;
	case 1://ESearchFname
		{
			iSearchFname.Copy(KtxStarDotStar);
		}
		break;
	case 2://ESearchBigthan
		{
			iSearchBigthan = 0;
			iSearchBigthantxt.Zero();
		}
		break;
	case 3://ESearchSmallThan	
		{
			iSearchSmallThan = 0;
			iSearchSmallThantxt.Zero();
		}
		break;
	case 4://ESearchModBefore
		{
			iSearchModBeforetxt.Zero();
		}
		break;
	case 5://ESearchModAfter
		{
			iSearchModAftertxt.Zero();
		}
		break;
	case 6://ESearchCheckTypes
		{
			iSearchCheckTypes = ETrue;
		}
		break;
	case 7://ESearchExludeTyp
		{
			iSearchExludeTyptxt.Zero();
			iSearchExludeTyp.ResetAndDestroy();
		}
		break;
	case 8://ESearchIncludeTyp, 
		{
			iSearchIncludeTyptxt.Zero();
			iSearchIncludeTyp.ResetAndDestroy();
		}
		break;
	case 9://ESearchAttr//
		{
			iAndAttributes = iHidden = iSystem = iReadOnly = EFalse;
			iESearchAttrtxt.Zero();	
		}
		break;
	case 10://ESearchString
		{
			iSearchString.Zero();
		}
		break;
	case 11://ESearchCaseSensitive
		{
			iSearchCaseSensitive = EFalse;
		}
		break;
	case 12://ESearchUnicode
		{
			iSearchUnicode = EFalse;
		}
		break;
	default:
		break;
	};

	LoadSettingsL();
}
/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
void CAknSearhcListListbox::GetSizeBuffer(TDes& aBuffer,TInt aSize)
{
	TBuf<60> Hjelpper;
	
	if(aSize > (1024* 1024 * 10))
	{
		StringLoader::Load(Hjelpper,R_SH_STR_MB);
		aBuffer.Num((aSize / (1024* 1024)));
		aBuffer.Append(Hjelpper);
	}
	else if(aSize > (1024* 10))
	{
		StringLoader::Load(Hjelpper,R_SH_STR_KB);
		aBuffer.Num(aSize / 1024);
		aBuffer.Append(Hjelpper);
	}
	else
	{
		StringLoader::Load(Hjelpper,R_SH_STR_BYTES);
		aBuffer.Num(aSize);
		aBuffer.Append(Hjelpper);
	}
}
/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
void CAknSearhcListListbox::GetAttribBuffer(TDes& aBuffer,TBool aHidden, TBool aSystem,TBool aRo,TBool aAnd)
{
	TBuf<60> Hjelpper;
	aBuffer.Zero();
	if(aHidden)
	{
		StringLoader::Load(iESearchAttrtxt,R_SH_STR_HIDD);
	}
	
	if(aSystem)
	{
		if(aBuffer.Length())
		{
			if(aAnd)
				StringLoader::Load(Hjelpper,R_SH_STR_AND);
			else
				StringLoader::Load(Hjelpper,R_SH_STR_OR);
	
			aBuffer.Append(Hjelpper);
		}
		
		StringLoader::Load(Hjelpper,R_SH_STR_SYS);
		aBuffer.Append(Hjelpper);
	}
	
	if(aRo)
	{
		if(aBuffer.Length())
		{
			if(aAnd)
				StringLoader::Load(Hjelpper,R_SH_STR_AND);
			else
				StringLoader::Load(Hjelpper,R_SH_STR_OR);
	
			aBuffer.Append(Hjelpper);
		}
		
		StringLoader::Load(Hjelpper,R_SH_STR_RO);
		aBuffer.Append(Hjelpper);
	}
}
/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
void CAknSearhcListListbox::GetTimeBuffer(TDes& aBuffer,TTime aTime)
{
	TInt Dayyy = (aTime.DateTime().Day() + 1);
	TInt Monthh= (aTime.DateTime().Month() + 1);
	TInt Yrrrrr= (aTime.DateTime().Year());
	TInt Hrrrrr= (aTime.DateTime().Hour());
	TInt Minsss= (aTime.DateTime().Minute());

	if(Yrrrrr >= 2000)
	{
		Yrrrrr = Yrrrrr - 2000;
	}
	else if(Yrrrrr >= 1900)
	{
		Yrrrrr = Yrrrrr - 1900;
	}
	
	aBuffer.Format(KtxFormatTimeBuf,Dayyy,Monthh,Yrrrrr,Hrrrrr,Minsss);
}
/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
TTime CAknSearhcListListbox::GetTimeL(TBool& Ok,TTime aInitialTime)
{
	Ok = EFalse;
	TTime AlDate = aInitialTime;
    TTime AlTime = aInitialTime;
    
  
    CAknMultiLineDataQueryDialog* dlg = CAknMultiLineDataQueryDialog::NewL(AlDate,AlTime);
    if(dlg->ExecuteLD(R_ALARM_TIME_QUERY))
    {
    	Ok = ETrue;
    }

	TDateTime NewAlarm(AlDate.DateTime().Year(),AlDate.DateTime().Month(),AlDate.DateTime().Day(),AlTime.DateTime().Hour(),AlTime.DateTime().Minute(), 0,0);
	TTime RetAlarm(NewAlarm);
	return RetAlarm;
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CAknSearhcListListbox::SelectFileTypesL(RPointerArray<CFFileTypeItem>& aArray)
{
	CArrayFixFlat<TInt>* SelectedItems = new(ELeave)CArrayFixFlat<TInt>(10);
	CleanupStack::PushL(SelectedItems);
	
	CDesCArrayFlat* SelectArrray = new(ELeave)CDesCArrayFlat(20);
	CleanupStack::PushL(SelectArrray);
	
	RPointerArray<CFFileTypeItem> ItemTypeArray  = iIconHandler.GetItemTypeArray();
	
	CAknListQueryDialog* dialog = new(ELeave)CAknListQueryDialog(SelectedItems);
	dialog->PrepareLC(R_TYPES_SELECTION_QUERY);
	
	TBuf<255> Hjelpper;
	for(TInt i=0; i < ItemTypeArray.Count(); i++)
	{
		if(ItemTypeArray[i])
		{
			if(ItemTypeArray[i]->iIdString) 
		//	&& ItemTypeArray[i]->iName)
			{
				Hjelpper.Num(ItemTypeArray[i]->iTypeId);
				Hjelpper.Append(_L("\t"));
				Hjelpper.Append(ItemTypeArray[i]->iIdString->Des());
				
				SelectArrray->AppendL(Hjelpper);
				
				for(TInt pp=0; pp < aArray.Count(); pp++)
				{
					if(aArray[pp])
					{
						if(aArray[pp]->iIdString)
						{
						/*	if(ItemTypeArray[i]->iPartialSting
							&& ItemTypeArray[i]->iIdString->Des().Length()  > aArray[pp]->iIdString->Des().Length())
							{
								if(aArray[pp]->iIdString->Des() == ItemTypeArray[i]->iIdString->Des().Left(aArray[pp]->iIdString->Des().Length()))
								{
									dialog->ListBox()->View()->ToggleItemL((SelectArrray->MdcaCount() - 1));
									break;
								}
							}
							else */
							if(aArray[pp]->iIdString->Des() == ItemTypeArray[i]->iIdString->Des())
							{
								dialog->ListBox()->View()->ToggleItemL((SelectArrray->MdcaCount() - 1));
								break;
							}
						}
					}
				}
			}
		}
	}

	
	
	dialog->SetItemTextArray(SelectArrray); 
	dialog->SetOwnershipType(ELbmDoesNotOwnItemArray); 
	dialog->SetIconArrayL(iIconHandler.GetIconArrayL()); 

	if (dialog->RunLD ())
	{
		aArray.ResetAndDestroy();
		for(TInt i=0; i < SelectedItems->Count(); i++)
		{
			TInt SelType = SelectedItems->At(i);
		
			if(SelType >=0 && SelType < ItemTypeArray.Count())
			{
				if(ItemTypeArray[SelType])
				{
					if(ItemTypeArray[SelType]->iIdString) 
				//	&& ItemTypeArray[SelType]->iName)
					{	
						CFFileTypeItem* newItem = new(ELeave)CFFileTypeItem();
						CleanupStack::PushL(newItem);
				
				//		newItem->iPartialSting 	= ItemTypeArray[SelType]->iPartialSting;
						newItem->iIdString 		= ItemTypeArray[SelType]->iIdString->Des().AllocL(); 
					//	newItem->iName 			= ItemTypeArray[SelType]->iName->Des().AllocL(); 
						newItem->iTypeId		= ItemTypeArray[SelType]->iTypeId;
			
						CleanupStack::Pop();//newItem
						aArray.Append(newItem);
					}
				}
			}
		}
	}

	CleanupStack::PopAndDestroy(2);  //SelectedItems,SelectArrray
}

			
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CSearchSettings::CSearchSettings(CEikButtonGroupContainer* aCba,CIconHandler& aIconHandler,CYuccaNavi* aYuccaNavi)
:iListBox(NULL),iCba(aCba),iIconHandler(aIconHandler),iYuccaNavi(aYuccaNavi)
{	
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
CSearchSettings::~CSearchSettings()
{		
	delete iSearchContainer;
	iSearchContainer = NULL;
	
	delete iListBox;
	iListBox = NULL;
	
	delete iSearchValues;
	iSearchValues = NULL;
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CSearchSettings::ConstructL(const TDesC& aPath,CSearchValues* aValues)
{
	if(aValues)
	{
		iSearchValues = aValues;
	}
	else
	{
		iSearchValues = GetSearchValuesL();
	}
	
	iAskToSave = EFalse;
	
	if(iSearchValues)
	{
		if(iSearchValues->iPath == NULL)
		{
			if(aPath.Length())
			{
				iAskToSave = ETrue;
				iSearchValues->iPath = aPath.AllocL();
			}
			else
			{
				iSearchValues->iPath = KtxAllDrives().AllocL();
			}
		}
		else if(iSearchValues->iPath->Des() != aPath)
		{
			HBufC* Buff= StringLoader::LoadLC(R_STR_STARTNEWSCRH);
			
			CAknQueryDialog* dlg = CAknQueryDialog::NewL();
			if(dlg->ExecuteLD(R_QUERY,*Buff))
			{
				HBufC* Buff2= StringLoader::LoadLC(R_STR_RESETSEARCH);
				CAknQueryDialog* dlg2 = CAknQueryDialog::NewL();
				if(dlg2->ExecuteLD(R_QUERY,*Buff2))
				{
					delete iSearchValues;
					iSearchValues = NULL;
					iSearchValues = GetSearchValuesL();
				}
				
				CleanupStack::PopAndDestroy(Buff2);

				delete iSearchValues->iPath;
				iSearchValues->iPath = NULL;
				
				iAskToSave = ETrue;
				
				if(aPath.Length())
				{
					iSearchValues->iPath = aPath.AllocL();
				}
				else
				{
					iSearchValues->iPath = KtxAllDrives().AllocL();
				}
				
				iSearchValues->iItemArray.ResetAndDestroy();
			}
			
			CleanupStack::PopAndDestroy(Buff);
		}
	}
	
    CreateWindowL();
	SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
	
	MakeListboxL();	
	
	SetValuesL(iSearchValues);
	
	ActivateL();
	
	if(aValues)
	{
		if(aValues->iItemArray.Count())
		{
			if(aValues->iPath)
			{
		    	iSearchContainer = CSearchContainer::NewL(CEikonEnv::Static()->EikAppUi()->ClientRect(),iCba,*iSearchValues,iIconHandler,*aValues->iPath,iYuccaNavi);
			}
			else
			{
		    	iSearchContainer = CSearchContainer::NewL(CEikonEnv::Static()->EikAppUi()->ClientRect(),iCba,*iSearchValues,iIconHandler,KtxAllDrives,iYuccaNavi);	
			}
		}
	}
	
	SetMenuL();
	DrawNow();
}
/*
-----------------------------------------------------------------------------

----------------------------------------------------------------------------
*/
TBool CSearchSettings::AskToSave()
{ 
	if(iSearchValues->iItemArray.Count())
	{
		return ETrue;
	}
	else
	{
		return EFalse;
	}
}
/*
-----------------------------------------------------------------------------

----------------------------------------------------------------------------
*/
void CSearchSettings::SetValuesL(CSearchValues* aValues)
{
	if(aValues && iListBox)
	{
		TBuf<60> Hjelpper;
		
		iListBox->iSearchPath.Zero();
		if(aValues->iPath)
		{
			iListBox->iSearchPath.Copy(aValues->iPath->Des());	
		}
		
		iListBox->iSearchFname.Copy(KtxStarDotStar);
		if(aValues->iWildName)
		{
			iListBox->iSearchFname.Copy(aValues->iWildName->Des());	
		}
		
		iListBox->iSearchString.Zero();
		if(aValues->iSearchString)
		{
			iListBox->iSearchString.Copy(aValues->iSearchString->Des());
		}
		else if(aValues->iSearchString8)
		{
			iListBox->iSearchString.Copy(aValues->iSearchString8->Des());
		}
		
		iListBox->iESearchAttrtxt.Zero();
		
	    iListBox->iHidden 				= aValues->iHidden;
	    iListBox->iSystem 				= aValues->iSystem;
	    iListBox->iReadOnly 			= aValues->iReadOnly;
	    iListBox->iAndAttributes		= aValues->iUseAndAttrib;
	    
	    iListBox->GetAttribBuffer(iListBox->iESearchAttrtxt,iListBox->iHidden,iListBox->iSystem,iListBox->iReadOnly,iListBox->iAndAttributes);
	        
	    iListBox->iSearchCheckTypes 	= aValues->iTypeCheckEnabled;
	    iListBox->iSearchUnicode 		= aValues->iUnicode;
		iListBox->iSearchCaseSensitive 	= aValues->iCaseSensitive;
	    
	    iListBox->iSearchModBeforetxt.Zero();
	    if(aValues->iTimeBeforeSet)
	    {
		    iListBox->iSearchModBefore		= aValues->iTimeBefore;
			iListBox->GetTimeBuffer(iListBox->iSearchModBeforetxt,iListBox->iSearchModBefore);
	    }
	    
	    iListBox->iSearchModAftertxt.Zero();
	    
	    if(aValues->iTimeAfterSet)
	    {
		    iListBox->iSearchModAfter		= aValues->iTimeAfter;    
			iListBox->GetTimeBuffer(iListBox->iSearchModAftertxt,iListBox->iSearchModAfter);
	    }
	    
	    iListBox->iSearchIncludeTyp.ResetAndDestroy();
	    CopyTypeArrayL(aValues->iIncludeArray,iListBox->iSearchIncludeTyp);
	 	
	 	iListBox->iSearchIncludeTyptxt.Zero();
	 	if(iListBox->iSearchIncludeTyp.Count())
	 	{
	 		StringLoader::Load(Hjelpper,R_SH_STR_TYPES);
			iListBox->iSearchIncludeTyptxt.Num(iListBox->iSearchIncludeTyp.Count());
			iListBox->iSearchIncludeTyptxt.Append(Hjelpper);
	 	}
	 
	 	iListBox->iSearchExludeTyp.ResetAndDestroy();
	    CopyTypeArrayL(aValues->iExcludeArray,iListBox->iSearchExludeTyp);
	    
	    iListBox->iSearchExludeTyptxt.Zero();
	 	if(iListBox->iSearchExludeTyp.Count())
	 	{
	 		StringLoader::Load(Hjelpper,R_SH_STR_TYPES);
			iListBox->iSearchExludeTyptxt.Num(iListBox->iSearchExludeTyp.Count());
			iListBox->iSearchExludeTyptxt.Append(Hjelpper);	
	 	}
	 	
	    iListBox->iSearchSmallThan = aValues->iSmaThan;
	    iListBox->iSearchSmallThantxt.Zero();
	    
	    if(iListBox->iSearchSmallThan > 0)
	    {
	    	iListBox->GetSizeBuffer(iListBox->iSearchSmallThantxt,iListBox->iSearchSmallThan);
	    }

		iListBox->iSearchBigthan = aValues->iBigThan;
		iListBox->iSearchBigthantxt.Zero();
	    if(iListBox->iSearchBigthan > 0)
	    {
	    	iListBox->GetSizeBuffer(iListBox->iSearchBigthantxt,iListBox->iSearchBigthan);
	    }
	     
		iListBox->LoadSettingsL();
	}
	
	DrawNow();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CSearchSettings::CopyTypeArrayL(RPointerArray<CFFileTypeItem>& aSource,RPointerArray<CFFileTypeItem>& aTarget)
{
	for(TInt i=0; i < aSource.Count(); i++)
	{
		if(aSource[i])
		{
			if(aSource[i]->iIdString) 
		//	&& aSource[i]->iName)
			{
				CFFileTypeItem* newItem = new(ELeave)CFFileTypeItem();
				CleanupStack::PushL(newItem);
		
		//		newItem->iPartialSting 	= aSource[i]->iPartialSting;
				newItem->iIdString 		= aSource[i]->iIdString->Des().AllocL(); 
		//		newItem->iName 			= aSource[i]->iName->Des().AllocL(); 
				newItem->iTypeId		= aSource[i]->iTypeId;
	
				CleanupStack::Pop();//newItem
				aTarget.Append(newItem);
			}
		}
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CSearchSettings::MakeListboxL(void)
{			
    delete iListBox;
    iListBox = NULL;
    iListBox = new (ELeave) CAknSearhcListListbox(iIconHandler);
    iListBox->SetMopParent(this); 
    iListBox->ConstructL();
    
    // set default values...
    iListBox->iSearchFname.Copy(KtxStarDotStar);
    iListBox->iHidden = EFalse;
    iListBox->iSystem = EFalse;
    iListBox->iReadOnly = EFalse;
    iListBox->iSearchCheckTypes = ETrue;
    iListBox->iSearchModBefore.HomeTime();
    iListBox->iSearchModAfter.HomeTime();
    iListBox->iSearchExludeTyp.ResetAndDestroy();
    iListBox->iSearchIncludeTyp.ResetAndDestroy();
    iListBox->iSearchUnicode = EFalse;
	iListBox->iSearchCaseSensitive = EFalse;
    	
	iListBox->ConstructFromResourceL(R_SEARCH_SETTING);

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
void CSearchSettings::SizeChanged()
{
//	MakeListboxL();	
	
	if(iListBox)
	{
		iListBox->SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
	}
	
	if(iSearchContainer)
	{
		iSearchContainer->SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
	}
	
	SetMenuL();
	DrawNow();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CSearchValues* CSearchSettings::GetSearchValues(void)
{
	CSearchValues* RetTmp(iSearchValues);
	iSearchValues = NULL;
	
	return RetTmp;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CSearchValues* CSearchSettings::GetSearchValuesL(void)
{
	CSearchValues* NewSearch = new(ELeave)CSearchValues();
	CleanupStack::PushL(NewSearch);
	
	NewSearch->iTypeCheckEnabled = ETrue; 
	
	if(iListBox)
	{
		// first get the latest search values
		iListBox->StoreSettingsL();
		
		if(iListBox->iSearchPath.Length())
		{
			NewSearch->iPath = iListBox->iSearchPath.AllocL();
		}
		
		if(iListBox->iSearchFname.Length())
		{
			NewSearch->iWildName = iListBox->iSearchFname.AllocL();
		}
		
		if(iListBox->iSearchString.Length())
		{
			NewSearch->iCaseSensitive = iListBox->iSearchCaseSensitive;
			NewSearch->iUnicode = iListBox->iSearchUnicode;
		
			TBuf<50> UniBuf;
			
			if(iListBox->iSearchUnicode)
			{
				if(iListBox->iSearchCaseSensitive)
					UniBuf.Copy(iListBox->iSearchString);
				else
					UniBuf.CopyLC(iListBox->iSearchString);
			
				NewSearch->iSearchString = UniBuf.AllocL();
			}
			else
			{
				if(iListBox->iSearchCaseSensitive)
					UniBuf.Copy(iListBox->iSearchString);
				else
					UniBuf.CopyLC(iListBox->iSearchString);
			
				TBuf8<50> NonUniBuf;
				NonUniBuf.Copy(UniBuf);
				
				NewSearch->iSearchString8 = NonUniBuf.AllocL();
			}
		}

		NewSearch->iTypeCheckEnabled = iListBox->iSearchCheckTypes;
		
		if(iListBox->iSearchCheckTypes && iListBox->iSearchExludeTyp.Count())
		{
			for(TInt i=0; i < iListBox->iSearchExludeTyp.Count(); i++)
			{
				if(iListBox->iSearchExludeTyp[i])
				{
					if(iListBox->iSearchExludeTyp[i]->iIdString) 
				//	&& iListBox->iSearchExludeTyp[i]->iName)
					{
						CFFileTypeItem* newItem = new(ELeave)CFFileTypeItem();
						CleanupStack::PushL(newItem);
				
					//	newItem->iPartialSting 	= iListBox->iSearchExludeTyp[i]->iPartialSting;
						newItem->iIdString 		= iListBox->iSearchExludeTyp[i]->iIdString->Des().AllocL(); 
					//	newItem->iName 			= iListBox->iSearchExludeTyp[i]->iName->Des().AllocL(); 
						newItem->iTypeId		= iListBox->iSearchExludeTyp[i]->iTypeId;
			
						CleanupStack::Pop();//newItem
						NewSearch->iExcludeArray.Append(newItem);
					}
				}
			}		
		}
		
		if(iListBox->iSearchCheckTypes && iListBox->iSearchIncludeTyp.Count())
		{
			for(TInt i=0; i < iListBox->iSearchIncludeTyp.Count(); i++)
			{
				if(iListBox->iSearchIncludeTyp[i])
				{
					if(iListBox->iSearchIncludeTyp[i]->iIdString) 
				//	&& iListBox->iSearchIncludeTyp[i]->iName)
					{
						CFFileTypeItem* newItem = new(ELeave)CFFileTypeItem();
						CleanupStack::PushL(newItem);
				
					//	newItem->iPartialSting 	= iListBox->iSearchIncludeTyp[i]->iPartialSting;
						newItem->iIdString 		= iListBox->iSearchIncludeTyp[i]->iIdString->Des().AllocL(); 
					//	newItem->iName 			= iListBox->iSearchIncludeTyp[i]->iName->Des().AllocL(); 
						newItem->iTypeId		= iListBox->iSearchIncludeTyp[i]->iTypeId;
			
						CleanupStack::Pop();//newItem
						NewSearch->iIncludeArray.Append(newItem);
					}
				}
			}		
		}
		
		NewSearch->iBigThan = iListBox->iSearchBigthan;
		NewSearch->iSmaThan = iListBox->iSearchSmallThan;
		
		NewSearch->iUseAndAttrib	= iListBox->iAndAttributes;
		NewSearch->iHidden			= iListBox->iHidden;
		NewSearch->iReadOnly		= iListBox->iReadOnly;
		NewSearch->iSystem			= iListBox->iSystem;
		
		NewSearch->iTimeBeforeSet = EFalse;
		NewSearch->iTimeAfterSet  = EFalse;
		if(iListBox->iSearchModAftertxt.Length())
		{
			NewSearch->iTimeAfterSet  = ETrue;
			NewSearch->iTimeAfter = iListBox->iSearchModAfter;
		}
		
		if(iListBox->iSearchModBeforetxt.Length())
		{
			NewSearch->iTimeBeforeSet = ETrue;
			NewSearch->iTimeBefore = iListBox->iSearchModBefore;
		}
		
		NewSearch->iItemArray.ResetAndDestroy();
	}
	
	CleanupStack::Pop();//NewSearch
	return NewSearch;
}

/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
void CSearchSettings::PageUpDown(TBool aUp)
{
	if(iSearchContainer)
	{
		iSearchContainer->PageUpDown(aUp);
	}
}

/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
void CSearchSettings::HandleViewCommandL(TInt aCommand)
{
	switch(aCommand)
    {
    case ESearchClearREsults:
    	if(iSearchValues)
    	{
    		iSearchValues->iItemArray.ResetAndDestroy();
    	}
    	break;
	case ESearchBackToResults:
		if(iSearchValues)
		{
			delete iSearchContainer;
			iSearchContainer = NULL;
			
			if(iSearchValues->iPath)
			{
		    	iSearchContainer = CSearchContainer::NewL(CEikonEnv::Static()->EikAppUi()->ClientRect(),iCba,*iSearchValues,iIconHandler,*iSearchValues->iPath,iYuccaNavi);
			}
			else
			{
		    	iSearchContainer = CSearchContainer::NewL(CEikonEnv::Static()->EikAppUi()->ClientRect(),iCba,*iSearchValues,iIconHandler,KtxAllDrives,iYuccaNavi);	
			}
		}
		SetMenuL();
    	DrawNow();
		break;
    case EBackFromSearchB:
    	{
    		delete iSearchContainer;
    		iSearchContainer = NULL;
    		
    		if(iYuccaNavi)
    		{
    			iYuccaNavi->SetTextL(KNullDesC);
    		}
    	}
    	SetMenuL();
    	DrawNow();
    	break;
	case ESearchStart:
		if(iListBox)
		{
			iAskToSave = ETrue;
			delete iSearchValues;
			iSearchValues = NULL;
			iSearchValues = GetSearchValuesL();
			if(iSearchValues->iPath)
			{
		    	iSearchContainer = CSearchContainer::NewL(CEikonEnv::Static()->EikAppUi()->ClientRect(),iCba,*iSearchValues,iIconHandler,*iSearchValues->iPath,iYuccaNavi);
			}
			else
			{
		    	iSearchContainer = CSearchContainer::NewL(CEikonEnv::Static()->EikAppUi()->ClientRect(),iCba,*iSearchValues,iIconHandler,KtxAllDrives,iYuccaNavi);	
			}
		}
		SetMenuL();
    	DrawNow();
		break;
    case ESearchClearCondition:
    	if(iListBox)
    	{
    		iListBox->ClearSelectedConditionL();
    	}
    	break;
    case ESearchShowTypes:
    	if(iListBox)
    	{
    		iListBox->ShowSelectedFileTypesL();
    	}
    	break;
    default:
    	if(iSearchContainer)
    	{
    		iSearchContainer->HandleViewCommandL(aCommand);
    	}
    	break;
    }
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
TKeyResponse CSearchSettings::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
	TKeyResponse Ret = EKeyWasNotConsumed;

	if(iSearchContainer)
	{
		iSearchContainer->OfferKeyEventL(aKeyEvent,aType);
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
TInt CSearchSettings::CountComponentControls() const
{	
	if(iSearchContainer)
	{
		return iSearchContainer->CountComponentControls ();
	}
	else if(iListBox)
		return iListBox->CountComponentControls ();
	else
		return 0;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CCoeControl* CSearchSettings::ComponentControl( TInt aIndex) const
{
	if(iSearchContainer)
	{
		return iSearchContainer->ComponentControl(aIndex); 
	}
	else
	{
		return iListBox->ComponentControl(aIndex); 
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CSearchSettings::Draw(const TRect& aRect) const
{
	CWindowGc& gc = SystemGc();
	gc.Clear(aRect); 
}

/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CSearchSettings::GetCurrSelFileL(TDes& aFileName,TInt& aFileType)
{
	if(iSearchContainer)
	{
		iSearchContainer->GetCurrSelFileL(aFileName,aFileType);
	}
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CSearchSettings::AddSelectedFilesL(CDesCArray* aArray,TBool AlsoFolders)
{
	if(iSearchContainer && aArray)
	{
		iSearchContainer->AddSelectedFilesL(aArray,AlsoFolders);
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CSearchSettings::SetMenuL(void)
{	
	if(iSearchContainer)
	{
		iSearchContainer->SetMenuL();
	}
	else
	{
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->SetMenuTitleResourceId(R_SEARCH_MENUBAR);	
		if(iCba)
		{
			iCba->SetCommandSetL(R_SEARCH_CBA);				
			iCba->DrawDeferred();
		}
	}
}

/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
void CSearchSettings::InitMenuPanelL(TInt aResourceId, CEikMenuPane* aMenuPane)
{
	if(R_SEARCH_MENU == aResourceId)
	{
		TBool Dim(ETrue);
		if(iListBox)
		{
			TInt CurrInd = iListBox->ListBox()->CurrentItemIndex();
			
			if(CurrInd == 7 && iListBox->iSearchExludeTyp.Count())
			{
				Dim = EFalse;
			}
			else if(CurrInd == 8 && iListBox->iSearchIncludeTyp.Count())
			{
				Dim = EFalse;
			}
		}
	
		if (Dim)
		{
			aMenuPane->SetItemDimmed(ESearchShowTypes,ETrue);
		}
		
		TBool DimSearchres(ETrue);
		
		if(iSearchValues)
		{
			if(iSearchValues->iItemArray.Count())
			{
				DimSearchres = EFalse;
			}
		}
		
		if (DimSearchres)
		{
			aMenuPane->SetItemDimmed(ESearchClearREsults,ETrue);
			aMenuPane->SetItemDimmed(ESearchBackToResults,ETrue);
		}
	}

	
	if(iSearchContainer)
	{
		iSearchContainer->InitMenuPanelL(aResourceId,aMenuPane);
	}
}


