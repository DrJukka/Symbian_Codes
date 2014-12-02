/*
	Copyright (C) 2004 Petteri Muilu
	Copyright (C) 2006-07 Dr. Jukka Silvennoinen
	
	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.
	
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	
	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <zipfilememberinputstream.h>
#include <coemain.h>
#include <eikprogi.h>
#include <charconv.h>
#include <apmstd.h>
#include <BAUTILS.H>
#include <AknQueryDialog.h>

#include "ZipReader.h"

/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
CZipReader* CZipReader::NewL(RFs& aFs,MZipCompressObserver& aObserver)
{
	CZipReader* self = new (ELeave)CZipReader(aFs,aObserver);
	self->ConstructL();
	return self;
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
CZipReader::CZipReader(RFs& aFs,MZipCompressObserver& aObserver)
:CActive(0),iFs(aFs),iObserver(aObserver),iError(KErrNone)
{
	
}
/*
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
*/
void CZipReader::ConstructL(void)
{	
	CActiveScheduler::Add(this);
}

/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
CZipReader::~CZipReader()
{
	Cancel();
	
	delete iIterator;
	iIterator = NULL;
	
	delete iZipFile;
	iZipFile = NULL;
}

/*
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
*/
void CZipReader::DoCancel()
{
	iCancel = ETrue;
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CZipReader::ReadFilesL(RFile& aFile)
{
#ifdef __SERIES60_3X__
	if(aFile.SubSessionHandle())
	{
		iCancel = iDone = EFalse;
		
		delete iZipFile;
		iZipFile = NULL;
		iZipFile = CZipFile::NewL(iFs,aFile);
		  
		delete iIterator;
		iIterator = NULL;
		iIterator = iZipFile->GetMembersL();
	}
	else
	{
		iObserver.GetFileHandler().GetFileUtils().ShowFileErrorNoteL(KNullDesC,KErrBadHandle);
	}
#else
	iError = KErrNotSupported;
#endif	
	TRequestStatus* status=&iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CZipReader::ReadFilesL(const TDesC& aZipFile)
{
	if(BaflUtils::FileExists(iFs,aZipFile))
	{
		iCancel = iDone = EFalse;
		
		delete iZipFile;
		iZipFile = NULL;
		
		delete iIterator;
		iIterator = NULL;
		
		TInt Err(KErrNone);
		TRAP(Err,
		iZipFile = CZipFile::NewL(iFs,aZipFile));
		  
		if(Err != KErrNone)
		{
			iDone = ETrue;
			iObserver.GetFileHandler().GetFileUtils().ShowFileErrorNoteL(aZipFile,KErrNotFound);
		}
		else
		{  
			iIterator = iZipFile->GetMembersL();
		}
	}
	else
	{
		iObserver.GetFileHandler().GetFileUtils().ShowFileErrorNoteL(aZipFile,KErrNotFound);
	}
	
	TRequestStatus* status=&iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
}
/*
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
*/
void CZipReader::RunL()
{
	if(iCancel || iDone || !iIterator)
	{// cancelled or just done, so let go out
		DoneNowL();
	}
	else
	{
		CZipFileMember* member(NULL);
		TBuf8<255> FilData;
	
		TRAP( iError, 
			member = iIterator->NextL();
			if(member && iZipFile)
			{
				RZipFileMemberReaderStream* inStream;
				iZipFile->GetInputStreamL( member, inStream );
				inStream->Read(FilData, 255);
				delete inStream;
			}
		);
	 	CleanupStack::PushL(member);
	 	
	 	if( iError == KErrNone && member != NULL)
	 	{		
	 		TPtrC name = *member->Name();
	 		
	 		CYBRecognitionResult* Res = new(ELeave)CYBRecognitionResult(); 
			CleanupStack::PushL(Res);
			
	 		iObserver.GetFileHandler().GetFileUtils().RecognizeData(*Res,FilData,name);
			
			TInt TypeIdNum(iObserver.GetFileHandler().GetFileUtils().GetGeneralFileItem().iTypeId);
			
			if(Res->iIdString)
			{
				TypeIdNum = iObserver.GetFileHandler().GetIconUtils().GetIconIndex(*Res->iIdString);
			}
			
			CleanupStack::PopAndDestroy(Res);
			
			iObserver.AddFileToListL(member->CompressedSize(),member->UncompressedSize(),name,TypeIdNum);
	 	}
	 	else
	 	{
	 		iDone = ETrue;
	 	}
	 	
	 	CleanupStack::PopAndDestroy(member);

		TRequestStatus* status=&iStatus;
		User::RequestComplete(status, KErrNone);
		SetActive();
	}
}

/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CZipReader::DoneNowL(void)
{	
	iObserver.ProcessFinnishedL(this);
}

