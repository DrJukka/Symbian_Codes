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

#ifndef CZipCompressor_H
#define CZipCompressor_H


#include <s32file.h>
#include <BAMDESCA.H>
#include "MZipCompressObserver.h"


class TFileEntry
{
public:
	TUint32 	iDateTime;
	TUint32 	iAttributes;
	TUint16 	iCompression;
	TUint16 	iVersion;
	TUint32 	iCrc32;
	TUint32 	iCompressedSize;
	TUint32 	iUncompressedSize;
	TUint32 	iRelativeOffset;
	TBuf8<255> 	iFileName;
};// can I add file type ID here ?, 

// check about file comment
// or
// extra field 

class CZipCompressor : public CActive
{
public:
	static CZipCompressor* NewL(RFs& aFs,const TDesC& aZipFileName,const MDesCArray& aFilesArray,MZipCompressObserver& aObserver);
	~CZipCompressor();
protected:
	void DoCancel();
	void RunL();
private:
	CZipCompressor(RFs& aFs,MZipCompressObserver& aObserver);
	void ConstructL(const TDesC& aZipFileName,const MDesCArray& aFilesArray);
	void BeginL(const TDesC& aZipFileName);
	void ReadFileL( const TDesC& aFileName );
	void AddFileL(const TDesC8& aFileData,const TDesC& aFileName);
	void EndL();
	TUint32 MsDosDateTime( TTime aTime );
private:
	RFs& 					iFs;
	CDesCArrayFlat* 		iFilesArray;
	MZipCompressObserver& 	iObserver;
	RFile 			 		iZipFile;
	RFileWriteStream 		iStream;
	TInt	 		 		iCurrentFile,iFileCounter,iError;
	TBool 					iDone,iCancel;
	RArray<TFileEntry> 		iFileEntries;
	RFile 					iReadFile;
	HBufC8*					iReadFileBuffer;
	TPtr8 					iReadFileBufferPtr;
	RFile					iDebugFile;
};

#endif	// CZipCompressor_H
