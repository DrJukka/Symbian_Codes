/* Copyright (c) 2001 - 2007 , J.P. Silvennoinen. All rights reserved */

#include "File_Writer.h"                     // own definitions
#include <COEUTILS.H>
#include <COEMAIN.H>

/*
-----------------------------------------------------------------------------
*****************************************************************************
-----------------------------------------------------------------------------
*/
CFileWriter::CFileWriter(MMemWriteNotify& aCallBack)
:CActive(0),iAmount(0),iCallBack(aCallBack),iWrittenNow(0)
{
}
/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
CFileWriter::~CFileWriter()
{
	Cancel();
}

/*
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
*/
void CFileWriter::ConstructL(void)
{
	CActiveScheduler::Add(this);
}
/*
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
*/
void CFileWriter::DoCancel()
{
	iCancel = ETrue;
}

/*
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
*/
void CFileWriter::RunL()
{
	if(!iCancel && (iWrittenNow < iAmount))
	{
		iCallBack.IncrementWriteL(iWrittenNow,iAmount);
		DoOneWriteL();
		
		TRequestStatus* status=&iStatus;
		User::RequestComplete(status, KErrNone);
		SetActive();	
	}
	else
	{
		FinnishWriteL();	
	}
}
/*
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
*/
TBool CFileWriter::WriteIsActive(void)
{
	return IsActive();
}

/*
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
*/
void CFileWriter::StartWritingL(TDesC& aDrive,TInt64 aAmount)
{
	iOutputFileStream.Close();
	iFile.Close();

	iWrittenNow = 0;
	iAmount = aAmount;
	
	TBuf<255> FileName;
	TInt i =0;

	do
	{
		FileName.Copy(aDrive);
		FileName.Append(_L("Test"));
		FileName.AppendNum(i);
		FileName.Append(_L(".tmp"));
		i++;

	}while(ConeUtils::FileExists(FileName));

	ConeUtils::EnsurePathExistsL(FileName);

	User::LeaveIfError(iFile.Create(CCoeEnv::Static()->FsSession(), FileName, EFileWrite));
	iOutputFileStream.Attach(iFile);

	TRequestStatus* status=&iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
}
/*
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
*/
void CFileWriter::DoOneWriteL(void)
{
	TChar FillChar = 'K';
	
	HBufC8* RamBuffer;
	
	if(iAmount < 10000)
	{
		RamBuffer = HBufC8::NewLC(iAmount);
		RamBuffer->Des().Fill(FillChar,iAmount);
		iWrittenNow = iAmount;
	}
	else
	{
		TInt DoNow(10000);
		if(iAmount < (iWrittenNow + DoNow))
		{
			DoNow = (iAmount - iWrittenNow);
		}
		
		RamBuffer = HBufC8::NewLC(DoNow);
		RamBuffer->Des().Fill(FillChar,DoNow);
		
		iWrittenNow = (iWrittenNow + DoNow);
	}
	
	iOutputFileStream << RamBuffer->Des();
	
	CleanupStack::PopAndDestroy(RamBuffer);
}

/*
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
*/
void CFileWriter::FinnishWriteL(void)
{
	iOutputFileStream.Close();
	iFile.Close();

	iCallBack.WriteFinishedL();
}
