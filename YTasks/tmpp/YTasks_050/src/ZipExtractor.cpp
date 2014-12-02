/* Copyright (c) 2001 - 2007 , J.P. Silvennoinen. All rights reserved */

#include <zipfilememberinputstream.h>
#include <coemain.h>
#include <eikprogi.h>
#include <charconv.h>
#include <apmstd.h>
#include <BAUTILS.H>
#include <AknQueryDialog.h>


#include "ZipExtractor.h"

/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
CZipExtractor* CZipExtractor::NewL(RFs& aFs,MZipObserver& aObserver)
{
	CZipExtractor* self = new (ELeave)CZipExtractor(aFs,aObserver);
	self->ConstructL();
	return self;
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
CZipExtractor::CZipExtractor(RFs& aFs,MZipObserver& aObserver)
:CActive(0),iFs(aFs),iObserver(aObserver),iError(KErrNone)
{
	
}
/*
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
*/
void CZipExtractor::ConstructL(void)
{	
	CActiveScheduler::Add(this);
}

/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
CZipExtractor::~CZipExtractor()
{
	Cancel();

	delete iWriteBuffer;
	iWriteBuffer = NULL;
	
	if(iWriteFile.SubSessionHandle())
	{
		iWriteFile.Close();
	}
	
	delete iIterator;
	iIterator = NULL;
	
	delete iZipFile;
	iZipFile = NULL;
}

/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CZipExtractor::ExtractL( const TDesC& aTarget, const TDesC& aZipFile)
{
	delete iZipFile;
	iZipFile = NULL;
	
	delete iIterator;
	iIterator = NULL;

	iDestination.Copy( aTarget );
	iCancel = iDone = EFalse;
	
	iError = KErrNone;
	iZipFileName.Copy(aZipFile);
	
	if(BaflUtils::FileExists(iFs,iZipFileName))
	{
		TRAP(iError,
		iZipFile = CZipFile::NewL(iFs,iZipFileName);
		iIterator = iZipFile->GetMembersL());
	}
	else
	{
		iError = KErrNotFound;
	}
	
	TRequestStatus* status=&iStatus;
	User::RequestComplete(status, iError);
	SetActive();
}
/*
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
*/
void CZipExtractor::DoCancel()
{
	iCancel = ETrue;
}
/*
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
*/
void CZipExtractor::RunL()
{
	if(iWriteFile.SubSessionHandle())
	{
		iWriteFile.Close();
	}
	
	delete iWriteBuffer;
	iWriteBuffer = NULL;
	
	iError = iStatus.Int();
		
	if(iCancel || iDone || iError != KErrNone)
	{// cancelled or just done, so let go out
		DoneNowL();
	}
	else if(iIterator && iZipFile)
	{
		TInt NowError = KErrNone;
		CZipFileMember* member(NULL);
	
		TRAP( NowError, member = iIterator->NextL() );
	 	CleanupStack::PushL(member);
	 	
	 	if(NowError == KErrNone && member != NULL)
	 	{			
	 		TFileName NowFile, HjelpDel;
	 		
			NowFile.Copy(iDestination);
			NowFile.Append(*member->Name());
			
			BaflUtils::EnsurePathExistsL(iFs,NowFile);
			iFs.Delete(NowFile);
						
			// ok a bit unneccessary, but just
			// want to make sure, if we swap between
			// mbm and mif, it will still work ok.
			TParsePtrC Hjelpper(NowFile);
			
			HjelpDel.Copy(Hjelpper.DriveAndPath());
			HjelpDel.Append(Hjelpper.Name());
			HjelpDel.Append(KtxMbmExtension);
			
			iFs.Delete(HjelpDel);
			
			HjelpDel.Copy(Hjelpper.DriveAndPath());
			HjelpDel.Append(Hjelpper.Name());
			HjelpDel.Append(KtxMifExtension);

			iFs.Delete(HjelpDel);
			
		 	iWriteBuffer = HBufC8::NewL(member->UncompressedSize());
	
			RZipFileMemberReaderStream* inStream;
			iZipFile->GetInputStreamL( member, inStream );
				
			TPtr8 ReadPtr(iWriteBuffer->Des());
			iError = inStream->Read(ReadPtr, member->UncompressedSize());
				
			delete inStream;
				
			iError = iWriteFile.Create( iFs, NowFile, EFileWrite);
			if(iError == KErrNone)
			{
				iWriteBufferPtr.Set(iWriteBuffer->Des());
				iWriteFile.Write(iWriteBufferPtr,iStatus);
			}
			else
			{
				TRequestStatus* status=&iStatus;
				User::RequestComplete(status, iError);
			}		
	 	}
	 	else
	 	{
	 		if(iError == KErrNone && NowError != KErrNone)
	 		{
	 			iError = NowError;
	 		}
	 		
	 		TRequestStatus* status=&iStatus;
			User::RequestComplete(status, iError);
	 		
	 		iDone = ETrue;
	 	}
	 	
	 	CleanupStack::PopAndDestroy(member);
	
		SetActive();
	}
	else
	{	
		// Nothing to do, so let go out
		iDone = ETrue;
		TRequestStatus* status=&iStatus;
		User::RequestComplete(status, KErrNone);
		SetActive();
	}
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CZipExtractor::DeleteFileL()
{
	Cancel();

	delete iWriteBuffer;
	iWriteBuffer = NULL;
	
	if(iWriteFile.SubSessionHandle())
	{
		iWriteFile.Close();
	}
	
	delete iIterator;
	iIterator = NULL;
	
	delete iZipFile;
	iZipFile = NULL;

	iFs.Delete(iZipFileName);
}

/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CZipExtractor::DoneNowL(void)
{
	delete iWriteBuffer;
	iWriteBuffer = NULL;
	
	if(iWriteFile.SubSessionHandle())
	{
		iWriteFile.Close();
	}
	
	delete iIterator;
	iIterator = NULL;
	
	delete iZipFile;
	iZipFile = NULL;

	iObserver.ExtractDoneL(this,iError);
}

