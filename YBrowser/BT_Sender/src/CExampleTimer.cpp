/* 
	Copyright (c) 2001 - 2006 ,
	Jukka Silvennoinen. 
	All rights reserved 
*/

#include "CExampleTimer.h"

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CExampleTimer::CExampleTimer(const TInt aPriority,MExampleTimerNotify& aNotify)
:CActive(aPriority),iNotify(aNotify)
{
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CExampleTimer::~CExampleTimer()
{	
	Cancel();
	iTimer.Close();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CExampleTimer* CExampleTimer::NewL(const TInt aPriority,MExampleTimerNotify& aNotify)
{
    CExampleTimer* me = new (ELeave) CExampleTimer(aPriority,aNotify);
    CleanupStack::PushL(me);
	me->ConstructL();
	CleanupStack::Pop();
    return me;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExampleTimer::ConstructL(void)
{
	CActiveScheduler::Add(this);
	iTimer.CreateLocal();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExampleTimer::After(TTimeIntervalMicroSeconds32 aInterval)
{
	Cancel();
	iTimer.After(iStatus,aInterval);
	SetActive();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExampleTimer::At(const TTime& aTime)
{
	Cancel();
	iTimer.At(iStatus,aTime);
	SetActive();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExampleTimer::Inactivity(TTimeIntervalSeconds aSeconds)
{
	Cancel();
	iTimer.Inactivity(iStatus,aSeconds);
	SetActive();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExampleTimer::DoCancel()
{
	iTimer.Cancel();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExampleTimer::RunL()
{
	if(iStatus.Int() != KErrCancel)
	{
		iNotify.TimerExpired(this,iStatus.Int());
	}
}



