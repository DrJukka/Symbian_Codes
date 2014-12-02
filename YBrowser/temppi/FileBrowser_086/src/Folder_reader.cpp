/* 
	Copyright (c) 2006, 
	Jukka Silvennoinen
	All rights reserved 
*/
#include "Folder_reader.h"
//#include "CommonStrings.h"
//#include "SCallThema.hrh" 

#include <COEMAIN.H>
#include <BAUTILS.H>
#include <apmrec.h> 
#include <apgcli.h>
#include <AknLists.h>
#include <AknGlobalNote.h>


/*
-------------------------------------------------------------------------------
****************************  CFFolderReader   *************************************
-------------------------------------------------------------------------------
*/  

CFFolderReader* CFFolderReader::NewL(MMyFolderReaderObserver& aObserver,CIconHandler& aIconUtils)
    {
    CFFolderReader* self = NewLC(aObserver,aIconUtils);
    CleanupStack::Pop(self);
    return self;
    }

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/    
CFFolderReader* CFFolderReader::NewLC(MMyFolderReaderObserver& aObserver,CIconHandler& aIconUtils)
    {
    CFFolderReader* self = new (ELeave) CFFolderReader(aObserver,aIconUtils);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }
    
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/   
CFFolderReader::CFFolderReader(MMyFolderReaderObserver& aObserver,CIconHandler& aIconUtils)
:CActive(0),iObserver(aObserver),iIconHandler(aIconUtils)
{
	iSettings.iEntryMask = KEntryAttMaskSupported;
	iSettings.iEntrySort = EDirsFirst;
	iSettings.iShowHiddenSystem = ETrue;
	iSettings.iOptimize 		= ETrue;
	iSettings.iDoRecocnize 		= ETrue;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CFFolderReader::~CFFolderReader()
{
	Cancel();
	delete iFile_list;
	iFile_list = NULL;
	delete iSelPath;
	iSelPath = NULL;
	
	delete iTypeFilterArray;
	iTypeFilterArray = NULL;
	
	iOptimizeArray.Reset();
	iOldArray.ResetAndDestroy();
	iItemArray.ResetAndDestroy();	
	iFsSession.Close();
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CFFolderReader::ConstructL(void)
{
	User::LeaveIfError(iFsSession.Connect());
	
	iItemArray.ResetAndDestroy();
	
	CActiveScheduler::Add(this);
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CFFolderReader::RemoveTypeFilterArrayL(void)
{
	delete iTypeFilterArray;
	iTypeFilterArray = NULL;
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CFFolderReader::SetTypeFilterArrayL(CDesCArrayFlat& aArray)
{
	RemoveTypeFilterArrayL();
	
	if(aArray.Count())
	{
		iTypeFilterArray = new(ELeave)CDesCArrayFlat(10);
	
		TBuf<100> TmpBuff;
		for(TInt i=0; i < aArray.MdcaCount(); i++)
		{
			if(aArray.MdcaPoint(i).Length() > 100)
				TmpBuff.CopyLC(aArray.MdcaPoint(i).Left(99));
			else
				TmpBuff.CopyLC(aArray.MdcaPoint(i));
			
			TmpBuff.TrimAll();
			
			if(TmpBuff.Length())
			{
				iTypeFilterArray->AppendL(TmpBuff);
			}
		}
	}
	
	if(iTypeFilterArray)
	{
		if(!iTypeFilterArray->MdcaCount())
		{
			RemoveTypeFilterArrayL();
		}
	}
	
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CFFolderReader::SetSettings(TFReadSettings aSettings)
{
	iSettings = aSettings;
	
	if(!iSettings.iOptimize)
	{
		iOldArray.ResetAndDestroy();
	}
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CFFolderReader::DoCancel()
{
	iCancel = ETrue;
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CFFolderReader::RemoveFromListL(const TDesC& aNameAndExt)
{
	for(TInt i =0;i < iItemArray.Count(); i++)
	{
		if(iItemArray[i])
		{
			if(iItemArray[i]->iName)
			{
				if(iItemArray[i]->iName->Des() == aNameAndExt)
				{	
					delete iItemArray[i];
					iItemArray.Remove(i);
				}
			}
		}
	}
}

/*
-------------------------------------------------------------------------------	
-------------------------------------------------------------------------------
*/

void CFFolderReader::AppendDriveToListL(TBool AddMe,const TDesC& aName,TInt32 aSize,TInt32 aFree)
{
	if(aName.Length() && AddMe)
	{
		CFFileItem* NewItem = new(ELeave)CFFileItem();
		CleanupStack::PushL(NewItem);
		NewItem->iName 		= aName.AllocL();
		NewItem->iType 		= CFFileItem::EFDriveFile;
		NewItem->iSize 		= aSize;
		NewItem->iExtra		= aFree;
		
		CleanupStack::Pop(NewItem);
		iItemArray.Append(NewItem);
	}
	
	TInt FullCount(0);
	
	if(iFile_list)
	{
		FullCount = iFile_list->Count();
	}
	
	iObserver.FolderReadOnProcessL(iReadNow,FullCount,iError);	
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/

void CFFolderReader::AppendOptimizedToListL(CFFileItem& aItem)
{	
	CFFileItem* NewItem = new(ELeave)CFFileItem();
	CleanupStack::PushL(NewItem);
	NewItem->iName 		= aItem.iName->Des().Alloc();
	NewItem->iType 		= aItem.iType;
	NewItem->iSize 		= aItem.iSize;
	NewItem->iModified	= aItem.iModified;
	NewItem->iFolders	= aItem.iFolders;
	NewItem->iFiles		= aItem.iFiles;
		
	CleanupStack::Pop(NewItem);
	iItemArray.Append(NewItem);
	
	TInt FullCount(0);
	
	if(iFile_list)
	{
		FullCount = iFile_list->Count();
	}
	
	if(iReadNow%200 == 0)
	{
		iObserver.FolderReadOnProcessL(iReadNow,FullCount,iError);
	}
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CFFolderReader::AppendToListL(const TEntry& aEntry,TInt32 aType)
{
	if(aEntry.iName.Find(_L("\t")) == KErrNotFound)
	{
		CFFileItem* NewItem = new(ELeave)CFFileItem();
		CleanupStack::PushL(NewItem);
		NewItem->iName 		= aEntry.iName.AllocL();
		NewItem->iType 		= aType;
		NewItem->iSize 		= aEntry.iSize;
		NewItem->iModified	= aEntry.iModified;
		
		TBool AddMe(ETrue);
		if(aType == CFFileItem::EFFolderFile)
		{
			AddMe = GetFolderFileCountL(aEntry.iName,NewItem->iFolders,NewItem->iFiles,EFalse);
		}
		
		if(AddMe)
		{
			CleanupStack::Pop(NewItem);
			iItemArray.Append(NewItem);
		}
		else
		{
			CleanupStack::PopAndDestroy(NewItem);
		}
	}

	
	TInt FullCount(0);
	
	if(iFile_list)
	{
		FullCount = iFile_list->Count();
	}
	
	iObserver.FolderReadOnProcessL(iReadNow,FullCount,iError);
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
TBool CFFolderReader::GetFolderFileCountL(const TDesC& aName,TInt32& aFolders,TInt32& aFiles,TBool aFullName)
{
	TBool Ret(ETrue);
	aFolders = aFiles = 0;
	
	if(iSelPath)
	{
		TFileName FullFileName;	
		
		if(aFullName)
		{
			FullFileName.Copy(aName);
		}
		else
		{
			FullFileName.Copy(iSelPath->Des());
			FullFileName.Append(aName);
			FullFileName.Append(_L("\\"));
		}
		
		CDir* tmpFile_list(NULL);
		TInt Err = iFsSession.GetDir(FullFileName,iSettings.iEntryMask,iSettings.iEntrySort,tmpFile_list);
		
		if(KErrNone == Err)
		{
			if(tmpFile_list)
			{
				for(TInt i=0; i < tmpFile_list->Count(); i++)
				{
					if((*tmpFile_list)[i].IsDir())
					{
						aFolders = aFolders + 1;
					}
					else
					{
						aFiles = aFiles + 1;
					}
				}
			}
			
		}
		else if(KErrPermissionDenied == Err)
		{
			Ret = EFalse;
		}
		
		delete tmpFile_list;
		tmpFile_list = NULL;			
	}	
	
	return Ret;
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CFFolderReader::RunL()
{
	iHadError = iError = iStatus.Int();
	
	if(iError == KErrCancel || iDone)
	{
		iError = KErrNone;
		FinnishReadL();
	}
	else if(!iCancel && iFile_list)
	{	
		iError = KErrNone;
		TRAP(iError,DoOneReadL());
		
		if(iError != KErrNone)
		{
			iReadNow = iReadNow + 1;
				
			if(iHadError == KErrNone)
			{
				iHadError = iError;
			}
		}
		
		TRequestStatus* status=&iStatus;
		User::RequestComplete(status, KErrNone);
		SetActive();	
	}
	else
	{
		FinnishReadL();
	}
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
TInt  CFFolderReader::RemovePathAndReFreshL(const TDesC& aFolder, TInt& aSave)
{
	TFileName TmpFilName;
	TmpFilName.Copy(aFolder);				
	TChar FEnd('\\');
	
	if(TmpFilName.Length() > 3)
	{
		TChar LChar(TmpFilName[TmpFilName.Length() - 1]);
		if(LChar != FEnd)
		{
			TmpFilName.Append(FEnd);
		}
	}
		
	for(TInt zs=0; zs < iOldArray.Count(); zs++)
	{
		if(iOldArray[zs])
		{
			if(iOldArray[zs]->iPath)
			{
				if(TmpFilName == iOldArray[zs]->iPath->Des())
				{
					COldArrays* RemMe = iOldArray[zs];
				
					iOldArray.Remove(zs);
					delete RemMe;
					RemMe = NULL;
				}
			}
		}
	}
	
	delete iSelPath;
	iSelPath = NULL;
	
	iItemArray.ResetAndDestroy();
	
	return StartReadingL(TmpFilName,aSave);
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
TInt CFFolderReader::StartReadingFoldersOnlyL(const TDesC& aFolder, TInt& aSave)
{
	iFoldersOnly = ETrue;
	return DoStartReadingL(aFolder,aSave);	
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
TInt CFFolderReader::StartReadingL(const TDesC& aFolder, TInt& aSave)
{
	iFoldersOnly = EFalse;
	return DoStartReadingL(aFolder,aSave);
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
TInt CFFolderReader::DoStartReadingL(const TDesC& aFolder, TInt& aSave)
{
	Cancel();
	
	iCount1 = iCount2 = 0;
	
	TInt Ret(0);
	iHadError = iError = KErrNone;
	iDone = iCancel = EFalse;
	iReadNow = 0;
	TFileName TmpFilName;
	delete iFile_list;
	iFile_list = NULL;
	
	iOptimizeArray.Reset();
	
	iDriveReadOnly = ETrue;
				
	TInt CurrentDrive;
	TVolumeInfo volumeInfo; 
	
	if(aFolder.Length() > 0)
	{
		TChar DriveChar = aFolder[0];
		
		if(KErrNone == CCoeEnv::Static()->FsSession().CharToDrive(DriveChar,CurrentDrive))
		{		
			if(CCoeEnv::Static()->FsSession().IsValidDrive(CurrentDrive))
			{
				if(KErrNone == CCoeEnv::Static()->FsSession().Volume(volumeInfo,CurrentDrive))
				{
					TUint AnotherHelper = volumeInfo.iDrive.iMediaAtt;

					if(AnotherHelper & KMediaAttWriteProtected)
					{
						iDriveReadOnly = ETrue;			
					}
					else
					{
						iDriveReadOnly = EFalse;
					}
				}
			}
		}
	}
	
	if(iSelPath && iSettings.iOptimize)
	{
		if(iSelPath->Des().Length() >= 3)
		{
			TBool ImAlreadySaved(EFalse);
			
			TmpFilName.Copy(iSelPath->Des());
				
			TChar FEnd('\\');
			
			if(TmpFilName.Length() > 3)
			{
				TChar LChar(TmpFilName[TmpFilName.Length() - 1]);
				if(LChar != FEnd)
				{
					TmpFilName.Append(FEnd);
				}
			}
				
			for(TInt zs=(iOldArray.Count() - 1); zs >= 0 ; zs--)
			{
				if(iOldArray[zs])
				{
					if(iOldArray[zs]->iPath)
					{
						if(TmpFilName == iOldArray[zs]->iPath->Des())
						{
							if(iOldArray[zs]->iItemArray.Count() == iItemArray.Count())
							{
								ImAlreadySaved = ETrue;
							}
							else
							{
								ImAlreadySaved = EFalse;
								COldArrays* REmItem = iOldArray[zs];
								iOldArray.Remove(zs);
								delete REmItem;
								REmItem = NULL;
								break;
							}
						}
					}
				}
			}
			
			if(!ImAlreadySaved)// going up, add to the list.
			{
				COldArrays* NewOldItem = new(ELeave)COldArrays();
				CleanupStack::PushL(NewOldItem);
				
				NewOldItem->iPath = TmpFilName.AllocL();
				
				for(TInt xx=0; xx < iItemArray.Count(); xx++)
				{
					if(iItemArray[xx])
					{
						if(iItemArray[xx]->iName)
						{
							CFFileItem* NewFilItem = new(ELeave)CFFileItem;
							CleanupStack::PushL(NewFilItem);
							
							NewFilItem->iType = iItemArray[xx]->iType;
							NewFilItem->iName = iItemArray[xx]->iName->Des().AllocL();	
							NewFilItem->iSize = iItemArray[xx]->iSize;
							NewFilItem->iExtra = iItemArray[xx]->iExtra;
							NewFilItem->iFolders = iItemArray[xx]->iFolders;
							NewFilItem->iFiles = iItemArray[xx]->iFiles;
							NewFilItem->iModified = iItemArray[xx]->iModified;
		
							CleanupStack::Pop(NewFilItem);
							NewOldItem->iItemArray.Append(NewFilItem);
						}
					}
				}
				
				CleanupStack::Pop(NewOldItem);
				iOldArray.Append(NewOldItem);
			}
		}	
	}		
	
	iOptimizeArray.Reset();
	iOldArray.Compress();
	
	if(aFolder.Length() >= 3 && iSettings.iOptimize && !iTypeFilterArray)
	{
		TmpFilName.Copy(aFolder);
			
		TChar FEnd('\\');
		
		if(TmpFilName.Length() > 3)
		{
			TChar LChar(TmpFilName[TmpFilName.Length() - 1]);
			if(LChar != FEnd)
			{
				TmpFilName.Append(FEnd);
			}
		}
		
		for(TInt z=0; z < iOldArray.Count(); z++)
		{
			if(iOldArray[z])
			{
				if(iOldArray[z]->iPath)
				{						
					if(iOldArray[z]->iPath->Des() == aFolder)
					{
						for(TInt y=0; y < iOldArray[z]->iItemArray.Count(); y++)
						{
							iOptimizeArray.Append(iOldArray[z]->iItemArray[y]);
						}
						break;
					}
				}
			}
		}
	}

//	if(!iDone)
	{	
		
		HBufC* SelPathTmp = aFolder.AllocL();				
		delete iSelPath;
		iSelPath = SelPathTmp;
		
		iItemArray.ResetAndDestroy();
		
		if(iSelPath->Des().Length() < 3)
		{
			iOptimizeArray.Reset();
			
			AddDriveListL();
			iDone = ETrue;
			Ret = 1;
		}
		else
		{		
			iError = iFsSession.GetDir(iSelPath->Des(),iSettings.iEntryMask,iSettings.iEntrySort,iFile_list);	
			if(iError != KErrNone)
			{
				delete iFile_list;
				iFile_list = NULL;
			}
			else
			{
				if(iFile_list)
				{
					Ret = iFile_list->Count();
				}
				
				
				iError = KErrNone;
				TRAP(iError,DoOneReadL());
				
				if(iError != KErrNone)
				{
					iReadNow = iReadNow + 1;
						
					if(iHadError == KErrNone)
					{
						iHadError = iError;
					}
				}
			}
		}
	}
	
	if(!iTypeFilterArray)
	{
		aSave = iOptimizeArray.Count();
	}
	else
	{
		aSave = 0;	
	}
	
	// lets fool the system a bit, to show the progress note.
	if(aSave > 200)
	{
		aSave = 0;
	}
	
	TRequestStatus* status=&iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
		
	
	return Ret;
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CFFolderReader::AddDriveListL(void)
{
//	TDriveList drivelist;
	TChar driveLetter; 
	TInt driveNumber=EDriveA; 
	TBuf<120> HelpBuffer;
	TVolumeInfo volumeInfo; 

//	User::LeaveIfError(CCoeEnv::Static()->FsSession().DriveList(drivelist)); 
	
	for(;driveNumber<=EDriveZ;driveNumber++) 
	{ 
//		if (drivelist[driveNumber]) 
		{	
			if(CCoeEnv::Static()->FsSession().IsValidDrive(driveNumber))
			{	
				if(KErrNone == CCoeEnv::Static()->FsSession().DriveToChar(driveNumber,driveLetter))
				{
					if(KErrNone == CCoeEnv::Static()->FsSession().Volume(volumeInfo,driveNumber))
					{	
						TBool AddDrive(ETrue);
						
						TUint AnotherHelper = volumeInfo.iDrive.iMediaAtt;
						if(AnotherHelper & KMediaAttWriteProtected
						&& iFoldersOnly)
						{
							AddDrive = EFalse;
						}
					
						if(AddDrive)
						{
							HelpBuffer.Zero();
							HelpBuffer.Append(driveLetter);
							HelpBuffer.Append(_L(":"));

							AppendDriveToListL(ETrue,HelpBuffer,(TInt)volumeInfo.iSize,(TInt)volumeInfo.iFree);
						}
					}
				}
			}
		}
	}
	
	iIconHandler.AddExtraFoldersL(iItemArray);	
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CFFolderReader::DoOneReadL(void)
{
	if(iFile_list)
	{
		if(iFile_list->Count() > iReadNow && iReadNow >= 0)
		{	
			if(((*iFile_list)[iReadNow].IsSystem() 
			|| (*iFile_list)[iReadNow].IsHidden())
			&& !iSettings.iShowHiddenSystem)
			{
				// ignore System & Hidden Files.
			}
			else 
			{
				TBool DoneAlready(EFalse);
				
				if(iOptimizeArray.Count() && !iTypeFilterArray)
				{
					for(TInt o=0;o < iOptimizeArray.Count(); o++)
					{
						if(iOptimizeArray[o])
						{
							if(iOptimizeArray[o]->iName)
							{
								if(iOptimizeArray[o]->iName->Des() == (*iFile_list)[iReadNow].iName)
								{
									if(iOptimizeArray[o]->iModified == (*iFile_list)[iReadNow].iModified)
									{
										DoneAlready = ETrue;
										AppendOptimizedToListL(*iOptimizeArray[o]);
										iOptimizeArray.Remove(o);
									}
								}
							}
						}
					}
				}
				
		
				if(!DoneAlready)
				{
					if((*iFile_list)[iReadNow].IsDir()) //Folder
					{	
						AppendToListL((*iFile_list)[iReadNow],CFFileItem::EFFolderFile);
					}
					else if(iSelPath && !iFoldersOnly)
					{						
						iCount1= iCount1 + 1;
						
						TInt MyType(-1);
										
						TFileName FilNam;
						FilNam.Copy(iSelPath->Des());
						FilNam.Append((*iFile_list)[iReadNow].iName);
						
						if(FileTypeOkL(FilNam,(*iFile_list)[iReadNow],MyType))
						{
							AppendToListL((*iFile_list)[iReadNow],MyType);
						}
					}
				}
				else
				{
					iCount2= iCount2 + 1;
				}	
			}
		}
		else
		{
			delete iFile_list;
			iFile_list = NULL;
		}
	}

	iReadNow = iReadNow + 1;
}	
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TBool CFFolderReader::FileTypeOkL(const TDesC& aFileName,const TEntry& aFileEntry,TInt& aIcon)
{	
	TBool Ret(ETrue);
	
	aIcon = CFFileItem::EFJustFile;
	
	if(aFileEntry.IsDir())
	{
		aIcon = CFFileItem::EFFolderFile;
	}
	else if(iSettings.iDoRecocnize || iTypeFilterArray)
	{	
		CYBRecognitionResult* Res = new(ELeave)CYBRecognitionResult(); 
		iIconHandler.RecognizeFile(*Res,aFileName);
		
		if(Res->iIdString)
		{		
			if(iTypeFilterArray)
			{	
				Ret = EFalse;
				
				TBuf<150> TypBuffer;
				TypBuffer.CopyLC(*Res->iIdString);
				TypBuffer.TrimAll();
				
				if(TypBuffer.Length())
				{
					for(TInt tt=0; tt < iTypeFilterArray->MdcaCount(); tt++)
					{
						if(iTypeFilterArray->MdcaPoint(tt) == TypBuffer)
						{
							Ret = ETrue;
							break;
						}
					}
				}
			}
				
			if(Ret)
			{
				aIcon = iIconHandler.GetIconIndex(*Res->iIdString);	
			}
		}
		else if(iTypeFilterArray)
		{
			Ret = EFalse;
		}
	
		delete Res;
		Res = NULL;
	}
	
	return Ret;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CFFolderReader::ShowNoteL(const TDesC&  aMessage, TBool aError)
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
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CFFolderReader::FinnishReadL(void)
{
	iOptimizeArray.Reset();
	
	delete iFile_list;
	iFile_list = NULL;
	iDone = ETrue;	
	
	iObserver.FolderReadFinnishL(this,iHadError);
}

