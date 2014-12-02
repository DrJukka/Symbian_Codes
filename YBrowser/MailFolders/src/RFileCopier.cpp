/* 
	Copyright (c) 2006, 
	Jukka Silvennoinen
	All rights reserved 
*/

#include <badesca.h>
#include <coeutils.h>
#include <eikenv.h>
#include <f32file.h>
#include <eiklabel.h>
#include <eikprogi.h>
#include <eikinfo.h>
#include <eikfutil.h>
#include <eikbutb.h>
#include <AknQueryDialog.h>
#include <aknglobalnote.h> 
#include <EIKPROGI.H>
#include <BABACKUP.H>
#include <FBS.H>
#include <stringloader.h>

#include "YuccaBrowser.hrh"
#include "RFileCopier.h"

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


/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
CRFileCopier* CRFileCopier::NewL(MYBrowserRFileCopyObserver& aObserver,RFs& aFs,TBool aShowProgress)
    {
    CRFileCopier* self = CRFileCopier::NewLC(aObserver,aFs,aShowProgress);
    CleanupStack::Pop(self);
    return self;
    }
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
CRFileCopier* CRFileCopier::NewLC(MYBrowserRFileCopyObserver& aObserver,RFs& aFs,TBool aShowProgress)
    {
    CRFileCopier* self = new (ELeave) CRFileCopier(aObserver,aFs,aShowProgress);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/

CRFileCopier::CRFileCopier(MYBrowserRFileCopyObserver& aObserver,RFs& aFs,TBool aShowProgress) 
: CActive(CActive::EPriorityLow),iObserver(aObserver),iFsSession(aFs)
,iShowProgress(aShowProgress)
{
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CRFileCopier::~CRFileCopier()
{
	Cancel();

	if(iSourceFile.SubSessionHandle())
	{
		iSourceFile.Close();
	}
	
	if(iTargetFile.SubSessionHandle())
	{
		iTargetFile.Close();
	}
	
	if(iProgressDialog)
	{
		iProgressDialog->ProcessFinishedL(); 
	}

	iProgressDialog = NULL;
	iProgressInfo = NULL;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CRFileCopier::ConstructL(void)
{
	CActiveScheduler::Add(this);
}


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CRFileCopier::StartCopyL(RFile& aFile,const TDesC& aTargetfile)
{
	if(aFile.SubSessionHandle() && iFsSession.Handle())
	{
		iReadAlready = iFileSize = 0;
	
		iSourceFile.Duplicate(aFile);	
	
		iCancel = EFalse;
		iLastError = aFile.Size(iFileSize);
		
		if(iLastError == KErrNone)
		{
			iLastError = iTargetFile.Create(iFsSession,aTargetfile,EFileWrite);
			
			if(iLastError == KErrNone && iShowProgress)
			{
				iProgressDialog = new (ELeave) CAknProgressDialog((REINTERPRET_CAST(CEikDialog**,&iProgressDialog)),ETrue);
				iProgressDialog->PrepareLC(R_PROGRESS_NOTE);
				iProgressInfo = iProgressDialog->GetProgressInfoL();
				iProgressDialog->SetCallback(this);
				iProgressDialog->RunLD();
				iProgressInfo->SetFinalValue(iFileSize);
			}
		}
	}
	else
	{
		iLastError = KErrGeneral;
	}
	
	if(iLastError != KErrNone)
	{
		HandleOperationEndedL();
	}
	else
	{
		DoOnCopyRoundL();
	}
}

/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
void CRFileCopier::DoOnCopyRoundL(void)
{
	HBufC8* FileBuffer = HBufC8::NewLC(100000);
	TPtr8 FileBuf(FileBuffer->Des());
	
	if(iFileSize > (iReadAlready + 100000))
	{
		FileBuf.Set(FileBuffer->Des());
		iLastError = iSourceFile.Read(iReadAlready,FileBuf,100000);
		iReadAlready = iReadAlready + 100000;
	}
	else
	{
		FileBuf.Set(FileBuffer->Des());
		iLastError = iSourceFile.Read(iReadAlready,FileBuf,(iFileSize - iReadAlready));
		iReadAlready = iFileSize;
	}

	if(iLastError == KErrNone)
	{
		TInt CurSiz(0);
		iLastError = iTargetFile.Size(CurSiz);
		if(iLastError == KErrNone)
		{
			iLastError = iTargetFile.Write(CurSiz,FileBuffer->Des(),FileBuffer->Des().Size());
		}
	}
	
	CleanupStack::PopAndDestroy(); //FileBuffer
	
	
	
	if(iProgressDialog && iFileSize > 0)
	{
		TBuf<150> ProgressText1;
		
		StringLoader::Load(ProgressText1,R_SH_STR_COPYING);
		
		ProgressText1.AppendNum(((iReadAlready * 100) / iFileSize));
		ProgressText1.Append(_L("%"));
		iProgressDialog->SetTextL(ProgressText1);
	}
	
	if(iProgressInfo)
	{
		iProgressInfo->SetAndDraw(iReadAlready);	
	}
	else
	{
		iObserver.RFileCopyProgressL(iReadAlready,iFileSize);
	}
		
	TRequestStatus* status=&iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CRFileCopier::RunL()
{
	if(iCancel)
	{
		iLastError = KErrCancel;
	}
	
	if((iLastError == KErrNone) && (iFileSize > iReadAlready))
	{
		DoOnCopyRoundL();
	}
	else
	{
		HandleOperationEndedL();
	}
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CRFileCopier::DialogDismissedL (TInt /*aButtonId*/)
{		  		
	iProgressDialog = NULL;
	iProgressInfo = NULL;
		
	Cancel();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CRFileCopier::DoCancel()
{
	iCancel = ETrue;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CRFileCopier::HandleOperationEndedL(void)
{
	if(iProgressDialog)
	{
		iProgressDialog->ProcessFinishedL(); 
	}

	iProgressDialog = NULL;
	iProgressInfo = NULL;
	
	if(iSourceFile.SubSessionHandle())
	{
		iSourceFile.Close();
	}
	
	if(iTargetFile.SubSessionHandle())
	{
		iTargetFile.Close();
	}
	
	iObserver.RFileCopyDoneL(this,iLastError);
}


