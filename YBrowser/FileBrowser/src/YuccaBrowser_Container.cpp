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

#include "Search_Settings.h"
#include "YuccaBrowser_Container.h"

#include "YuccaBrowser.h"
#include "YuccaBrowser.hrh"
#include "Navipanel.h"
#include "ShowString.h"
#include "Help_Container.h"

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

#include <APACMDLN.H> 

#include "Search_Container.h"
#include "Selection_Container.h"
#include "General_Settings.h"
#include "Settings_Container.h"

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
	
	delete iTimeOutTimer;
	iHandlerArray.ResetAndDestroy();
	
	delete iMyHelpContainer;
	iMyHelpContainer = NULL;
	
	delete iSearchSettings;
	iSearchSettings = NULL;
	
	delete iShowString;
	iShowString = NULL;
	
	delete iSettingsContainer;
	iSettingsContainer = NULL;
	
	delete iYuccaSettings;
	iYuccaSettings = NULL;
	
	delete iGeneralSettings;
	iGeneralSettings = NULL;
	
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

	iGeneralSettings = CGeneralSettings::NewL();
	iOkToContinue = iGeneralSettings->ShowDisclaimerL();
	
	TFileName StartPath;
	iGeneralSettings->GetIconNameL(StartPath);
	
	iIconHandler = new(ELeave)CIconHandler(*this,this);
	iIconHandler->SetUtils(this);
	iIconHandler->SetIconFile(StartPath);
	iIconHandler->ConstructL();
	
	iGeneralSettings->SetIconHandler(iIconHandler);

    SetRect(aRect);
	
	if(iOkToContinue)
	{
		StartPath.Zero();
		
		iGeneralSettings->GetCommandSCFromStoreL();
		iGeneralSettings->GetSettingsValuesL(iSettings);

		iYuccaNavi = CYuccaNavi::NewL(iSettings.iPathScroll);
	    iFFolderReader = CFFolderReader::NewL(*this,*iIconHandler);
		iFFolderReader->SetSettings(iSettings);
		
		iGeneralSettings->SetFolderReader(iFFolderReader);
		
		if(iSettings.iSavePath)
		{
			iGeneralSettings->GetPathL(StartPath);
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
	
	ChangeStatusPaneLayoutL();
	ActivateL();
	SetMenuL();
}

/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CFileListContainer::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
    TPoint pointter(aPointerEvent.iPosition);
    TPoint relll(PositionRelativeToScreen());
            
    pointter.iY = (pointter.iY - relll.iY);
    pointter.iX = (pointter.iX - relll.iX);
    
    TBool forward(ETrue);
    
    switch (aPointerEvent.iType)
    {
        case TPointerEvent::EButton1Down:
            {
                iDragLeft = iDragRight = EFalse;
                
                if(iGeneralSettings){
                    delete iTimeOutTimer;
                    iTimeOutTimer = NULL;            
                    iTimeOutTimer = CTimeOutTimer::NewL(EPriorityNormal, *this);
                    iTimeOutTimer->After(iGeneralSettings->LongPressTimeOut());
                }
                iStartPoint = pointter;
            }
            break;
        case TPointerEvent::EButton1Up:
            if(iGeneralSettings)
            {
                delete iTimeOutTimer;
                iTimeOutTimer = NULL;
                if (iDragLeft || iDragRight)
                {
                    if(iDragRight)
                        HandleViewCommandL(iGeneralSettings->SwipeRight());
                    else
                        HandleViewCommandL(iGeneralSettings->SwipeLeft());
                    
                    forward = EFalse;
                }
                iDragLeft = iDragRight = EFalse;
            }
            break;
        case TPointerEvent::EDrag:
            {
                TInt heighP = (Rect().Height() / 10);
                TInt widthP = (Rect().Width() / 4);
                
                TInt huhuu(iStartPoint.iY - pointter.iY );
                
                TBool rightWay(ETrue);
                if(huhuu <= 0){
                    rightWay = EFalse;
                    huhuu = (huhuu * -1);
                }
                
                if(heighP > huhuu){
                
                    TInt stufff(iStartPoint.iX - pointter.iX );
                
                    if(stufff < 0){
                        stufff = (stufff * -1);
                    }
                    
                    if(widthP < stufff){
                        delete iTimeOutTimer;
                        iTimeOutTimer = NULL; 
                            
                        if(rightWay){
                            iDragRight = ETrue;
                            iDragLeft = EFalse;
                        }else{
                            iDragLeft = ETrue;
                            iDragRight = EFalse;
                        }
                    }else{
                        iDragLeft = iDragRight = EFalse;
                    }
                }else{
                    delete iTimeOutTimer;
                    iTimeOutTimer = NULL; 
                    iDragLeft = iDragRight = EFalse;
                }
            }
            break;
        default:
            break;
    }
    
    if(forward){
        CCoeControl::HandlePointerEventL(aPointerEvent);
    }
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CFileListContainer::TimerExpired()
{
    if(iGeneralSettings){
        HandleViewCommandL(iGeneralSettings->LongPress());
    }
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CFileListContainer::SavePathL(void)
{
	if(iFFolderReader && iGeneralSettings && iSettings.iSavePath)
	{
		HBufC* CurrPath = iFFolderReader->CurrentPath();
		if(CurrPath)
		{
			iGeneralSettings->SavePathL(*CurrPath);
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
void CFileListContainer::FileManOperatioDoneL(CEikFileHandler* /*aObject*/,TInt aOperation, TInt /*aError*/,TFileManError /*aErrorInfo*/)
{
	if(iFindBox)
	{
		iFindBox->ResetL();
	}
	
	if(aOperation == EEikFMove){
        delete iPasteArray;
        iPasteArray = NULL;
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
		
		if(iSelectionBox && iFFolderReader && iGeneralSettings)
		{
			TBool IsDriveList(ETrue);
			
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
					
					if(Hjelpper.Length() > 3)
					{
						IsDriveList = EFalse;
					}
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
									iGeneralSettings->GetTimeBuffer(CurrArray[i]->iModified.DateTime(),DateHelp,ETrue,ETrue);
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
									iGeneralSettings->GetTimeBuffer(CurrArray[i]->iModified.DateTime(),DateHelp,ETrue,ETrue);
									
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
			
			if(iSettings.iOrder == 2 && !IsDriveList)
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
									iGeneralSettings->GetTimeBuffer(CurrArray[i]->iModified.DateTime(),DateHelp,ETrue,ETrue);
										
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
			
			if (iSelectionBox->ScrollBarFrame())
        	{
        		iSelectionBox->ScrollBarFrame()->MoveVertThumbTo(iSelectionBox->View()->CurrentItemIndex());
        	}
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
	
//	iSelectionBox->ItemDrawer()->ColumnData()->SetMarqueeParams (2, 20, 1000000, 200000);
	
	TBuf<100> Hejlpper;
    StringLoader::Load(Hejlpper,R_SH_STR_NOFILES);
	
	iSelectionBox->View()->SetListEmptyTextL(Hejlpper);
	
	if(iExtendedInfoOn)
	{
	    STATIC_CAST(CAknDoubleGraphicStyleListBox*,iSelectionBox)->ItemDrawer()->ColumnData()->SetIconArray(iIconHandler->GetIconArrayL());
		STATIC_CAST(CAknDoubleGraphicStyleListBox*,iSelectionBox)->ItemDrawer()->ColumnData()->EnableMarqueeL(ETrue);
	}
	else
	{
		STATIC_CAST(CAknSingleGraphicStyleListBox*,iSelectionBox)->ItemDrawer()->ColumnData()->SetIconArray(iIconHandler->GetIconArrayL());
		STATIC_CAST(CAknSingleGraphicStyleListBox*,iSelectionBox)->ItemDrawer()->ColumnData()->EnableMarqueeL(ETrue);
	}
	
	
	if(iSearchOn)
	{
		iFindBox = CreateFindBoxL(iSelectionBox, iSelectionBox->Model(),CAknSearchField::ESearch);
		
		CAknColumnListBox* aknListBox = STATIC_CAST(CAknColumnListBox*, iSelectionBox);
        AknFind::HandleFixedFindSizeChanged(this, aknListBox, iFindBox);
	}	
	
	iSelectionBox->ActivateL();	
	if (iSelectionBox->ScrollBarFrame())
	{
		iSelectionBox->ScrollBarFrame()->MoveVertThumbTo(iSelectionBox->View()->CurrentItemIndex());
	}

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
void CFileListContainer::ChangeStatusPaneLayoutL(void)
{
	CEikStatusPane*		sp = ((CAknAppUi*)iEikonEnv->EikAppUi())->StatusPane();
	if(sp)
	{
		CAknTitlePane* TitlePane = STATIC_CAST(CAknTitlePane*,sp->ControlL(TUid::Uid(EEikStatusPaneUidTitle)));
		
		if(TitlePane)
			TitlePane->SetTextL(KMainApplicationName);

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
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CFileListContainer::HandleViewCommandL(TInt aCommand)
{
	TFileName Hjelpper;	

    switch(aCommand)
        {
    	case EAppHelpBack:
    		{
				delete iMyHelpContainer;
				iMyHelpContainer = NULL;
    		}
    		SetMenuL();
    		DrawNow();
    		break;
    	case EAppHelp:
    		{
				delete iMyHelpContainer;
				iMyHelpContainer = NULL;    		
				iMyHelpContainer = CMyHelpContainer::NewL();
    		}
    		SetMenuL();
    		DrawNow();    		
    		break;
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
							iLastFolder.Copy(FolPar.NameAndExt());	
						}
						else if(CurrPath->Des().Length() >= 2)
						{
							iLastFolder.Copy(CurrPath->Des().Left(2));
						}
					}
					
					iSelectionBox->ClearSelection();
					
					// fix for highlight May 23 2010
					iFilemanNameBuf.Zero();
					
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
        	if(iGeneralSettings)
        	{
        		if(iGeneralSettings->SelectAndChangeIconL())
        		{
        			TInt LastItemIndex(-1);
					if(iSelectionBox)
					{
						LastItemIndex = GetSelectedIndex();
					}
	
        			DisplayListBoxL();
					FolderReadFinnishL(NULL, KErrNone);
        		
        			if(LastItemIndex > 0 && iSelectionBox)
					{
						CDesCArray* itemArray = STATIC_CAST( CDesCArray* ,iSelectionBox->Model()->ItemTextArray());
					
						if(itemArray && LastItemIndex >= 0)
						{
							if(itemArray->Count() > LastItemIndex)
							{
								iSelectionBox->SetCurrentItemIndex(LastItemIndex);
				        		if (iSelectionBox->ScrollBarFrame())
				            	{
				            		iSelectionBox->ScrollBarFrame()->MoveVertThumbTo(iSelectionBox->View()->CurrentItemIndex());
				            	}
								DrawNow();
							}
						}
					} 
        		}
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
        		ChangeStatusPaneLayoutL();
        		RemoveLastHandler();
        		
        		CYBrowserBaseHandler* TmpHand = GetCurrentHandler();
        		if(TmpHand)
        		{
        			TmpHand->SetFocusL(ETrue);
        		}
        		else
        		{
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
    		if(iIconHandler)
			{
				TInt CurrFilType(CFFileItem::EFDriveFile);
				TFileName RunFile;
			
				if(iSearchSettings)
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
        	break;
        case EVBOpenFile:
        	if(iIconHandler)
			{
				TFileName RunFile;
			
				if(iSearchSettings)
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
        		ChangeStatusPaneLayoutL();
        	
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
						TLocale MyLocale;
						
						iYuccaNavi->SetTextL(Hjelpper);	
						
						iYuccaSettings = new(ELeave)CYuccaSettings();
						iYuccaSettings->ConstructL(MyEntry.IsReadOnly(),MyEntry.IsHidden(),MyEntry.IsSystem(),(MyEntry.iModified + MyLocale.UniversalTimeOffset()));	
					}
				}
        	
        		SetMenuL();
				DrawNow();
        	}
			break;
		case EFileAttributesSave:
			if(iYuccaNavi && iYuccaSettings)
			{
				TLocale MyLocale;
				TEntry MyEntry;
				
				Hjelpper.Zero();
				iYuccaNavi->GetTextL(Hjelpper);
				
			
				TInt ErrNo = CCoeEnv::Static()->FsSession().Entry(Hjelpper,MyEntry);
				
				if(KErrNone == ErrNo)
				{
				
					TBool MyReadOnly(MyEntry.IsReadOnly());
					TBool MyHidden(MyEntry.IsHidden());
					TBool MySystem(MyEntry.IsSystem());
					TTime MyTime(MyEntry.iModified  + MyLocale.UniversalTimeOffset());
						  
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
						
						if(MyEntry.IsDir()){
                           TRAP(ErrNo,
                           
                           HBufC* Helppp = StringLoader::LoadLC(R_STR_SETTOALLCO);
                           CAknQueryDialog* dlg = CAknQueryDialog::NewL();
                           if(dlg->ExecuteLD(R_QUERY,*Helppp))
                           {
                                CFileMan* filMan = CFileMan::NewL(CCoeEnv::Static()->FsSession());
                                CleanupStack::PushL(filMan);
                                ErrNo = filMan->Attribs(Hjelpper, SetAttMask, ClearAttMask,TTime(0),CFileMan::ERecurse);
                                CleanupStack::PopAndDestroy(filMan);
                           }
                           CleanupStack::PopAndDestroy(Helppp));
                        }
						
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
						SetNewTime = (SetNewTime - MyLocale.UniversalTimeOffset());
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
        		
        		iSettingsContainer= CSettingsContainer::NewL(iCba,iSettings,iIconHandler,*iGeneralSettings);
        				
				SetMenuL();
				DrawNow();
			}
			break;
		case ESettingsBack:
			{
				TBool GenSetChanges(EFalse);
				if(iSettingsContainer && iGeneralSettings)
				{
					GenSetChanges = iSettingsContainer->GetGeneralSettings(iSettings);
					
					iGeneralSettings->SetSettingsValuesL(iSettings);
					iGeneralSettings->GetCommandSCFromStoreL();
				}
				
				if(iIconHandler)
				{
					iIconHandler->ReFreshFTOverWritesL();
				}
				
				delete iSettingsContainer;
				iSettingsContainer = NULL;
				
				ChangeStatusPaneLayoutL();
				
				if(iFFolderReader && iYuccaNavi)
				{
                    iYuccaNavi->SetScroll(iSettings.iPathScroll);
				
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
				
				if(GenSetChanges && iFFolderReader)
				{
					TInt SAve(0);
					TInt FilCount(0);
					
					iFFolderReader->Cancel();
					iFFolderReader->RemoveTypeFilterArrayL();
					iFFolderReader->SetSettings(iSettings);
					
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
			
				SetMenuL();
				DrawNow();
			}
			break;
		
		case EExtrasShowShortCut:
			if(!iSearchSettings && !iSearchOn && iOkToContinue && iGeneralSettings)
			{
				TInt Sel = iGeneralSettings->ShowShortCutListL();
				if(Sel >= 0 && Sel <= 10)
				{
					Hjelpper.Zero();
					TInt KeyRet = iGeneralSettings->MoveToShortCutL((Sel + 48),Hjelpper);
					
					if(KeyRet == EKeyWasConsumed && iFFolderReader && iSelectionBox)
					{
						iFFolderReader->Cancel();
						iSelectionBox->ClearSelection();
						
						TInt SAve(0);
						TInt FilCount = iFFolderReader->StartReadingL(Hjelpper,SAve);

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
			}	
			break;
		case EExtrasSetShortCut:
			if(!iSearchSettings && iGeneralSettings && iFFolderReader)
			{
				HBufC* CurrPath = iFFolderReader->CurrentPath();
				if(CurrPath)
				{
					HBufC* Helppp = StringLoader::LoadLC(R_STR_SELSCNUMBER);
					
					TInt ScNumber(0);
					CAknNumberQueryDialog* Dialog = CAknNumberQueryDialog::NewL(ScNumber,CAknQueryDialog::ENoTone);
					Dialog->PrepareLC(R_SC_DIALOG);
					Dialog->SetPromptL(*Helppp);
					if(Dialog->RunLD())
					{
						iGeneralSettings->SetShortCutL((48 + ScNumber),*CurrPath);
					}
					
					CleanupStack::PopAndDestroy(Helppp);	
				}
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
			if(iGeneralSettings)
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
					iGeneralSettings->ShowInfoL(FileFullName,CurrFilType);
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
					}else{
                        iFilemanNameBuf.Zero();
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
        	if(iGeneralSettings)
        	{
        		TBuf<60> Hjelppp;
	        	if(iSelectContainer)
	        	{
	        		
	        		iGeneralSettings->ShowPasteFilesL(iSelectContainer->PasteArray(),Hjelppp);
	        	}
	        	else 
	        	{
	        		if(iIsCutOperation)
	        			StringLoader::Load(Hjelppp,R_SH_STR_FILCUT);
					else
						StringLoader::Load(Hjelppp,R_SH_STR_FILCOPY);
					
					iGeneralSettings->ShowPasteFilesL(iPasteArray,Hjelppp);
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
		case EFolderViewMarkWild:
			{
				
				HBufC* Buff= StringLoader::LoadLC(R_SH_STR_FILENAME);

				TFileName Hjelpper2,Hjelpper(KtxStarDotStar);
				CAknTextQueryDialog* Dialog = CAknTextQueryDialog::NewL(Hjelpper,CAknQueryDialog::ENoTone);
				Dialog->PrepareLC(R_ASK_NAME_DIALOG);
				Dialog->SetPromptL(*Buff);
				if(Dialog->RunLD())
				{
					Hjelpper.CopyLC(Hjelpper);
					if(iSelectContainer)
					{
						iSelectContainer->SelectionWithWildL(Hjelpper);
					}
		        	else if(iSearchSettings)
					{
						iSearchSettings->SelectionWithWildL(Hjelpper);
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
									Hjelpper2.Zero();
									if(AppendFileName(Hjelpper2,i)) 
									{
										Hjelpper2.CopyLC(Hjelpper2);
										if(KErrNotFound != Hjelpper2.MatchF(Hjelpper))
										{
											iSelectionBox->View()->SelectItemL(i);
										}
									}
								}
							}
						}
					}
				}
				
				CleanupStack::PopAndDestroy(Buff);
			}
			DrawNow();
			break;
		case EFolderViewMarkInverse:
			if(iSelectContainer)
			{
				iSelectContainer->InverseSelectionL();
			}
        	else if(iSearchSettings)
			{
				iSearchSettings->HandleViewCommandL(EFolderViewMarkInverse);
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
							iSelectionBox->View()->ToggleItemL(i);
						}
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
		case EChangeExtraInfo:
			if(!iSearchOn)
			{
                ChangeViewTypeL(EFalse);
			    SetMenuL();
			    //DrawNow();
			}
			break;
        default:
        	{
        		CYBrowserBaseHandler* TmpHand = GetCurrentHandler();
        		if(TmpHand)
        		{
	        		TmpHand->HandleCommandL(aCommand);
	        	}
	        	else if(iSettingsContainer)
	        	{
	        		iSettingsContainer->HandleViewCommandL(aCommand);
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
        		if (iSelectionBox->ScrollBarFrame())
            	{
            		iSelectionBox->ScrollBarFrame()->MoveVertThumbTo(iSelectionBox->View()->CurrentItemIndex());
            	}
				DrawNow();
			}
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
	
	TInt LastItemIndex(-1);
	if(iSelectionBox)
	{
		LastItemIndex = GetSelectedIndex();
	}
	DisplayListBoxL();
	FolderReadFinnishL(NULL, KErrNone);
	
	if(LastItemIndex > 0 && iSelectionBox)
	{
		CDesCArray* itemArray = STATIC_CAST( CDesCArray* ,iSelectionBox->Model()->ItemTextArray());
	
		if(itemArray && LastItemIndex >= 0)
		{
			if(itemArray->Count() > LastItemIndex)
			{
				iSelectionBox->SetCurrentItemIndex(LastItemIndex);
        		if (iSelectionBox->ScrollBarFrame())
            	{
            		iSelectionBox->ScrollBarFrame()->MoveVertThumbTo(iSelectionBox->View()->CurrentItemIndex());
            	}
				DrawNow();
			}
		}
	} 
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
		if(iMyHelpContainer)
		{
			iMyHelpContainer->SetRect(rect);	
		}
		
 		if(iSelectContainer)
		{
			iSelectContainer->SetRect(rect);
		}
			 	
	 	if(iShowString)
	 	{
	 		iShowString->UpDateSizeL();
	 	}
	 	
	 	if(iSettingsContainer)
	 	{
	 		iSettingsContainer->SetRect(rect);	
	 	}
	 	
	 	if(iYuccaSettings)
	 	{
	 		iYuccaSettings->SetRect(rect);
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
	if(iMyHelpContainer)
	{
		Ret = iMyHelpContainer->OfferKeyEventL(aKeyEvent,aType);
	}
	else if(iSelectContainer)
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
	else if (iSettingsContainer)
	{
		Ret = iSettingsContainer->OfferKeyEventL(aKeyEvent,aType);
	}
	else if(iYuccaSettings)
	{
		Ret = iYuccaSettings->OfferKeyEventL(aKeyEvent,aType);
	}
	else if(iSearchSettings)
	{	
		if(!iSearchSettings->SearchOn() && iOkToContinue && iGeneralSettings)
		{
			Ret = iGeneralSettings->SelectAndExecuteSCCommandL(aKeyEvent.iScanCode);
		}
		
		if(Ret == EKeyWasNotConsumed)
		{		
			Ret = iSearchSettings->OfferKeyEventL(aKeyEvent,aType);
		}
	}
	else 
	{	
		if(EEventKey == aType)
		{
			if((EModifierShift 	& CCoeEnv::Static()->WsSession().GetModifierState()))
			{
				if(!iSearchOn && iOkToContinue && iGeneralSettings)
				{
				 	TFileName NewPath;
					Ret = iGeneralSettings->MoveToShortCutL(aKeyEvent.iScanCode,NewPath);
					
					if(Ret == EKeyWasConsumed && iFFolderReader && iSelectionBox)
					{
						iFFolderReader->Cancel();
						iSelectionBox->ClearSelection();
						
						TInt SAve(0);
						TInt FilCount = iFFolderReader->StartReadingL(NewPath,SAve);

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
			}
			else if(!iSearchOn && iOkToContinue && iGeneralSettings)
			{
				Ret = iGeneralSettings->SelectAndExecuteSCCommandL(aKeyEvent.iScanCode);
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
                    HandleViewCommandL(EChangeExtraInfo);
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
    	
		if(iMyHelpContainer)
		{
			CEikonEnv::Static()->AppUiFactory()->MenuBar()->SetMenuTitleResourceId(R_SHOWSTRING_MENUBAR);	
			if(iCba)
			{
				iCba->SetCommandSetL(R_APPHELP_CBA);				
				iCba->DrawDeferred();
			}
		}
		else if(iSelectContainer)
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
		else if(iSettingsContainer)
		{
			iSettingsContainer->SetMenuL();
		}
		else if(iYuccaSettings)
		{
			if(iCba)
			{
				if(iYuccaSettings->AttribSelector())
				{
					TBool isReadOnly(iDriveReadOnly);
				
					if(iSearchSettings)
					{
						isReadOnly = iSearchSettings->IsFileOnReadOnlyDrive();
					}
				
					if(isReadOnly)	
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
		else
		{
			CEikonEnv::Static()->AppUiFactory()->MenuBar()->SetMenuTitleResourceId(R_Y_BROWSER_MENUBAR);	
			if(iCba)
			{
				if(iSearchOn)
				{
					iCba->SetCommandSetL(R_MYOBSR_CBA);
				}
				else
				{
					TBool ExitBut(ETrue);
					
					if(iFFolderReader && iSelectionBox)
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
				}
				
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
		
		if(iSearchOn){
            aMenuPane->SetItemDimmed(EChangeExtraInfo,ETrue);
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
			if(iPasteArray->Count())
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
	else if(iSettingsContainer)
	{
		iSettingsContainer->InitMenuPanelL(aResourceId,aMenuPane);
	}
	
	if(aMenuPane && iGeneralSettings)
	{
		iGeneralSettings->AddSCNumbertoMenuL(aMenuPane);
	}
}
/*
-----------------------------------------------------------------------

-----------------------------------------------------------------------
*/
TInt CFileListContainer::CountComponentControls() const
{
	CYBrowserBaseHandler* TmpHand = GetCurrentHandler();
    
	if(iMyHelpContainer)
	{
		return 1;
	}
	else if(iSelectContainer)
	{
		return 1;
	}
	else if(TmpHand)
	{
		return 1;//TmpHand->CountComponentControls();
	}
	else if(iSettingsContainer)
	{
		return 1;
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
    
	if(iMyHelpContainer)
	{
		return iMyHelpContainer;
	}
	else if(iSelectContainer)
	{
		return iSelectContainer;
	}
	else if(TmpHand)
	{
		return TmpHand;//->ComponentControl(aIndex);
	}
	else if(iSettingsContainer)
	{
		return iSettingsContainer;
	}
	else if(iYuccaSettings)
		return iYuccaSettings;
	else if(iShowString && aIndex)
		return iShowString;
	else if(iSearchSettings)
		return iSearchSettings;
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





