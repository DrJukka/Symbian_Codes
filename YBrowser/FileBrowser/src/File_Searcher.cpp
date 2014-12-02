/* 
	Copyright (c) 2006, 
	Jukka Silvennoinen
	All rights reserved 
*/

#include "File_Searcher.h"                     // own definitions
#include "Search_Container.h"
#include "Folder_reader.h"// for CFileItem
 
#include <BAUTILS.H> 
#include <apgcli.h>
#include <coeutils.h> 


/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
CFileSearcher::CFileSearcher(MMyFileSearchObserver& aObServer,CSearchValues& aSearchValues,MYBrowserFileUtils& aFileUtils,MYBrowserIconUtils& aIconUtils)
:CActive(0),iObServer(aObServer),iSearchValues(aSearchValues),iYBrowserFileUtils(aFileUtils),iYBrowserIconUtils(aIconUtils)
{
}
/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
CFileSearcher::~CFileSearcher()
{
	Cancel();

	delete iDirArray;
	iTempArray.ResetAndDestroy();
}

/*
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
*/
void CFileSearcher::ConstructL(void)
{
	iSearchFileType = CFFileItem::EFJustFile;
	iCancel = iReadingFile = EFalse;
	iReadFileSoFar = 0;
	
	CActiveScheduler::Add(this);
}

/*
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
*/
void CFileSearcher::DoCancel()
{
	iCancel = ETrue;
	
	delete iDirArray;
	iDirArray = NULL;
	iTempArray.ResetAndDestroy();
	
	delete iFile_list;
	iFile_list = NULL; 

	FinnishSearch();
}

/*
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
*/
void CFileSearcher::RunL()
{
	if(iCancel)
	{
		// already infomed in do-cancel function
		// so no need to do anything..
	}
	else if(!iDirArray && !iTempArray.Count())
	{
		FinnishSearch();
	}
	else
	{
		if(iReadingFile && iStringSearchFile.Length())
		{
			DoCheckSearchStringL(iStringSearchFile);
		}
		else if(iTempArray.Count() && (iSearchValues.iSearchString || iSearchValues.iSearchString8))
		{
			iSearchFileType = CFFileItem::EFJustFile;
		
			if(!iReadingFile)
			{
				iReadFileSoFar = 0;
				
				if(iTempArray[0])
				{
					if(iTempArray[0]->iPath && iTempArray[0]->iName)
					{
						iStringSearchFile.Copy(iTempArray[0]->iPath->Des());	
						iStringSearchFile.Append(iTempArray[0]->iName->Des());
						
						iSearchFileType = iTempArray[0]->iType;
					}
				}
				
				iTempArray.Remove(0);
			}
			
			
			iObServer.FileSearchProcessL(iSearchValues.iItemArray.Count(),iCurrFolder);
				
			DoCheckSearchStringL(iStringSearchFile);
		}
		else if(iFile_list)
		{
			if(iFile_list)
			{
				if(iCurrFile >=0 && iCurrFile < iFile_list->Count())
				{
					DoCheckOneFileL();
				
					if(iSearchValues.iItemArray.Count() % 10 == 0)
					{
						iObServer.FileSearchProcessL(iSearchValues.iItemArray.Count(),iCurrFolder);
					}
				}
				else
				{
					delete iFile_list;
					iFile_list = NULL;
				}
			}
		}
		else if(iDirArray)
		{
			if(iDirArray->Count())
			{
				TBuf<255> Help;
				Help.Copy(iDirArray->MdcaPoint(0));
				Help.Append(_L("\\"));
				iDirArray->Delete(0);
				
				iObServer.FileSearchProcessL(iSearchValues.iItemArray.Count(),Help);
				DoOneFolderSearchL(Help);
			}
			else
			{
				delete iDirArray;
				iDirArray = NULL;
			}
		}
		

		TRequestStatus* status=&iStatus;
		User::RequestComplete(status, KErrNone);
		SetActive();	
	}

}
/*
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
*/
TBool CFileSearcher::SearchIsActive(void)
{
	return IsActive();
}

/*
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
*/
void CFileSearcher::StartTheSearchL(const TDesC& aSearchPath)
{
	delete iFile_list;
	iFile_list = NULL; 
	
	delete iDirArray;
	iDirArray = NULL;

	iCancel = iReadingFile = EFalse;
	iCurrFile = iReadFileSoFar = 0;
	
	iTempArray.ResetAndDestroy();
	iSearchFileType = CFFileItem::EFJustFile;
	iStringSearchFile.Zero();
	
	iDirArray = new (ELeave) CDesCArrayFlat(20);

	TInt FirstDriv = aSearchPath.Find(KtxDrivesStuff);
	if(FirstDriv != KErrNotFound)
	{
		FirstDriv = FirstDriv + (KtxDrivesStuff().Length() - 1);
		iCurrFolder.Copy(aSearchPath.Left(FirstDriv));
		
		TFileName Hjelp;
		Hjelp.Copy(aSearchPath.Right(aSearchPath.Length() - (FirstDriv + 1)));
		
		FirstDriv = KErrNotFound;
		do
		{
			FirstDriv = Hjelp.Find(KtxDrivesStuff);
			
			if(FirstDriv != KErrNotFound)
			{
				FirstDriv = FirstDriv + (KtxDrivesStuff().Length() - 1);
				
				if(FirstDriv < aSearchPath.Length())
				{
					iDirArray->AppendL(Hjelp.Left(2));
					
					Hjelp.Copy(Hjelp.Right(Hjelp.Length() - (FirstDriv + 1)));
				}
			}
			else if(Hjelp.Length() >= 2)
			{
				iDirArray->AppendL(Hjelp.Left(2));
			}
			
		}while(FirstDriv != KErrNotFound);
	}
	else
	{
		iCurrFolder.Copy(aSearchPath);
	}

	DoOneFolderSearchL(iCurrFolder);

	TRequestStatus* status=&iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CFileSearcher::DoOneFolderSearchL(const TDesC& aSearchPath)
{
	iCurrFolder.Copy(aSearchPath);
	
	delete iFile_list;
	iFile_list = NULL;  
	iCurrFile = 0;
	
	TInt attribs=KEntryAttNormal|KEntryAttDir;
	
	attribs|=KEntryAttHidden;
	attribs|=KEntryAttAllowUid;
	attribs|=KEntryAttSystem;
	attribs|=KEntryAttXIP;
			
	attribs|=KEntryAttVolume;
		
	if(KErrNone != CCoeEnv::Static()->FsSession().GetDir(iCurrFolder,attribs,EAscending,iFile_list))
	{	
		delete iFile_list;
		iFile_list = NULL;
	}	
}

/*
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
*/
void CFileSearcher::DoCheckOneFileL(void)
{
	if(iFile_list)
	{
		if(iCurrFile >=0 && iCurrFile < iFile_list->Count())
		{
			HBufC* FullName = HBufC::NewLC(255);
		
			if((*iFile_list)[iCurrFile].IsDir()) //Folder
			{
				if(iSearchValues.iSearchString || iSearchValues.iSearchString8)
				{
					// Folders can not have strings
				}
				else
				{				
					FullName->Des().Copy(iCurrFolder);
					if(FullName->Des().Length())	
					{
						if(!iDirArray)
						{
							iDirArray = new (ELeave) CDesCArrayFlat(1);
						}
						
						FullName->Des().Append((*iFile_list)[iCurrFile].iName);	
						iDirArray->AppendL(FullName->Des()); 
					}
				}
			}
			else //File
			{			
				FullName->Des().CopyLC((*iFile_list)[iCurrFile].iName);
				
				TBool AddThis = EFalse;
				if(iSearchValues.iWildName)
				{
					if(iSearchValues.iWildName->Des().Length())
					{
						if(KErrNotFound != FullName->Des().MatchF(iSearchValues.iWildName->Des()))
						{
							AddThis = ETrue;
						}
					}
					else
					{
						AddThis = ETrue;
					}
				}
				else
				{
					AddThis = ETrue;
				}
				
				if(AddThis)
				{
					FullName->Des().Copy(iCurrFolder);
					if(FullName->Des().Length())
					{
						FullName->Des().Append((*iFile_list)[iCurrFile].iName);							
						
						TInt Type(CFFileItem::EFJustFile);
						if(OkForSearchConditions(FullName->Des(),Type))
						{
							TLocale MyLocale;
							
							if(iSearchValues.iSearchString || iSearchValues.iSearchString8)
							{
								CSFileItem* NewItem = new(ELeave)CSFileItem();
								CleanupStack::PushL(NewItem);
								NewItem->iPath 		= iCurrFolder.AllocL();
								NewItem->iName 		= (*iFile_list)[iCurrFile].iName.AllocL();
								NewItem->iType 		= Type;
								NewItem->iSize 		= (*iFile_list)[iCurrFile].iSize;
								NewItem->iModified	= ((*iFile_list)[iCurrFile].iModified + MyLocale.UniversalTimeOffset());

								CleanupStack::Pop(NewItem);
								iTempArray.Append(NewItem); 
								iReadingFile= EFalse;
							}
							else
							{		
								CSFileItem* NewItem = new(ELeave)CSFileItem();
								CleanupStack::PushL(NewItem);
								NewItem->iPath 		= iCurrFolder.AllocL();
								NewItem->iName 		= (*iFile_list)[iCurrFile].iName.AllocL();
								NewItem->iType 		= Type;
								NewItem->iSize 		= (*iFile_list)[iCurrFile].iSize;
								NewItem->iModified	= ((*iFile_list)[iCurrFile].iModified + MyLocale.UniversalTimeOffset());

								CleanupStack::Pop(NewItem);
								AppendItemL(NewItem);
							}
						}
					}
				}
			}
			
			CleanupStack::PopAndDestroy(1);//,FullName
		}
	}
	
	iCurrFile = iCurrFile + 1;
}
/*
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
*/
TBool CFileSearcher::DoCheckSearchStringL(const TDesC& aFullName)
{
	TBool Ret(EFalse);
	
	iReadingFile = EFalse;

	if((iSearchValues.iSearchString || iSearchValues.iSearchString8)
	&&  BaflUtils::FileExists(CCoeEnv::Static()->FsSession(),aFullName))
	{
		TEntry MyEntry;
		if(KErrNone == CCoeEnv::Static()->FsSession().Entry(aFullName,MyEntry))
		{
			HBufC8* Buffer = HBufC8::NewLC(500);
			TInt ReadLength = 500;
			TPtr8 BufPoint(Buffer->Des());

			if(MyEntry.iSize < iReadFileSoFar + ReadLength)
			{
				ReadLength = MyEntry.iSize - iReadFileSoFar;
			}
			
			if(ReadLength > 0)
			{
				CCoeEnv::Static()->FsSession().ReadFileSection(aFullName,iReadFileSoFar,BufPoint,ReadLength);
				
				if(iSearchValues.iSearchString8)
					iReadFileSoFar = iReadFileSoFar + (ReadLength - iSearchValues.iSearchString8->Des().Length());
				else if(iSearchValues.iSearchString)
					iReadFileSoFar = iReadFileSoFar + (ReadLength - iSearchValues.iSearchString->Des().Length());
				
				if(!iSearchValues.iCaseSensitive)
				{
					Buffer->Des().CopyLC(Buffer->Des());
				}
				
				TInt hhh(KErrNotFound);
				
				if(iSearchValues.iUnicode && iSearchValues.iSearchString)
				{
					TPtrC16 UniSearch((TUint16*)Buffer->Des().Ptr(),(Buffer->Des().Size()/2));
					hhh = UniSearch.FindF(iSearchValues.iSearchString->Des());
				}
				else if(iSearchValues.iSearchString8)
				{
					hhh = Buffer->Des().FindF(iSearchValues.iSearchString8->Des());	
				}
				
				if(KErrNotFound != hhh)
				{
					TLocale MyLocale;
					Ret = ETrue;
					TParsePtrC Hjelpper(aFullName);
								
					CSFileItem* NewItem = new(ELeave)CSFileItem();
					CleanupStack::PushL(NewItem);
					NewItem->iPath 		= Hjelpper.DriveAndPath().AllocL();
					NewItem->iName 		= Hjelpper.NameAndExt().AllocL();
					NewItem->iType 		= iSearchFileType;
					NewItem->iSize 		= MyEntry.iSize;
					NewItem->iModified	= (MyEntry.iModified + MyLocale.UniversalTimeOffset());

					CleanupStack::Pop(NewItem);
					AppendItemL(NewItem);
					
					iReadingFile	= EFalse;
					iSearchFileType = CFFileItem::EFJustFile; 
				}
				else
				{
					TInt Cunt(MyEntry.iSize);
					
					if(iSearchValues.iSearchString8)
						Cunt = iReadFileSoFar + iSearchValues.iSearchString8->Des().Length();
					else if(iSearchValues.iSearchString)
						Cunt = iReadFileSoFar + iSearchValues.iSearchString->Des().Length();
					
					if(Cunt < MyEntry.iSize)
					{
						iReadingFile = ETrue;
					}
				}
			}

			CleanupStack::PopAndDestroy(Buffer);
		}
	}
	else
	{
		iReadingFile = EFalse;
	}
	
	return Ret;
}

/*
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
*/
TBool CFileSearcher::OkForSearchConditions(const TDesC& aFullName,TInt& aType)
{
	TEntry MyEntry;
	if(CCoeEnv::Static()->FsSession().Entry(aFullName,MyEntry))
	{
		// error getting file info, so lets go out.
		return EFalse;
	}
	
	if((MyEntry.iSize < iSearchValues.iBigThan) && iSearchValues.iBigThan > 0)
	{
		// file Smaller than wanted, so lets go out.
		return EFalse;
	}

	if((MyEntry.iSize > iSearchValues.iSmaThan) && iSearchValues.iSmaThan > 0)
	{
		// file Bigger than wanted, so lets go out.
		return EFalse;
	}
			
	if(iSearchValues.iUseAndAttrib)
	{
		if(iSearchValues.iHidden && !MyEntry.IsHidden())
		{
			// attribute value is wrong, so lets go out.
			return EFalse;
		}
		
		if(iSearchValues.iReadOnly && !MyEntry.IsReadOnly())
		{
			// attribute value is wrong, so lets go out.
			return EFalse;
		}
		
		if(iSearchValues.iSystem && !MyEntry.IsSystem())
		{
			// attribute value is wrong, so lets go out.
			return EFalse;
		}
	}
	else if(iSearchValues.iHidden 
	||		iSearchValues.iReadOnly
	||		iSearchValues.iSystem)
	{
		TBool OktoContinue(EFalse);
		
		if(MyEntry.IsHidden() && iSearchValues.iHidden)
		{
			OktoContinue = ETrue;	
		}
		
		if(MyEntry.IsReadOnly() && iSearchValues.iReadOnly)
		{
			OktoContinue = ETrue;
		}
		
		if(MyEntry.IsSystem() && iSearchValues.iSystem) 
		{
			OktoContinue = ETrue;
		}
		
		if(!OktoContinue)
		{
			// attribute value is wrong, so lets go out.
			return EFalse;
		}
	}
	
	TLocale MyLocale;
	TTime EntryTime = MyEntry.iModified + MyLocale.UniversalTimeOffset();
	
	if(iSearchValues.iTimeBeforeSet)
	{
		TTimeIntervalDays DaysTo = iSearchValues.iTimeBefore.DaysFrom(EntryTime);
		if(DaysTo.Int() > 0)
		{
			// at least 1 day before set time, so lets continue.
		}
		else if(DaysTo.Int() == 0)
		{
			TTimeIntervalHours 	 HoursTo(0);
			iSearchValues.iTimeBefore.HoursFrom(EntryTime,HoursTo);
			if(HoursTo.Int() > 0)
			{
				// at least 1 hour before set time, so lets continue.
			}
			else if(HoursTo.Int() == 0)
			{
				TTimeIntervalMinutes MinutesTo(0);
				iSearchValues.iTimeBefore.MinutesFrom(EntryTime,MinutesTo);
				if(MinutesTo.Int() <= 0)
				{	// File made after or at the set date, so lets go out.
					return EFalse;
				}
				else
				{
					// at least 1 minute before set time, so lets continue.
				}
			}
			else
			{	// File made after the set date, so lets go out.
				return EFalse;	
			}
		}
		else
		{	// File made after the set date, so lets go out.
			return EFalse;
		}		
	}

	if(iSearchValues.iTimeAfterSet)
	{
		TTimeIntervalDays DaysTo = EntryTime.DaysFrom(iSearchValues.iTimeAfter);
		if(DaysTo.Int() > 0)
		{
			// at least 1 day before set time, so lets continue.
		}
		else if(DaysTo.Int() == 0)
		{
			TTimeIntervalHours 	 HoursTo(0);
			EntryTime.HoursFrom(iSearchValues.iTimeAfter,HoursTo);
			if(HoursTo.Int() > 0)
			{
				// at least 1 hour before set time, so lets continue.
			}
			else if(HoursTo.Int() == 0)
			{
				TTimeIntervalMinutes MinutesTo(0);
				EntryTime.MinutesFrom(iSearchValues.iTimeAfter,MinutesTo);
				if(MinutesTo.Int() <= 0)
				{	// File made after or at the set date, so lets go out.
					return EFalse;
				}
				else
				{
					// at least 1 minute before set time, so lets continue.
				}
			}
			else
			{	// File made after the set date, so lets go out.
				return EFalse;	
			}
		}
		else
		{	// File made after the set date, so lets go out.
			return EFalse;
		}		
	}
	
	if(iSearchValues.iTypeCheckEnabled)
	{
		TBuf<100> Hjelpper;// has the filetype
		
		CYBRecognitionResult* RecResult = new(ELeave)CYBRecognitionResult();
		
		iYBrowserFileUtils.RecognizeFile(*RecResult,aFullName);
		
		if(RecResult->iIdString)
		{
			if(RecResult->iIdString->Des().Length() > 100)
				Hjelpper.Copy(RecResult->iIdString->Des().Left(100));
			else
				Hjelpper.Copy(RecResult->iIdString->Des());
		}
	
		delete RecResult;
		RecResult = NULL;
		
		TBool Included(ETrue);
			
		if(iSearchValues.iIncludeArray.Count())
		{
			if(MyEntry.IsDir())
			{
				// No Folderas a file type for selection, so lets go out.
				return EFalse;
			}
			
			Included = EFalse;
			
			for(TInt z=0; z < iSearchValues.iIncludeArray.Count(); z++)
			{
				if(iSearchValues.iIncludeArray[z])
				{
					if(iSearchValues.iIncludeArray[z]->iIdString)
					{
					/*	if(iSearchValues.iIncludeArray[z]->iPartialSting)
						{
							if(Hjelpper.Length() > iSearchValues.iIncludeArray[z]->iIdString->Des().Length())
							{
								if(Hjelpper.Left(iSearchValues.iIncludeArray[z]->iIdString->Des().Length()) == iSearchValues.iIncludeArray[z]->iIdString->Des())
								{
									Included = ETrue;
									break;
								}
							}
						}
						else */
						if(iSearchValues.iIncludeArray[z]->iIdString->Des() == Hjelpper)
						{
							Included = ETrue;
							break;													
						}
					}
				}
			}
		}
		
		if(!Included)
		{	// File type not in the included list, so lets go out.
			return EFalse;	
		}
		else if(iSearchValues.iExcludeArray.Count())
		{
			for(TInt s=0; s < iSearchValues.iExcludeArray.Count(); s++)
			{
				if(iSearchValues.iExcludeArray[s])
				{
					if(iSearchValues.iExcludeArray[s]->iIdString)
					{
					/*	if(iSearchValues.iExcludeArray[s]->iPartialSting)
						{
							if(Hjelpper.Length() > iSearchValues.iExcludeArray[s]->iIdString->Des().Length())
							{
								if(Hjelpper.Left(iSearchValues.iExcludeArray[s]->iIdString->Des().Length()) == iSearchValues.iExcludeArray[s]->iIdString->Des())
								{
									// File included in the exclude list, so lets go out.
									return EFalse;
								}
							}
						}
						else */
						if(iSearchValues.iExcludeArray[s]->iIdString->Des() == Hjelpper)
						{
							// File included in the exclude list, so lets go out.
							return EFalse;													
						}
					}
				}
			}
		}
		
		aType = iYBrowserIconUtils.GetIconIndex(Hjelpper);
	}

	// if we get through all test, then the file is ok !!
	return ETrue;
}

/*
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
*/
void CFileSearcher::AppendItemL(CSFileItem* aItem)
{
	if(aItem)
	{
		iSearchValues.iItemArray.Append(aItem);
	}
}

/*
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
*/
void CFileSearcher::FinnishSearch(void)
{
	iObServer.FileSearchFinnishL(this);
	// inform search container that we are finished
}

