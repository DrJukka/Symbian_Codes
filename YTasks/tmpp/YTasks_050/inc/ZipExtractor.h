/* Copyright (c) 2001 - 2007 , J.P. Silvennoinen. All rights reserved */

#ifndef ZIPEXTRACTOR_H
#define ZIPEXTRACTOR_H

#include <e32base.h>
#include <zipfile.h>

class CZipExtractor;


_LIT(KtxMbmExtension		,".mbm");
_LIT(KtxMifExtension		,".mif");

class MZipObserver
	{
	public:
		virtual void ExtractDoneL(CZipExtractor* aObject,TInt aError) = 0;
	};

class CZipExtractor: public CActive
{
public:
	static CZipExtractor* NewL(RFs& aFs,MZipObserver& aObserver);
	virtual ~CZipExtractor();
	void ExtractL(const TDesC& aTarget, const TDesC& aZipFile);
	TInt LastError(void){ return iError;};
	void DeleteFileL();
protected:
	void DoCancel();
	void RunL();
private:
	CZipExtractor(RFs& aFs,MZipObserver& aObserver);
	void ConstructL(void);
	void DoneNowL(void);
private:
	RFs& 					iFs;
	MZipObserver& 			iObserver;
	CZipFileMemberIterator* iIterator;
	CZipFile* 				iZipFile;
	TBool 					iDone,iCancel;
	TInt 					iError;
	TFileName 				iDestination,iZipFileName;
	TPtrC8					iWriteBufferPtr;
	HBufC8*					iWriteBuffer;
	RFile 					iWriteFile;
};

#endif	// ZIPEXTRACTOR_H
