/* 
	Copyright (c) 2006-2007, 
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
#include <aknmessagequerydialog.h>
#include <akntitle.h> 
#include <akncontext.h> 
#include <APGICNFL.H>
#include <stringloader.h> 
#include "Selection_Container.h"

#include "YuccaBrowser.h"
#include "YuccaBrowser.hrh"
#include "Navipanel.h"

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
CSelectContainer* CSelectContainer::NewL(CFFolderReader& aReader,MYBrowserFileHandlerUtils& aUtils,TSettingsItem aSettings,TBool aFolderSel,const TDesC& aFolder,const TDesC& aTitle,CDesCArrayFlat* aTypeFilter)
    {
    CSelectContainer* self = CSelectContainer::NewLC(aReader,aUtils,aSettings,aFolderSel,aFolder,aTitle,aTypeFilter);
    CleanupStack::Pop(self);
    return self;
    }
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
CSelectContainer* CSelectContainer::NewLC(CFFolderReader& aReader,MYBrowserFileHandlerUtils& aUtils,TSettingsItem aSettings,TBool aFolderSel,const TDesC& aFolder,const TDesC& aTitle,CDesCArrayFlat* aTypeFilter)
    {
    CSelectContainer* self = new (ELeave) CSelectContainer(aReader,aUtils,aSettings,aFolderSel);
    CleanupStack::PushL(self);
    self->ConstructL(aFolder,aTitle,aTypeFilter);
    return self;
    }
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
CSelectContainer::CSelectContainer(CFFolderReader& aReader,MYBrowserFileHandlerUtils& aUtils,TSettingsItem aSettings,TBool aFolderSel)
:iFFolderReader(aReader),iUtils(aUtils),iSettings(aSettings),iFolderSel(aFolderSel)
    {
	
    }
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
CSelectContainer::~CSelectContainer()
{
	if(iProgressDialog)
	{
		iProgressDialog->ProcessFinishedL(); 
	}

	iProgressDialog = NULL;
	iProgressInfo = NULL;
	
	delete iSelectionBox;
	iSelectionBox = NULL;	
	
	delete iPasteArray;
	iPasteArray = NULL;
}

/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CSelectContainer::ConstructL(const TDesC& aFolder,const TDesC& aTitle,CDesCArrayFlat* aTypeFilter)
{
    CreateWindowL();
	
	if(aTypeFilter)
	{
		iFFolderReader.SetTypeFilterArrayL(*aTypeFilter);
	}
	
	iPasteArray = new(ELeave)CDesCArrayFlat(10);
	
    SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
	
	CEikStatusPane*		sp = ((CAknAppUi*)iEikonEnv->EikAppUi())->StatusPane();
	if(sp && aTitle.Length())
	{	
		CAknTitlePane* TitlePane = STATIC_CAST(CAknTitlePane*,sp->ControlL(TUid::Uid(EEikStatusPaneUidTitle)));
		TitlePane->SetTextL(aTitle);
		
		sp->DrawNow();
	}
			
	StartReadingFolder(aFolder);
	
	ActivateL();
	SetMenuL();
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CSelectContainer::SetCallBack(MYbSelectorCallBack* aCallBack)
{
	iCallBack = aCallBack;
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
TBool CSelectContainer::UpdateCallBackL(TBool aCancel)
{
	TBool Ret(EFalse);
	
	if(aCancel)
	{
		if(iCallBack)
		{
			Ret = ETrue;
			iCallBack->SelectorCancelledL();
		}
	}
	else if(iFolderSel && iCallBack)
	{
		TFileName Hjelpper;
	
		HBufC* CurrPath = iFFolderReader.CurrentPath();
		if(CurrPath)
		{
			Hjelpper.Copy(CurrPath->Des());
		}
		
		if(iFolderSel)
		{
			AppendFileName(Hjelpper,-1);
		}
		Ret = ETrue;
		iCallBack->SelectedFolderL(Hjelpper);
	}
	else if(iCallBack)
	{
		if(iPasteArray == NULL)
		{
			iPasteArray = new(ELeave)CDesCArrayFlat(10);
		}
		
		Ret = ETrue;
		iCallBack->SelectedFilesL(*iPasteArray);
	}
	
	return Ret;
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CSelectContainer::AddSelectedFilesL(void)
{
	if(iSelectionBox)
	{
		if(iPasteArray == NULL)
		{
			iPasteArray = new(ELeave)CDesCArrayFlat(10);
		}
	
		TFileName AddFileName;
		HBufC* CurrPath = iFFolderReader.CurrentPath();
		if(CurrPath)
		{
			if(CurrPath->Des().Length())
			{
				TBool NotSelection(ETrue);
				
				const CListBoxView::CSelectionIndexArray* SelStuff = iSelectionBox->SelectionIndexes();
				if(SelStuff)
				{
					if(SelStuff->Count())
					{
						NotSelection = EFalse;
						
						for(TInt i=0; i < SelStuff->Count(); i++)
						{
							TInt CurrFilType(CFFileItem::EFDriveFile);

							GetFileType(CurrFilType, -1);
							
							if(CurrFilType >= CFFileItem::EFJustFile)
							{
								AddFileName.Copy(CurrPath->Des());	
								if(AppendFileName(AddFileName,SelStuff->At(i)))
								{
									iPasteArray->AppendL(AddFileName);
								}
							}
						}
					}
					
				}
				
				if(NotSelection)
				{
					TInt CurrFilType(CFFileItem::EFDriveFile);

					GetFileType(CurrFilType, -1);
					
					if(CurrFilType == CFFileItem::EFDriveFile)
					{
						//do nothing
					}
					else if(CurrFilType == CFFileItem::EFFolderFile)
					{
						//do nothing
					}
					else // so select current file..
					{
						AddFileName.Copy(CurrPath->Des());
						if(AppendFileName(AddFileName,-1))
						{
							iPasteArray->AppendL(AddFileName);
						}
					}
				}
			}
		}
		
		iSelectionBox->ClearSelection();
	}
	
	DrawNow();
}

/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CSelectContainer::FolderReadOnProcessL(TInt aReadCount,TInt aFullCount, TInt /*aError*/)
{
	if(iProgressDialog && (aReadCount % 10 == 0))
	{
		TBuf<150> ProgressText1,Hjelppp;
			
		StringLoader::Load(Hjelppp,R_SH_STR_READINGFF);
		ProgressText1.Format(Hjelppp,aReadCount,aFullCount);
		iProgressDialog->SetTextL(ProgressText1);
	}
	
	if(iProgressInfo)
	{
		iProgressInfo->SetAndDraw(aReadCount);	
	}
}
/*
-----------------------------------------------------------------------

-----------------------------------------------------------------------
*/
void CSelectContainer::FolderReadFinnishL(CFFolderReader* /*aObject*/, TInt /*aError*/)
{
	if(iProgressDialog)
	{
		iProgressDialog->ProcessFinishedL(); 
	}

	iProgressDialog = NULL;
	iProgressInfo = NULL;
	
	TBuf<255> Hjelpper,NameHjrelp;
	TBuf<20> DateHelp;
	
	if(iSelectionBox)
	{
		TInt SelFolder(-1);
		
		CDesCArray* itemArray = STATIC_CAST( CDesCArray* ,iSelectionBox->Model()->ItemTextArray());
				
		TInt OldCount = itemArray->Count();
		
		itemArray->Reset();
		
		RPointerArray<CFFileItem> CurrArray =  iFFolderReader.ItemArray();
		
		Hjelpper.Zero();
		
		HBufC* CurrPath = iFFolderReader.CurrentPath();
		if(CurrPath)
		{
			if(CurrPath->Des().Length())
			{
				Hjelpper.Copy(CurrPath->Des());
			}
		}
		 	
		CDesCArrayFlat* DirArray = new(ELeave)CDesCArrayFlat(1);
		CleanupStack::PushL(DirArray);
		
		for(TInt i = 0; i  < CurrArray.Count(); i++)
		{
			if(CurrArray[i])
			{
				if(CurrArray[i]->iName)
				{
					if(CurrArray[i]->iType == CFFileItem::EFFolderFile)
					{
						DirArray->AppendL(CurrArray[i]->iName->Des());
					}
				}
			}
		}
		
		if(iSettings.iFolders)
		{
			for(TInt i = 0; i  < CurrArray.Count(); i++)
			{
				Hjelpper.Zero();
				
				if(CurrArray[i])
				{
					if(CurrArray[i]->iName)
					{
						NameHjrelp.Copy(CurrArray[i]->iName->Des());
						if(CurrArray[i]->iType == CFFileItem::EFFolderFile)
						{
							if(iExtendedInfoOn)
							{	
								GetTimeBuffer(CurrArray[i]->iModified.DateTime(),DateHelp,ETrue,ETrue);
								Hjelpper.Format(KtxLongListFormatFolder,CFFileItem::EFFolderFile,&NameHjrelp,&DateHelp,CurrArray[i]->iFiles,CurrArray[i]->iFolders);
							}				
							else
							{
								Hjelpper.Format(KtxShortListFormat,CFFileItem::EFFolderFile,&NameHjrelp);
							}
		
							if(CurrArray[i]->iName->Des() == iLastFolder
							&& SelFolder < 0)
							{
								SelFolder = itemArray->Count();
							}
							
							itemArray->AppendL(Hjelpper);
						}
					}
				}
			}
		}
		
		for(TInt i = 0; i  < CurrArray.Count(); i++)
		{
			Hjelpper.Zero();
			
			if(CurrArray[i])
			{
				if(CurrArray[i]->iName)
				{
					NameHjrelp.Copy(CurrArray[i]->iName->Des());
					if(CurrArray[i]->iType != CFFileItem::EFFolderFile)
					{											
						if(iExtendedInfoOn)
						{
							if(CurrArray[i]->iType == CFFileItem::EFDriveFile)
							{		
								if(CurrArray[i]->iExtra > (1024* 1024 * 10))
								{
									
									Hjelpper.Format(KtxLongListFormatDriveMB,CurrArray[i]->iType,&NameHjrelp,(CurrArray[i]->iExtra / (1024* 1024)),(CurrArray[i]->iSize / (1024* 1024)));
								}
								else if(CurrArray[i]->iExtra > (1024* 10))
								{
									Hjelpper.Format(KtxLongListFormatDriveKb,CurrArray[i]->iType,&NameHjrelp,(CurrArray[i]->iExtra / (1024)),(CurrArray[i]->iSize / (1024)));
								}
								else
								{
									Hjelpper.Format(KtxLongListFormatDriveB,CurrArray[i]->iType,&NameHjrelp,CurrArray[i]->iExtra,CurrArray[i]->iSize);
								}
							}
							else if(CurrArray[i]->iType >= CFFileItem::EFJustFile)
							{	
								GetTimeBuffer(CurrArray[i]->iModified.DateTime(),DateHelp,ETrue,ETrue);
								
								if(CurrArray[i]->iSize > (1024* 1024 * 10))
								{	
									
									TInt Siz = (CurrArray[i]->iSize / (1024* 1024));
									Hjelpper.Format(KtxLongListFormatFileMb,CurrArray[i]->iType,&NameHjrelp,&DateHelp,Siz);
								}
								else if(CurrArray[i]->iSize > (1024* 10))
								{
									Hjelpper.Format(KtxLongListFormatFileKb,CurrArray[i]->iType,&NameHjrelp,&DateHelp,(CurrArray[i]->iSize / 1024));
								}
								else
								{
									Hjelpper.Format(KtxLongListFormatFileB,CurrArray[i]->iType,&NameHjrelp,&DateHelp,CurrArray[i]->iSize);
								}
							}
						}
						else if(CurrArray[i]->iType != CFFileItem::EFExtraFolder)
						{
							Hjelpper.Format(KtxShortListFormat,CurrArray[i]->iType,&NameHjrelp);
						}
						
						if(CurrArray[i]->iName->Des() == iLastFolder
						&& SelFolder < 0)
						{
							SelFolder = itemArray->Count();
						}
						
						if(Hjelpper.Length())
						{
							itemArray->AppendL(Hjelpper);
						}
					}
					
				}
			}
		}
		
		if(iSettings.iOrder == 2)
		{
			itemArray->Sort();
		}
		
		if(!iSettings.iFolders)
		{
			for(TInt i = 0; i  < CurrArray.Count(); i++)
			{
				Hjelpper.Zero();
				
				if(CurrArray[i])
				{
					if(CurrArray[i]->iName)
					{
						NameHjrelp.Copy(CurrArray[i]->iName->Des());
						if(CurrArray[i]->iType == CFFileItem::EFFolderFile)
						{
							if(iExtendedInfoOn)
							{
								GetTimeBuffer(CurrArray[i]->iModified.DateTime(),DateHelp,ETrue,ETrue);
									
								Hjelpper.Format(KtxLongListFormatFolder,CFFileItem::EFFolderFile,&NameHjrelp,&DateHelp,CurrArray[i]->iFiles,CurrArray[i]->iFolders);
							}				
							else
							{
								Hjelpper.Format(KtxShortListFormat,CFFileItem::EFFolderFile,&NameHjrelp);
							}				
		
							if(CurrArray[i]->iName->Des() == iLastFolder
							&& SelFolder < 0)
							{
								SelFolder = itemArray->Count();
							}
							
							if(Hjelpper.Length())
							{
								itemArray->AppendL(Hjelpper);
							}
						}
					}
				}
			}
		}
		
		CleanupStack::PopAndDestroy(DirArray);
		
		TInt NewCount = itemArray->Count();
		
		if(NewCount < OldCount)
		{
			iSelectionBox->HandleItemRemovalL();
		}
		else
		{
			iSelectionBox->HandleItemAdditionL();
		}

		if(NewCount > 0)
		{
			if(NewCount > SelFolder && SelFolder >= 0)
			{
				iSelectionBox->SetCurrentItemIndex(SelFolder);
			}
			else
			{
				iSelectionBox->SetCurrentItemIndex(0);
			}
		}
		
		UpdateScrollBar(iSelectionBox);
	}
	
	SetNaviTextL();
	DrawNow();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CSelectContainer::SetNaviTextL(void)
{
	TFileName Hjelpper;
	
	HBufC* CurrPath = iFFolderReader.CurrentPath();
	if(CurrPath)
	{
		Hjelpper.Copy(CurrPath->Des());
	}
	
	if(iFolderSel)
	{
		AppendFileName(Hjelpper,-1);
	}

	iUtils.GetFileUtils().SetNaviTextL(Hjelpper);
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CSelectContainer::GetTimeBuffer(const TDateTime& aDateTime,TDes& aBuffer,TBool aDate,TBool aList)
{
	TBuf<2> HourHjlep,MinHjelp,SecHjelp;

	if(aDate)
	{
		if((aDateTime.Day() + 1) < 10)
		{
			HourHjlep.Num(0);
			HourHjlep.AppendNum((aDateTime.Day() + 1));
		}
		else
		{
			HourHjlep.Num((aDateTime.Day() + 1));
		}
		
		if((aDateTime.Month() + 1) < 10)
		{
			MinHjelp.Num(0);
			MinHjelp.AppendNum((aDateTime.Month() + 1));
		}
		else
		{
			MinHjelp.Num((aDateTime.Month() + 1));
		}

		if(aList)
		{
			aBuffer.Format(KtxFormatInfoDateList,&HourHjlep,&MinHjelp,aDateTime.Year());
		}
		else
		{
			TBuf<60> Hjelpper;
			Hjelpper.Format(KtxFormatInfoDateList,&HourHjlep,&MinHjelp,aDateTime.Year());		
			
			StringLoader::Load(aBuffer,R_SH_STR_DATE);
			aBuffer.Append(_L("\t"));
			aBuffer.Append(Hjelpper);
		}
	}
	else
	{
		if(aDateTime.Hour() < 10)
		{
			HourHjlep.Num(0);
			HourHjlep.AppendNum(aDateTime.Hour());
		}
		else
		{
			HourHjlep.Num(aDateTime.Hour());
		}
		
		if(aDateTime.Minute() < 10)
		{
			MinHjelp.Num(0);
			MinHjelp.AppendNum(aDateTime.Minute());
		}
		else
		{
			MinHjelp.Num(aDateTime.Minute());
		}
		
		if(aDateTime.Second() < 10)
		{
			SecHjelp.Num(0);
			SecHjelp.AppendNum(aDateTime.Second());
		}
		else
		{
			SecHjelp.Num(aDateTime.Second());
		}
	
		if(aList)
		{
			aBuffer.Format(KtxFormatInfoTimeList,&HourHjlep,&MinHjelp,&SecHjelp);
		}
		else
		{
			TBuf<60> Hjelpper;
			Hjelpper.Format(KtxFormatInfoTimeList,&HourHjlep,&MinHjelp,aDateTime.Year());		
			
			StringLoader::Load(aBuffer,R_SH_STR_TIME);
			aBuffer.Append(_L("\t"));
			aBuffer.Append(Hjelpper);
		}
	}
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CSelectContainer::DialogDismissedL (TInt /*aButtonId*/)
{		  		
	iProgressDialog = NULL;
	iProgressInfo = NULL;
		
	if(iFFolderReader.IsActive())
	{
		iFFolderReader.Cancel();

		FolderReadFinnishL(NULL,KErrCancel);
	}	
}
/*
-----------------------------------------------------------------------
CEikFormattedCellListBox 
-----------------------------------------------------------------------
*/
void CSelectContainer::DisplayListBoxL(void)
{	
	delete iSelectionBox;
	iSelectionBox = NULL;
		
	if(iExtendedInfoOn)
	{
	    iSelectionBox   = new( ELeave ) CAknDoubleGraphicStyleListBox();//CAknSingleGraphicStyleListBox();//CAknDoubleNumberStyleListBox();
	}
	else
	{
	    iSelectionBox   = new( ELeave ) CAknSingleGraphicStyleListBox();//CAknSingleGraphicStyleListBox();//CAknDoubleNumberStyleListBox();		
	}
	
	iSelectionBox->ConstructL(this,EAknListBoxMarkableList);
		
	CDesCArrayFlat* ListArray = new(ELeave)CDesCArrayFlat(1);
	iSelectionBox->Model()->SetItemTextArray(ListArray);
    iSelectionBox->Model()->SetOwnershipType(ELbmOwnsItemArray);
	iSelectionBox->CreateScrollBarFrameL( ETrue );
    iSelectionBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
	iSelectionBox->SetRect(Rect());
		
	if(iExtendedInfoOn)
	{
	    STATIC_CAST(CAknDoubleGraphicStyleListBox*,iSelectionBox)->ItemDrawer()->ColumnData()->SetIconArray(iUtils.GetIconUtils().GetIconArrayL(TSize(13,13)));
	}
	else
	{
		STATIC_CAST(CAknSingleGraphicStyleListBox*,iSelectionBox)->ItemDrawer()->ColumnData()->SetIconArray(iUtils.GetIconUtils().GetIconArrayL(TSize(13,13)));
	}
		
	
	iSelectionBox->ActivateL();
	
	UpdateScrollBar(iSelectionBox);
}


/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CSelectContainer::UpdateScrollBar(CEikListBox* aListBox)
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
-------------------------------------------------------------------------------
*/
void CSelectContainer::PageUpDownL(TBool aUp)
{
	if(iSelectionBox)
	{
		if(aUp)  
			iSelectionBox->View()->MoveCursorL(CListBoxView::ECursorPrevScreen,CListBoxView::ENoSelection);
		else
			iSelectionBox->View()->MoveCursorL(CListBoxView::ECursorNextScreen,CListBoxView::ENoSelection);
	    
	    iSelectionBox->UpdateScrollBarsL();
		iSelectionBox->DrawNow();
	}
	DrawNow();
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CSelectContainer::SelectionWithWildL(const TDesC& aWild)
{
	TInt FilTyp(-1);
		
	CDesCArray* itemArray= STATIC_CAST( CDesCArray* ,iSelectionBox->Model()->ItemTextArray());

	if(itemArray)
	{
		TFileName Hjelpper2;
		
		for(TInt i=0; i < itemArray->Count(); i++)
		{
			FilTyp = -1;
			GetFileType(FilTyp, i); 	
			if(FilTyp > CFFileItem::EFFolderFile)	
			{
				Hjelpper2.Zero();
				if(AppendFileName(Hjelpper2,i)) 
				{
					Hjelpper2.CopyLC(Hjelpper2);
					if(KErrNotFound != Hjelpper2.MatchF(aWild))
					{
						iSelectionBox->View()->SelectItemL(i);
					}
				}
			}
		}
	}
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CSelectContainer::InverseSelectionL(void)
{
	if(iSelectionBox && !iFolderSel)
	{
		TInt FilTyp(-1);
				
		CDesCArray* itemArray= STATIC_CAST( CDesCArray* ,iSelectionBox->Model()->ItemTextArray());
	
		if(itemArray)
		{
			for(TInt i=0; i < itemArray->Count(); i++)
			{
				FilTyp = -1;
				GetFileType(FilTyp, i); 	
				if(FilTyp > CFFileItem::EFFolderFile)	
				{
					iSelectionBox->View()->ToggleItemL(i);
				}
			}
		}
	
	}
	DrawNow();
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CSelectContainer::MarkUnMarkL(TBool aMarkAll, TBool aJustTogle)
{
	if(iSelectionBox && !iFolderSel)
	{
		if(aJustTogle)
		{
			TInt CurrSel = iSelectionBox->CurrentItemIndex();
			
			if(CurrSel >= 0)
			{	
				TInt FilTyp = -1;
				
				GetFileType(FilTyp, CurrSel); 
					
				if(FilTyp >= CFFileItem::EFFolderFile)	
				{		
					iSelectionBox->View()->ToggleItemL(CurrSel);
				}
			}
		}
		else if(aMarkAll)
		{
			TInt FilTyp(-1);
				
			CDesCArray* itemArray= STATIC_CAST( CDesCArray* ,iSelectionBox->Model()->ItemTextArray());
		
			if(itemArray)
			{
				for(TInt i=0; i < itemArray->Count(); i++)
				{
					FilTyp = -1;
					GetFileType(FilTyp, i); 	
					if(FilTyp > CFFileItem::EFFolderFile)	
					{
						iSelectionBox->View()->SelectItemL(i);
					}
				}
			}
		}
		else
		{ 
			iSelectionBox->ClearSelection();
		}
	}
	DrawNow();
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CSelectContainer::ChangeViewTypeL(void)
{	
	TInt LastItemIndex(-1);
	if(iSelectionBox)
	{
		LastItemIndex = iSelectionBox->CurrentItemIndex();
	}

	CArrayFixFlat<TInt>* SelStuff = new(ELeave)CArrayFixFlat<TInt>(10);
	CleanupStack::PushL(SelStuff);
	
	if(iSelectionBox)
	{
		const CArrayFix<TInt>* SelStuffTmp = iSelectionBox->SelectionIndexes();
		for(TInt i=0; i < SelStuffTmp->Count(); i++)
		{
			SelStuff->AppendL(SelStuffTmp->At(i));
		}
	}
	

	if(iExtendedInfoOn)
	{
		iExtendedInfoOn = EFalse;
	}
	else
	{
		iExtendedInfoOn = ETrue;
	}

	
	DisplayListBoxL();
	FolderReadFinnishL(NULL, KErrNone);
	
	if(SelStuff && iSelectionBox)
	{
		for(TInt i=0; i < SelStuff->Count(); i++)
		{
			TInt FilTyp(CFFileItem::EFFolderFile);
			GetFileType(FilTyp, SelStuff->At(i)); 	
			if(FilTyp > CFFileItem::EFFolderFile)	
			{
				iSelectionBox->View()->SelectItemL(SelStuff->At(i));
			}
		}
	}
	
	CleanupStack::PopAndDestroy(SelStuff);
	
	if(LastItemIndex > 0 && iSelectionBox)
	{
		CDesCArray* itemArray = STATIC_CAST( CDesCArray* ,iSelectionBox->Model()->ItemTextArray());
	
		if(itemArray && LastItemIndex >= 0)
		{
			if(itemArray->Count() > LastItemIndex)
			{
				iSelectionBox->SetCurrentItemIndex(LastItemIndex);
				UpdateScrollBar(iSelectionBox);
				DrawNow();
			}
		}
	}    
}




/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CSelectContainer::SizeChanged()
{
	if (iSelectionBox)
    {
        iSelectionBox->SetRect(Rect()); // Sets rectangle of lstbox.
    }
    else
    {
		DisplayListBoxL();
    }
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CSelectContainer::HandleResourceChange(TInt aType)
{
	TRect rect;
	TBool SetR(EFalse);
	
#ifdef __SERIES60_28__
    if ( aType==KEikDynamicLayoutVariantSwitch )
    {  	
    	SetR = ETrue;
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, rect);
    }
#else
    if ( aType == 0x101F8121 )
    {
    	SetR = ETrue;
    	rect = CEikonEnv::Static()->EikAppUi()->ClientRect();
    }
#endif   
 
 	if(SetR)
 	{	    
	    SetRect(rect);	
 	}
 	
	CCoeControl::HandleResourceChange(aType);
}
	
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
TKeyResponse CSelectContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
	TKeyResponse Ret = EKeyWasNotConsumed;
	

	switch (aKeyEvent.iCode)
    {
    case '*':
    	{
    		CEikonEnv::Static()->EikAppUi()->HandleCommandL(EFolderViewMarkTogle);
    	}
    	break;
    case '#':
    	{
    		ChangeViewTypeL();
    	}
    	break;				
	case EKeyRightArrow:
	case EKeyDevice3:
		if(iSelectionBox)
		{
			iLastFolder.Zero();
			
			TInt CurrType(-1);
			GetFileType(CurrType, -1);
			
			if(CurrType == CFFileItem::EFExtraFolder)
			{
				
			}
			else if(CurrType == CFFileItem::EFDriveFile
			|| CurrType == CFFileItem::EFFolderFile)
			{	
				iFFolderReader.Cancel();
				if(iFFolderReader.IsActive())
				{
					// wait for next key input...
				}
				else
				{
					TFileName NewFolder;
					
					HBufC* CurrPath = iFFolderReader.CurrentPath();
					if(CurrPath)
					{
						NewFolder.Copy(CurrPath->Des());
						AppendFileName(NewFolder,-1);	
					}
					
					StartReadingFolder(NewFolder);
					SetMenuL();
				}
			}
			else
			{
				CEikonEnv::Static()->EikAppUi()->HandleCommandL(EFolderViewMarkTogle);
			}
			
			break;
		}
	case EKeyLeftArrow:
		if(iSelectionBox)
		{
			iFFolderReader.Cancel();
			
			if(iFFolderReader.IsActive())
			{
				//wait for next key input..
			}
			else
			{
				TFileName NewFolder;
				iLastFolder.Zero();
			
				HBufC* CurrPath = iFFolderReader.CurrentPath();
				if(CurrPath)
				{
					if(CurrPath->Des().Length() > 3)
					{				
						CurrPath->Des().Copy(CurrPath->Des().Left(CurrPath->Des().Length() - 1));
						
						TParsePtrC FolPar(CurrPath->Des());
						
						NewFolder.Copy(FolPar.DriveAndPath());
						iLastFolder.Copy(FolPar.Name());	
					}
					else if(CurrPath->Des().Length() >= 2)
					{
						iLastFolder.Copy(CurrPath->Des().Left(2));
					}
				}
				
				iSelectionBox->ClearSelection();
				
				StartReadingFolder(NewFolder);
				SetMenuL();
			}
			break;
		}
	case EKeyDownArrow:
	case EKeyUpArrow:
	default:
		if(iSelectionBox)
		{   
			Ret = iSelectionBox->OfferKeyEventL(aKeyEvent,aType);
			
			if(iFolderSel
			&& Ret == EKeyWasConsumed)
			{
				SetNaviTextL();
			}
		}
		break;
	}


	return Ret;
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CSelectContainer::StartReadingFolder(const TDesC& aFileName) 
{
	if(iSelectionBox)
	{
		iSelectionBox->ClearSelection();
	}
	
	TInt SAve(0);
	
	
	TInt FilCount(0);
	if(iFolderSel)
		FilCount = iFFolderReader.StartReadingFoldersOnlyL(aFileName,SAve);
	else
		FilCount = iFFolderReader.StartReadingL(aFileName,SAve);

	if((FilCount - SAve) > KMinFileCountForProgress)
	{
		if(iProgressDialog)
		{
			iProgressDialog->ProcessFinishedL(); 
		}

		iProgressDialog = NULL;
		iProgressInfo = NULL;
			
		iProgressDialog = new (ELeave) CAknProgressDialog((REINTERPRET_CAST(CEikDialog**,&iProgressDialog)),ETrue);
		iProgressDialog->PrepareLC(R_PROGRESS_NOTE);
		iProgressInfo = iProgressDialog->GetProgressInfoL();
		iProgressDialog->SetCallback(this);
		iProgressDialog->RunLD();
		iProgressInfo->SetFinalValue(FilCount + 1);
	}
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
TBool CSelectContainer::AppendFileName(TDes& aFileName,TInt aIndex) 
{
	TBool Ret(EFalse);
	
	if(iSelectionBox)
	{
		CDesCArray* itemArray = STATIC_CAST( CDesCArray* ,iSelectionBox->Model()->ItemTextArray());		
	
		if(itemArray)
		{
			TInt CurrSel(aIndex);
			
			if(CurrSel < 0)
			{
				CurrSel = iSelectionBox->CurrentItemIndex();
			}
			
			if(itemArray->Count() > CurrSel && CurrSel >= 0)
			{
				TPtrC ItName,Item;
				Item.Set(itemArray->MdcaPoint(CurrSel));
				if(KErrNone == TextUtils::ColumnText(ItName,1,&Item))
				{
					Ret = ETrue;
					aFileName.Append(ItName);
					
					TInt CurrTypeId(CFFileItem::EFFolderFile);
					GetFileType(CurrTypeId,CurrSel);
					if(CurrTypeId == CFFileItem::EFFolderFile
					|| CurrTypeId == CFFileItem::EFDriveFile)
					{
						aFileName.Append(KtxDoubleSlash);
					}
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
void CSelectContainer::GetFileType(TInt& aTypeId, TInt aIndex) 
{
	aTypeId = -1;
	
	if(iSelectionBox)
	{
		CDesCArray* itemArray = STATIC_CAST( CDesCArray* ,iSelectionBox->Model()->ItemTextArray());
				
		if(itemArray)
		{
			TInt CurrSel(aIndex);
			
			if(CurrSel < 0)
			{
				CurrSel = iSelectionBox->CurrentItemIndex();
			}
				
			if(itemArray->Count() > CurrSel && CurrSel >= 0)
			{
				TPtrC IdNumber,Item;
				Item.Set(itemArray->MdcaPoint(CurrSel));
				if(KErrNone == TextUtils::ColumnText(IdNumber,0,&Item))
				{
					TLex16 MyLex(IdNumber);

					if(KErrNone != MyLex.Val(aTypeId))
					{
						aTypeId = -1;
					}
				}
			}
		}
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CSelectContainer::SetMenuL(void)
{	
	TBool ExitBut(ETrue);
					
	if(iSelectionBox)
	{
		HBufC* Bufff = iFFolderReader.CurrentPath();
		if(Bufff)
		{
			if(Bufff->Des().Length() >= 3)
			{
				ExitBut = EFalse;
			}
		}
	}

	if(iFolderSel)
	{
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->SetMenuTitleResourceId(R_SELECTION2_MENUBAR);
	
		if(ExitBut)
			iUtils.GetCba().SetCommandSetL(R_SELECTION2A_CBA);
		else
			iUtils.GetCba().SetCommandSetL(R_SELECTION2B_CBA);
		
		iUtils.GetCba().DrawDeferred();
	}
	else
	{
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->SetMenuTitleResourceId(R_SELECTION_MENUBAR);	
		
		if(ExitBut)
			iUtils.GetCba().SetCommandSetL(R_SELECTIONB_CBA);
		else
			iUtils.GetCba().SetCommandSetL(R_SELECTIONA_CBA);
		
		iUtils.GetCba().DrawDeferred();
	}
}

/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
void CSelectContainer::InitMenuPanelL(TInt aResourceId, CEikMenuPane* aMenuPane)
{
	TBool HasStuff(EFalse),HasSelection(EFalse),HasCutCopyFiles(EFalse);
	TInt CurrTypeId(CFFileItem::EFDriveFile);
			
	if(iSelectionBox)
	{	
		TInt SelCur = iSelectionBox->CurrentItemIndex();
		if(SelCur >= 0)
		{
			HasStuff = ETrue;
		}
		
		GetFileType(CurrTypeId, SelCur); 
		
		const CListBoxView::CSelectionIndexArray* SelStuff = iSelectionBox->SelectionIndexes();
		if(SelStuff)
		{
			if(SelStuff->Count())
			{
				HasSelection = ETrue;
			}
		}
	}
	
	if(iPasteArray)
	{
		if(iPasteArray->Count())
		{
			HasCutCopyFiles = ETrue;
		}
	}
		
	if(R_SELECTION_MENU == aResourceId)
	{
		aMenuPane->SetItemDimmed(EFolderViewMark,EFalse);
		
	
		if(!HasStuff)
		{
			aMenuPane->SetItemDimmed(EAddFilesToSelection,ETrue);
			aMenuPane->SetItemDimmed(EFolderViewMark,ETrue);
		}
		else if(!HasSelection && CurrTypeId < CFFileItem::EFJustFile)
		{
			aMenuPane->SetItemDimmed(EAddFilesToSelection,ETrue);
		}
	
		TBool DimP(ETrue);
		
		if(iPasteArray)
		{
			if(iPasteArray->Count())
			{
				DimP = EFalse;
			}
		}
		
		aMenuPane->SetItemDimmed(EAddFilesDone,DimP);
		aMenuPane->SetItemDimmed(EEditShowPaste,DimP);
	}
	else if(R_MARK_MENU == aResourceId)
	{
		aMenuPane->SetItemDimmed(EFolderViewMarkAll,EFalse);
		
		if(CurrTypeId < CFFileItem::EFJustFile)
		{
			aMenuPane->SetItemDimmed(EFolderViewMarkTogle,ETrue);
		}
		else
		{
			aMenuPane->SetItemDimmed(EFolderViewMarkTogle,EFalse);	
		}
		
		if(!HasSelection)
		{
			aMenuPane->SetItemDimmed(EFolderViewUnMarkAll,ETrue);
		}
		else
		{
			aMenuPane->SetItemDimmed(EFolderViewUnMarkAll,EFalse);	
		}
	}
}


/*
-----------------------------------------------------------------------

-----------------------------------------------------------------------
*/
TInt CSelectContainer::CountComponentControls() const
{
	if(iSelectionBox)
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
CCoeControl* CSelectContainer::ComponentControl(TInt /*aIndex*/) const
{	
	return iSelectionBox;
}

/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CSelectContainer::Draw(const TRect& /*aRect*/) const
{
    // Get the standard graphics context 
 //   CWindowGc& gc = SystemGc();
 //   gc.Clear(aRect);
}


