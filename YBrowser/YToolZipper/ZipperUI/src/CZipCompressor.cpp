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

#include "CZipCompressor.h"
#include <e32svr.h>
#include <coemain.h>
#include <ezlib.h>
#include <EZCompressor.h>
#include <EZDecompressor.h>
#include <ezgzip.h>
#include <s32file.h>
#include <BAUTILS.H>

#include "Public_Interfaces.h"

/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
CZipCompressor* CZipCompressor::NewL(RFs& aFs,const TDesC& aZipFileName,const MDesCArray& aFilesArray,MZipCompressObserver& aObserver)
{
	CZipCompressor* self = new (ELeave)CZipCompressor(aFs,aObserver);
	self->ConstructL(aZipFileName,aFilesArray);
	return self;
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
CZipCompressor::CZipCompressor(RFs& aFs,MZipCompressObserver& aObserver)
:CActive(0),iFs(aFs),iObserver(aObserver),iReadFileBufferPtr(NULL,0,0)
{
}
/*
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
*/
void CZipCompressor::ConstructL(const TDesC& aZipFileName,const MDesCArray& aFilesArray)
{	
//	_LIT(KRecFilename			,"C:\\ZipCompress.txt");
//	iFs.Delete(KRecFilename);		
//	User::LeaveIfError(iDebugFile.Create(iFs,KRecFilename,EFileWrite|EFileShareAny));	

	iFilesArray = new(ELeave)CDesCArrayFlat(10);

	for (TInt i=0; i < aFilesArray.MdcaCount(); i++)
	{
		iFilesArray->AppendL(aFilesArray.MdcaPoint(i));
	}

	if(iDebugFile.SubSessionHandle())
	{	
		iDebugFile.Write(_L8("start, "));
	}
	
	CActiveScheduler::Add(this);
	BeginL(aZipFileName);
}

/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
CZipCompressor::~CZipCompressor()
{
	Cancel();
	
	delete iReadFileBuffer;
	iReadFileBuffer = NULL;
	iReadFile.Close();
	
	iStream.Close();
	iZipFile.Close();
	iFileEntries.Close();
	
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
void CZipCompressor::BeginL( const TDesC& aZipFileName)
{
	if(iDebugFile.SubSessionHandle())
	{	
		iDebugFile.Write(_L8("begin, "));
	}
	
	iCurrentFile = iFileCounter = 0;
	iCancel = iDone = EFalse;
	
	if(iFilesArray->MdcaCount())
	{// higher levels have to ask for replacing
	 // we just assume here that we can do it without asking
		if(BaflUtils::FileExists(iFs,aZipFileName))
		{
			iError = iFs.Delete(aZipFileName);
		}
		
		if(iError == KErrNone)
		{
			BaflUtils::EnsurePathExistsL(iFs,aZipFileName);
		
			iError = iZipFile.Create( iFs, aZipFileName, EFileWrite|EFileRead );
			if(KErrNone == iError)
			{
				iStream.Attach(iZipFile);
			}
		}
	}
	
	if(iError != KErrNone)
	{// error opening/creating the target zip file, 
	 // so we  can not do anything, thus lets pretend that we are done.
		
		iObserver.GetFileHandler().GetFileUtils().ShowFileErrorNoteL(aZipFileName, iError);
		iDone = ETrue;
	}
	
	if(iDebugFile.SubSessionHandle())
	{	
		iDebugFile.Write(_L8("beg done, "));
	}
	
	TRequestStatus* status=&iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
}
/*
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
*/
void CZipCompressor::DoCancel()
{
	if(iDebugFile.SubSessionHandle())
	{	
		iDebugFile.Write(_L8("me cancel, "));
	}

#ifdef __SERIES60_3X__
	if(iReadFile.SubSessionHandle())
	{// we are currently reading, so lets cancel it.
		iReadFile.ReadCancel();
	}
#else
#endif
	EndL();
	// here we just mark out that next call to RunL will end the loop
	iCancel = ETrue;
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CZipCompressor::RunL()
{
	if(iDebugFile.SubSessionHandle())
	{	
		iDebugFile.Write(_L8("me run, "));
	}
	
	if(iCancel || iDone || iStatus.Int() == KErrCancel)
	{// We are done or concelled, so lets finalize our zip
		EndL();
	}
	else if(iFilesArray->MdcaCount() > iCurrentFile)
	{
		if(iStatus.Int() != KErrNone )
		{// this would be an error on file read.
			iObserver.GetFileHandler().GetFileUtils().ShowFileErrorNoteL(iFilesArray->MdcaPoint(iCurrentFile), iStatus.Int());
		}
		
		if(iReadFile.SubSessionHandle() && iReadFileBuffer)
		{// we just finished reading a file to the buffer, 
		 // so first lets close the file
			iReadFile.Close();
			
			TInt AddErr(KErrNone);
			TRAP(AddErr,AddFileL(*iReadFileBuffer,iFilesArray->MdcaPoint(iCurrentFile)));	
			if(AddErr!= KErrNone)
			{
				iObserver.GetFileHandler().GetFileUtils().ShowFileErrorNoteL(iFilesArray->MdcaPoint(iCurrentFile), AddErr);
			}
			// lets clear some memory already in here
			delete iReadFileBuffer;
			iReadFileBuffer = NULL;
		}
		else
		{	// we are starting a new file
			// this will call SetActive in succesfull cases
			ReadFileL(iFilesArray->MdcaPoint(iCurrentFile));
			iObserver.CompressProcessL(iCurrentFile,iFilesArray->MdcaPoint(iCurrentFile));
		}
			
		if(iError != KErrNone)
		{	// errors cathed in Add/Read FileL functions
			iObserver.GetFileHandler().GetFileUtils().ShowFileErrorNoteL(iFilesArray->MdcaPoint(iCurrentFile), iError);
			// it has been repoted so we can now clear the error
			iError = KErrNone;
		}
					
		if(!IsActive())// No Read operation issued
		{
			// either we just added a file
			// or we had error while opening new one
			// anyway, lets move to next one
			iCurrentFile++;
			
			TRequestStatus* status=&iStatus;
			User::RequestComplete(status, KErrNone);
			SetActive();
		}
	}
	else
	{// All done so lets finalize our zip
		EndL();
	}
	
	if(iDebugFile.SubSessionHandle())
	{	
		iDebugFile.Write(_L8("rn done, "));
	}
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CZipCompressor::ReadFileL( const TDesC& aFileName )
{
	if(iDebugFile.SubSessionHandle())
	{	
		iDebugFile.Write(_L8("ReadF, "));
	}
	
	delete iReadFileBuffer;
	iReadFileBuffer = NULL;
	
	if(iReadFile.SubSessionHandle())
	{
		iReadFile.Close();
	}
	
	// we could change this to be done with RFs ReadFileSection
	// then we could also handle open files.
	iError = iReadFile.Open( iFs, aFileName , EFileRead );
	if(iError == KErrNone)
	{
		TInt FileSize(0);
		// with ReadFileSection, use TEntry to get the size
		iError = iReadFile.Size(FileSize);
		if(iError == KErrNone)
		{
			// also if we would read the file in smaller pieces
			// we would run out-of-memory less times.
			iReadFileBuffer = HBufC8::New(FileSize);
			if(iReadFileBuffer)
			{
				iReadFileBufferPtr.Set(iReadFileBuffer->Des());
				iReadFile.Read(iReadFileBufferPtr,iStatus);
				SetActive();
			}
			else
			{	
				// not enough memory to handle this file, so close it now
				iReadFile.Close();
				// and report an error.
				iError = KErrNoMemory;
			}
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
void CZipCompressor::AddFileL(const TDesC8& aFileData,const TDesC& aFileName)
{
	if(iDebugFile.SubSessionHandle())
	{	
		iDebugFile.Write(_L8("Add-F, "));
	}
	
	TFileEntry fileEntry;

	HBufC8* out = HBufC8::New( aFileData.Size() + 100);
	if(out)
	{	
		CleanupStack::PushL(out);
		
		if(iDebugFile.SubSessionHandle())
		{	
			iDebugFile.Write(_L8("AF1, "));
		}
	
		TEntry entry;
		iFs.Entry( aFileName, entry ); 

		TPtr8 outPtr = out->Des();

		fileEntry.iVersion 		= 0x000A;
		// we could always make settings to handle different compressions within a file
	    fileEntry.iCompression 	= 0x0008;
		
		TInt ErrNo(KErrNone);
		
		TRAP(ErrNo,CEZCompressor::CompressL( outPtr, aFileData, CEZCompressor::EDefaultCompression ));
		if(ErrNo != KErrNone)
		{
			fileEntry.iCompression 	= 0;
			out->Des().Copy(aFileData);
		}
		
		if(iDebugFile.SubSessionHandle())
		{	
			iDebugFile.Write(_L8("AF2, "));
		}
		
		iFileCounter++;
		
		TParsePtrC NameHelp(aFileName);
		fileEntry.iFileName.Copy(NameHelp.NameAndExt());
		
	    fileEntry.iCompressedSize 	= outPtr.Length() - 6;
	    fileEntry.iUncompressedSize = aFileData.Length();
	 	
	    fileEntry.iRelativeOffset 	= iStream.Sink()->SizeL();
		fileEntry.iAttributes 		= entry.iAtt;
		fileEntry.iDateTime 		= MsDosDateTime( entry.iModified );
		
		if(iDebugFile.SubSessionHandle())
		{	
			iDebugFile.Write(_L8("AF3, "));
		}
		
		iStream.WriteInt32L( 0x04034b50 );				// sign
		iStream.WriteInt16L( fileEntry.iVersion );		// version
		iStream.WriteInt16L( 0x0000 );					// general purpose bit flag
		iStream.WriteInt16L( fileEntry.iCompression );	// compression method
		iStream.WriteInt32L( fileEntry.iDateTime );		// time

		TUint32 crc = crc32(0L, Z_NULL, 0);
		fileEntry.iCrc32 = crc32( crc, aFileData.Ptr(), aFileData.Length() );

		iStream.WriteUint32L( fileEntry.iCrc32 );				// crc
		iStream.WriteInt32L( fileEntry.iCompressedSize );		// compressed size
		iStream.WriteInt32L( fileEntry.iUncompressedSize );		// uncompressed size
		iStream.WriteInt16L( fileEntry.iFileName.Length() );	// filename length
		iStream.WriteInt16L( 0x0000 );							// extra field length

		iStream.WriteL( fileEntry.iFileName );

		iStream.WriteL( outPtr.Mid( 2, outPtr.Length() - 6 ) );

		iFileEntries.Append( fileEntry );

		CleanupStack::PopAndDestroy(out); 
		
		if(iDebugFile.SubSessionHandle())
		{	
			iDebugFile.Write(_L8("AF3, "));
		}
		
		CYBRecognitionResult* Res = new(ELeave)CYBRecognitionResult(); 
		CleanupStack::PushL(Res);
		
		iObserver.GetFileHandler().GetFileUtils().RecognizeData(*Res,aFileData,NameHelp.NameAndExt());
		
		TInt TypeIdNum(iObserver.GetFileHandler().GetFileUtils().GetGeneralFileItem().iTypeId);
		
		if(Res->iIdString)
		{
			TypeIdNum = iObserver.GetFileHandler().GetIconUtils().GetIconIndex(*Res->iIdString);
		}
		
		CleanupStack::PopAndDestroy(Res);
		
		if(iDebugFile.SubSessionHandle())
		{	
			iDebugFile.Write(_L8("AF4, "));
		}
		
		iObserver.AddFileToListL(fileEntry.iCompressedSize,fileEntry.iUncompressedSize,NameHelp.NameAndExt(),TypeIdNum);
	}
	else
	{
		iError = KErrNoMemory;
	}
	
	if(iDebugFile.SubSessionHandle())
	{	
		iDebugFile.Write(_L8("Add-Done, "));
	}
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CZipCompressor::EndL()
{
	if(iDebugFile.SubSessionHandle())
	{	
		iDebugFile.Write(_L8("Me-End, "));
	}
	
	if(iStream.Sink())
	{   
		TInt relativeOffset = iStream.Sink()->SizeL();
		
		// Write central file headers
		for( TInt i=0; i < iFileEntries.Count(); i++ )
		{
			TFileEntry fileEntry = iFileEntries[i];
			
			iStream.WriteInt32L( 0x02014b50 );			// central file header signature
			iStream.WriteInt16L( fileEntry.iVersion );	// version made by
			iStream.WriteInt16L( fileEntry.iVersion );	// version needed to extract
			iStream.WriteInt16L( 0x0000 );				// general purpose bit flag 
			iStream.WriteInt16L( fileEntry.iCompression );	// compression method
			iStream.WriteInt32L( fileEntry.iDateTime );		// last mod file time
			iStream.WriteInt32L( fileEntry.iCrc32 );		// crc-32 
			iStream.WriteInt32L( fileEntry.iCompressedSize );	// compressed size
			iStream.WriteInt32L( fileEntry.iUncompressedSize );	// uncompressed size
			iStream.WriteInt16L( fileEntry.iFileName.Length() );// file name length
			iStream.WriteInt16L( 0x0000 );			// extra field length 
			iStream.WriteInt16L( 0x0000 );			// file comment length 
			iStream.WriteInt16L( 0x0000 );			// disk number start
			iStream.WriteInt16L( 0x0000 );			// internal file attributes
			iStream.WriteInt32L( 0x81800020 );		// external file attributes
			iStream.WriteInt32L( fileEntry.iRelativeOffset );	// relative offset of local header
			iStream.WriteL( fileEntry.iFileName );
		}


		TInt centralDirSize = iStream.Sink()->SizeL() - relativeOffset;

		iStream.WriteInt32L( 0x06054b50 );		// end of central dir signature
		iStream.WriteInt16L( 0x0000 );			// number of this disk 
		iStream.WriteInt16L( 0x0000 );			// number of the disk with the start of the central directory 
		iStream.WriteInt16L( iFileCounter );	// start of the central directory
		iStream.WriteInt16L( iFileCounter );	// total number of entries in the central directory on this disk
		iStream.WriteInt32L( centralDirSize );	// size of the central directory
		iStream.WriteInt32L( relativeOffset );	// offset of start of central directory with respect to the starting disk number
		iStream.WriteInt16L( 0x0000 );			// comment length
	}
	
	delete iReadFileBuffer;
	iReadFileBuffer = NULL;
	iReadFile.Close();
	
	iStream.Close();
	iZipFile.Close();
	iFileEntries.Close();
	
	iObserver.ProcessFinnishedL(this);
	
	if(iDebugFile.SubSessionHandle())
	{	
		iDebugFile.Write(_L8("End done, "));
	}
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
TUint32 CZipCompressor::MsDosDateTime( TTime aTime )
{
	TDateTime dateTime = aTime.DateTime();

	TUint8 day 		= dateTime.Day() + 1;
	TUint8 month 	= dateTime.Month() + 1;
	TUint8 year 	= dateTime.Year() - 1980;
	TUint8 seconds 	= dateTime.Second();
	TUint8 minutes 	= dateTime.Minute();
	TUint8 hours 	= dateTime.Hour();
	
	TUint32 date = 0;
	date |= ( year & 0x3f ) << 9;
	date |= ( month & 0x0f ) << 5;
	date |= ( day & 0x1f );
	date <<= 16;
	date |= ( hours & 0x1f ) << 11;
	date |= minutes << 5;
	date |= seconds >> 1;
	return date;
}
