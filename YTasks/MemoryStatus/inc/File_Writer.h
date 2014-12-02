/* Copyright (c) 2001 - 2007 , J.P. Silvennoinen. All rights reserved */

#ifndef __MEMFILEWRITER_H__
#define __MEMFILEWRITER_H__

#include <S32FILE.H>
#include <e32base.h>

	class MMemWriteNotify
	{
	public:
		virtual void IncrementWriteL(TInt64 aDone, TInt64 aTotal) = 0;
		virtual void WriteFinishedL(void)  = 0;
		};
//
// Container class to draw text on screen
//
class CFileWriter : public CActive
    {
public:
	~CFileWriter();
	CFileWriter(MMemWriteNotify& aCallBack);
    void ConstructL(void);
	void StartWritingL(TDesC& aDrive,TInt64 aAmount);
	TBool WriteIsActive(void);
	void CancelWrite(void){iCancel = ETrue;};
protected:	
	void DoCancel();
	void RunL();
private:
	void DoOneWriteL(void);
	void FinnishWriteL(void);
private:
	TInt64 		 	 iAmount;
	MMemWriteNotify& iCallBack;
	TInt64 		 	 iWrittenNow;
	TBool 			 iCancel;
	RFile 			 iFile;
	RFileWriteStream iOutputFileStream;
	
    };


#endif // __MEMFILEWRITER_H__
