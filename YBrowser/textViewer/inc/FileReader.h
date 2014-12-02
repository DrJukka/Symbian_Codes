/*
	Copyright (C) 2006 Jukka Silvennoinen
	All right reserved
*/

#ifndef CFileReader_H
#define CFileReader_H

#include <e32base.h>

class MMyFileReadObserver
{
public:
	virtual void ReadprocessL(const TDesC& aStuff, TInt aReadSoFar) = 0;
	virtual void ReadDoneL(TInt aError) = 0;
};

const TInt ReadAtOneTime		= 1024;

	
class CFileReader: public CActive
{
public:
	enum TReadStyle
	{
	ETextAscii,
	ETextUnicode,
	ETextHex,
	ETextCharConv
	};
	static CFileReader* NewL(RFs& aFs,MMyFileReadObserver& aObserver);
	virtual ~CFileReader();
	TInt ReadFileL(const TDesC& aFileName);
	TInt ReadFileL(RFile& aOpenFile);
	TInt ReReadFileL(void);
	TInt LastError(void){ return iError;};
	void SetReadStyle(TReadStyle aStyle, TUint aConverter = 0);
protected:
	void DoCancel();
	void RunL();
private:
	void ConvertTextL(const TDesC8& aData,TDes& aText);
	void UpdateToEditorL(const TDesC8& aData);
	CFileReader(RFs& aFs,MMyFileReadObserver& aObserver);
	void ConstructL(void);
	void DoneNowL(void);
private:
	RFs& 					iFs;
	RFile					iFile;
	TFileName 				iFileName;
	MMyFileReadObserver& 	iObserver;
	TBool 					iDone,iCancel;
	TInt 					iError,iReadSoFar,iFileSize;
	HBufC8*					iReadBuffer;
	HBufC*					iReadBuffer16;
	TReadStyle				iReadStyle;
	TUint					iCharConverter;
};


#endif	// CFileReader_H
