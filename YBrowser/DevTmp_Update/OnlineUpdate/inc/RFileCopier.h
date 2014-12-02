/* 
	Copyright (c) 2006, 
	Jukka Silvennoinen
	All rights reserved 
*/
#ifndef __RFILECOPIER_H__
#define __RFILECOPIER_H__
   
#include <eikenv.h>
#include <basched.h>
#include <badesca.h>
#include <coecntrl.h>
#include <coeutils.h>
#include <eikfutil.h>
#include <eikappui.h>
#include <e32keys.h>
#include <eikdef.h>
#include <eikmenup.h>
#include <eikdialg.h>
#include <eikon.hrh>
#include <AknProgressDialog.h>

#include <eikon.rsg>
#include <eikapp.h>
#include <eikdoc.h>


class CRFileCopier;

class MYBrowserRFileCopyObserver
	{
	public:	//	New methods
		virtual void RFileCopyDoneL(CRFileCopier* aObject,TInt aError) = 0;
		virtual void RFileCopyProgressL(TInt aProgress,TInt aFinalValue) = 0;
	};	

class CRFileCopier : public CActive,MProgressDialogCallback
	{
public:
   	static CRFileCopier* NewL(MYBrowserRFileCopyObserver& aObserver,RFs& aFs,TBool aShowProgress);
    static CRFileCopier* NewLC(MYBrowserRFileCopyObserver& aObserver,RFs& aFs,TBool aShowProgress);
	~CRFileCopier();
	void StartCopyL(RFile& aFile,const TDesC& aTargetfile);
protected:	
	void DialogDismissedL (TInt aButtonId);				
protected: // from CActive
	virtual void DoCancel();
	virtual void RunL();
private:
	CRFileCopier(MYBrowserRFileCopyObserver& aObserver,RFs& aFs,TBool aShowProgress);
	void ConstructL(void);
	void DoOnCopyRoundL(void);
	void HandleOperationEndedL(void);
private:	
	MYBrowserRFileCopyObserver& iObserver;
	RFs& 						iFsSession;
	CAknProgressDialog* 		iProgressDialog;
	CEikProgressInfo* 			iProgressInfo;
	TInt 						iLastError,iFileSize;
	TInt						iReadAlready;
	TBool 						iCancel,iShowProgress;	
	RFile						iSourceFile;
	RFile						iTargetFile;
	};
 

#endif //__RFILECOPIER_H__
