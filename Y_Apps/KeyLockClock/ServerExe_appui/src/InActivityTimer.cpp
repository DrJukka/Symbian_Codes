/* 
	Copyright (c) 2001 - 2009 ,
	Jukka Silvennoinen. 
	All rights reserved 
*/

#include "InActivityTimer.h"


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CInActivityTimer* CInActivityTimer::NewL(const TInt aPriority,MExampleTimerNotify& aNotify)
{
	CInActivityTimer* me = new (ELeave) CInActivityTimer(aPriority,aNotify);
    CleanupStack::PushL(me);
	me->ConstructL();
	CleanupStack::Pop();
    return me;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CInActivityTimer::CInActivityTimer(const TInt aPriority,MExampleTimerNotify& aNotify)
:CActive(aPriority),iNotify(aNotify)
{
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CInActivityTimer::~CInActivityTimer()
{	
	Cancel();
	iTimer.Close();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CInActivityTimer::ConstructL(void)
{
	CActiveScheduler::Add(this);
	iTimer.CreateLocal();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CInActivityTimer::Inactivity(TTimeIntervalSeconds aSeconds)
{
	Cancel();
	iTimer.Inactivity(iStatus,aSeconds);
	SetActive();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CInActivityTimer::DoCancel()
{
	iTimer.Cancel();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CInActivityTimer::RunL()
{
	iNotify.InActivityExpiredL(iStatus.Int());
}



