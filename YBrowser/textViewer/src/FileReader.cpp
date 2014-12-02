/*
	Copyright (C) 2006 Jukka Silvennoinen
	All right reserved
*/

#include <coemain.h>
#include <eikprogi.h>
#include <charconv.h>
#include <apmstd.h>
#include <BAUTILS.H>
#include <AknQueryDialog.h>
#include <CHARCONV.H>

#include "FileReader.h"



/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
CFileReader* CFileReader::NewL(RFs& aFs,MMyFileReadObserver& aObserver)
{
	CFileReader* self = new (ELeave)CFileReader(aFs,aObserver);
	self->ConstructL();
	return self;
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
CFileReader::CFileReader(RFs& aFs,MMyFileReadObserver& aObserver)
:CActive(0),iFs(aFs),iObserver(aObserver),iError(KErrNone)
{
	
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
CFileReader::~CFileReader()
{
	Cancel();
	iFile.Close();	
	delete iReadBuffer;
	iReadBuffer = NULL;
	delete iReadBuffer16;
	iReadBuffer16 = NULL;
}
/*
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
*/
void CFileReader::ConstructL(void)
{	
	CActiveScheduler::Add(this);
	iReadBuffer 	= HBufC8::NewL(ReadAtOneTime);
	iReadBuffer16	= HBufC::NewL(ReadAtOneTime * 3);
}

/*
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
*/
void CFileReader::SetReadStyle(TReadStyle aStyle, TUint aConverter)
{
	iReadStyle 	   = aStyle;
	iCharConverter = aConverter;
}


/*
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
*/
void CFileReader::DoCancel()
{
	iCancel = ETrue;
}


/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
TInt CFileReader::ReadFileL(const TDesC& aFileName)
{
	TInt FileSize(0);
	
	if(iFile.SubSessionHandle())
	{
		iFile.Close();
	}
	
	iFileName.Zero();
	
	TEntry MyEntry;
	if(KErrNone == iFs.Entry(aFileName,MyEntry)
	&& aFileName.Length())
	{
		iReadSoFar = 0;
		iCancel = iDone = EFalse;	
		iFileName.Copy(aFileName);
		FileSize = MyEntry.iSize;
	}
	else
	{
		iDone 	= EFalse;
		iError 	= KErrNotFound;
	}

	TRequestStatus* status=&iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
	
	iFileSize = FileSize;
	return FileSize;
}

/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
TInt CFileReader::ReadFileL(RFile& aOpenFile)
{
	TInt FileSize(0);

#ifdef __SERIES60_3X__	
	if(iFile.SubSessionHandle())
	{
		iFile.Close();
	}
	iFileName.Zero();
	
	
	if(aOpenFile.SubSessionHandle())
	{
		iReadSoFar = 0;
		iCancel = iDone = EFalse;
		iFile.Duplicate(aOpenFile);
		iError = iFile.Size(FileSize);
	}
	else
	{
		iError = KErrBadHandle;
	}
#else
	iError = KErrNotSupported;
#endif

	TRequestStatus* status=&iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
	
	iFileSize = FileSize;
	return FileSize;
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
TInt CFileReader::ReReadFileL(void)
{
	TInt FileSize(0);
	
	TEntry MyEntry;
	if(KErrNone == iFs.Entry(iFileName,MyEntry)
	&& iFileName.Length())
	{
		iReadSoFar = 0;
		iCancel = iDone = EFalse;	
		FileSize = MyEntry.iSize;
	}
	else if(iFile.SubSessionHandle())
	{
		iReadSoFar = 0;
		iCancel = iDone = EFalse;
		iError = iFile.Size(FileSize);
	}
	else 
	{
		iError = KErrNotFound;
	}

	TRequestStatus* status=&iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
	
	iFileSize = FileSize;
	return FileSize;
}
/*	
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
*/
void CFileReader::RunL()
{
	if(iCancel 
	|| iDone 
	|| (iError != KErrNone)
	|| (iReadSoFar >= iFileSize))
	{// cancelled or just done, so let go out
		DoneNowL();
	}
	else 
	{	
		TRAP(iError,
		TInt NowRead(ReadAtOneTime);
		
		if(NowRead > (iFileSize - iReadSoFar))
		{
			NowRead = (iFileSize - iReadSoFar);
		}
		
		iReadBuffer->Des().Zero();
		TPtr8 REadBuff(iReadBuffer->Des());
		if(iFile.SubSessionHandle())
		{
			iError = iFile.Read(iReadSoFar,REadBuff,NowRead);
		}
		else if(iFileName.Length())
		{
			iError = iFs.ReadFileSection(iFileName,iReadSoFar,REadBuff,NowRead);
		}
		else
		{
			iError = KErrNotFound;
		}
		
		iReadSoFar = iReadSoFar + NowRead;
		UpdateToEditorL(*iReadBuffer););
		
		TRequestStatus* status=&iStatus;
		User::RequestComplete(status, KErrNone);
		SetActive();
	}
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CFileReader::UpdateToEditorL(const TDesC8& aData)
{
	if(iReadStyle == ETextUnicode)
	{
		TPtrC Hjelpper((TUint16*)aData.Ptr(),(aData.Size()/2));
		iReadBuffer16->Des().Copy(Hjelpper);
	}
	else if(iReadStyle == ETextHex) 
	{
		iReadBuffer16->Des().Zero();
		
		for(TInt i=0; i < aData.Length(); i++)
		{
			TInt HexNum = aData[i];	
			if(HexNum < 0x10)
			{
				iReadBuffer16->Des().AppendNum(0,EHex);	
			}
			
			iReadBuffer16->Des().AppendNum(HexNum,EHex);
			
			if(i%2 && (i > 0))
			{
				iReadBuffer16->Des().Append(_L("\t"));	
			}
		}		
	}
	else if(iReadStyle == ETextCharConv)
	{
		iReadBuffer16->Des().Zero();
		
		TPtr ReadPoint(iReadBuffer16->Des());
		
		ConvertTextL(aData,ReadPoint);
	}
	else //if(iReadStyle == ETextAscii)
	{
		iReadBuffer16->Des().Copy(aData);
	}
	
	iObserver.ReadprocessL(*iReadBuffer16,iReadSoFar);
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CFileReader::ConvertTextL(const TDesC8& aData,TDes& aText)
{
	TInt state = CCnvCharacterSetConverter::KStateDefault;
	CCnvCharacterSetConverter* CSConverter = CCnvCharacterSetConverter::NewLC();
	
	TPtrC8 Remainder(aData);

	if (CSConverter->PrepareToConvertToOrFromL(iCharConverter,iFs) != CCnvCharacterSetConverter::EAvailable)
	{
		CSConverter->PrepareToConvertToOrFromL(iCharConverter,iFs);
	}
	
	for(;;)
    {
      	const TInt returnValue = CSConverter->ConvertToUnicode(aText,Remainder,state);
        if (returnValue <= 0) // < error
        {
             break;
        }
        
		Remainder.Set(Remainder.Right(returnValue));
     }

	CleanupStack::PopAndDestroy(CSConverter);
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CFileReader::DoneNowL(void)
{	
	iObserver.ReadDoneL(iError);
}

