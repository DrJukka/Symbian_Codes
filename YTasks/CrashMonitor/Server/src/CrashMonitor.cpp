/* 
	Copyright (c) 2006-07, 
	Jukka Silvennoinen
	All rights reserved 
*/

#include <f32file.h>
#include "CrashMonitor.h"

/*
-------------------------------------------------------------------------------
****************************  CExitMonitor   *************************************
-------------------------------------------------------------------------------
*/  

CExitMonitor* CExitMonitor::NewL(MExitMonitorCallBack* aCallBack)
    {
    CExitMonitor* self = NewLC(aCallBack);
    CleanupStack::Pop(self);
    return self;
    }

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/    
CExitMonitor* CExitMonitor::NewLC(MExitMonitorCallBack* aCallBack)
    {
    CExitMonitor* self = new (ELeave) CExitMonitor(aCallBack);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }


/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CExitMonitor::CExitMonitor(MExitMonitorCallBack* aCallBack)
:CActive(0),iCallBack(aCallBack)
    {
    }
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CExitMonitor::~CExitMonitor()
{
	Cancel();
	iUndertaker.Close();
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CExitMonitor::ConstructL(void)
{
	CActiveScheduler::Add(this);
	
	iUndertaker.Create(); 
	iUndertaker.Logon(iStatus,iHandle);
	SetActive();
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CExitMonitor::DoCancel()
{
	iUndertaker.LogonCancel();
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/	
void CExitMonitor::RunL()
{
	if(iStatus.Int() != KErrCancel)
	{
		TTime NowTime;
		NowTime.HomeTime();
		
		RThread r;
		r.SetHandle(iHandle);
		
		RProcess Process;
	  	if(KErrNone == r.Process(Process))
	  	{
	  		TParsePtrC Hjelppper(Process.FileName());
	  		iCallBack->HandleExit(r.Name(),r.ExitType(),r.ExitReason(),r.ExitCategory(),Hjelppper.NameAndExt(),NowTime);
	  		
	  		Process.Close();
	  	}
	  	else
	  	{
			iCallBack->HandleExit(r.Name(),r.ExitType(),r.ExitReason(),r.ExitCategory(),_L(""),NowTime);
	  	}
	  	
		r.Close();
		
		iUndertaker.Logon(iStatus,iHandle);
		SetActive();
	}
}



