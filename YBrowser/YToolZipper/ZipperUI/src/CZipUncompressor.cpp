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



#include "CZipUncompressor.h"

/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
CZipUncompressor* CZipUncompressor::NewL(RFs& aFs,MZipCompressObserver& aObserver)
{
	CZipUncompressor* self = new (ELeave)CZipUncompressor(aFs,aObserver);
	self->ConstructL();
	return self;
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
CZipUncompressor::CZipUncompressor(RFs& aFs,MZipCompressObserver& aObserver)
:CActive(0),iFs(aFs),iObserver(aObserver),iError(KErrNone)
{
	
}
/*
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
*/
void CZipUncompressor::ConstructL(void)
{	
//	_LIT(KRecFilename			,"C:\\ZipUnComp.txt");
//	iFs.Delete(KRecFilename);		
//	User::LeaveIfError(iDebugFile.Create(iFs,KRecFilename,EFileWrite|EFileShareAny));	

	if(iDebugFile.SubSessionHandle())
	{	
		iDebugFile.Write(_L8("start, "));
	}
	
	CActiveScheduler::Add(this);
}

/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
CZipUncompressor::~CZipUncompressor()
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
		
	delete iFilesArray;
	iFilesArray = NULL;
	
	if(iDebugFile.SubSessionHandle())
	{
		iDebugFile.Write(_L8("Bye, bye, "));	
	  	iDebugFile.Close();
	}
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CZipUncompressor::EncompressL( const TDesC& aOutputDir,RFile& aFile,MDesCArray* aFiles)
{
#ifdef __SERIES60_3X__
	if(iDebugFile.SubSessionHandle())
	{	
		iDebugFile.Write(_L8("Enc, "));
	}
	
	if(aFile.SubSessionHandle())
	{
		iRoot.Copy( aOutputDir );
		iCancel = iDone = EFalse;
		iFileCount = 0;
		
		delete iFilesArray;
		iFilesArray = NULL;
		
		TBool AllFiles(ETrue);
		// if we have aFiles, then we have a selections to be extracted
		// otherwise we would just go and extract all
		if(aFiles)
		{
			iFilesArray = new(ELeave)CDesCArrayFlat(10);
				
			for (TInt i=0; i < aFiles->MdcaCount(); i++)
			{
				iFilesArray->AppendL(aFiles->MdcaPoint(i));
				AllFiles = EFalse;
			}
		}
		
		delete iZipFile;
		iZipFile = NULL;
		iZipFile = CZipFile::NewL(iFs,aFile);
		  
		delete iIterator;
		iIterator = NULL;
		
		if(AllFiles)
		{
			iIterator = iZipFile->GetMembersL();
		}
	}
	else
	{
		iError = KErrNotFound;
	}
	
	if(iDebugFile.SubSessionHandle())
	{	
		iDebugFile.Write(_L8("End-Done, "));
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
void CZipUncompressor::EncompressL( const TDesC& aOutputDir, const TDesC& aZipFile,MDesCArray* aFiles)
{
	if(iDebugFile.SubSessionHandle())
	{	
		iDebugFile.Write(_L8("Enc2, "));
	}
	
	if(BaflUtils::FileExists(iFs,aZipFile))
	{
		iRoot.Copy( aOutputDir );
		iCancel = iDone = EFalse;
		iFileCount = 0;
		
		delete iFilesArray;
		iFilesArray = NULL;
		
		TBool AllFiles(ETrue);
		// if we have aFiles, then we have a selections to be extracted
		// otherwise we would just go and extract all
		if(aFiles)
		{
			iFilesArray = new(ELeave)CDesCArrayFlat(10);
			
			for (TInt i=0; i < aFiles->MdcaCount(); i++)
			{
				iFilesArray->AppendL(aFiles->MdcaPoint(i));
				AllFiles = EFalse;
			}
		}
		
		delete iZipFile;
		iZipFile = NULL;
		iZipFile = CZipFile::NewL(iFs,aZipFile);
		  
		delete iIterator;
		iIterator = NULL;
		
		if(AllFiles)
		{
			iIterator = iZipFile->GetMembersL();
		}
	}
	else
	{
		iError = KErrNotFound;
	}
	
	if(iDebugFile.SubSessionHandle())
	{	
		iDebugFile.Write(_L8("Enc2-Done, "));
	}
	
	TRequestStatus* status=&iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
}
/*
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
*/
void CZipUncompressor::DoCancel()
{
	iCancel = ETrue;
}
/*
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
*/
void CZipUncompressor::RunL()
{
	if(iDebugFile.SubSessionHandle())
	{	
		iDebugFile.Write(_L8("Run-Me, "));
	}
	
	if(iCancel || iDone || iStatus.Int() == KErrCancel)
	{// cancelled or just done, so let go out
		DoneNowL();
	}
	else
	{
		if(iStatus.Int() != KErrNone )
		{
			iObserver.GetFileHandler().GetFileUtils().ShowFileErrorNoteL(iLastFile, iStatus.Int());
		}
		
		iError = KErrNone;
		iLastFile.Zero();
		
		if(iFilesArray)
		{
			if(iFilesArray->Count())
			{
				iLastFile.Copy( iRoot );
				iLastFile.Append(iFilesArray->MdcaPoint(0));
				// we own this array here, so we can delete them one,by one.
				// and always just use the index zero
				iFilesArray->Delete(0);	
			}
			else
			{
				iDone = ETrue;
				delete iFilesArray;
				iFilesArray = NULL;
			}
		}
		else if(iIterator)
		{
			CZipFileMember* member(NULL);
 		
			TRAP( iError, member = iIterator->NextL() );
		 	CleanupStack::PushL(member);
		 	
		 	if( iError == KErrNone && member != NULL)
		 	{			
				TPtrC name = *member->Name();
				iLastFile.Copy( iRoot );
				iLastFile.Append( name );
		 	}
		 	else
		 	{
		 		iDone = ETrue;
		 	}
		 	
		 	CleanupStack::PopAndDestroy(member);
		}
		else
		{	
			// Nothing to do, so let go out
			iDone = ETrue;
		}
		
		ExtractOneL(iLastFile);
		
		if(iError != KErrNone )
		{
			iObserver.GetFileHandler().GetFileUtils().ShowFileErrorNoteL(iLastFile, iError);
			iError = KErrNone;
		}
		
		iFileCount = iFileCount + 1;
		iObserver.CompressProcessL(iFileCount,iLastFile);
		
		if(!IsActive())
		{
			TRequestStatus* status=&iStatus;
			User::RequestComplete(status, KErrNone);
			SetActive();
		}
	}
	
	if(iDebugFile.SubSessionHandle())
	{	
		iDebugFile.Write(_L8("R-Done, "));
	}
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CZipUncompressor::ExtractOneL(const TDesC& aFileName)
{	
	if(iDebugFile.SubSessionHandle())
	{	
		iDebugFile.Write(_L8("Ex-One, "));
	}
	
	delete iWriteBuffer;
	iWriteBuffer = NULL;
	
	if(iWriteFile.SubSessionHandle())
	{
		iWriteFile.Close();
	}
	
	if(aFileName.Length())
	{
		BaflUtils::EnsurePathExistsL(iFs,aFileName);
		
		if(OkToExtractFileL(aFileName))
		{	
			// we could change write to be done is parts as well to
			// minimalize out-of-memory errors.
			
			iWriteBuffer = GetFileBufferL(iZipFile,aFileName);
			if(iWriteBuffer)
			{
				iError = iWriteFile.Create( iFs, aFileName, EFileWrite);
				if(iError == KErrNone)
				{
					iWriteBufferPtr.Set(iWriteBuffer->Des());
					iWriteFile.Write(iWriteBufferPtr,iStatus);
					SetActive();
				}
				else
				{
					iWriteFile.Close();
				}
			}// else
			//no need to set iError, its already done in GetFileBufferL.	
		}
	}
	
	if(iDebugFile.SubSessionHandle())
	{	
		iDebugFile.Write(_L8("Ex-Done, "));
	}
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
TBool CZipUncompressor::OkToExtractFileL(const TDesC& aFile )
{
	if(iDebugFile.SubSessionHandle())
	{	
		iDebugFile.Write(_L8("OkEx, "));
	}
	
	TBool Ret(ETrue);
		
	if(BaflUtils::FileExists(iFs,aFile ))
	{
		TParsePtrC FileHelp(aFile );
		
		TBuf<150> EmbBuffer;
		EmbBuffer.Copy(FileHelp.NameAndExt());
		EmbBuffer.Append(_L(" exists, replace ?"));
		
		CAknQueryDialog* dlg = CAknQueryDialog::NewL();
		if(dlg->ExecuteLD(R_QUERY,EmbBuffer))
		{			
			iError = iFs.Delete(aFile );
			if(iError != KErrNone)
			{
				Ret = EFalse;
			}
		}
		else
		{
			Ret = EFalse;
		}
	}
	else
	{
		iError = KErrNotFound;
	}
	
	if(iDebugFile.SubSessionHandle())
	{	
		iDebugFile.Write(_L8("OKEX-D, "));
	}
	
	return Ret;
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
HBufC8* CZipUncompressor::GetFileBufferL(CZipFile* aZipFile, const TDesC& aFile )
{
	if(iDebugFile.SubSessionHandle())
	{	
		iDebugFile.Write(_L8("FilBuf, "));
	}
	
	HBufC8* Ret(NULL);
	
	if(aZipFile)
	{
		TParsePtrC Parsehelp(aFile);
		
		CZipFileMember* member = iZipFile->CaseSensitiveOrCaseInsensitiveMemberL(Parsehelp.NameAndExt());
		CleanupStack::PushL(member);
		
	 	Ret = HBufC8::New(member->UncompressedSize());
		if(Ret)
		{
			CleanupStack::PushL(Ret);
			
			RZipFileMemberReaderStream* inStream;
			aZipFile->GetInputStreamL( member, inStream );
			
			TPtr8 RetPtr = Ret->Des();
			iError = inStream->Read(RetPtr, member->UncompressedSize());
			
			delete inStream;
			
			CleanupStack::Pop();//Ret
		}
		else
		{
			iError = KErrNoMemory;
		}
		
		CleanupStack::PopAndDestroy();//member
	}
	else
	{
		iError = KErrNotFound;
	}
	
	if(iDebugFile.SubSessionHandle())
	{	
		iDebugFile.Write(_L8("FB-Done, "));
	}
	
	return Ret;
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CZipUncompressor::DoneNowL(void)
{
	if(iDebugFile.SubSessionHandle())
	{	
		iDebugFile.Write(_L8("Me-Done, "));
	}
	
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
		
	delete iFilesArray;
	iFilesArray = NULL;
	
	if(iDebugFile.SubSessionHandle())
	{	
		iDebugFile.Write(_L8("Done-Done, "));
	}

	iObserver.ProcessFinnishedL(this);
}

