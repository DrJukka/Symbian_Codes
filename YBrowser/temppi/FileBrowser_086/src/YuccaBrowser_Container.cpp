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
#include <aknmessagequerydialog.h>
#include <akntitle.h> 
#include <akncontext.h> 
#include <APGICNFL.H>
#include <stringloader.h> 
//#include <cecombrowserplugininterface.h> 

#include "Search_Settings.h"
#include "YuccaBrowser_Container.h"

#include "YuccaBrowser.h"
#include "YuccaBrowser.hrh"
#ifdef __YTOOLS_SIGNED
	#include <YuccaBrowser_2000713D.rsg>
#else
	#include <YuccaBrowser.rsg>
#endif
#include "Navipanel.h"
#include "ShowString.h"


#include <APACMDLN.H> 
#include "CommandSC_Settings.h"
#include "Search_Container.h"
#include "Selection_Container.h"


const TInt KAknExListFindBoxTextLength = 20;


/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
CFileListContainer* CFileListContainer::NewL(const TRect& aRect,CEikButtonGroupContainer* aCba)
    {
    CFileListContainer* self = CFileListContainer::NewLC(aRect,aCba);
    CleanupStack::Pop(self);
    return self;
    }
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
CFileListContainer* CFileListContainer::NewLC(const TRect& aRect,CEikButtonGroupContainer* aCba)
    {
    CFileListContainer* self = new (ELeave) CFileListContainer(aCba);
    CleanupStack::PushL(self);
    self->ConstructL(aRect);
    return self;
    }
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
CFileListContainer::CFileListContainer(CEikButtonGroupContainer* aCba)
:iCba(aCba)
    {
	
    }
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
CFileListContainer::~CFileListContainer()
{
	if(iProgressDialog)
	{
		iProgressDialog->ProcessFinishedL(); 
	}

	iProgressDialog = NULL;
	iProgressInfo = NULL;
	
	iHandlerArray.ResetAndDestroy();
	
	delete iSearchSettings;
	iSearchSettings = NULL;
	
	delete iCommandSCSettings;
	iCommandSCSettings = NULL;
	
	delete iCommandSCArray;
	iCommandSCArray = NULL;
	
	delete iShowString;
	iShowString = NULL;
	
	delete iYuccaSettings;
	iYuccaSettings = NULL;
	
	delete iFFolderReader;
	iFFolderReader = NULL;
	
	delete iFindBox;
	iFindBox = NULL;
	
	delete iSelectionBox;
	iSelectionBox = NULL;	
	
	delete iEikFileHandler;
	iEikFileHandler = NULL;
	
	delete iFilemanNameArray;
	iFilemanNameArray = NULL;
	
	delete iPasteArray;
	iPasteArray = NULL;
	
	delete iYuccaNavi;
	iYuccaNavi = NULL;
	
	delete iIconHandler;
	iIconHandler = NULL;
	
	delete iSearchValues;
	iSearchValues = NULL;
	
	delete iSelectContainer;
	iSelectContainer = NULL;
	
	delete iTmpPath;
	iTmpPath = NULL;
	
	delete iBgContext;
	iBgContext = NULL;
	
	REComSession::FinalClose();
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CFileListContainer::ConstructL(const TRect& aRect)
{
    CreateWindowL();
	
	iBgContext = CAknsBasicBackgroundControlContext::NewL(KAknsIIDQsnBgAreaMain,TRect(0,0,1,1), ETrue);

	iOkToContinue = ShowDisclaimerL();

	TFileName StartPath;

	GetIconNameL(StartPath);
	
	iIconHandler = new(ELeave)CIconHandler(*this,this);
	iIconHandler->SetUtils(this);
	iIconHandler->SetIconFile(StartPath);
	iIconHandler->ConstructL();
	
    SetRect(aRect);
	
	if(iOkToContinue)
	{
		StartPath.Zero();
		
		GetCommandSCFromStoreL();
		GetSettingsValuesL();

		iYuccaNavi = CYuccaNavi::NewL();
	    iFFolderReader = CFFolderReader::NewL(*this,*iIconHandler);
	    
		UpdateSettingsForReadingL();
		
		if(iSettings.iSavePath)
		{
			GetPathL(StartPath);
		}
		
		TInt SAve(0);		
		TInt FilCount = iFFolderReader->StartReadingL(StartPath,SAve);

		if((FilCount  - SAve) > KMinFileCountForProgress)
		{
			iProgressDialog = new (ELeave) CAknProgressDialog((REINTERPRET_CAST(CEikDialog**,&iProgressDialog)),ETrue);
			iProgressDialog->PrepareLC(R_PROGRESS_NOTE);
			iProgressInfo = iProgressDialog->GetProgressInfoL();
			iProgressDialog->SetCallback(this);
			iProgressDialog->RunLD();
			iProgressInfo->SetFinalValue(FilCount + 1);
		}
	}
	
	ActivateL();
	SetMenuL();

	CEikStatusPane*		sp = ((CAknAppUi*)iEikonEnv->EikAppUi())->StatusPane();
	if(sp)
	{
		TInt UseStatRes(R_AVKON_STATUS_PANE_LAYOUT_USUAL);//);
		
		if(iSettings.iFullScreen)
		{
			UseStatRes = R_AVKON_STATUS_PANE_LAYOUT_SMALL_WITH_SIGNAL_PANE;
		}
		
		if(sp->CurrentLayoutResId() != UseStatRes)//R_AVKON_STATUS_PANE_LAYOUT_SMALL_WITH_SIGNAL_PANE)
		{
			sp->SwitchLayoutL(UseStatRes);
		}
	}
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CFileListContainer::AddFileHandler(CYBrowserBaseHandler* aHandler)
{
	CYBrowserBaseHandler* TmpHand = GetCurrentHandler();
	if(TmpHand)
    {// remove focus from the last active handler.
    	TmpHand->SetFocusL(EFalse);
    }
    
	iHandlerArray.Append(aHandler);

	SetMenuL();
	DrawNow();
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CFileListContainer::StartSelectionViewL(MYbSelectorCallBack* aCallback,TBool aFolderSelector,const TDesC& aFolder,const TDesC& aTitle,CDesCArrayFlat* aTypeFilter)
{
	if(aCallback && iFFolderReader)
		{
		delete iTmpPath;
		iTmpPath = NULL;
		
		HBufC* CurrPath = iFFolderReader->CurrentPath();
		if(CurrPath)
		{
			iFilemanNameBuf.Copy(CurrPath->Des());
			AppendFileName(iFilemanNameBuf, -1);
			
			iTmpPath = CurrPath->Des().AllocL();
		}
		
		delete iSelectContainer;
		iSelectContainer = NULL;
		iSelectContainer = CSelectContainer::NewL(*iFFolderReader,*this,iSettings,aFolderSelector,aFolder,aTitle,aTypeFilter);
		iSelectContainer->SetCallBack(aCallback);
	}
	
	SetMenuL();
	DrawNow();
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CFileListContainer::SetNaviTextL(const TDesC& aText)
{
	if(iYuccaNavi)
	{
		iYuccaNavi->SetTextL(aText);
	}
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CFileListContainer::GetCurrentPath(TDes& aFullPath)
{
	aFullPath.Zero();
	
	if(iFFolderReader)
	{
		HBufC* Currpath = iFFolderReader->CurrentPath();
		if(Currpath)
		{
			aFullPath.Copy(*Currpath);
		}
	}
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CFileListContainer::HandleExitL(TAny* /*aFileHandler*/, TInt aChanges)
{
	if(aChanges == MYBrowserFileHandlerUtils::ENoChangesMade)
		HandleViewCommandL(ERemoveLastFileHandlerNoChange);
	else
		HandleViewCommandL(ERemoveLastFileHandler);
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
CEikButtonGroupContainer& CFileListContainer::GetCba()
{
	return *iCba;
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CFileListContainer::HandleServerAppExit(TInt /*aReason*/)
{
	
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
MYBrowserFileUtils& CFileListContainer::GetFileUtils()
{
	return *iIconHandler;
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
MYBrowserIconUtils& CFileListContainer::GetIconUtils()
{
	return *iIconHandler;
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
TBool CFileListContainer::ShowDisclaimerL(void)
{
	TBool OkToContinue(EFalse);

	TFindFile AppFile(CCoeEnv::Static()->FsSession());
	if(KErrNone != AppFile.FindByDir(KtxDisclaimerFileName, KNullDesC))
	{
		HBufC* Abbout = KtxDisclaimer().AllocLC();
		TPtr Pointter(Abbout->Des());
		CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL(Pointter);
		dlg->PrepareLC(R_DDD_HEADING_PANE);
		dlg->SetHeaderTextL(KtxDisclaimerTitle);  
		if(dlg->RunLD())
		{
			TFileName ShortFil;
			if(KErrNone ==CCoeEnv::Static()->FsSession().PrivatePath(ShortFil))
			{
				TFindFile privFile(CCoeEnv::Static()->FsSession());
				if(KErrNone == privFile.FindByDir(ShortFil, KNullDesC))
				{
					TParsePtrC hjelp(privFile.File());
					ShortFil.Copy(hjelp.Drive());
					ShortFil.Append(KtxDisclaimerFileName);
				}
			}

		
			BaflUtils::EnsurePathExistsL(CCoeEnv::Static()->FsSession(),ShortFil);
		
			RFile MyFile;
			if(KErrNone == MyFile.Create(CCoeEnv::Static()->FsSession(),ShortFil,EFileWrite))
			{
				TTime NowTime;
				NowTime.HomeTime();
				
				TBuf8<255> InfoLine;
				InfoLine.Copy(_L8("Accepted on Date\t"));
					
				InfoLine.AppendNum(NowTime.DateTime().Day() + 1);
				InfoLine.Append(_L8("."));
				InfoLine.AppendNum((NowTime.DateTime().Month() + 1));
				InfoLine.Append(_L8("."));
				InfoLine.AppendNum(NowTime.DateTime().Year());
				InfoLine.Append(_L8(" "));
				InfoLine.Append(_L8("--"));
				InfoLine.AppendNum(NowTime.DateTime().Hour());
				InfoLine.Append(_L8(":"));		
				TInt HelperInt = NowTime.DateTime().Minute();
				if(HelperInt < 10)
					InfoLine.AppendNum(0);
				InfoLine.AppendNum(HelperInt);
				InfoLine.Append(_L8(":"));
				HelperInt = NowTime.DateTime().Second();
				if(HelperInt < 10)
					InfoLine.AppendNum(0);
				InfoLine.AppendNum(HelperInt);
				InfoLine.Append(_L8(" "));
				
				MyFile.Write(InfoLine);
				MyFile.Close();
			}
			
			OkToContinue = ETrue;
		}
		
		CleanupStack::PopAndDestroy(Abbout);
	}
	else
	{
		OkToContinue = ETrue;
	}
	
	return OkToContinue;
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CFileListContainer::AddSelectedFilesL(CDesCArray* aArray,TBool AlsoFolders)
{
	if(iSearchSettings)
	{
		iSearchSettings->AddSelectedFilesL(aArray,AlsoFolders);
	}
	else if(iSelectionBox && iFFolderReader && aArray)
	{
		TFileName AddFileName;
		HBufC* CurrPath = iFFolderReader->CurrentPath();
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
							AddFileName.Copy(CurrPath->Des());
							if(AppendFileName(AddFileName,SelStuff->At(i)))
							{
								aArray->AppendL(AddFileName);
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
					else if(!AlsoFolders && CurrFilType == CFFileItem::EFFolderFile)
					{
						//do nothing
					}
					else // so select current file..
					{
						AddFileName.Copy(CurrPath->Des());
						if(AppendFileName(AddFileName,-1))
						{
							aArray->AppendL(AddFileName);
						}
					}
				}
			}
		}
	}
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CFileListContainer::FileManOperatioDoneL(CEikFileHandler* /*aObject*/,TInt /*aOperation*/, TInt /*aError*/,TFileManError /*aErrorInfo*/)
{
	if(iFindBox)
	{
		iFindBox->ResetL();
	}

	if(iFFolderReader && iSelectionBox)
	{
		iSelectionBox->ClearSelection();		
		iFFolderReader->Cancel();
		
		HBufC* CurrPath = iFFolderReader->CurrentPath();
		if(CurrPath)
		{
 			TInt SAve(0);
 			
 			
			TInt FilCount = iFFolderReader->StartReadingL(CurrPath->Des(),SAve);

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
	}
	
	delete iFilemanNameArray;
	iFilemanNameArray = NULL;
	
	iIsCutOperation = EFalse;
	
	SetMenuL();
	DrawNow();
}

/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CFileListContainer::FolderReadOnProcessL(TInt aReadCount,TInt aFullCount, TInt aError)
{
	if(iSelectContainer)
	{
		iSelectContainer->FolderReadOnProcessL(aReadCount,aFullCount,aError);
	}
	else
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
}
/*
-----------------------------------------------------------------------

-----------------------------------------------------------------------
*/
void CFileListContainer::FolderReadFinnishL(CFFolderReader* aObject, TInt aError)
{
	if(iSelectContainer)
	{
		iSelectContainer->FolderReadFinnishL(aObject,aError);
	}
	else
	{
		if(iProgressDialog)
		{
			iProgressDialog->ProcessFinishedL(); 
		}

		iProgressDialog = NULL;
		iProgressInfo = NULL;
		iDriveReadOnly = ETrue;
		
		TBuf<255> Hjelpper,NameHjrelp;
		TBuf<20> DateHelp;
		
		if(aError != KErrNone && aError != KErrCancel)
		{
			StringLoader::Load(NameHjrelp,R_STR_FILREADERR);
			Hjelpper.Format(NameHjrelp,aError);
			ShowNoteL(Hjelpper, ETrue);
		}
		
		if(iSelectionBox && iFFolderReader)
		{
			TInt SelFolder(-1);
			
			CDesCArray* itemArray = STATIC_CAST( CDesCArray* ,iSelectionBox->Model()->ItemTextArray());
					
			TInt OldCount = itemArray->Count();
			
			itemArray->Reset();
			
			RPointerArray<CFFileItem> CurrArray =  iFFolderReader->ItemArray();
			
			Hjelpper.Zero();
			iDriveReadOnly = iFFolderReader->IsDriveReadOnly();
			HBufC* CurrPath = iFFolderReader->CurrentPath();
			if(CurrPath)
			{
				if(CurrPath->Des().Length())
				{
					Hjelpper.Copy(CurrPath->Des());
				}
			}
			
			if(iYuccaNavi)
			{
				iYuccaNavi->SetTextL(Hjelpper);
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
			
								if(iFilemanNameBuf.Length())
								{
									TParsePtrC SelFile(iFilemanNameBuf.Left(iFilemanNameBuf.Length() - 1));
									
									if(CurrArray[i]->iName->Des() == SelFile.NameAndExt())
									{
										SelFolder = itemArray->Count();
										iFilemanNameBuf.Zero();
									}
								}
								else if(CurrArray[i]->iName->Des() == iLastFolder
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
								else //EFDriveFile
								{
									Hjelpper.Format(KtxShortListFormat,CurrArray[i]->iType,&NameHjrelp);	
								}
							}
							else
							{
								Hjelpper.Format(KtxShortListFormat,CurrArray[i]->iType,&NameHjrelp);
							}
							
							if(iFilemanNameBuf.Length())
							{
								TParsePtrC SelFile(iFilemanNameBuf);
								if(CurrArray[i]->iName->Des() == SelFile.NameAndExt())
								{
									SelFolder = itemArray->Count();
									iFilemanNameBuf.Zero();
								}
							}
							else if(CurrArray[i]->iName->Des() == iLastFolder
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
			
								if(iFilemanNameBuf.Length())
								{
									TParsePtrC SelFile(iFilemanNameBuf.Left(iFilemanNameBuf.Length() - 1));
									
									if(CurrArray[i]->iName->Des() == SelFile.NameAndExt())
									{
										SelFolder = itemArray->Count();
										iFilemanNameBuf.Zero();
									}
								}
								else if(CurrArray[i]->iName->Des() == iLastFolder
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

			if(iFilemanSelIndex >=0
			&& NewCount > 0)
			{
				if(iFilemanSelIndex > NewCount)
				{
					iFilemanSelIndex = (NewCount - 1);
				}
				
				if(iFilemanSelIndex >=0 && iFilemanSelIndex < NewCount)
					iSelectionBox->SetCurrentItemIndex(iFilemanSelIndex);
				else
					iSelectionBox->SetCurrentItemIndex(0);
				
				iFilemanSelIndex = -1;
			}
			else if(NewCount > 0)
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
		
		iFilemanNameBuf.Zero();
		iFilemanSelIndex = -1;
		
		SetMenuL();
		DrawNow();
	}
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CFileListContainer::DialogDismissedL (TInt /*aButtonId*/)
{		  		
	iProgressDialog = NULL;
	iProgressInfo = NULL;
		
	if(iFFolderReader)
	{
		if(iFFolderReader->IsActive())
		{
			iFFolderReader->Cancel();
	
			FolderReadFinnishL(NULL,KErrCancel);
		}
	}
	
}

// R_AVKON_STATUS_PANE_LAYOUT_SMALL_WITH_SIGNAL_PANE;
// R_AVKON_STATUS_PANE_LAYOUT_USUAL;
/*
-----------------------------------------------------------------------
CEikFormattedCellListBox 
-----------------------------------------------------------------------
*/
void CFileListContainer::DisplayListBoxL(void)
{	
	delete iFindBox;
	iFindBox = NULL;
	
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
	    STATIC_CAST(CAknDoubleGraphicStyleListBox*,iSelectionBox)->ItemDrawer()->ColumnData()->SetIconArray(iIconHandler->GetIconArrayL());
	}
	else
	{
		STATIC_CAST(CAknSingleGraphicStyleListBox*,iSelectionBox)->ItemDrawer()->ColumnData()->SetIconArray(iIconHandler->GetIconArrayL());
	}
	
	
	if(iSearchOn)
	{
		iFindBox = CreateFindBoxL(iSelectionBox, iSelectionBox->Model(),CAknSearchField::ESearch);
		SizeChanged();
	}	
	
	iSelectionBox->ActivateL();
	
	UpdateScrollBar(iSelectionBox);

}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CAknSearchField* CFileListContainer::CreateFindBoxL(CEikListBox* aListBox,CTextListBoxModel* aModel, CAknSearchField::TSearchFieldStyle aStyle )
{
    CAknSearchField* findbox = NULL;

    if ( aListBox && aModel )
	{
        CAknFilteredTextListBoxModel* model =STATIC_CAST( CAknFilteredTextListBoxModel*, aModel );
        findbox = CAknSearchField::NewL( *this,aStyle,NULL,KAknExListFindBoxTextLength);
        CleanupStack::PushL(findbox);
        model->CreateFilterL( aListBox, findbox );
        CleanupStack::Pop(findbox); // findbox
 	}

    return findbox;
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CFileListContainer::UpdateScrollBar(CEikListBox* aListBox)
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
CYBrowserBaseHandler* CFileListContainer::GetCurrentHandler(void) const
{
	CYBrowserBaseHandler* Ret(NULL);
	
	TInt SelHandler = iHandlerArray.Count();
	if(SelHandler > 0 )
	{
		Ret = iHandlerArray[(SelHandler - 1)];
	}
	
	return Ret;
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CFileListContainer::RemoveLastHandler(void)
{
	TInt CurrCount = iHandlerArray.Count();
	if(CurrCount > 0 )
	{
		delete iHandlerArray[(CurrCount - 1)];
		iHandlerArray.Remove((CurrCount - 1));
	}
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
TBool CFileListContainer::SendWithSendUIL(void)
{
	TBool Ret(ETrue);
	
	if(iIconHandler)
	{
		if(iIconHandler->HasSenders())
		{
			CDesCArrayFlat* MyArray = new(ELeave)CDesCArrayFlat(1);
			CleanupStack::PushL(MyArray);
		
			AddSelectedFilesL(MyArray,EFalse);
		
			if(MyArray->Count())
			{
				iIconHandler->SendFilesL(*MyArray,Ret);
			}
			
			CleanupStack::PopAndDestroy(MyArray);	
		}
	}
	
	return Ret;
}
/*
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/
void CFileListContainer::SelectAndChangeIconL(void)
{	
	TFileName IconName,SelFile;

	TFindFile AppFile(CCoeEnv::Static()->FsSession());
	if(KErrNone == AppFile.FindByDir(KtxImagesFileName, KNullDesC))
	{
		IconName.Copy(AppFile.File());
	}
	
	if(IconName.Length())
	{	
		CAknSinglePopupMenuStyleListBox* list = new(ELeave) CAknSinglePopupMenuStyleListBox;
		CleanupStack::PushL(list);

	    CAknPopupList* popupList = CAknPopupList::NewL(list, R_AVKON_SOFTKEYS_SELECT_BACK,AknPopupLayouts::EMenuDoubleLargeGraphicWindow);  
	    CleanupStack::PushL(popupList);

	    list->ConstructL(popupList, 0);
	    list->CreateScrollBarFrameL(ETrue);
	    list->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,CEikScrollBarFrame::EAuto);
	    
		CDesCArrayFlat* AnimList = new(ELeave)CDesCArrayFlat(10);
		CleanupStack::PushL(AnimList);	
	
		CDir* File_list(NULL);
		
		TParsePtrC Hjelpper(IconName);
		
		SelFile.Copy(Hjelpper.Drive());
		SelFile.Append(KTxtIconsFolder);
		
		if(KErrNone == CCoeEnv::Static()->FsSession().GetDir(SelFile,KEntryAttMaskSupported,EDirsFirst,File_list))	
		{
			if(File_list)
			{
				TBuf<5> HelpBuffer;
				
				File_list->Sort(ESortByName);
				for(TInt i=0; i < File_list->Count(); i++)
				{
					if((*File_list)[i].IsSystem() 
					|| (*File_list)[i].IsHidden()
					|| (*File_list)[i].IsDir())
					{
						// ignore System & Hidden Files
					}
					else if((*File_list)[i].iName.Length() > 4)
					{
						HelpBuffer.CopyLC((*File_list)[i].iName.Right(4));
						
						if(HelpBuffer == KTxtMifExtension)
						{
							AnimList->AppendL((*File_list)[i].iName);
						}
					}
				}
			}
		}
		
		delete File_list;
		File_list = NULL;
	
		CleanupStack::Pop(AnimList);
	    list->Model()->SetItemTextArray(AnimList);
	    list->Model()->SetOwnershipType(ELbmOwnsItemArray);

		StringLoader::Load(SelFile,R_SH_STR_SELICONFF);
		popupList->SetTitleL(SelFile);
		
		if (popupList->ExecuteLD())
	    {
			TInt Selected = list->CurrentItemIndex();
			if(Selected >= 0 && AnimList->MdcaCount() > Selected)
			{
				SelFile.Copy(AnimList->MdcaPoint(Selected));
				
		
				TParsePtrC AnotherHjelp1(IconName);
				TParsePtrC AnotherHjelp2(SelFile);
					
				TFileName Target;
				Target.Copy(AnotherHjelp1.DriveAndPath());
				Target.Append(AnotherHjelp2.NameAndExt());

				SaveIconNameL(Target);
	
				if(iIconHandler)
				{
					iIconHandler->SetIconFile(Target);
					iIconHandler->ReReadIcosL();
				}
				
				DisplayListBoxL();
				ReFreshListL(EFalse,NULL);
			}
		}
	  	CleanupStack::Pop();             // popuplist
		CleanupStack::PopAndDestroy(1);  // list
	}
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CFileListContainer::HandleViewCommandL(TInt aCommand)
{
	TFileName Hjelpper;	

    switch(aCommand)
        {
        case EBackOneLevel:
        	if(iFFolderReader && iSelectionBox && !iSearchOn)
			{
				iFFolderReader->Cancel();
				
				if(iFFolderReader->IsActive())
				{
					//wait for next key input..
				}
				else
				{
					TFileName NewFolder;
					iLastFolder.Zero();
				
					HBufC* CurrPath = iFFolderReader->CurrentPath();
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
					
					TInt SAve(0);
					iFFolderReader->RemoveTypeFilterArrayL();
					TInt FilCount = iFFolderReader->StartReadingL(NewFolder,SAve);

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
				break;
			}
        	break;
        case EExtrasIcons:
        	{
        		SelectAndChangeIconL();
        	}
        	break;
        case EAddFilesDone:
		case EAddFilesCancel:
			if(iSelectContainer)
			{
				CEikStatusPane* sp = ((CAknAppUi*)iEikonEnv->EikAppUi())->StatusPane();
				if(sp)
				{	
					CAknTitlePane* TitlePane = STATIC_CAST(CAknTitlePane*,sp->ControlL(TUid::Uid(EEikStatusPaneUidTitle)));
					if(TitlePane)
						TitlePane->SetTextToDefaultL();
				
					sp->DrawNow();		
				}
				
				TBool OkNa(EFalse);
				
				if(aCommand == EAddFilesDone)
					OkNa = iSelectContainer->UpdateCallBackL(EFalse);
				else
					OkNa = iSelectContainer->UpdateCallBackL(ETrue);
				
				delete iSelectContainer;
				iSelectContainer = NULL;
			
				if(!OkNa)
				{
					CYBrowserBaseHandler* TmpHand = GetCurrentHandler();
	        		if(TmpHand)
    	    		{
        				TmpHand->SetFocusL(ETrue);
        			}
				}
			}
			SetMenuL();
        	DrawNow();
			break;
		case EAddFilesToSelection:
			if(iSelectContainer)
			{
				iSelectContainer->AddSelectedFilesL();
			}
			DrawNow();
			break;
		case EFileNewFile:
        	if(iFFolderReader && !iFindBox 
			&& !iDriveReadOnly && iIconHandler && !iSearchSettings)
			{
				
        		iIconHandler->MakeNewFileL();
        	}
        	SetMenuL();
        	DrawNow();
        	break;
        case ERemoveLastFileHandlerNoChange:
        case ERemoveLastFileHandler:
        	{
        		RemoveLastHandler();
        		
        		CYBrowserBaseHandler* TmpHand = GetCurrentHandler();
        		if(TmpHand)
        		{
        			TmpHand->SetFocusL(ETrue);
        		}
        		else
        		{
	        		CEikStatusPane*		sp = ((CAknAppUi*)iEikonEnv->EikAppUi())->StatusPane();
					if(sp)
					{
						CAknTitlePane* TitlePane = STATIC_CAST(CAknTitlePane*,sp->ControlL(TUid::Uid(EEikStatusPaneUidTitle)));
						
						if(TitlePane)
							TitlePane->SetTextL(KtxApplicationName);
					
				    	CAknContextPane* 	ContextPane = (CAknContextPane *)sp->ControlL(TUid::Uid(EEikStatusPaneUidContext));
						if(ContextPane)
						{
							ContextPane->SetPictureToDefaultL();
						}
					
						TInt UseStatRes(R_AVKON_STATUS_PANE_LAYOUT_USUAL);//R_AVKON_STATUS_PANE_LAYOUT_USUAL);
		
						if(iSettings.iFullScreen)
						{
							UseStatRes = R_AVKON_STATUS_PANE_LAYOUT_SMALL_WITH_SIGNAL_PANE;
						}
					
						if(sp->CurrentLayoutResId() != UseStatRes)//R_AVKON_STATUS_PANE_LAYOUT_SMALL_WITH_SIGNAL_PANE)
						{
							sp->SwitchLayoutL(UseStatRes);
						}
					}
					
					if(iFFolderReader && iYuccaNavi)
					{
						if(iTmpPath)
						{
							iYuccaNavi->SetTextL(iTmpPath->Des());
						}
						else
						{
							HBufC* CurrPath = iFFolderReader->CurrentPath();
							if(CurrPath)
							{
								iYuccaNavi->SetTextL(CurrPath->Des());
							}
							else
							{
								iYuccaNavi->SetTextL(KtxEmpty);
							}
						}
						
						TBool FullReFresh(EFalse);
						
						if(!iFFolderReader->IsDriveReadOnly()
						&& aCommand != ERemoveLastFileHandlerNoChange)
						{
							FullReFresh = ETrue;
						}
						
						ReFreshListL(FullReFresh,iTmpPath);
						
						delete iTmpPath;
						iTmpPath = NULL;
					}
        		}
        	}
        	SetMenuL();
        	DrawNow();
        	break;
        case EOpenWithViewer:
        	{	
        		OpenFileWithL(KtxEmpty);
        	}
        	break;
        case EVBOpenFile:
        	{
        		OpenFileL(KtxEmpty); 
        	}
        	break;
        case EMovePageUp:
        case EMovePageDown:
        	if(iSelectContainer)
			{
				TBool Upp(ETrue);
				
				if(aCommand == EMovePageDown) 
				{
					Upp = EFalse;
				}
					
				iSelectContainer->PageUpDownL(Upp);
			}
        	else if(iSearchSettings)
        	{
        		if(aCommand == EMovePageUp) 
        			iSearchSettings->PageUpDown(ETrue);
        		else
        			iSearchSettings->PageUpDown(EFalse);
        	}
        	else if(iSelectionBox)
        	{
        		if(aCommand == EMovePageUp)  
					iSelectionBox->View()->MoveCursorL(CListBoxView::ECursorPrevScreen,CListBoxView::ENoSelection);
        		else
        			iSelectionBox->View()->MoveCursorL(CListBoxView::ECursorNextScreen,CListBoxView::ENoSelection);
		        
		        iSelectionBox->UpdateScrollBarsL();
				iSelectionBox->DrawNow();
			}
			DrawNow();
			break;
        case ESearchFiles:
        	if(!iSearchSettings)
        	{
        		HBufC* CurrPath(NULL);
        	
        		if(iFFolderReader)
        		{
        			CurrPath = iFFolderReader->CurrentPath();
        		}
				
				if(iYuccaNavi)
				{
					iYuccaNavi->SetTextL(KtxEmpty);
				}
				
        		iSearchSettings = new(ELeave)CSearchSettings(iCba,*iIconHandler,iYuccaNavi);
        		
        		if(CurrPath)
        			iSearchSettings->ConstructL(CurrPath->Des(),iSearchValues);
        		else
        			iSearchSettings->ConstructL(KtxEmpty,iSearchValues);
        	
        		iSearchValues = NULL;
        	}
        	SetMenuL();
        	DrawNow();
        	break;
        case EBackFromSearch:
        	{
        		if(iSearchSettings)
        		{	
        			delete iSearchValues;
					iSearchValues = NULL;
					
					if(iSearchSettings->AskToSave())
					{	
						HBufC* Helppp = StringLoader::LoadLC(R_STR_SAVESEARCH);
		        		CAknQueryDialog* dlg = CAknQueryDialog::NewL();
						if(dlg->ExecuteLD(R_QUERY,*Helppp))
						{
							iSearchValues = iSearchSettings->GetSearchValues();
						}
						
						CleanupStack::PopAndDestroy(Helppp);
					}
        		}
        		delete iSearchSettings;
        		iSearchSettings = NULL;
        		
        		if(iFFolderReader && iYuccaNavi)
				{
					HBufC* CurrPath = iFFolderReader->CurrentPath();
					if(CurrPath)
					{
						iYuccaNavi->SetTextL(CurrPath->Des());
					}
					else
					{
						iYuccaNavi->SetTextL(KtxEmpty);
					}
				}
        	}
        	SetMenuL();
        	DrawNow();
        	break;
        case EGoToFolder:
        	if(iSearchSettings)
        	{
        		TInt Dummy(-1);
        		TFileName CurrFile;
        		iSearchSettings->GetCurrSelFileL(CurrFile,Dummy);
        		
        		if(CurrFile.Length())
        		{
        			delete iSearchValues;
					iSearchValues = NULL;
					iSearchValues = iSearchSettings->GetSearchValues();
					
					delete iSearchSettings;
					iSearchSettings = NULL;
					
        			TParsePtrC Hjelp(CurrFile);
        			
        			iLastFolder.Copy(Hjelp.NameAndExt());
        			
        			TInt SAve(0);
        			iFFolderReader->RemoveTypeFilterArrayL();
        			TInt FilCount = iFFolderReader->StartReadingL(Hjelp.DriveAndPath(),SAve);

					if((FilCount - SAve) > KMinFileCountForProgress)
					{
						iProgressDialog = new (ELeave) CAknProgressDialog((REINTERPRET_CAST(CEikDialog**,&iProgressDialog)),ETrue);
						iProgressDialog->PrepareLC(R_PROGRESS_NOTE);
						iProgressInfo = iProgressDialog->GetProgressInfoL();
						iProgressDialog->SetCallback(this);
						iProgressDialog->RunLD();
						iProgressInfo->SetFinalValue(FilCount + 1);
					}
        		}
        	}
        	SetMenuL();
        	DrawNow();
        	break;
        case EExtrasCommandSC:
        	if(!iSearchSettings)
        	{
        		delete iCommandSCSettings;
        		iCommandSCSettings = NULL;
        		iCommandSCSettings = new(ELeave)CCommandSCSettings();
				iCommandSCSettings->ConstructL();
        	}
        	SetMenuL();
        	DrawNow();
        	break;
        case EExtrasCommandSCSave:
        	if(iCommandSCSettings)
        	{
        		iCommandSCSettings->SaveValuesL();
        		
        		GetCommandSCFromStoreL();
        	}
        case EExtrasCommandSCBack:
			{
        		delete iCommandSCSettings;
        		iCommandSCSettings = NULL;
        	}
        	SetMenuL();
        	DrawNow();
        	break;
        case EExtrasSearchOn:
        	if(iSearchSettings)
        	{
        		iSearchSettings->HandleViewCommandL(EExtrasSearchOn);
        		break;
        	}
        	else if(iExtendedInfoOn)
        	{
        		break;
        	}
		case EExtrasSearchOff:
			if(iSearchSettings)
        	{
        		iSearchSettings->HandleViewCommandL(EExtrasSearchOff);
        	}
        	else
			{
				ChangeViewTypeL(ETrue);
				SetMenuL();
			}
			break;
        case EExtrasRefresh:
        	if(iFFolderReader && !iSearchSettings)
			{
				ReFreshListL(ETrue,NULL);
			}
			SetMenuL();
			DrawNow();
        	break;
        case EFileAttributes:
        	if(iYuccaNavi)
        	{
        		TEntry MyEntry;
        		Hjelpper.Zero();
        		
        		if(iSearchSettings)
        		{
        			TInt Dummy;
        			iSearchSettings->GetCurrSelFileL(Hjelpper,Dummy);
        		}
	        	else if(iFFolderReader)
	        	{
					HBufC* CurrPath = iFFolderReader->CurrentPath();
					if(CurrPath)
					{
						Hjelpper.Copy(CurrPath->Des());			
						if(!AppendFileName(Hjelpper, -1))
						{
							Hjelpper.Zero();
						}
					}
				}
				
				if(Hjelpper.Length())
				{
					if(KErrNone == CCoeEnv::Static()->FsSession().Entry(Hjelpper,MyEntry))
					{
						iYuccaNavi->SetTextL(Hjelpper);	
					
						iYuccaSettings = new(ELeave)CYuccaSettings();
						iYuccaSettings->ConstructL(MyEntry.IsReadOnly(),MyEntry.IsHidden(),MyEntry.IsSystem(),MyEntry.iModified);	
					}
				}
        	
        		SetMenuL();
				DrawNow();
        	}
			break;
		case EFileAttributesSave:
			if(iYuccaNavi && iYuccaSettings)
			{
				TEntry MyEntry;
				
				Hjelpper.Zero();
				iYuccaNavi->GetTextL(Hjelpper);
				TInt ErrNo = CCoeEnv::Static()->FsSession().Entry(Hjelpper,MyEntry);
				
				if(KErrNone == ErrNo)
				{
					TBool MyReadOnly(MyEntry.IsReadOnly());
					TBool MyHidden(MyEntry.IsHidden());
					TBool MySystem(MyEntry.IsSystem());
					TTime MyTime(MyEntry.iModified);
						  
					TInt Hours(0);
					TInt Minutes(0);
					TInt Seconds(0);
					
					iYuccaSettings->GetValuesL(MyReadOnly,MyHidden,MySystem,MyTime,Hours,Minutes,Seconds);
					
					if(MyReadOnly != MyEntry.IsReadOnly()
					|| MyHidden   != MyEntry.IsHidden()
					|| MySystem   != MyEntry.IsSystem())
					{
						TUint SetAttMask(0);
						TUint ClearAttMask(0);
						
						if(MyHidden)
							SetAttMask = KEntryAttHidden;
						else
							ClearAttMask = KEntryAttHidden;
					
						if(MySystem)
							SetAttMask = SetAttMask | KEntryAttSystem;
						else
							ClearAttMask = ClearAttMask | KEntryAttSystem;
						
						if(MyReadOnly)
							SetAttMask = SetAttMask | KEntryAttReadOnly;
						else
							ClearAttMask = ClearAttMask | KEntryAttReadOnly;
						
						ErrNo = CCoeEnv::Static()->FsSession().SetAtt(Hjelpper,SetAttMask,ClearAttMask);
					}
					
					TDateTime MyNewTime;
					MyNewTime = MyTime.DateTime();
					
					MyNewTime.SetHour(Hours);
					MyNewTime.SetMinute(Minutes);
					MyNewTime.SetSecond(Seconds);
					
					TTime SetNewTime(MyNewTime);
					
					if(SetNewTime != MyEntry.iModified)
					{
						ErrNo = CCoeEnv::Static()->FsSession().SetModified(Hjelpper,SetNewTime);
					}
				}
				
				if(KErrNone != ErrNo && iIconHandler)
				{
					iIconHandler->ShowFileErrorNoteL(Hjelpper, ErrNo);
				}				
			}
		case EFileAttributesCancel:
			{
				if(iFFolderReader && iYuccaNavi)
				{
					HBufC* CurrPath = iFFolderReader->CurrentPath();
					if(CurrPath)
					{
						iYuccaNavi->SetTextL(CurrPath->Des());
					}
					else
					{
						iYuccaNavi->SetTextL(KtxEmpty);
					}
				}
			
				delete iYuccaSettings;
				iYuccaSettings = NULL;
				SetMenuL();
				DrawNow();
			}
			break;
        case ESettings:
        	if(!iSearchSettings)
        	{
        		if(iYuccaNavi)
        		{
        			iYuccaNavi->SetTextL(KtxEmpty);
        		}
        		
				iYuccaSettings = new(ELeave)CYuccaSettings();
				iYuccaSettings->ConstructL(iSettings);
			
				SetMenuL();
				DrawNow();
			}
			break;
		case ESettingsSave:
			if(iYuccaSettings)
			{
				iYuccaSettings->GetValuesL(iSettings);
				SetSettingsValuesL();
				UpdateSettingsForReadingL();
			
				CEikStatusPane*		sp = ((CAknAppUi*)iEikonEnv->EikAppUi())->StatusPane();
				if(sp)
				{
					TInt UseStatRes(R_AVKON_STATUS_PANE_LAYOUT_USUAL);//);
					
					if(iSettings.iFullScreen)
					{
						UseStatRes = R_AVKON_STATUS_PANE_LAYOUT_SMALL_WITH_SIGNAL_PANE;
					}
					
					if(sp->CurrentLayoutResId() != UseStatRes)//R_AVKON_STATUS_PANE_LAYOUT_SMALL_WITH_SIGNAL_PANE)
					{
						sp->SwitchLayoutL(UseStatRes);
					}
				}
				
				if(iFFolderReader)
				{
					TInt SAve(0);
					TInt FilCount(0);
					
					iFFolderReader->Cancel();
					iFFolderReader->RemoveTypeFilterArrayL();
					
					HBufC* CurrPath = iFFolderReader->CurrentPath();
					if(CurrPath)
					{
						FilCount = iFFolderReader->StartReadingL(CurrPath->Des(),SAve);
					}
					else
					{
						FilCount = iFFolderReader->StartReadingL(KtxEmpty,SAve);	
					}
					 
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
				
				delete iYuccaSettings;
				iYuccaSettings = NULL;
				SetMenuL();
				DrawNow();
			}
			break;
		case ESettingsCancel:
			{
				if(iFFolderReader && iYuccaNavi)
				{
					HBufC* CurrPath = iFFolderReader->CurrentPath();
					if(CurrPath)
					{
						iYuccaNavi->SetTextL(CurrPath->Des());
					}
					else
					{
						iYuccaNavi->SetTextL(KtxEmpty);
					}
				}
			
				delete iYuccaSettings;
				iYuccaSettings = NULL;
				SetMenuL();
				DrawNow();
			}
			break;
		case EExtrasShowShortCut:
			if(!iSearchSettings)
			{
				TInt Sel = ShowShortCutListL();
				if(Sel >= 0 && Sel <= 10)
				{
					MoveToShortCutL(Sel + 48);
				}
			}	
			break;
		case EExtrasSetShortCut:
			if(!iSearchSettings)
			{
				HBufC* Helppp = StringLoader::LoadLC(R_STR_SELSCNUMBER);
				
				TInt ScNumber(0);
				CAknNumberQueryDialog* Dialog = CAknNumberQueryDialog::NewL(ScNumber,CAknQueryDialog::ENoTone);
				Dialog->PrepareLC(R_SC_DIALOG);
				Dialog->SetPromptL(*Helppp);
				if(Dialog->RunLD())
				{
					SetShortCutL(48 + ScNumber);
				}
				
				CleanupStack::PopAndDestroy(Helppp);			
			}
			break;
        case EFileShowName:
			{
				Hjelpper.Zero();
				
				if(iSearchSettings)
        		{
        			TInt Dummy;
        			iSearchSettings->GetCurrSelFileL(Hjelpper,Dummy);
        		}
				else if(iFFolderReader)
				{
					HBufC* CurrPath = iFFolderReader->CurrentPath();
					if(CurrPath)
					{
						Hjelpper.Copy(CurrPath->Des());			
						if(!AppendFileName(Hjelpper, -1))
						{
							Hjelpper.Zero();
						}
					}
				}
				
				if(BaflUtils::FileExists(CCoeEnv::Static()->FsSession(),Hjelpper))
				{
					iShowString = new(ELeave)CShowString();
					iShowString->ConstructL(Hjelpper);
				}
				
				SetMenuL();
				DrawNow();
			}
			break;
		case ESSBack:
			{
				delete iShowString;
				iShowString = NULL;
				SetMenuL();
				DrawNow();
			}
			break;
		case EFileDetails:
			{	
				TInt CurrFilType(CFFileItem::EFDriveFile);
				TFileName FileFullName;
				
				if(iSearchSettings)
				{
					iSearchSettings->GetCurrSelFileL(FileFullName,CurrFilType);
				}
				else if(iFFolderReader)
				{
					GetFileType(CurrFilType, -1);
					if(CurrFilType >= CFFileItem::EFDriveFile)
					{
						HBufC* CurrPath = iFFolderReader->CurrentPath();
						if(CurrPath)
						{
							FileFullName.Copy(CurrPath->Des());
							
							if(!AppendFileName(FileFullName, -1))
							{
								FileFullName.Zero();
							}
						}
					}
				}
				
				if(CurrFilType >= CFFileItem::EFDriveFile
				&& FileFullName.Length())
				{
					ShowInfoL(FileFullName,CurrFilType);
				}
			}
			break;
		case EFileDelete:
			if((!iFindBox || iSearchSettings) && iIconHandler)
			{		
				TFileName Hjelpper2;
						
				if(iEikFileHandler)
				{
					iEikFileHandler->Cancel();
				}
				
				delete iEikFileHandler;
				iEikFileHandler = NULL;
				
				iFilemanNameBuf.Zero();
				if(iSelectionBox && !iSearchSettings)
				{
					iFilemanSelIndex = GetSelectedIndex();
				}
				
				delete iFilemanNameArray;
				iFilemanNameArray = NULL;
				iFilemanNameArray = new(ELeave)CDesCArrayFlat(1);
				AddSelectedFilesL(iFilemanNameArray,ETrue);
				
				if(iFilemanNameArray->MdcaCount() > 1)
				{
					StringLoader::Load(Hjelpper2,R_STR_DELFOILES);
					Hjelpper.Format(Hjelpper2,iFilemanNameArray->MdcaCount());
				}
				else if(iFilemanNameArray->MdcaCount())
				{
					TInt CurrTypeId(CFFileItem::EFFolderFile);
					
					CYBRecognitionResult* Res = new(ELeave)CYBRecognitionResult(); 
			
					iIconHandler->RecognizeFile(*Res,iFilemanNameArray->MdcaPoint(0));
					
					if(Res->iIdString)
					{
						CurrTypeId = iIconHandler->GetIconIndex(*Res->iIdString);
					}
					
					delete Res;
					Res = NULL;

					if(CurrTypeId <= CFFileItem::EFDriveFile)
					{
						return;// do not delete drives
					}
					
					
					Hjelpper2.Copy(iFilemanNameArray->MdcaPoint(0));
					if(CurrTypeId == CFFileItem::EFFolderFile)
					{
						Hjelpper2.Copy(Hjelpper2.Left(Hjelpper2.Length() - 1));
					}
					TParsePtrC NameHjelp(Hjelpper2);
					
					StringLoader::Load(Hjelpper,R_CMD_DELETE);
					Hjelpper.Append(_L(" "));
					Hjelpper.Append(NameHjelp.NameAndExt());
				}
				else
				{
					delete iFilemanNameArray;
					iFilemanNameArray = NULL;
					return;
				}
				
				CAknQueryDialog* dlg = CAknQueryDialog::NewL();
				if(dlg->ExecuteLD(R_QUERY,Hjelpper))
				{
					if(iSelectionBox && !iSearchSettings)
					{
						iSelectionBox->ClearSelection();
					}
			
					iEikFileHandler = new(ELeave)CEikFileHandler(*this,*iIconHandler);
					iEikFileHandler->ConstructL();
					iEikFileHandler->SetOperationL(EEikFDelete,iFilemanNameArray,NULL);
					iEikFileHandler->Start();
				}
				else
				{
					delete iFilemanNameArray;
					iFilemanNameArray = NULL;
				}
			}
			break;
		case EFileRename:
			{
				if(iEikFileHandler)
				{
					iEikFileHandler->Cancel();
				}
				
				delete iEikFileHandler;
				iEikFileHandler = NULL;
				
				iFilemanNameBuf.Zero();
				iFilemanSelIndex = -1;

				delete iFilemanNameArray;
				iFilemanNameArray = NULL;
				iFilemanNameBuf.Zero();
				
				TInt CurrTypeId(CFFileItem::EFDriveFile);
				
				TBuf<160> OldName;

				if(iSearchSettings)
				{
					iFilemanNameBuf.Zero();
					iSearchSettings->GetCurrSelFileL(iFilemanNameBuf,CurrTypeId);
				
					TParsePtrC NamHjelpper(iFilemanNameBuf);
					
					// NOTE:: see what happens with folders
					OldName.Copy(NamHjelpper.NameAndExt());
				}
				else if(iFFolderReader && !iFindBox)
				{
					HBufC* CurrPath = iFFolderReader->CurrentPath();
					if(CurrPath)
					{	
						if(AppendFileName(OldName, -1))
						{
							if(iSelectionBox)
							{
								iSelectionBox->ClearSelection();
							}
							
							iFilemanNameBuf.Copy(CurrPath->Des());
							iFilemanNameBuf.Append(OldName);
							
							GetFileType(CurrTypeId,-1);
						}
					}
				}
				
				if(!BaflUtils::FileExists(CCoeEnv::Static()->FsSession(),iFilemanNameBuf))
				{
					iFilemanNameBuf.Zero();
				}
				else
				{
					
					if(CurrTypeId <= CFFileItem::EFDriveFile)
					{
						return;// do not rename drives
					}
					else if(CurrTypeId == CFFileItem::EFFolderFile)					
						Hjelpper.Copy(OldName.Left(OldName.Length() - 1));
					else
						Hjelpper.Copy(OldName);
					
					
					HBufC* Buff= StringLoader::LoadLC(R_STR_NEWNAME);

					CAknTextQueryDialog* Dialog = CAknTextQueryDialog::NewL(Hjelpper,CAknQueryDialog::ENoTone);
					Dialog->PrepareLC(R_ASK_NAME_DIALOG);
					Dialog->SetPromptL(*Buff);
				//	Dialog->SetMaxLength(TInt aLength);
					
					if(Dialog->RunLD())
					{
						if(Hjelpper != OldName
						&& iFilemanNameBuf.Length())
						{
							TParsePtrC NamHjelp(iFilemanNameBuf.Left(iFilemanNameBuf.Length() -1 ));
							
							iFilemanNameArray = new(ELeave)CDesCArrayFlat(1);
							iFilemanNameArray->AppendL(iFilemanNameBuf);
							
							iFilemanNameBuf.Copy(NamHjelp.DriveAndPath());
							iFilemanNameBuf.Append(Hjelpper);
							
							if(CurrTypeId == CFFileItem::EFFolderFile)
							{
								iFilemanNameBuf.Append(KtxDoubleSlash);
							}
							
							iEikFileHandler = new(ELeave)CEikFileHandler(*this,*iIconHandler);
							iEikFileHandler->ConstructL();
							iEikFileHandler->SetOperationL(EEikFRename,iFilemanNameArray,&iFilemanNameBuf);
							iEikFileHandler->Start();
						}
					}
					
					CleanupStack::PopAndDestroy(Buff);
				}
			}
			break;
		case EFileNewFolder:
			if(iFFolderReader && !iFindBox && !iSearchSettings)
			{
				HBufC* CurrPath = iFFolderReader->CurrentPath();
				if(CurrPath)
				{
					if(CurrPath->Des().Length())
					{
						iFilemanSelIndex = -1;
						TBuf<160> FolderNamHjelp;
						
						StringLoader::Load(Hjelpper,R_STR_FDERNAME);

						CAknTextQueryDialog* Dialog = CAknTextQueryDialog::NewL(FolderNamHjelp,CAknQueryDialog::ENoTone);
						Dialog->PrepareLC(R_ASK_NAME_DIALOG);
						Dialog->SetPromptL(Hjelpper);
						if(Dialog->RunLD())
						{	
							Hjelpper.Copy(CurrPath->Des());
							Hjelpper.Append(FolderNamHjelp);
							Hjelpper.Append(KtxDoubleSlash);
							if(!EikFileUtils::FolderExists(Hjelpper))
							{
								if(iSelectionBox)
								{
									iSelectionBox->ClearSelection();
								}
								
								TInt Errno = CEikonEnv::Static()->FsSession().MkDir(Hjelpper);
								if(Errno == KErrNone)
								{
									TEntry MyEntry;
									if(KErrNone == CCoeEnv::Static()->FsSession().Entry(Hjelpper,MyEntry)
									&& Hjelpper.Length())
									{
										iFFolderReader->AppendToListL(MyEntry,CFFileItem::EFFolderFile);	
										iFilemanNameBuf.Copy(Hjelpper);
									}
									
									
									FolderReadFinnishL(NULL, KErrNone);
								}
								else
								{
									StringLoader::Load(Hjelpper,R_STR_ERRCREATEFLDR);
									Hjelpper.AppendNum(Errno);
									ShowNoteL(Hjelpper, ETrue);
								}
								
							}
							else
							{
								StringLoader::Load(Hjelpper,R_STR_ERRCREATEFLDR);
								ShowNoteL(Hjelpper, ETrue);
							}
						}
					}
				}
			}
			break;
		case EEditCopy:
			{
				CDesCArrayFlat* TmpPasteArray = new(ELeave)CDesCArrayFlat(5);
				CleanupStack::PushL(TmpPasteArray);
					
				AddSelectedFilesL(TmpPasteArray,ETrue);//EFalse);// test if cut/copy works for folders	
				
				CleanupStack::Pop(TmpPasteArray);
					
				if(TmpPasteArray->MdcaCount())
				{
					TBool Appendpst(EFalse);
					
					TInt AppendCount(0);
					if(!iIsCutOperation && iPasteArray)
					{
						StringLoader::Load(Hjelpper,R_STR_APPENDLIST);
						CAknQueryDialog* dlg = CAknQueryDialog::NewL();
						if(dlg->ExecuteLD(R_QUERY,Hjelpper))
						{
							Appendpst = ETrue;
						}
					}
					
					if(Appendpst && iPasteArray)
					{
						for(TInt i=0; i < TmpPasteArray->Count(); i++)
						{
							AppendCount = AppendCount + 1;
							iPasteArray->AppendL(TmpPasteArray->MdcaPoint(i));
						}
					
						delete TmpPasteArray;
						TmpPasteArray = NULL;
					}
					else
					{
						delete iPasteArray;
						iPasteArray = NULL;
						iPasteArray = TmpPasteArray;
						TmpPasteArray = NULL;
					
						AppendCount = iPasteArray->MdcaCount();
					}
					
					iIsCutOperation = EFalse;
					
					TBuf<60> Hjelpper,Format;
					StringLoader::Load(Format,R_STR_FILESADCY);
					Hjelpper.Format(Format,AppendCount);
			
					ShowNoteL(Hjelpper, EFalse);	
				}
				
				delete TmpPasteArray;
			}
			break;
		case EEditCut:
			if(iSearchSettings || !iDriveReadOnly)
			{
				CDesCArrayFlat* TmpPasteArray = new(ELeave)CDesCArrayFlat(5);
				CleanupStack::PushL(TmpPasteArray);	
				
				AddSelectedFilesL(TmpPasteArray,ETrue);//EFalse);// test if cut/copy works for folders		
				
				CleanupStack::Pop(TmpPasteArray);
				
				if(TmpPasteArray->MdcaCount())
				{
					TInt AppendCount(0);
					TBool Appendpst(EFalse);
					
					if(iIsCutOperation && iPasteArray)
					{
						StringLoader::Load(Hjelpper,R_STR_APPENDLIST);
						CAknQueryDialog* dlg = CAknQueryDialog::NewL();
						if(dlg->ExecuteLD(R_QUERY,Hjelpper))
						{
							Appendpst = ETrue;
						}
					}
					
					if(Appendpst && iPasteArray)
					{
						for(TInt i=0; i < TmpPasteArray->Count(); i++)
						{
							AppendCount = AppendCount + 1;
							iPasteArray->AppendL(TmpPasteArray->MdcaPoint(i));
						}
					
						delete TmpPasteArray;
						TmpPasteArray = NULL;
					}
					else
					{
						delete iPasteArray;
						iPasteArray = NULL;
						iPasteArray = TmpPasteArray;
						TmpPasteArray = NULL;
						
					}
				
					iIsCutOperation = ETrue;
					
					if(AppendCount > 0)
					{
						HBufC* Buff= StringLoader::LoadLC(R_STR_FILESADCY);
						Hjelpper.Format(*Buff,iPasteArray->MdcaCount());
						CleanupStack::PopAndDestroy(Buff);
					}
					else
					{	
						HBufC* Buff= StringLoader::LoadLC(R_STR_PASTEWILLREM);
						Hjelpper.Format(*Buff,iPasteArray->MdcaCount());
						CleanupStack::PopAndDestroy(Buff);
					}
				
					ShowNoteL(Hjelpper, EFalse);	
				}
				
				delete TmpPasteArray;
			}
			break;
        case EEditClearPaste:
        	{
        		delete iPasteArray;
        		iPasteArray = NULL;
        	}
        	break;
        case EEditShowPaste:
        	{
        		TBuf<60> Hjelppp;
	        	if(iSelectContainer)
	        	{
	        		
	        		ShowPasteFilesL(iSelectContainer->PasteArray(),Hjelppp);
	        	}
	        	else 
	        	{
	        		if(iIsCutOperation)
	        			StringLoader::Load(Hjelppp,R_SH_STR_FILCUT);
					else
						StringLoader::Load(Hjelppp,R_SH_STR_FILCOPY);
					
					ShowPasteFilesL(iPasteArray,Hjelppp);
	        	}
        	}
        	break;
        case EEditPasteNewFile:
        	if(iPasteArray && iFFolderReader && !iFindBox 
			&& !iDriveReadOnly && iIconHandler && !iSearchSettings)
			{
        		iIconHandler->PasteFilesToHandlerL(*iPasteArray);
        	}
        	break;
		case EEditPaste:
			if(iPasteArray && iFFolderReader && !iFindBox 
			&& !iDriveReadOnly && iIconHandler && !iSearchSettings)
			{
				HBufC* CurrPath = iFFolderReader->CurrentPath();
				if(iPasteArray->MdcaCount() && CurrPath)
				{
					if(CurrPath->Des().Length())
					{
						HBufC* Buff= StringLoader::LoadLC(R_STR_PASTEFTOFOL);
						Hjelpper.Format(*Buff,iPasteArray->MdcaCount());
						
						CAknQueryDialog* dlg = CAknQueryDialog::NewL();
						if(dlg->ExecuteLD(R_QUERY,Hjelpper))
						{
							if(iSelectionBox)
							{
								iSelectionBox->ClearSelection();
								iFilemanSelIndex = GetSelectedIndex();
							}
							
							iFilemanNameBuf.Copy(CurrPath->Des());
							
							iEikFileHandler = new(ELeave)CEikFileHandler(*this,*iIconHandler);
							iEikFileHandler->ConstructL();
							if(iIsCutOperation)
								iEikFileHandler->SetOperationL(EEikFMove,iPasteArray,&iFilemanNameBuf);
							else
								iEikFileHandler->SetOperationL(EEikFCopy,iPasteArray,&iFilemanNameBuf);
				
							iEikFileHandler->Start();
						}
						
						CleanupStack::PopAndDestroy(Buff);
					}
				}
			}
			break;
        case EFolderViewMarkTogle:
        	if(iSelectContainer)
			{
				iSelectContainer->MarkUnMarkL(EFalse, ETrue);
			}
        	else if(iSearchSettings)
			{
				iSearchSettings->HandleViewCommandL(EFolderViewMarkTogle);
			}
			else if(iSelectionBox && !iFindBox)
			{
				//TInt CurrSel = GetSelectedIndex();
				TInt CurrSel = iSelectionBox->CurrentItemIndex();
				
				if(CurrSel >= 0)
				{	
					TInt FilTyp = -1;
					
					if(iFindBox)
					{
						CAknFilteredTextListBoxModel* model = STATIC_CAST(CAknFilteredTextListBoxModel*,iSelectionBox->Model());		
		
						if(model)
						{
							GetFileType(FilTyp, model->Filter()->FilteredItemIndex(CurrSel)); 
						}
					}
					else
						GetFileType(FilTyp, CurrSel); 
						
					if(FilTyp >= CFFileItem::EFFolderFile)	
					{		
						iSelectionBox->View()->ToggleItemL(CurrSel);
					}
				}
			}
			DrawNow();
			break;
		case EFolderViewMarkAll:
			if(iSelectContainer)
			{
				iSelectContainer->MarkUnMarkL(ETrue, EFalse);
			}
        	else if(iSearchSettings)
			{
				iSearchSettings->HandleViewCommandL(EFolderViewMarkAll);
			}
			else if(iSelectionBox && !iFindBox)
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
			DrawNow();
			break;
		case EFolderViewUnMarkAll: 
			if(iSelectContainer)
			{
				iSelectContainer->MarkUnMarkL(EFalse, EFalse);
			}
        	else if(iSearchSettings)
			{
				iSearchSettings->HandleViewCommandL(EFolderViewUnMarkAll);
			}
			else if(iSelectionBox)
			{
				iSelectionBox->ClearSelection();
			}
			DrawNow();
			break; 
        default:
        	{
        		CYBrowserBaseHandler* TmpHand = GetCurrentHandler();
        		if(TmpHand)
        		{
	        		TmpHand->HandleCommandL(aCommand);
	        	}
	        	else if(iSearchSettings)
	        	{
	        		iSearchSettings->HandleViewCommandL(aCommand);
	        	}
	        	else if(iShowString)
	        	{
			        iShowString->HandleViewCommandL(aCommand);
	        	}
        	}
            break;
        }
    }
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CFileListContainer::ReFreshListL(TBool aFullReFresh,HBufC* aTmpFolder)
{
	if(iFFolderReader && !iSearchSettings)
	{
		iFFolderReader->RemoveTypeFilterArrayL();
		
		TInt SAve(0);
		TInt FilCount(0);
		
		if(aTmpFolder)
		{
			if(aFullReFresh)
				FilCount = iFFolderReader->RemovePathAndReFreshL(aTmpFolder->Des(),SAve);
			else
				FilCount = iFFolderReader->StartReadingL(aTmpFolder->Des(),SAve);	
		}
		else 
		{
			HBufC* CurrPath = iFFolderReader->CurrentPath();
			
			if(CurrPath)
			{
				iFilemanNameBuf.Copy(CurrPath->Des());
				AppendFileName(iFilemanNameBuf, -1);
				
				if(aFullReFresh)
					FilCount = iFFolderReader->RemovePathAndReFreshL(CurrPath->Des(),SAve);
				else
					FilCount = iFFolderReader->StartReadingL(CurrPath->Des(),SAve);	
			}
			else
			{
				FilCount = iFFolderReader->StartReadingL(KtxEmpty,SAve);	
			}
		}
		
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
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CFileListContainer::ChangeViewTypeL(TBool aSearchChange)
{	
	TInt LastItemIndex(-1);
	if(iSelectionBox)
	{
		LastItemIndex = GetSelectedIndex();
	}

	CArrayFixFlat<TInt>* SelStuff = new(ELeave)CArrayFixFlat<TInt>(10);
	CleanupStack::PushL(SelStuff);
	
	if(iSelectionBox && !iFindBox)
	{
		const CArrayFix<TInt>* SelStuffTmp = iSelectionBox->SelectionIndexes();
		for(TInt i=0; i < SelStuffTmp->Count(); i++)
		{
			SelStuff->AppendL(SelStuffTmp->At(i));
		}
	}
	
	if(aSearchChange)
	{
		if(iSearchOn)
		{
			iSearchOn = EFalse;
		}
		else
		{
			iSearchOn = ETrue;
		}
	}
	else
	{
		if(iExtendedInfoOn)
		{
			iExtendedInfoOn = EFalse;
		}
		else
		{
			iExtendedInfoOn = ETrue;
		}
	}
	
	DisplayListBoxL();
	FolderReadFinnishL(NULL, KErrNone);
	
	if(SelStuff && iSelectionBox && !iFindBox)
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
void CFileListContainer::ShowPasteFilesL(CDesCArray* aArray,const TDesC& aTitle)
{
	if(aArray && iFFolderReader)
	{
		if(aArray->MdcaCount())
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
			
			TInt FileType(CFFileItem::EFJustFile);
			TFileName AddFil,Hjrlp;
			TEntry MyEntry;
			
			for(TInt i=0; i < aArray->MdcaCount(); i++)
			{
				if(KErrNone == CCoeEnv::Static()->FsSession().Entry(aArray->MdcaPoint(i),MyEntry))
				{
					if(iFFolderReader->FileTypeOkL(aArray->MdcaPoint(i),MyEntry,FileType))
					{
						TParsePtrC HjelpP(aArray->MdcaPoint(i));
						Hjrlp.Copy(HjelpP.NameAndExt());
					
						AddFil.Format(KtxShortListFormat,FileType,&Hjrlp);
						Array->AppendL(AddFil);
					}
				}
			}
			
		    list->Model()->SetItemTextArray(Array);
		    CleanupStack::Pop();//Array
			list->Model()->SetOwnershipType(ELbmOwnsItemArray);
			
			list->ItemDrawer()->ColumnData()->SetIconArray(iIconHandler->GetIconArrayL());
			
			popupList->SetTitleL(aTitle);
			popupList->ExecuteLD();
			
		    CleanupStack::Pop();            // popuplist
		    CleanupStack::PopAndDestroy();  // list
		}
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TKeyResponse CFileListContainer::MoveToShortCutL(TInt aShortCut)
{
	TKeyResponse Ret = EKeyWasNotConsumed;
	
	 if(iOkToContinue && !iSearchOn)
	 {
		TFileName ShortFil;
		TFindFile AppFile(CCoeEnv::Static()->FsSession());
		if(KErrNone != AppFile.FindByDir(KtxShortCutFileName, KNullDesC))
		{
			if(KErrNone ==CCoeEnv::Static()->FsSession().PrivatePath(ShortFil))
			{
				TFindFile privFile(CCoeEnv::Static()->FsSession());
				if(KErrNone == privFile.FindByDir(ShortFil, KNullDesC))
				{
					TParsePtrC hjelp(privFile.File());
					ShortFil.Copy(hjelp.Drive());
					ShortFil.Append(KtxShortCutFileName);
				}
			}
		}
		else
		{
			ShortFil.Copy(AppFile.File());
		}

		if(ShortFil.Length() && iFFolderReader)
		{
			CDictionaryFileStore* iDStore = CDictionaryFileStore::OpenLC(CCoeEnv::Static()->FsSession(),ShortFil, TUid::Uid(0x101F7BBD));

			TUid FileUid = {0};
			FileUid.iUid = 0x10 + aShortCut;

			if(iDStore->IsPresentL(FileUid))
			{
				RDictionaryReadStream in;
				in.OpenLC(*iDStore,FileUid);
				HBufC* HelpBuffer = HBufC::NewL(in,KMaxTInt);
				CleanupStack::PushL(HelpBuffer);

				iFFolderReader->Cancel();
				iSelectionBox->ClearSelection();
				
				TInt SAve(0);
				TInt FilCount = iFFolderReader->StartReadingL(HelpBuffer->Des(),SAve);

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

				CleanupStack::PopAndDestroy(2);// in,HelpBuffer
				
				Ret = EKeyWasConsumed;
			}

			CleanupStack::PopAndDestroy(1);// Store
		}
	 }
	 
	 return Ret;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CFileListContainer::SetShortCutL(TInt aShortCut)
{
	TFileName ShortFil;
	TFindFile AppFile(CCoeEnv::Static()->FsSession());
	if(KErrNone != AppFile.FindByDir(KtxShortCutFileName, KNullDesC))
	{
		if(KErrNone ==CCoeEnv::Static()->FsSession().PrivatePath(ShortFil))
		{
			TFindFile privFile(CCoeEnv::Static()->FsSession());
			if(KErrNone == privFile.FindByDir(ShortFil, KNullDesC))
			{
				TParsePtrC hjelp(privFile.File());
				ShortFil.Copy(hjelp.Drive());
				ShortFil.Append(KtxShortCutFileName);
			}
		}
	}
	else
	{
		ShortFil.Copy(AppFile.File());
	}

	if(ShortFil.Length()&& iFFolderReader)
	{
		HBufC* CurrPath = iFFolderReader->CurrentPath();
		if(CurrPath)
		{
			CDictionaryFileStore* iDStore = CDictionaryFileStore::OpenLC(CCoeEnv::Static()->FsSession(),ShortFil, TUid::Uid(0x101F7BBD));

			TUid FileUid = {0};
			FileUid.iUid = 0x10 + aShortCut;

			if(iDStore->IsPresentL(FileUid))
			{
				iDStore->Remove(FileUid);
				iDStore->CommitL();
			}

			RDictionaryWriteStream out1;
			out1.AssignLC(*iDStore,FileUid);

			out1 << CurrPath->Des();

			out1.CommitL(); 
			CleanupStack::PopAndDestroy(1);// out1

			iDStore->CommitL();
			CleanupStack::PopAndDestroy(1);// Store,
		}
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TInt CFileListContainer::ShowShortCutListL(void)
{
	TInt Ret = -1;

	TFileName ShortFil;
	TFindFile AppFile(CCoeEnv::Static()->FsSession());
	if(KErrNone != AppFile.FindByDir(KtxShortCutFileName, KNullDesC))
	{
		if(KErrNone ==CCoeEnv::Static()->FsSession().PrivatePath(ShortFil))
		{
			TFindFile privFile(CCoeEnv::Static()->FsSession());
			if(KErrNone == privFile.FindByDir(ShortFil, KNullDesC))
			{
				TParsePtrC hjelp(privFile.File());
				ShortFil.Copy(hjelp.Drive());
				ShortFil.Append(KtxShortCutFileName);
			}
		}
	}
	else
	{
		ShortFil.Copy(AppFile.File());
	}

	if(ShortFil.Length())
	{
		TFileName Hjelpper;
		CDesCArray* Array = new (ELeave) CDesCArrayFlat(1);
		CleanupStack::PushL(Array);

		if(BaflUtils::FileExists(CCoeEnv::Static()->FsSession(),ShortFil))
		{
			CDictionaryFileStore* iDStore = CDictionaryFileStore::OpenLC(CCoeEnv::Static()->FsSession(),ShortFil, TUid::Uid(0x101F7BBD));

			TUid FileUid = {0};
			
			for(TInt i = 0; i < 10; i++)
			{
				FileUid.iUid = 0x10 + 48 + i;

				if(iDStore->IsPresentL(FileUid))
				{
					RDictionaryReadStream in;
					in.OpenLC(*iDStore,FileUid);

					in >> Hjelpper;

					if(!Hjelpper.Length())
					{
						StringLoader::Load(Hjelpper,R_SH_STR_ROOT);
					}

					CleanupStack::PopAndDestroy(1);// in
				}
				else
				{	
					StringLoader::Load(Hjelpper,R_SH_STR_FREE);
				}
				
				ShortFil.Format(KtxFormatShortCutlList,i,&Hjelpper);
				Array->AppendL(ShortFil);
			}

			CleanupStack::PopAndDestroy();// Store
		}
		else
		{	
			StringLoader::Load(Hjelpper,R_SH_STR_FREE);
		
			for(TInt i = 0; i < 10; i++)
			{
				ShortFil.Format(KtxFormatShortCutlList,i,&Hjelpper);
				Array->AppendL(ShortFil);
			}
		}

		CAknSinglePopupMenuStyleListBox* list = new(ELeave)CAknSinglePopupMenuStyleListBox;
	    CleanupStack::PushL(list);
	    CAknPopupList* popupList = CAknPopupList::NewL(list,R_AVKON_SOFTKEYS_SELECT_CANCEL);
	    CleanupStack::PushL(popupList);
		
	    list->ConstructL(popupList, CEikListBox::ELeftDownInViewRect);
		list->CreateScrollBarFrameL(ETrue);
	    list->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,CEikScrollBarFrame::EAuto);
	    
	    list->Model()->SetItemTextArray(Array);
		list->Model()->SetOwnershipType(ELbmDoesNotOwnItemArray);
		
		StringLoader::Load(ShortFil,R_SH_STR_SCTITLE);
		popupList->SetTitleL(ShortFil);
		if (popupList->ExecuteLD())
	    {
			Ret = list->CurrentItemIndex();
		};
		
	    CleanupStack::Pop();            // popuplist
	    CleanupStack::PopAndDestroy(2);  // list,Array
	}
	return Ret;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CFileListContainer::ShowInfoL(const TDesC& aFileName,TInt aFileType)
{
	if(aFileType >= CFFileItem::EFDriveFile)
	{
		CDesCArrayFlat* Array = new (ELeave) CDesCArrayFlat(1);
		CleanupStack::PushL(Array);
		
		TEntry MyEntry;
		TFileName InfoLine,Helper;
		TBuf<100> Helper2;
		
		if(aFileType == CFFileItem::EFDriveFile)
		{
			if(aFileName.Length())
			{
				TInt CurrentDrive;
				TVolumeInfo volumeInfo; 
				TChar DriveChar = aFileName[0];
				
				CCoeEnv::Static()->FsSession().CharToDrive(DriveChar,CurrentDrive); 

				if(CCoeEnv::Static()->FsSession().IsValidDrive(CurrentDrive))
				{
					CCoeEnv::Static()->FsSession().Volume(volumeInfo,CurrentDrive);
					
					StringLoader::Load(Helper,R_SH_STR_FORMETNAME);
					InfoLine.Format(Helper,&volumeInfo.iName);
					Array->AppendL(InfoLine); 
					
					Helper2.Zero();
					Helper.Zero();
					CCoeEnv::Static()->FsSession().FileSystemName(Helper,CurrentDrive);
					StringLoader::Load(Helper2,R_SH_STR_FORMETFSYS);
					InfoLine.Format(Helper2,&Helper);
					Array->AppendL(InfoLine); 

			/**/		Helper.Num(volumeInfo.iUniqueID,EHex);
					
					StringLoader::Load(Helper2,R_SH_STR_FORMETID);
					InfoLine.Format(Helper2,&Helper);
					Array->AppendL(InfoLine);
					
					
			
					TUint32 HelperInt = (volumeInfo.iFree / 1024);
					
					StringLoader::Load(Helper,R_SH_STR_FORMETFREE);
					InfoLine.Format(Helper,HelperInt,EDecimal);	
					Array->AppendL(InfoLine);
					
					HelperInt = ((volumeInfo.iSize - volumeInfo.iFree) / 1024);

					StringLoader::Load(Helper,R_SH_STR_FORMETUSED);
					InfoLine.Format(Helper,HelperInt,EDecimal);
					Array->AppendL(InfoLine);
					
					HelperInt = (volumeInfo.iSize / 1024);	
					
					StringLoader::Load(Helper,R_SH_STR_FORMETSIZEKB);
					InfoLine.Format(Helper,HelperInt,EDecimal);
					Array->AppendL(InfoLine);
					
					switch(volumeInfo.iDrive.iType)
					{
					case EMediaNotPresent:
						StringLoader::Load(Helper,R_SH_STR_NOTPRESENT);
						break;
					case EMediaFloppy:
						StringLoader::Load(Helper,R_SH_STR_FLOPPY);
						break;
					case EMediaHardDisk:
						StringLoader::Load(Helper,R_SH_STR_HARDDISK);
						break;
					case EMediaCdRom:
						StringLoader::Load(Helper,R_SH_STR_CDROM);
						break;
					case EMediaRam:
						StringLoader::Load(Helper,R_SH_STR_RAM);
						break;
					case EMediaFlash:
						StringLoader::Load(Helper,R_SH_STR_FLASH);
						break;
					case EMediaRom:
						StringLoader::Load(Helper,R_SH_STR_ROM);
						break;	
					case EMediaRemote:
						StringLoader::Load(Helper,R_SH_STR_REMOTE);
						break;
					default: //case TMediaType::EMediaUnknown:
						StringLoader::Load(Helper,R_SH_STR_UNKNOWN);
						break;
					}
					
					StringLoader::Load(Helper2,R_SH_STR_FORMETTYPE);
					InfoLine.Format(Helper2,&Helper);
					Array->AppendL(InfoLine);

					TUint AnotherHelper = volumeInfo.iDrive.iMediaAtt;

					if(AnotherHelper & KMediaAttWriteProtected)
					{
						StringLoader::Load(Helper,R_SH_STR_ATRWPROT);
						InfoLine.Copy(Helper);
						Array->AppendL(InfoLine);
					}

					if(AnotherHelper & KMediaAttLocked)
					{
						StringLoader::Load(Helper,R_SH_STR_ATRLOCKED);
						InfoLine.Copy(Helper);
						Array->AppendL(InfoLine);
					}

					if(AnotherHelper & KMediaAttFormattable)
					{
						StringLoader::Load(Helper,R_SH_STR_ATRFRMATTABLE);
						InfoLine.Copy(Helper);
						Array->AppendL(InfoLine);
					}

					if(AnotherHelper & KMediaAttVariableSize)
					{
						StringLoader::Load(Helper,R_SH_STR_ATRVARSIZE);
						InfoLine.Copy(Helper);
						Array->AppendL(InfoLine);
					}

					AnotherHelper = volumeInfo.iDrive.iDriveAtt;

					if(AnotherHelper & KDriveAttRom)
					{
						StringLoader::Load(Helper,R_SH_STR_ATRROM);
						InfoLine.Copy(Helper);
						Array->AppendL(InfoLine);
					}

					if(AnotherHelper & KDriveAttRemovable)
					{
						StringLoader::Load(Helper,R_SH_STR_ATRREMABLE);
						InfoLine.Copy(Helper);
						Array->AppendL(InfoLine);
					}
			/*	*/
				}
			}
		}
		else if(KErrNone == CCoeEnv::Static()->FsSession().Entry(aFileName,MyEntry))
		{
			if(aFileType == CFFileItem::EFFolderFile)
			{			
				if(MyEntry.IsDir())
				{
					TInt32 HelperInt(0);
					
					TInt32 Ret = 0;
					
					if(iFFolderReader)
					{
						iFFolderReader->GetFolderFileCountL(aFileName,HelperInt,Ret,ETrue);
					}
					
					StringLoader::Load(Helper,R_SH_STR_FORMETFILES);
					InfoLine.Format(Helper,Ret);
					Array->AppendL(InfoLine);
					
					StringLoader::Load(Helper,R_SH_STR_FORMETFLDR);
					InfoLine.Format(Helper,HelperInt);
					Array->AppendL(InfoLine);
					
					GetTimeBuffer(MyEntry.iModified.DateTime(),InfoLine,ETrue,EFalse);
					Array->AppendL(InfoLine);
					
					GetTimeBuffer(MyEntry.iModified.DateTime(),InfoLine,EFalse,EFalse);
					Array->AppendL(InfoLine);
					
					if(MyEntry.IsTypeValid())
					{
						TUint Num = MyEntry.MostDerivedUid().iUid;
						Helper.AppendNum(Num,EHex);
						
						
						StringLoader::Load(Helper,R_SH_STR_FORMETTYPEID);
						InfoLine.Format(Helper,&Helper);
						Array->AppendL(InfoLine);
					}

					if(MyEntry.IsHidden())
					{	
						StringLoader::Load(InfoLine,R_SH_STR_ATRHIDDEN);
						Array->AppendL(InfoLine);
					}

					if(MyEntry.IsReadOnly())
					{
						StringLoader::Load(InfoLine,R_SH_STR_ATRREADONLY);
						Array->AppendL(InfoLine);
					}
					 
					if(MyEntry.IsSystem())
					{
						StringLoader::Load(InfoLine,R_SH_STR_ATRSYSTEM);
						Array->AppendL(InfoLine);
					}

					if(MyEntry.iAtt & KEntryAttVolume)
					{
						StringLoader::Load(InfoLine,R_SH_STR_ATRVOLUME);
						Array->AppendL(InfoLine);
					}
				}
			}
			else // it is a file
			{
				TBuf<100> Hjelpper,Hjelpper2;
				
				TInt HelperInt = MyEntry.iSize;
			
				if(HelperInt < 1024)
				{
					StringLoader::Load(Helper,R_SH_STR_FORMETSIZEB);
					InfoLine.Format(Helper,HelperInt);
				}
				else
				{
					StringLoader::Load(Helper,R_SH_STR_FORMETSIZEKB);
					
					HelperInt = HelperInt /1024;
					InfoLine.Format(Helper,HelperInt);
				}

				Array->AppendL(InfoLine);
				
				Hjelpper.Zero();
				if(iIconHandler)
				{
					CYBRecognitionResult* Res = new(ELeave)CYBRecognitionResult(); 
			
					iIconHandler->RecognizeFile(*Res,aFileName);
					
					if(Res->iIdString)
					{
						if(Res->iIdString->Des().Length() > 100)
							Hjelpper.Copy(Res->iIdString->Des().Left(100));
						else
							Hjelpper.Copy(Res->iIdString->Des());
					}
					
					delete Res;
					Res = NULL;
				}
					
				TInt i =0;
							
				for(i = 0; i < Hjelpper.Length(); i++)
				{
					if(Hjelpper[i] == 47)
						break;
				}

				if(i < Hjelpper.Length())
				{
					TBuf<100> Hjelpper3;
					
					StringLoader::Load(Hjelpper3,R_SH_STR_FORMETTYPE);
					Hjelpper2.Copy(Hjelpper.Left(i));
					InfoLine.Format(Hjelpper3,&Hjelpper2);
					Array->AppendL(InfoLine);
					
					
					StringLoader::Load(Hjelpper3,R_SH_STR_FORMETTYPEID);
					Hjelpper2.Copy(Hjelpper.Right(Hjelpper.Length() - (i+1)));
					InfoLine.Format(Hjelpper3,&Hjelpper2);
					Array->AppendL(InfoLine);
				}
				else
				{
					StringLoader::Load(Hjelpper2,R_SH_STR_FORMETTYPE);
					InfoLine.Format(Hjelpper2,&Hjelpper);
					Array->AppendL(InfoLine);
				}
				
				GetTimeBuffer(MyEntry.iModified.DateTime(),InfoLine,ETrue,EFalse);
				Array->AppendL(InfoLine);
				
				GetTimeBuffer(MyEntry.iModified.DateTime(),InfoLine,EFalse,EFalse);
				Array->AppendL(InfoLine);

				if(MyEntry.IsTypeValid())
				{
					TUint Num = MyEntry.MostDerivedUid().iUid;
					Helper.Num(Num,EHex);
					
					StringLoader::Load(Hjelpper2,R_SH_STR_FORMETID);
					InfoLine.Format(Hjelpper2,&Helper);
					
					InfoLine.TrimAll();
					if(InfoLine.Length())
					{
						Array->AppendL(InfoLine);
					}
				}

				if(MyEntry.IsArchive())
				{
					StringLoader::Load(InfoLine,R_SH_STR_ATRARCHIV);
					Array->AppendL(InfoLine);
				}

				if(MyEntry.IsHidden())
				{
					StringLoader::Load(InfoLine,R_SH_STR_ATRHIDDEN);
					Array->AppendL(InfoLine);
				}

				if(MyEntry.IsReadOnly())
				{
					StringLoader::Load(InfoLine,R_SH_STR_ATRREADONLY);
					Array->AppendL(InfoLine);
				}
				 
				if(MyEntry.IsSystem())
				{
					StringLoader::Load(InfoLine,R_SH_STR_ATRSYSTEM);
					Array->AppendL(InfoLine);
				}
			}
		}
	
		if(Array->MdcaCount())
		{
/*			for(TInt p=0; p < Array->MdcaCount(); p++)
			{
				ShowNoteL(Array->MdcaPoint(p), ETrue);
			}
*/			
			CAknSingleHeadingPopupMenuStyleListBox* list = new(ELeave) CAknSingleHeadingPopupMenuStyleListBox;
		    CleanupStack::PushL(list);

			CAknPopupList* popupList = CAknPopupList::NewL(list,R_AVKON_SOFTKEYS_BACK);
		    CleanupStack::PushL(popupList);
			
		    list->ConstructL(popupList, CEikListBox::ELeftDownInViewRect);
			list->CreateScrollBarFrameL(ETrue);
		    list->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,CEikScrollBarFrame::EAuto);
		    list->Model()->SetItemTextArray(Array);
			list->Model()->SetOwnershipType(ELbmDoesNotOwnItemArray);
			
			TParsePtrC NameHjelp(aFileName);
			
			popupList->SetTitleL(NameHjelp.NameAndExt());
			popupList->ExecuteLD();
			
		    CleanupStack::Pop();            // popuplist
		    CleanupStack::PopAndDestroy();  // list
		}
	
		CleanupStack::PopAndDestroy(Array);
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CFileListContainer::GetTimeBuffer(const TDateTime& aDateTime,TDes& aBuffer,TBool aDate,TBool aList)
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
			Hjelpper.Format(KtxFormatInfoTimeList,&HourHjlep,&MinHjelp,&SecHjelp);		
			
			StringLoader::Load(aBuffer,R_SH_STR_TIME);
			aBuffer.Append(_L("\t"));
			aBuffer.Append(Hjelpper);
		}
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CFileListContainer::SizeChanged()
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
	ReFreshListL(EFalse,NULL);
}
/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
TTypeUid::Ptr CFileListContainer::MopSupplyObject(TTypeUid aId)
{	
	if (iBgContext)
	{
		return MAknsControlContext::SupplyMopObject(aId, iBgContext );
	}
	
	return CCoeControl::MopSupplyObject(aId);
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CFileListContainer::SizeChangedForFindBox()
    {
    if ( iSelectionBox&& iFindBox )
        {
   
        CAknColumnListBox* aknListBox = STATIC_CAST(CAknColumnListBox*, iSelectionBox);
        AknFind::HandleFixedFindSizeChanged(this, aknListBox, iFindBox);
        }
    }
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CFileListContainer::HandleResourceChange(TInt aType)
{
	TRect NowRect(Rect());
	TRect rect(CEikonEnv::Static()->EikAppUi()->ClientRect());
	TBool SetR(EFalse);
	
    if((aType==KEikDynamicLayoutVariantSwitch) 
    || (NowRect.Height() != rect.Height())
    || (NowRect.Width() != rect.Width()))
    {  	
    	SetR = ETrue;
    //    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, rect);
        
        
    }
  
 	if(SetR)
 	{	    
 		if(iSelectContainer)
		{
			iSelectContainer->SetRect(rect);
		}
		
	 	if(iShowString)
	 	{
	 		iShowString->SetRect(rect);
	 	}
	 	
	 	if(iShowString)
	 	{
	 		iShowString->UpDateSizeL();
	 	}
	 	
	 	if(iYuccaSettings)
	 	{
	 		iYuccaSettings->SetRect(rect);
	 	}
	 	
	 	if(iCommandSCSettings)
	 	{
	 		iCommandSCSettings->SetRect(rect);
	 	}
	 		
	 	if(iSearchSettings)
	 	{
	 		iSearchSettings->SetRect(rect);
	 	}
	 	
	 	CYBrowserBaseHandler* TmpHand = GetCurrentHandler();
        if(TmpHand)
	 	{
	 		TmpHand->SetRect(rect);
	 	}
	 	
	    SetRect(rect);	
 	}
 	
	CCoeControl::HandleResourceChange(aType);
}
	
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
TKeyResponse CFileListContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
	TKeyResponse Ret = EKeyWasNotConsumed;
	
	CYBrowserBaseHandler* TmpHand = GetCurrentHandler();
    if(iSelectContainer)
	{
		iSelectContainer->OfferKeyEventL(aKeyEvent,aType);
	}
    else if(TmpHand)
	{
		Ret = TmpHand->OfferKeyEventL(aKeyEvent,aType);
	}
	else if(iShowString)
	{
		Ret = iShowString->OfferKeyEventL(aKeyEvent,aType);
	}
	else if(iYuccaSettings)
	{
		Ret = iYuccaSettings->OfferKeyEventL(aKeyEvent,aType);
	}
	else if(iSearchSettings)
	{	
		Ret = SelectAndExecuteSCCommandL(aKeyEvent.iScanCode);
		
		if(Ret == EKeyWasNotConsumed)
		{		
			Ret = iSearchSettings->OfferKeyEventL(aKeyEvent,aType);
		}
	}
	else if(iCommandSCSettings)
	{
		Ret = iCommandSCSettings->OfferKeyEventL(aKeyEvent,aType);
	}
	else 
	{	
		if(EEventKey == aType)
		{
			if((EModifierShift 	& CCoeEnv::Static()->WsSession().GetModifierState()))
			{
				Ret = MoveToShortCutL(aKeyEvent.iScanCode);
			}
			else
			{
				Ret = SelectAndExecuteSCCommandL(aKeyEvent.iScanCode);
			}
			
			if(Ret == EKeyWasNotConsumed)
			{		
				if(aKeyEvent.iCode == EKeyDelete 
				|| aKeyEvent.iCode == EKeyBackspace)
				{
					if(!iSearchOn)
					{
						HandleViewCommandL(EFileDelete);
						Ret = EKeyWasConsumed;
					}
				}
			}
		}
		
		if(Ret == EKeyWasNotConsumed)
		{
			switch (aKeyEvent.iCode)
		    {
		    case '*':
		    	if(!iSearchOn)
		    	{
		    		HandleViewCommandL(EFolderViewMarkTogle);
		    	}
		    	break;
		    case '#':
		    	if(!iSearchOn)
		    	{
		    		ChangeViewTypeL(EFalse);
		    		SetMenuL();
		    	}
		    	break;				
			case EKeyRightArrow:
			case EKeyDevice3:
				if(iSearchOn && (aKeyEvent.iCode == EKeyDevice3))
				{
					TInt CurrType(-1);
					GetFileType(CurrType, -1);
					
					if(CurrType >= CFFileItem::EFJustFile)
					{		
						HandleViewCommandL(EVBOpenFile);
						break;
					}
				}
				else if(iFFolderReader && iSelectionBox && !iSearchOn)
				{
					iLastFolder.Zero();
					
					TInt CurrType(-1);
					GetFileType(CurrType, -1);
					
					if(CurrType == CFFileItem::EFExtraFolder)
					{
						TInt Sel = iSelectionBox->CurrentItemIndex();
						RPointerArray<CFFileItem> CurrArray =  iFFolderReader->ItemArray();

						if(Sel < CurrArray.Count() && Sel >= 0)
						{
							if(CurrArray[Sel])
							{
								if(CurrArray[Sel]->iName
								&& CurrArray[Sel]->iExtra != 0)
								{
									iIconHandler->OpenExtraFoldersL(CurrArray[Sel]->iName->Des(),CurrArray[Sel]->iExtra);
								}
							}
						}
					}
					else if(CurrType == CFFileItem::EFDriveFile
					|| CurrType == CFFileItem::EFFolderFile)
					{	
						iFFolderReader->Cancel();
						if(iFFolderReader->IsActive())
						{
							// wait for next key input...
						}
						else
						{
							TFileName NewFolder;
							
							HBufC* CurrPath = iFFolderReader->CurrentPath();
							if(CurrPath)
							{
								NewFolder.Copy(CurrPath->Des());
								AppendFileName(NewFolder,-1);	
							}
							
							iSelectionBox->ClearSelection();
							
							TInt SAve(0);
							TInt FilCount = iFFolderReader->StartReadingL(NewFolder,SAve);

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
					}
					else if(aKeyEvent.iCode == EKeyDevice3)
					{
						HandleViewCommandL(EVBOpenFile);
					}
					else
					{
						HandleViewCommandL(EFolderViewMarkTogle);
					}
					
					break;
				}
			case EKeyLeftArrow:
				if(iFFolderReader && iSelectionBox && !iSearchOn)
				{
					HandleViewCommandL(EBackOneLevel);
					break;
				}
			case EKeyDownArrow:
			case EKeyUpArrow:
			default:
				if(iSelectionBox)
				{
					if ( iFindBox )
		            {
		            TBool needRefresh( EFalse );
		            
		            // Offers the key event to find box.
		            if ( AknFind::HandleFindOfferKeyEventL( aKeyEvent, aType, this,
		                                                    iSelectionBox, iFindBox,
		                                                    EFalse,
		                                                    needRefresh ) == EKeyWasConsumed )
		                {
		                	if ( needRefresh )
		                    {
		                	    SizeChanged();
		                    	DrawNow();
		                    }

		                	return EKeyWasConsumed;
		                }
		            }
		            
					Ret = iSelectionBox->OfferKeyEventL(aKeyEvent,aType);
				}
				break;
			}
		}
	}

	return Ret;
}

/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
TKeyResponse CFileListContainer::SelectAndExecuteSCCommandL(TInt aKey)
{
	TKeyResponse Ret = EKeyWasNotConsumed;

	if(iCommandSCArray && !iSearchOn && iOkToContinue)
	{
		TInt CommandToExecute(-1);			;
	
		switch (aKey)
    	{
		  	case 48:
		  		if(iCommandSCArray->Count() > 0)
		  		{
		  			CommandToExecute  = iCommandSCArray->At(0);
		  		}
		  		break;
			case 49:
				if(iCommandSCArray->Count() > 1)
				{
		  			CommandToExecute  = iCommandSCArray->At(1);
		  		}
		  		break;
			case 50:
				if(iCommandSCArray->Count() > 2)
				{
		  			CommandToExecute  = iCommandSCArray->At(2);
		  		}
		  		break;
			case 51:
				if(iCommandSCArray->Count() > 3)
				{
		  			CommandToExecute  = iCommandSCArray->At(3);
		  		}
		  		break;
			case 52:
				if(iCommandSCArray->Count() > 4)	
				{
		  			CommandToExecute  = iCommandSCArray->At(4);
		  		}
		  		break;
			case 53:
				if(iCommandSCArray->Count() > 5)
				{
		  			CommandToExecute  = iCommandSCArray->At(5);
		  		}
		  		break;
			case 54:
				if(iCommandSCArray->Count() > 6)
				{
		  			CommandToExecute  = iCommandSCArray->At(6);
		  		}
		  		break;
			case 55:
				if(iCommandSCArray->Count() > 7)
				{
		  			CommandToExecute  = iCommandSCArray->At(7);
		  		}
		  		break;
			case 56:
				if(iCommandSCArray->Count() > 8)
				{
		  			CommandToExecute  = iCommandSCArray->At(8);
		  		}
		  		break;
			case 57:
				if(iCommandSCArray->Count() > 9)
				{
		  			CommandToExecute  = iCommandSCArray->At(9);
		  		}
		  		break;
			default:
				break;
    	}
    	
    	if(CommandToExecute > 0)
    	{
    		CEikonEnv::Static()->EikAppUi()-> HandleCommandL(CommandToExecute);
    		Ret = EKeyWasConsumed;
    	}
	}

	return Ret;
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
TInt CFileListContainer::GetSelectedIndex(void)
{
	TInt Ret(-1);

	if(iSelectionBox)
	{
		TInt CurrItemInd = iSelectionBox->CurrentItemIndex();
		
		if(iFindBox)
		{
			CAknFilteredTextListBoxModel* model = STATIC_CAST(CAknFilteredTextListBoxModel*,iSelectionBox->Model());		
		
			if(model && CurrItemInd >= 0)
			{	
				Ret = model->Filter()->FilteredItemIndex(CurrItemInd);
			}
		}
		else
		{
			Ret = CurrItemInd;
		}
	}
	
	return Ret;
}

/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CFileListContainer::OpenFileWithL(const TDesC& aFileName)
{
	if(iIconHandler)
	{
		TInt CurrFilType(CFFileItem::EFDriveFile);
		TFileName RunFile;
	
		if(aFileName.Length())
		{
			RunFile.Copy(aFileName);
		}
		else if(iSearchSettings)
		{
			iSearchSettings->GetCurrSelFileL(RunFile,CurrFilType);
		}
		else
		{		
			GetFileType(CurrFilType, -1);
			
			HBufC* CurrPath = iFFolderReader->CurrentPath();
			if(CurrPath)
			{
				RunFile.Copy(CurrPath->Des());
				if(!AppendFileName(RunFile, -1))
				{
					return;
				}
			}			
		}
		
		if(RunFile.Length()
		&& CurrFilType >= CFFileItem::EFJustFile)
		{
			iIconHandler->OpenFileWithL(RunFile);
		}
	}
}

/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CFileListContainer::OpenFileL(const TDesC& aFileName) 
{
	if(iIconHandler)
	{
		TFileName RunFile;
	
		if(aFileName.Length())
		{
			RunFile.Copy(aFileName);
		}
		else if(iSearchSettings)
		{
			TInt Dummy(-1);
			iSearchSettings->GetCurrSelFileL(RunFile,Dummy);
		}
		else
		{
			HBufC* CurrPath = iFFolderReader->CurrentPath();
			if(CurrPath)
			{
				RunFile.Copy(CurrPath->Des());
				if(!AppendFileName(RunFile, -1))
				{
					return;
				}
			}
		}
		
		if(RunFile.Length())
		{
			TBool ErrorNote(ETrue);
			
			if(!BaflUtils::FileExists(CCoeEnv::Static()->FsSession(),RunFile))
			{
				RunFile.Zero();
				StringLoader::Load(RunFile,R_STR_FILNOTFOND);
				ErrorNote = ETrue;
			}
			else if(RunFile.Length())
			{
				TInt Ret = iIconHandler->OpenFileWithHandler(RunFile);
				
				if(Ret == KErrNotFound)
				{
					StringLoader::Load(RunFile,R_STR_NOAPPFORFIL);
					ErrorNote = EFalse;
				}
				else if(Ret != KErrNone)
				{
					iIconHandler->ShowFileErrorNoteL(RunFile,Ret);
					RunFile.Zero();
				}
				else
				{
					RunFile.Zero();
				}
			}
			
			if(RunFile.Length())
			{
				ShowNoteL(RunFile, ErrorNote);
			}	
		}
	}
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
TBool CFileListContainer::AppendFileName(TDes& aFileName,TInt aIndex) 
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
				CurrSel = GetSelectedIndex();
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
void CFileListContainer::GetFileType(TInt& aTypeId, TInt aIndex) 
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
				CurrSel = GetSelectedIndex();
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
void CFileListContainer::SetMenuL(void)
{	
	if(!iOkToContinue)
	{
		if(iCba)
		{
			iCba->SetCommandSetL(R_MYEXIT_CBA);				
			iCba->DrawDeferred();
		}
	}
	else
	{
		CYBrowserBaseHandler* TmpHand = GetCurrentHandler();
    	
    	if(iSelectContainer)
		{
			iSelectContainer->SetMenuL();
		}
		else if(TmpHand)
		{
			//handled by it self	
		}
		else if(iShowString)
		{
			CEikonEnv::Static()->AppUiFactory()->MenuBar()->SetMenuTitleResourceId(R_SHOWSTRING_MENUBAR);	
			if(iCba)
			{
				iCba->SetCommandSetL(R_SHOWSTRING_CBA);				
				iCba->DrawDeferred();
			}
		}
		else if(iYuccaSettings)
		{
			if(iCba)
			{
				if(iYuccaSettings->AttribSelector())
				{
					if(iDriveReadOnly)	
						iCba->SetCommandSetL(R_ATTR_BACK_CBA);
					else
						iCba->SetCommandSetL(R_ATTR_CBA);
				}
				else
					iCba->SetCommandSetL(R_SETTINGS_CBA);
				
				iCba->DrawDeferred();
			}
		}
		else if(iSearchSettings)
		{
			iSearchSettings->SetMenuL();
		}
		else if(iCommandSCSettings)
		{
			iCba->SetCommandSetL(R_COMMANDSC_CBA);
			iCba->DrawDeferred();
		}
		else
		{
			CEikonEnv::Static()->AppUiFactory()->MenuBar()->SetMenuTitleResourceId(R_Y_BROWSER_MENUBAR);	
			if(iCba)
			{
				TBool ExitBut(ETrue);
				
				if(iFFolderReader && iSelectionBox && !iSearchOn)
				{
					HBufC* Bufff = iFFolderReader->CurrentPath();
					if(Bufff)
					{
						if(Bufff->Des().Length() >= 3)
						{
							ExitBut = EFalse;
						}
					}
				}
		
				if(ExitBut)
					iCba->SetCommandSetL(R_MYOPTEXIT_CBA);
				else
					iCba->SetCommandSetL(R_MYOB_CBA);
								
				iCba->DrawDeferred();
			}
		}
	}
}

/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
void CFileListContainer::InitMenuPanelL(TInt aResourceId, CEikMenuPane* aMenuPane)
{
	TBool HasStuff(EFalse),HasSelection(EFalse),HasCutCopyFiles(EFalse);
	TInt CurrTypeId(CFFileItem::EFDriveFile);
			
	if(iSelectionBox)
	{	
		TInt SelCur = GetSelectedIndex();
		if(SelCur >= 0)
		{
			HasStuff = ETrue;
		}
		
		if(iFindBox)
		{
			CAknFilteredTextListBoxModel* model = STATIC_CAST(CAknFilteredTextListBoxModel*,iSelectionBox->Model());		

			if(model)
			{
				GetFileType(CurrTypeId, SelCur);//model->Filter()->FilteredItemIndex(SelCur)); 
			}
		}
		else
		{
			GetFileType(CurrTypeId, SelCur); 
		}
		
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
		
	if(R_EXTRAS_MENU == aResourceId)
	{
		if(iExtendedInfoOn || iSearchOn)
		{
			aMenuPane->SetItemDimmed(EExtrasSearchOn,ETrue);
		}
		
		if (iFindBox)
		{
			aMenuPane->SetItemDimmed(ESettings,ETrue);
		}
	}	
	else if(R_Y_BROWSER_MENU == aResourceId)
	{
		if (iFindBox)
		{
			aMenuPane->SetItemDimmed(EFolderViewMark,ETrue);
			aMenuPane->SetItemDimmed(ESearchFiles,ETrue);
			aMenuPane->SetItemDimmed(EExtrasMenu,ETrue);
		}
		else
		{
			aMenuPane->SetItemDimmed(EExtrasSearchOff,ETrue);
		}
		
		if(CurrTypeId == CFFileItem::EFExtraFolder)
		{
			aMenuPane->SetItemDimmed(EFileMenu,ETrue);
		}
		
		if(CurrTypeId < CFFileItem::EFFolderFile
		&& HasStuff)
		{
			aMenuPane->SetItemDimmed(EEditMenu,ETrue);
			aMenuPane->SetItemDimmed(EFolderViewMark,ETrue);
		}
		else if(!HasStuff)
		{
			aMenuPane->SetItemDimmed(EFolderViewMark,ETrue);
		}
		
		if(!HasStuff
		|| (CurrTypeId < CFFileItem::EFJustFile && !HasSelection))
		{
			if(!HasCutCopyFiles)
			{
				aMenuPane->SetItemDimmed(EEditMenu,ETrue);
			}
			else if(iDriveReadOnly)
			{
				aMenuPane->SetItemDimmed(EEditMenu,ETrue);
			}
		}
	}
	else if(R_EDIT_MENU == aResourceId)
	{
		if(iFFolderReader && iPasteArray)
		{
			TParsePtrC Hjelpper(iPasteArray->MdcaPoint(0));
			
			HBufC* CurrPath = iFFolderReader->CurrentPath();
			if(CurrPath)
			{
				if(CurrPath->Des() == Hjelpper.DriveAndPath())
				{
					aMenuPane->SetItemDimmed(EEditPaste,ETrue);
				}
			}
		}
		
		if (iFindBox)
		{
			aMenuPane->SetItemDimmed(EEditPaste,ETrue);
			aMenuPane->SetItemDimmed(EEditPasteNewFile,ETrue);
		}
	
		if(iDriveReadOnly)
		{
			aMenuPane->SetItemDimmed(EEditCut,ETrue);
			aMenuPane->SetItemDimmed(EEditPaste,ETrue);
			aMenuPane->SetItemDimmed(EEditPasteNewFile,ETrue);
		}
		
		if(!HasStuff
		|| (CurrTypeId < CFFileItem::EFJustFile && !HasSelection))
		{
			aMenuPane->SetItemDimmed(EEditCopy,ETrue);
			aMenuPane->SetItemDimmed(EEditCut,ETrue);
		}
		
		if(!HasCutCopyFiles)
		{
			aMenuPane->SetItemDimmed(EEditClearPaste,ETrue);
			aMenuPane->SetItemDimmed(EEditShowPaste,ETrue);
			aMenuPane->SetItemDimmed(EEditPaste,ETrue);
			aMenuPane->SetItemDimmed(EEditPasteNewFile,ETrue);
		}
		else if(iIconHandler)
		{
			if(!iIconHandler->HasPasteMany())
			{
				aMenuPane->SetItemDimmed(EEditPasteNewFile,ETrue);
			}
		}
	}
	else if(R_FILE_MENU == aResourceId)
	{
		if (iFindBox)
		{
			aMenuPane->SetItemDimmed(EFileDelete,ETrue);
			aMenuPane->SetItemDimmed(EFileRename,ETrue);
			aMenuPane->SetItemDimmed(EFileNewFolder,ETrue);
			aMenuPane->SetItemDimmed(EFileNewFile,ETrue);
		}
			
		if(iDriveReadOnly)
		{
			aMenuPane->SetItemDimmed(EFileDelete,ETrue);
			aMenuPane->SetItemDimmed(EFileRename,ETrue);
			aMenuPane->SetItemDimmed(EFileNewFolder,ETrue);
			aMenuPane->SetItemDimmed(EFileNewFile,ETrue);
		}
		
		if(iIconHandler)
		{
			if(!iIconHandler->HasNewFile())
			{
				aMenuPane->SetItemDimmed(EFileNewFile,ETrue);
			}
		}
		
		if(CurrTypeId < CFFileItem::EFFolderFile)
		{
			if(HasStuff)
			{
				aMenuPane->SetItemDimmed(EFileNewFolder,ETrue);
				aMenuPane->SetItemDimmed(EFileNewFile,ETrue);
			}
			else
			{
				aMenuPane->SetItemDimmed(EFileDetails,ETrue);
			}
			
			aMenuPane->SetItemDimmed(EFileAttributes,ETrue);
			aMenuPane->SetItemDimmed(ESendFiles,ETrue);
			aMenuPane->SetItemDimmed(EFileDelete,ETrue);
			aMenuPane->SetItemDimmed(EFileRename,ETrue);
			aMenuPane->SetItemDimmed(EFileShowName,ETrue);
			aMenuPane->SetItemDimmed(EOpenWithViewer,ETrue);
		}
		else if(!HasStuff)
		{
			aMenuPane->SetItemDimmed(EFileAttributes,ETrue);
			aMenuPane->SetItemDimmed(ESendFiles,ETrue);
			aMenuPane->SetItemDimmed(EFileDetails,ETrue);
			aMenuPane->SetItemDimmed(EFileDelete,ETrue);
			aMenuPane->SetItemDimmed(EFileRename,ETrue);
			aMenuPane->SetItemDimmed(EFileShowName,ETrue);
			aMenuPane->SetItemDimmed(EOpenWithViewer,ETrue);
		}
		else if(CurrTypeId == CFFileItem::EFFolderFile)
		{
			aMenuPane->SetItemDimmed(EOpenWithViewer,ETrue);
			
			if(!HasSelection)
			{
				aMenuPane->SetItemDimmed(ESendFiles,ETrue);
			}
		}
	}
	else if(R_MARK_MENU == aResourceId)
	{
		if(CurrTypeId < CFFileItem::EFJustFile)
		{
			aMenuPane->SetItemDimmed(EFolderViewMarkTogle,ETrue);
		}
	}
	else if(R_EDITB_MENU == aResourceId)
	{	
		TBool DimP(ETrue);
		
		if(iPasteArray)
		{
			if(iPasteArray->Count())
			{
				DimP = EFalse;
			}
		}
		
		if(DimP)
		{
			aMenuPane->SetItemDimmed(EEditClearPaste,ETrue);
			aMenuPane->SetItemDimmed(EEditShowPaste,ETrue);
		}
	}
	
	CYBrowserBaseHandler* TmpHand = GetCurrentHandler();
    if(iSelectContainer)
	{
		iSelectContainer->InitMenuPanelL(aResourceId,aMenuPane);
	}
    else if(TmpHand)
	{
		TmpHand->DynInitMenuPaneL(aResourceId,aMenuPane);
	}
	else if(iSearchSettings)
	{
		iSearchSettings->InitMenuPanelL(aResourceId,aMenuPane);
	}
	else if(iSearchSettings)
	{
		iSearchSettings->InitMenuPanelL(aResourceId,aMenuPane);
	}
}
/*
-----------------------------------------------------------------------

-----------------------------------------------------------------------
*/
TInt CFileListContainer::CountComponentControls() const
{
	CYBrowserBaseHandler* TmpHand = GetCurrentHandler();
    
    if(iSelectContainer)
	{
		return 1;
	}
	else if(TmpHand)
	{
		return 1;//TmpHand->CountComponentControls();
	}
	else if(iYuccaSettings)
	{
		return 1;
	}
	else if(iSearchSettings)
	{
		if(iShowString)
			return 2;
		else
			return 1;
	}
	else if(iCommandSCSettings)
	{
		return iCommandSCSettings->CountComponentControls();
	}
	else if(iSelectionBox)
	{
		if(iShowString)
			return 2;
		else if(iFindBox)
			return 2;
		else
			return 1;
	}
	else
		return 0;
}

/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
CCoeControl* CFileListContainer::ComponentControl(TInt aIndex) const
{	
	CYBrowserBaseHandler* TmpHand = GetCurrentHandler();
    
    if(iSelectContainer)
	{
		return iSelectContainer;
	}
	else if(TmpHand)
	{
		return TmpHand;//->ComponentControl(aIndex);
	}
	else if(iYuccaSettings)
		return iYuccaSettings;
	else if(iShowString && aIndex)
		return iShowString;
	else if(iSearchSettings)
		return iSearchSettings;
	else if(iCommandSCSettings)
	{
		return iCommandSCSettings->ComponentControl(aIndex);
	}
	else if(iFindBox && aIndex)
		return iFindBox;
	else
		return iSelectionBox;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CFileListContainer::ShowNoteL(const TDesC&  aMessage, TBool aError)
{
	if(iIconHandler)
	{
		iIconHandler->ShowNoteL(aMessage,aError);
	}	
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CFileListContainer::Draw(const TRect& /*aRect*/) const
{
 	CWindowGc& gc = SystemGc();
 	if(iBgContext)
	{
		TRect pillu(Rect());
		MAknsSkinInstance* skin = AknsUtils::SkinInstance();
		AknsDrawUtils::Background( skin, iBgContext, this, gc, pillu );
	}
	else
	{
	 	gc.Clear(Rect());
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CFileListContainer::GetIconNameL(TDes& aFileName)
{
	aFileName.Zero();
	
	TFileName ShortFil;
	TFindFile AppFile(CCoeEnv::Static()->FsSession());
	if(KErrNone != AppFile.FindByDir(KtxIconFileName, KNullDesC))
	{
		if(KErrNone ==CCoeEnv::Static()->FsSession().PrivatePath(ShortFil))
		{
			TFindFile privFile(CCoeEnv::Static()->FsSession());
			if(KErrNone == privFile.FindByDir(ShortFil, KNullDesC))
			{
				TParsePtrC hjelp(privFile.File());
				ShortFil.Copy(hjelp.Drive());
				ShortFil.Append(KtxIconFileName);
			}
		}	
	}
	else
	{
		ShortFil.Copy(AppFile.File());
	}

	if(ShortFil.Length())
	{
		CDictionaryFileStore* MyDStore = CDictionaryFileStore::OpenLC(CCoeEnv::Static()->FsSession(),ShortFil, TUid::Uid(0x102013AD));
			
		TUid FileUid = {0x10};

		if(MyDStore->IsPresentL(FileUid))
		{
			RDictionaryReadStream in;
			in.OpenLC(*MyDStore,FileUid);
			in >> aFileName;
			CleanupStack::PopAndDestroy(1);// in
		}
			
		CleanupStack::PopAndDestroy(1);// Store
	}
	
	if(!BaflUtils::FileExists(CCoeEnv::Static()->FsSession(),aFileName) || aFileName.Length() < 5)
	{	
		TFindFile AppFile(CCoeEnv::Static()->FsSession());
		if(KErrNone == AppFile.FindByDir(KtxImagesFileName, KNullDesC))
		{
			aFileName.Copy(AppFile.File());
		}
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CFileListContainer::SaveIconNameL(const TDesC& aFileName)
{
	TFileName ShortFil;
	TFindFile AppFile(CCoeEnv::Static()->FsSession());
	if(KErrNone != AppFile.FindByDir(KtxIconFileName, KNullDesC))
	{
		if(KErrNone ==CCoeEnv::Static()->FsSession().PrivatePath(ShortFil))
		{
			TFindFile privFile(CCoeEnv::Static()->FsSession());
			if(KErrNone == privFile.FindByDir(ShortFil, KNullDesC))
			{
				TParsePtrC hjelp(privFile.File());
				ShortFil.Copy(hjelp.Drive());
				ShortFil.Append(KtxIconFileName);
			}
		}
	}
	else
	{
		ShortFil.Copy(AppFile.File());
	}

	if(ShortFil.Length() && iFFolderReader)
	{
		CCoeEnv::Static()->FsSession().Delete(ShortFil);
		CDictionaryFileStore* MyDStore = CDictionaryFileStore::OpenLC(CCoeEnv::Static()->FsSession(),ShortFil, TUid::Uid(0x102013AD));
			
		TUid FileUid = {0x10};	
				
		RDictionaryWriteStream out1;
		out1.AssignLC(*MyDStore,FileUid);
		
		out1 << aFileName; 
		out1.CommitL(); 	
		CleanupStack::PopAndDestroy(1);// out1
		
		MyDStore->CommitL();
		CleanupStack::PopAndDestroy(1);// Store
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CFileListContainer::GetPathL(TDes& aFolder)
{
	TFileName ShortFil;
	TFindFile AppFile(CCoeEnv::Static()->FsSession());
	if(KErrNone != AppFile.FindByDir(KtxPathSaveFileName, KNullDesC))
	{
		if(KErrNone ==CCoeEnv::Static()->FsSession().PrivatePath(ShortFil))
		{
			TFindFile privFile(CCoeEnv::Static()->FsSession());
			if(KErrNone == privFile.FindByDir(ShortFil, KNullDesC))
			{
				TParsePtrC hjelp(privFile.File());
				ShortFil.Copy(hjelp.Drive());
				ShortFil.Append(KtxPathSaveFileName);
			}
		}	
	}
	else
	{
		ShortFil.Copy(AppFile.File());
	}

	if(ShortFil.Length())
	{
		CDictionaryFileStore* MyDStore = CDictionaryFileStore::OpenLC(CCoeEnv::Static()->FsSession(),ShortFil, TUid::Uid(0x102013AD));
			
		TUid FileUid = {0x10};

		if(MyDStore->IsPresentL(FileUid))
		{
			RDictionaryReadStream in;
			in.OpenLC(*MyDStore,FileUid);
			in >> aFolder;
			CleanupStack::PopAndDestroy(1);// in
		}
			
		CleanupStack::PopAndDestroy(1);// Store
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CFileListContainer::SavePathL(void)
{
	if(iSettings.iSavePath)
	{
		TFileName ShortFil;
		TFindFile AppFile(CCoeEnv::Static()->FsSession());
		if(KErrNone != AppFile.FindByDir(KtxPathSaveFileName, KNullDesC))
		{
			if(KErrNone ==CCoeEnv::Static()->FsSession().PrivatePath(ShortFil))
			{
				TFindFile privFile(CCoeEnv::Static()->FsSession());
				if(KErrNone == privFile.FindByDir(ShortFil, KNullDesC))
				{
					TParsePtrC hjelp(privFile.File());
					ShortFil.Copy(hjelp.Drive());
					ShortFil.Append(KtxPathSaveFileName);
				}
			}
		}
		else
		{
			ShortFil.Copy(AppFile.File());
		}

		if(ShortFil.Length() && iFFolderReader)
		{
			CCoeEnv::Static()->FsSession().Delete(ShortFil);
			CDictionaryFileStore* MyDStore = CDictionaryFileStore::OpenLC(CCoeEnv::Static()->FsSession(),ShortFil, TUid::Uid(0x102013AD));
				
			TUid FileUid = {0x10};	
					
			RDictionaryWriteStream out1;
			out1.AssignLC(*MyDStore,FileUid);
			
			HBufC* CurrPath = iFFolderReader->CurrentPath();
			if(CurrPath)
			{
				out1 << CurrPath->Des(); 
			}
			
			out1.CommitL(); 	
			CleanupStack::PopAndDestroy(1);// out1
			
			MyDStore->CommitL();
			CleanupStack::PopAndDestroy(1);// Store
		}
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CFileListContainer::SetSettingsValuesL(void)
{
	TFileName ShortFil;
	TFindFile AppFile(CCoeEnv::Static()->FsSession());
	if(KErrNone != AppFile.FindByDir(KtxSettingsFileName, KNullDesC))
	{
		if(KErrNone ==CCoeEnv::Static()->FsSession().PrivatePath(ShortFil))
		{
			TFindFile privFile(CCoeEnv::Static()->FsSession());
			if(KErrNone == privFile.FindByDir(ShortFil, KNullDesC))
			{
				TParsePtrC hjelp(privFile.File());
				ShortFil.Copy(hjelp.Drive());
				ShortFil.Append(KtxSettingsFileName);
			}
		}		
	}
	else
	{
		ShortFil.Copy(AppFile.File());
	}

	if(ShortFil.Length())
	{
		CCoeEnv::Static()->FsSession().Delete(ShortFil);
		CDictionaryFileStore* MyDStore = CDictionaryFileStore::OpenLC(CCoeEnv::Static()->FsSession(),ShortFil, TUid::Uid(0x102013AD));
			
		TUid FileUid = {0x10};	
				
		RDictionaryWriteStream out1;
		out1.AssignLC(*MyDStore,FileUid);
		if(iSettings.iFolders)
			out1.WriteInt32L(0x100);
		else
			out1.WriteInt32L(0);
		out1.CommitL(); 	
		CleanupStack::PopAndDestroy(1);// out1
			
		//iSettings.iSort
		FileUid.iUid = 0x11;	
		RDictionaryWriteStream out2;
		out2.AssignLC(*MyDStore,FileUid);
		if(iSettings.iSort)
			out2.WriteInt32L(0x100);
		else
			out2.WriteInt32L(0);
		out2.CommitL(); 	
		CleanupStack::PopAndDestroy(1);// out2
		
		//iSettings.iShow
		FileUid.iUid = 0x12;
		RDictionaryWriteStream out3;
		out3.AssignLC(*MyDStore,FileUid);
		if(iSettings.iShow)
			out3.WriteInt32L(0x100);
		else
			out3.WriteInt32L(0);
		out3.CommitL(); 	
		CleanupStack::PopAndDestroy(1);// out3
		
		//iSettings.iOrder
		FileUid.iUid = 0x13;
		RDictionaryWriteStream out4;
		out4.AssignLC(*MyDStore,FileUid);
		out4.WriteInt32L(iSettings.iOrder);
		out4.CommitL(); 	
		CleanupStack::PopAndDestroy(1);// out4
		
		//iSettings.iOptimize
		FileUid.iUid = 0x14;
		RDictionaryWriteStream out63;
		out63.AssignLC(*MyDStore,FileUid);
		if(iSettings.iOptimize)
			out63.WriteInt32L(0x100);
		else
			out63.WriteInt32L(0);
		out63.CommitL(); 	
		CleanupStack::PopAndDestroy(1);// out63
		
		//iSettings.iSavePath
		FileUid.iUid = 0x15;
		RDictionaryWriteStream out35;
		out35.AssignLC(*MyDStore,FileUid);
		if(iSettings.iSavePath)
			out35.WriteInt32L(0x100);
		else
			out35.WriteInt32L(0);
		out35.CommitL(); 	
		CleanupStack::PopAndDestroy(1);// out35
		
		FileUid.iUid = 0x20;
		RDictionaryWriteStream out37;
		out37.AssignLC(*MyDStore,FileUid);
		if(iSettings.iDoRecocnize)
			out37.WriteInt32L(0x100);
		else
			out37.WriteInt32L(0);
		out37.CommitL(); 	
		CleanupStack::PopAndDestroy(1);// out37	
		
		FileUid.iUid = 0x22;
		RDictionaryWriteStream out39;
		out39.AssignLC(*MyDStore,FileUid);
		if(iSettings.iConfirmExit)
			out39.WriteInt32L(0x100);
		else
			out39.WriteInt32L(0);
		out39.CommitL(); 	
		CleanupStack::PopAndDestroy(1);// out39
		
		// iFullScreen
		FileUid.iUid = 0x30;
		RDictionaryWriteStream out40;
		out40.AssignLC(*MyDStore,FileUid);
		if(iSettings.iFullScreen)
			out40.WriteInt32L(0x100);
		else
			out40.WriteInt32L(0);
		out40.CommitL(); 	
		CleanupStack::PopAndDestroy(1);// out40
		
		
		MyDStore->CommitL();
		CleanupStack::PopAndDestroy(1);// Store
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CFileListContainer::UpdateSettingsForReadingL(void)
{
	if(iFFolderReader)
	{
		TUint sortOrder(EAscending);
		
		if (iSettings.iOrder == 1) // type
		{	
			sortOrder = ESortByExt;
		}
		else if (iSettings.iOrder == 3) // size
		{	
			sortOrder = ESortBySize;
		}
		else if (iSettings.iOrder == 4) // time
		{	
			sortOrder = ESortByDate;
		}
		else
		{
			sortOrder= ESortByName;// 	
		}
		
		if(!iSettings.iSort)
		{
			sortOrder|=EDescending;
		}
		
		
		TInt attribs=KEntryAttNormal|KEntryAttDir;
		
		if (iSettings.iShow)
		{
			attribs|=KEntryAttHidden;
			attribs|=KEntryAttAllowUid;
			attribs|=KEntryAttSystem;
			attribs|=KEntryAttXIP;	
			attribs|=KEntryAttVolume;
		}
	
		TFReadSettings ReadSetting;
	   
	   	ReadSetting.iEntryMask = attribs;
		ReadSetting.iEntrySort = sortOrder;
		ReadSetting.iShowHiddenSystem 	= iSettings.iShow;
		ReadSetting.iOptimize 			= iSettings.iOptimize;
		ReadSetting.iDoRecocnize 		= iSettings.iDoRecocnize;

	    iFFolderReader->SetSettings(ReadSetting);
	}
}
/*
-----------------------------------------------------------------------------
	
-----------------------------------------------------------------------------
*/

void CFileListContainer::GetCommandSCFromStoreL(void)
{	
	delete iCommandSCArray;
	iCommandSCArray = NULL;
	
	TFileName ShortFil;
	TFindFile AppFile(CCoeEnv::Static()->FsSession());
	if(KErrNone != AppFile.FindByDir(KtxCommandSCFileName, KNullDesC))
	{
		if(KErrNone ==CCoeEnv::Static()->FsSession().PrivatePath(ShortFil))
		{
			TFindFile privFile(CCoeEnv::Static()->FsSession());
			if(KErrNone == privFile.FindByDir(ShortFil, KNullDesC))
			{
				TParsePtrC hjelp(privFile.File());
				ShortFil.Copy(hjelp.Drive());
				ShortFil.Append(KtxCommandSCFileName);
			}
		}
	}
	else
	{
		ShortFil.Copy(AppFile.File());
	}
	
	if(ShortFil.Length())
	{
		CDictionaryFileStore* MyDStore = CDictionaryFileStore::OpenLC(CCoeEnv::Static()->FsSession(),ShortFil, TUid::Uid(0x102013AD));
		
		iCommandSCArray = new(ELeave)CArrayFixFlat<TInt>(10);	
	
		TInt CommandSc(0);
		GetValuesL(MyDStore,0x5000,CommandSc);
		iCommandSCArray->AppendL(CommandSc);
		
		CommandSc =0;
		GetValuesL(MyDStore,0x5001,CommandSc);
		iCommandSCArray->AppendL(CommandSc);
		
		CommandSc =0;
		GetValuesL(MyDStore,0x5002,CommandSc);
		iCommandSCArray->AppendL(CommandSc);
		
		CommandSc =0;
		GetValuesL(MyDStore,0x5003,CommandSc);
		iCommandSCArray->AppendL(CommandSc);
		
		CommandSc =0;
		GetValuesL(MyDStore,0x5004,CommandSc);
		iCommandSCArray->AppendL(CommandSc);

		CommandSc =0;
		GetValuesL(MyDStore,0x5005,CommandSc);
		iCommandSCArray->AppendL(CommandSc);
		
		CommandSc =0;
		GetValuesL(MyDStore,0x5006,CommandSc);
		iCommandSCArray->AppendL(CommandSc);
		
		CommandSc =0;
		GetValuesL(MyDStore,0x5007,CommandSc);
		iCommandSCArray->AppendL(CommandSc);
		
		CommandSc =0;
		GetValuesL(MyDStore,0x5008,CommandSc);
		iCommandSCArray->AppendL(CommandSc);
		
		CommandSc =0;
		GetValuesL(MyDStore,0x5009,CommandSc);
		iCommandSCArray->AppendL(CommandSc);

		CleanupStack::PopAndDestroy(1);// Store
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CFileListContainer::GetValuesL(CDictionaryFileStore* aDStore,TInt aUID,TInt& aValue)
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

void CFileListContainer::GetSettingsValuesL(void)
{
	TFileName ShortFil;
	TFindFile AppFile(CCoeEnv::Static()->FsSession());
	if(KErrNone != AppFile.FindByDir(KtxSettingsFileName, KNullDesC))
	{
		if(KErrNone ==CCoeEnv::Static()->FsSession().PrivatePath(ShortFil))
		{
			TFindFile privFile(CCoeEnv::Static()->FsSession());
			if(KErrNone == privFile.FindByDir(ShortFil, KNullDesC))
			{
				TParsePtrC hjelp(privFile.File());
				ShortFil.Copy(hjelp.Drive());
				ShortFil.Append(KtxSettingsFileName);
			}
		}
	}
	else
	{
		ShortFil.Copy(AppFile.File());
	}

	iSettings.iFolders 	= ETrue;
	iSettings.iSort 	= ETrue;
	iSettings.iShow 	= ETrue;
	iSettings.iOrder 	= 0; // file name;
	iSettings.iOptimize = ETrue;
	iSettings.iSavePath = EFalse;
	iSettings.iDoRecocnize= ETrue;
	iSettings.iConfirmExit= EFalse;	
	
	if(ShortFil.Length())
	{
		CDictionaryFileStore* MyDStore = CDictionaryFileStore::OpenLC(CCoeEnv::Static()->FsSession(),ShortFil, TUid::Uid(0x102013AD));
			
		TUid FileUid = {0x10};

		if(MyDStore->IsPresentL(FileUid))
		{
			RDictionaryReadStream in;
			in.OpenLC(*MyDStore,FileUid);
			TInt MyValue = in.ReadInt32L();
			CleanupStack::PopAndDestroy(1);// in
		
			if(MyValue > 50)
			{
				iSettings.iFolders 	= ETrue;
			}
			else
			{
				iSettings.iFolders 	= EFalse;
			}
		}
		
		FileUid.iUid = 0x11;

		if(MyDStore->IsPresentL(FileUid))
		{
			RDictionaryReadStream in;
			in.OpenLC(*MyDStore,FileUid);
			TInt MyValue = in.ReadInt32L();
			CleanupStack::PopAndDestroy(1);// in
		
			if(MyValue > 50)
			{
				iSettings.iSort 	= ETrue;
			}
			else
			{
				iSettings.iSort 	= EFalse;
			}
		}
		
		FileUid.iUid = 0x12;

		if(MyDStore->IsPresentL(FileUid))
		{
			RDictionaryReadStream in;
			in.OpenLC(*MyDStore,FileUid);
			TInt MyValue = in.ReadInt32L();
			CleanupStack::PopAndDestroy(1);// in
		
			if(MyValue > 50)
			{
				iSettings.iShow 	= ETrue;
			}
			else
			{
				iSettings.iShow 	= EFalse;
			}
		}
		
		FileUid.iUid = 0x13;

		if(MyDStore->IsPresentL(FileUid))
		{
			RDictionaryReadStream in;
			in.OpenLC(*MyDStore,FileUid);
			TInt MyValue = in.ReadInt32L();
			CleanupStack::PopAndDestroy(1);// in
		
			if(MyValue >=0 && MyValue <= 4 )
			{
				iSettings.iOrder 	= MyValue;;
			}
			else
			{
				iSettings.iOrder 	= 2; // file type;
			}
		}
				
		FileUid.iUid = 0x14;

		if(MyDStore->IsPresentL(FileUid))
		{
			RDictionaryReadStream in;
			in.OpenLC(*MyDStore,FileUid);
			TInt MyValue = in.ReadInt32L();
			CleanupStack::PopAndDestroy(1);// in
		
			if(MyValue > 50)
			{
				iSettings.iOptimize 	= ETrue;
			}
			else
			{
				iSettings.iOptimize 	= EFalse;
			}
		}	
		
		FileUid.iUid = 0x15;

		if(MyDStore->IsPresentL(FileUid))
		{
			RDictionaryReadStream in;
			in.OpenLC(*MyDStore,FileUid);
			TInt MyValue = in.ReadInt32L();
			CleanupStack::PopAndDestroy(1);// in
		
			if(MyValue > 50)
			{
				iSettings.iSavePath 	= ETrue;
			}
			else
			{
				iSettings.iSavePath 	= EFalse;
			}
		}
		
		FileUid.iUid = 0x20;

		if(MyDStore->IsPresentL(FileUid))
		{
			RDictionaryReadStream in;
			in.OpenLC(*MyDStore,FileUid);
			TInt MyValue = in.ReadInt32L();
			CleanupStack::PopAndDestroy(1);// in
		
			if(MyValue > 50)
			{
				iSettings.iDoRecocnize 	= ETrue;
			}
			else
			{
				iSettings.iDoRecocnize 	= EFalse;
			}
		}
		
		FileUid.iUid = 0x22;

		if(MyDStore->IsPresentL(FileUid))
		{
			RDictionaryReadStream in;
			in.OpenLC(*MyDStore,FileUid);
			TInt MyValue = in.ReadInt32L();
			CleanupStack::PopAndDestroy(1);// in
		
			if(MyValue > 50)
			{
				iSettings.iConfirmExit 	= ETrue;
			}
			else
			{
				iSettings.iConfirmExit 	= EFalse;
			}
		}
		
		FileUid.iUid = 0x30;

		if(MyDStore->IsPresentL(FileUid))
		{
			RDictionaryReadStream in;
			in.OpenLC(*MyDStore,FileUid);
			TInt MyValue = in.ReadInt32L();
			CleanupStack::PopAndDestroy(1);// in
		
			if(MyValue > 50)
			{
				iSettings.iFullScreen 	= ETrue;
			}
			else
			{
				iSettings.iFullScreen 	= EFalse;
			}
		}
	
		CleanupStack::PopAndDestroy(1);// Store
	}

}

