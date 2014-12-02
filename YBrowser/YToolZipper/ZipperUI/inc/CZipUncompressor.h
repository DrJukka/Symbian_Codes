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

#ifndef CZipUncompressor_H
#define CZipUncompressor_H

#include <e32base.h>
#include <zipfile.h>
#include "MZipCompressObserver.h"

class CZipUncompressor;

	
class CZipUncompressor: public CActive
{
public:
	static CZipUncompressor* NewL(RFs& aFs,MZipCompressObserver& aObserver);
	virtual ~CZipUncompressor();
	void EncompressL( const TDesC& aOutputDir, const TDesC& aZipFile,MDesCArray* aFiles = NULL);
	void EncompressL( const TDesC& aOutputDir,RFile& aFile,MDesCArray* aFiles = NULL);
	HBufC8* GetFileBufferL(CZipFile* aZipFile, const TDesC& aFile );
	TInt LastError(void){ return iError;};
protected:
	void DoCancel();
	void RunL();
private:
	void ExtractOneL(const TDesC& aFileName);
	TBool OkToExtractFileL(const TDesC& aFile );
	CZipUncompressor(RFs& aFs,MZipCompressObserver& aObserver);
	void ConstructL(void);
	void DoneNowL(void);
private:
	RFs& 			iFs;
	MZipCompressObserver& 	iObserver;
	CZipFileMemberIterator* iIterator;
	CZipFile* 		iZipFile;
	CDesCArrayFlat*	iFilesArray;
	TBool 			iDone,iCancel;
	TInt 			iFileCount,iError;
	TFileName 		iRoot;
	TFileName 		iLastFile;
	TPtrC8			iWriteBufferPtr;
	HBufC8*			iWriteBuffer;
	RFile 			iWriteFile;
	RFile			iDebugFile;
};

#endif	// CZipUncompressor_H
