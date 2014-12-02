/* 
	Copyright (c) 2006, 
	Jukka Silvennoinen
	All rights reserved 
*/

#include <badesca.h>
#include <coeutils.h>
#include <eikenv.h>
#include <f32file.h>
#include <eiklabel.h>
#include <eikprogi.h>
#include <eikinfo.h>
#include <eikfutil.h>
#include <eikbutb.h>
#include <AknQueryDialog.h>
#include <aknglobalnote.h> 
#include <EIKPROGI.H>
#include <BABACKUP.H>
#include <FBS.H>
#include <stringloader.h> 

#include "YuccaBrowser.h"
#include "YuccaBrowser.hrh"
#ifdef __YTOOLS_SIGNED
	#include <YuccaBrowser_2000713D.rsg>
#else
	#include <YuccaBrowser.rsg>
#endif

#include "FileManHandler.h"
#include "Public_Interfaces.h"


_LIT(KtxDoubleSlash				,"\\");


const TInt KMaxDisplayedFullNameLen			= 30;

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/

CEikFileHandler::CEikFileHandler(MMyFileManReaderObserver& aObserver,MYBrowserFileUtils& aFileHandler) 
: CActive(CActive::EPriorityLow),iObserver(aObserver),iYBrowserFileUtils(aFileHandler)
,iFsSession(CEikonEnv::Static()->FsSession())
{
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CEikFileHandler::~CEikFileHandler()
{
	Cancel();

	delete iFileMan;
	iFileMan = NULL;
	
	if(iProgressDialog)
	{
		iProgressDialog->ProcessFinishedL(); 
	}

	iProgressDialog = NULL;
	iProgressInfo = NULL;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CEikFileHandler::ConstructL(void)
	{
	iFileMan = CFileMan::NewL(iFsSession);
	CActiveScheduler::Add(this);
	}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TInt CEikFileHandler::IsFolder(const TDesC& aFullName, TBool& aIsFolder)
{
	TEntry entry;
	TInt retcode = CEikonEnv::Static()->FsSession().Entry(aFullName, entry);
	if (retcode == KErrNone)
	{
		aIsFolder =entry.IsDir();
	}
		 
	return retcode;
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CEikFileHandler::FileManStartedL(void)
{
	if (iProgressDialog && !iCancel)
	{
		TBuf<60> Hjelpper;
		
		TBuf<KMaxDisplayedFullNameLen> abbrevSourceName;
		EikFileUtils::AbbreviateFileName(iCurrentSource, abbrevSourceName);

		TBuf<KMaxDisplayedFullNameLen> abbrevTargetName;
		EikFileUtils::AbbreviateFileName(iCurrentTarget, abbrevTargetName);
	
		TFileName currentSource;
		
		if (iOperation == EEikFCopy)
		{
			StringLoader::Load(Hjelpper,R_SH_STR_COPYINGFF);
			currentSource.Format(Hjelpper,&abbrevSourceName,&abbrevTargetName);
		}
		else if (iOperation == EEikFMove)
		{
			StringLoader::Load(Hjelpper,R_SH_STR_MOVINGFF);
			currentSource.Format(Hjelpper,&abbrevSourceName,&abbrevTargetName);
		}
		else if (iOperation == EEikFRename)
		{
			StringLoader::Load(Hjelpper,R_SH_STR_RENAMINGFF);
			currentSource.Format(Hjelpper,&abbrevSourceName,&abbrevTargetName);
		}
		else if (iOperation == EEikFDelete)
		{
			StringLoader::Load(Hjelpper,R_SH_STR_DELETINGFF);
			currentSource.Format(Hjelpper,&abbrevSourceName);
		}
			
		iProgressDialog->SetTextL(currentSource);
		
		if(iProgressInfo)
		{
			iProgressInfo->SetAndDraw(iIndex + 1);	
		}
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CEikFileHandler::FileManEnded(void)
{	
	// CFileMan::TAction action = fileMan->CurrentAction();
	if(iFileMan)
	{
		iLastError = iFileMan->GetLastError();
		iErrorInfo = iFileMan->GetMoreInfoAboutError();
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CEikFileHandler::HandleFileServerErrorL(TInt aAdditionalError)
{
	if(iHadError == KErrNone)
	{
		if(aAdditionalError != KErrNone)
			iHadError = aAdditionalError;
		else
			iHadError = iLastError;
	}

	if(aAdditionalError != KErrNone)
	{
		iLastError = aAdditionalError;
	}

	TBool ShowTarget(EFalse);
	
	if (iLastError == KErrAlreadyExists)
	{
		ShowTarget = ETrue;
	}
	else if (iLastError == KErrInUse)
	{
		if (iErrorInfo == ETrgOpenFailed)
			ShowTarget = ETrue;
		else
			ShowTarget = EFalse;
	}
	else if (iLastError == KErrAccessDenied)
	{
		if (iErrorInfo == ETrgOpenFailed)
			ShowTarget = ETrue;
		else
			ShowTarget = EFalse;
	}
	else if (iLastError == KErrDiskFull)
	{
		ShowTarget = ETrue;
	}
	else if (iLastError == KErrNotReady)
	{
//		TBuf<1> driveLetter;
		if (iErrorInfo == ETrgOpenFailed)
			ShowTarget = ETrue;
		else
			ShowTarget = EFalse;
	}
	else if (iLastError == KErrCorrupt)
	{
		ShowTarget = EFalse;
	}
	else if (iLastError == KErrBadName)
	{
		if (iErrorInfo == ETrgOpenFailed)
			ShowTarget = ETrue;
		else
			ShowTarget = EFalse;
	}
	else
	{	
		ShowTarget = EFalse;
	}

	if(ShowTarget)
		iYBrowserFileUtils.ShowFileErrorNoteL(iCurrentTarget,iLastError);
	else
		iYBrowserFileUtils.ShowFileErrorNoteL(iCurrentSource,iLastError);
	
	iLastError = KErrNone;
}


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CEikFileHandler::SetOperationL(TInt aOperation, const CDesCArray* aSourceFullNameArray, const TFileName* aTargetFullName)
{
	iCancel = EFalse;
	iOperation = aOperation;
	iSourceFullNameArray = aSourceFullNameArray;
	iTargetFullName = aTargetFullName;
	
	if(aSourceFullNameArray)
	{
		if (aSourceFullNameArray->MdcaCount())
		{
			iProgressDialog = new (ELeave) CAknProgressDialog((REINTERPRET_CAST(CEikDialog**,&iProgressDialog)),ETrue);
			iProgressDialog->PrepareLC(R_PROGRESS_NOTE);
			iProgressInfo = iProgressDialog->GetProgressInfoL();
			iProgressDialog->SetCallback(this);
			iProgressDialog->RunLD();
			
			TInt CVount = (aSourceFullNameArray->MdcaCount() + 1);
			
			iProgressInfo->SetFinalValue(CVount);
		}
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CEikFileHandler::Start()
{
	Cancel();
	iCancel = EFalse;
	SetActive();
	iStatus = KRequestPending;
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TInt CEikFileHandler::PerformOperationL()
{
	TInt retcode = KErrNone;
	
	iCurrentTarget.Zero();
	iCurrentSource.Zero();
	
	iCurrentSource.Copy((*iSourceFullNameArray)[iIndex]);
	TBool currentEntryIsAFolder;
	retcode = IsFolder(iCurrentSource, currentEntryIsAFolder);
	if(retcode != KErrNone)
	{
		return retcode;
	}
	
	TFileName TempName;

	switch (iOperation)
	{
	case EEikFCopy:
		if (currentEntryIsAFolder)
		{
			TempName = *iTargetFullName;
			retcode = EikFileUtils::Parse(iCurrentSource);
			if(retcode == KErrNone)
			{		
				TParsePtrC sourceParse(iCurrentSource);
				if (sourceParse.Path().Compare(KtxDoubleSlash) != 0)
				{
					TempName.Append(EikFileUtils::FolderNameFromFullName(iCurrentSource));
					TempName.Append(KtxDoubleSlash);
				}
				
				if (EikFileUtils::FolderExists(TempName))
				{
					retcode = KErrAlreadyExists;
				}
				else
				{	
					retcode = iFsSession.MkDir(TempName);
				}
			}
			
			if(TempName.Length())
			{	
				iCurrentTarget.Copy(TempName);
				FileManStartedL();
				retcode = iFileMan->Copy(iCurrentSource, TempName, CFileMan::ERecurse);
				FileManEnded();
				
				if(retcode == KErrInUse)
				{
					TParsePtrC NameHjelp(iCurrentSource);
					TempName.Copy(*iTargetFullName);
					TempName.Append(NameHjelp.NameAndExt());
			
					if(DoSpecialCopyL(iCurrentSource,TempName, ETrue))
					{
						retcode = KErrNone;
					}
				}
			}
			
			if (retcode == KErrNotFound)
			{	
				retcode = KErrNone;
			}
			else if (retcode == KErrArgument)	// assuming this was caused by a cyclic copy/move op
			{	
				FileManStartedL();
				retcode = iFileMan->RmDir(TempName);
				FileManEnded();
			}
		}
		else
		{
			TBool OkContinue(ETrue);
			TParsePtrC NameHjelp(iCurrentSource);
			TempName.Copy(*iTargetFullName);
			TempName.Append(NameHjelp.NameAndExt());
			
			if(BaflUtils::FileExists(iFsSession,TempName))
			{
				TFileName AskBuf;
				
				StringLoader::Load(TempName,R_SH_STR_EXISTREPLACE);
				
				AskBuf.Copy(NameHjelp.NameAndExt());
				AskBuf.Append(TempName);
	
				CAknQueryDialog* dlgTwo = CAknQueryDialog::NewL();
				if(dlgTwo->ExecuteLD(R_QUERY,AskBuf))
				{
					TempName.Copy(*iTargetFullName);
					TempName.Append(NameHjelp.NameAndExt());
					iFsSession.Delete(TempName);
				}
				else
				{
					OkContinue = EFalse;
				}
			}
			
			if(OkContinue)
			{
				iCurrentTarget.Copy(*iTargetFullName);
				
				FileManStartedL();
				retcode = iFileMan->Copy(iCurrentSource, *iTargetFullName, CFileMan::EOverWrite);
				FileManEnded();
				
				if(retcode == KErrInUse)
				{
					TParsePtrC NameHjelp(iCurrentSource);
					TempName.Copy(*iTargetFullName);
					TempName.Append(NameHjelp.NameAndExt());
					
					if(DoSpecialCopyL(iCurrentSource,TempName, ETrue))
					{
						retcode = KErrNone;
					}
				}
			}
		}
		break;
	case EEikFMove:
		if (currentEntryIsAFolder)
		{
			TempName = *iTargetFullName;
			TempName.Append(EikFileUtils::FolderNameFromFullName(iCurrentSource));
			TempName.Append(KtxDoubleSlash);
			
			if(EikFileUtils::FolderExists(TempName))
			{
				retcode = KErrAlreadyExists;
			}
			else
			{
				retcode = iFsSession.MkDir(TempName);
				if (retcode == KErrNone)
				{
					iCurrentTarget.Copy(TempName);
					FileManStartedL();
					retcode = iFileMan->Move(iCurrentSource,iCurrentTarget, CFileMan::ERecurse);
					FileManEnded();
				}
			}
		}
		else
		{
			TBool OkContinue(ETrue);
			
			TParsePtrC NameHjelp(iCurrentSource);
			TempName.Copy(*iTargetFullName);
			TempName.Append(NameHjelp.NameAndExt());
			
			if(BaflUtils::FileExists(iFsSession,TempName))
			{
				TFileName AskBuf;;
	
				StringLoader::Load(TempName,R_SH_STR_EXISTREPLACE);
				
				AskBuf.Copy(NameHjelp.NameAndExt());
				AskBuf.Append(TempName);
	
				CAknQueryDialog* dlgTwo = CAknQueryDialog::NewL();
				if(dlgTwo->ExecuteLD(R_QUERY,AskBuf))
				{
					TempName.Copy(*iTargetFullName);
					TempName.Append(NameHjelp.NameAndExt());
					iFsSession.Delete(TempName);
				}
				else
				{
					OkContinue = EFalse;
				}
			}
			
			if(OkContinue)
			{
				iCurrentTarget.Copy(*iTargetFullName);
				
				FileManStartedL();
				
				CheckAndChnageReadOnlyL(iCurrentSource);
				
				retcode = iFileMan->Move(iCurrentSource,iCurrentTarget, CFileMan::EOverWrite);
				FileManEnded();
				
				if(retcode == KErrInUse)
				{
					TParsePtrC NameHjelp(iCurrentSource);
					TempName.Copy(*iTargetFullName);
					TempName.Append(NameHjelp.NameAndExt());
					
					if(DoSpecialCopyL(iCurrentSource,TempName, EFalse))
					{
						retcode = KErrNone;
					}
				}
			}
		}
	break;
		case EEikFRename:
		if (currentEntryIsAFolder)
		{
			iCurrentTarget.Copy(*iTargetFullName);
			
			if(iCurrentSource.Length()
			&& iCurrentTarget.Length())
			{
				TempName.Copy(iCurrentTarget);
				if (EikFileUtils::FolderExists(TempName))
				{
					retcode = KErrAlreadyExists;
				}
				else
				{
					FileManStartedL();
					retcode = iFileMan->Rename(iCurrentSource.Left(iCurrentSource.Length()-1), iTargetFullName->Left(iTargetFullName->Length()-1), CFileMan::ERecurse);
					FileManEnded();
				}
			}
		}
		else
		{	
			TBool OkContinue(ETrue);
			TParsePtrC NameHjelp(iCurrentSource);
			TempName.Copy(*iTargetFullName);
			TempName.Append(NameHjelp.NameAndExt());
			if(BaflUtils::FileExists(iFsSession,TempName))
			{				
				TFileName AskBuf;
				
				StringLoader::Load(TempName,R_SH_STR_EXISTREPLACE);
				
				AskBuf.Copy(NameHjelp.NameAndExt());
				AskBuf.Append(TempName);
				
				CAknQueryDialog* dlgTwo = CAknQueryDialog::NewL();
				if(dlgTwo->ExecuteLD(R_QUERY,AskBuf))
				{
					TempName.Copy(*iTargetFullName);
					TempName.Append(NameHjelp.NameAndExt());
					iFsSession.Delete(TempName);
				}
				else
				{
					OkContinue = EFalse;
				}
			}
			
			if(OkContinue)
			{
				iCurrentTarget.Copy(*iTargetFullName);
				
				FileManStartedL();
				retcode = iFileMan->Rename(iCurrentSource, *iTargetFullName, CFileMan::EOverWrite);
				FileManEnded();
				
				if(retcode == KErrInUse)
				{
					TParsePtrC NameHjelp(iCurrentSource);
					TempName.Copy(*iTargetFullName);
					TempName.Append(NameHjelp.NameAndExt());
					
					if(DoSpecialCopyL(iCurrentSource,TempName, EFalse))
					{
						retcode = KErrNone;
					}
				}
			}
		}
		break;
	case EEikFDelete:
		if (currentEntryIsAFolder)
		{
			FileManStartedL();
			retcode = iFileMan->RmDir(iCurrentSource);
			FileManEnded();
		}
		else
		{
			FileManStartedL();
			
			CheckAndChnageReadOnlyL(iCurrentSource);
			
			retcode = iFileMan->Delete(iCurrentSource);
			
			FileManEnded();
			
			if(retcode == KErrInUse)
			{
				if(DoSpecialDeleteL(iCurrentSource))
				{
					retcode = KErrNone;
				}
			}
		}
		break;
	default:
		break;
		}
	
	return(retcode);
	}


/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
void CEikFileHandler::CheckAndChnageReadOnlyL(const TDesC& aSourceFile)
{
	TBool DriveReadOnly = ETrue;
				
	if(aSourceFile.Length())
	{
		TInt CurrentDrive;
		TVolumeInfo volumeInfo; 
		TChar DriveChar = aSourceFile[0];
		
		if(KErrNone == CCoeEnv::Static()->FsSession().CharToDrive(DriveChar,CurrentDrive))
		{		
			if(CCoeEnv::Static()->FsSession().IsValidDrive(CurrentDrive))
			{
				if(KErrNone == CCoeEnv::Static()->FsSession().Volume(volumeInfo,CurrentDrive))
				{
					TUint AnotherHelper = volumeInfo.iDrive.iMediaAtt;

					if(AnotherHelper & KMediaAttWriteProtected)
					{
						DriveReadOnly = ETrue;			
					}
					else
					{
						DriveReadOnly= EFalse;
					}
				}
			}
		}
	}
	
	if(!DriveReadOnly)
	{
		TEntry MyEntry;
		if(KErrNone == CCoeEnv::Static()->FsSession().Entry(aSourceFile,MyEntry))
		{
			if(MyEntry.IsReadOnly())
			{
				TBuf<255> AskBuffer,Hjelpper;
				
				StringLoader::Load(Hjelpper,R_SH_STR_ISROMODIFY);
				
				TParsePtrC NameHjelp(aSourceFile);
				
				AskBuffer.Copy(NameHjelp.NameAndExt());
				AskBuffer.Append(Hjelpper);
				
				CAknQueryDialog* dlg = CAknQueryDialog::NewL();
				if(dlg->ExecuteLD(R_QUERY,AskBuffer))
				{
					TUint SetAttMask(0);
					TUint ClearAttMask(0);
					
					ClearAttMask = ClearAttMask | KEntryAttReadOnly;
					
					TInt Err = CCoeEnv::Static()->FsSession().SetAtt(aSourceFile,SetAttMask,ClearAttMask);
					
					if(Err != KErrNone)
					{
						HandleFileServerErrorL(Err);
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
_LIT(KtxSystemFonts			,"\\system\\fonts\\");

TBool CEikFileHandler::DoSpecialDeleteL(const TDesC& aSourceFile)
{	
	TInt err=KErrNone;
	TBool OkAlready(EFalse);
	
	TFileName Hjelpper;
	Hjelpper.CopyLC(aSourceFile);
	
	if(Hjelpper.Find(KtxSystemFonts) != KErrNone)
	{
		CFbsTypefaceStore* fbsTypefaceStore = CFbsTypefaceStore::NewL(CEikonEnv::Static()->ScreenDevice());
		CleanupStack::PushL(fbsTypefaceStore);

		fbsTypefaceStore->RemoveFontFileLocksL(aSourceFile);	
		
		for (TInt i=0; i < 10; i++)
		{
			User::After(600000);

			err = iFileMan->Delete(aSourceFile);
			if (err==KErrNone)
			{
				OkAlready = ETrue;
				break;
			}
		}
	
		CleanupStack::PopAndDestroy(fbsTypefaceStore);
	}
	
	if(!OkAlready)
	{
		CBaBackupSessionWrapper* server = CBaBackupSessionWrapper::NewL();
		CleanupStack::PushL(server);
		
		server->CloseFileL(aSourceFile, MBackupObserver::EReleaseLockNoAccess);//EReleaseLockReadOnly);		
		
		TInt i =0;
		do
		{
			i++;
			User::After(600000);
			
			err	= iFileMan->Delete(aSourceFile);
			
			if (err==KErrNone)
				break;
		
		}while(i < 5);
		
		CleanupStack::PopAndDestroy();//server
	}
	
	if (err==KErrNone)
		return ETrue;
	else
		return EFalse;
}
/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
TBool CEikFileHandler::DoSpecialCopyL(const TDesC& aSourceFile,const TDesC& aDestFile, TBool aCopy)
{
	CBaBackupSessionWrapper* server = CBaBackupSessionWrapper::NewL();
	CleanupStack::PushL(server);
	
	TBool SourceOpen(EFalse);
	TBool TargetOpen(EFalse);
	
	iFsSession.IsFileOpen(aSourceFile,SourceOpen);
	
	if(SourceOpen)
	{	
		server->CloseFileL(aSourceFile, MBackupObserver::EReleaseLockNoAccess);//);	
	}
	
	if(BaflUtils::FileExists(iFsSession,aDestFile))
	{
		if(iFsSession.Delete(aDestFile) != KErrNone)
		{
			TargetOpen = ETrue;
			server->CloseFileL(aDestFile, MBackupObserver::EReleaseLockNoAccess);//EReleaseLockReadOnly);	
		}
	}
	
	TInt err=KErrNone;
	TInt i =0;
	do
	{
		i++;
		User::After(600000);
		
		if(aCopy)
		{
			err	= iFileMan->Copy(aSourceFile,aDestFile, CFileMan::EOverWrite);
		}
		else
		{
			err	= iFileMan->Move(aSourceFile,aDestFile, CFileMan::EOverWrite);
		}
		
		if (err == KErrNone)
		{
			break;
		}
	
	}while(i < 5);

	if(aCopy && SourceOpen)
	{
		server->RestartFile(aSourceFile);
	}
	
	if(TargetOpen)
	{
		server->RestartFile(aDestFile);
	}
	
	CleanupStack::PopAndDestroy();//server
	
	TBool Ret(EFalse);
	if(err != KErrNone)
	{
		if(TargetOpen)
		{// not trying to replace open files...
			Ret = EFalse;
		}
		else
		{
			HBufC* Buff= StringLoader::LoadLC(R_STR_FORSECOPY);
			
			CAknQueryDialog* dlg = CAknQueryDialog::NewL();
			if(dlg->ExecuteLD(R_QUERY,*Buff))
			{
				Ret = DoForceCopyL(aSourceFile,aDestFile);
			}
			else
			{
				Ret = EFalse;
			}
			
			CleanupStack::PopAndDestroy(Buff);
		}
	}
	else
	{	
		Ret = ETrue;
	}
	
	return Ret;
}
/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
TBool CEikFileHandler::DoForceCopyL(const TDesC& aSourceFile,const TDesC& aDestFile)
{
	TInt TargetSiz(-100),FileSize(-100);
	TEntry FileEntry;
	RFile MFile;

	TInt Ret = iFsSession.Entry(aSourceFile,FileEntry);
	if(Ret == KErrNone)
	{
		FileSize = FileEntry.iSize;
		Ret = MFile.Replace(iFsSession,aDestFile,EFileShareAny);
		if(Ret == KErrNone)
		{
			CleanupClosePushL(MFile);

			HBufC8* FileBuffer = HBufC8::NewLC(3000);
			TPtr8 FileBuf(FileBuffer->Des());

			for(TInt i = 0; i < FileSize; i = i + 3000)
			{
				if(FileSize > i + 3000)
				{
					FileBuf.Set(FileBuffer->Des());
					Ret = iFsSession.ReadFileSection(aSourceFile,i,FileBuf,3000);
				}
				else
				{
					FileBuf.Set(FileBuffer->Des());
					Ret = iFsSession.ReadFileSection(aSourceFile,i,FileBuf,(FileSize - i));
				}

				if(Ret != KErrNone)
				{
					if(!i)
					{
						break;
					}
				}

				if(FileBuffer->Des().Length())
				{
					if(MFile.Write(i,FileBuffer->Des()) != KErrNone)
					{
						if(!i)
						{
							break;
						}
					}
				}
			}
	
			
			Ret = MFile.Size(TargetSiz);
			if(Ret == KErrNone)
			{
				Ret = iFsSession.Entry(aSourceFile,FileEntry);
			}
			
			CleanupStack::PopAndDestroy(2); //MFile, FileBuffer
		}
	}
	
	if(Ret != KErrNone) // erros so we failed
		return EFalse;
	else if(FileEntry.iSize != TargetSiz)// size different (write inprocess on source ?)
		return EFalse;                  // so we should say that we failed..
	else
		return ETrue;
}
/*
-----------------------------------------------------------------------------
HandleOperationEndedL();
-----------------------------------------------------------------------------
*/
void CEikFileHandler::RunL()
{
	if(iSourceFullNameArray)
	{
		if(!iCancel
		&& (iIndex < iSourceFullNameArray->Count()))
		{
			TInt errorCode = iStatus.Int();
			
			if (errorCode == KErrNone)
			{
				errorCode = PerformOperationL();
			
				if (errorCode != KErrNone)
				{
					HandleFileServerErrorL(errorCode);
				}
			}
			else
			{
				HandleFileServerErrorL(iStatus.Int());
			}
			
			++iIndex;
			
			SetActive();
			iStatus = KRequestPending;
			TRequestStatus* status = &iStatus;
			User::RequestComplete(status,KErrNone);
		}
		else
		{
			HandleOperationEndedL();
		}
	}
	else
	{
		HandleOperationEndedL();
	}
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CEikFileHandler::DialogDismissedL (TInt /*aButtonId*/)
{		  		
	iProgressDialog = NULL;
	iProgressInfo = NULL;
		
	Cancel();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CEikFileHandler::DoCancel()
{
	iCancel = ETrue;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CEikFileHandler::HandleOperationEndedL(void)
{
	if(iProgressDialog)
	{
		iProgressDialog->ProcessFinishedL(); 
	}

	iProgressDialog = NULL;
	iProgressInfo = NULL;

	iObserver.FileManOperatioDoneL(this,iOperation,iHadError,iErrorInfo);
}




