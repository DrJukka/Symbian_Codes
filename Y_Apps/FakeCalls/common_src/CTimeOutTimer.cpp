/* Copyright (c) 2001 - 2008, Dr. Jukka Silvennoinen. All rights reserved */

#include "CTimeOutTimer.h"

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CTimeOutTimer::CTimeOutTimer(const TInt aPriority):CTimer(aPriority)
    {
    }
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CTimeOutTimer::~CTimeOutTimer()
    {
	Cancel();
    }
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CTimeOutTimer* CTimeOutTimer::NewL(const TInt aPriority, MTimeOutNotify& aTimeOutNotify)
    {
    CTimeOutTimer *p = new (ELeave) CTimeOutTimer(aPriority);
    CleanupStack::PushL(p);
	p->ConstructL(aTimeOutNotify);
	CleanupStack::Pop();
    return p;
    }
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CTimeOutTimer::ConstructL(MTimeOutNotify &aTimeOutNotify)
    {
	iNotify=&aTimeOutNotify;
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
    }
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CTimeOutTimer::RunL()
    {
	iNotify->TimerExpired();
	}


/*
-----------------------------------------------------------------------------
****************************************************************************
-----------------------------------------------------------------------------
*/
CTimeChnageMonitor::CTimeChnageMonitor(const TInt aPriority,MTimeChnageNotify &aTimeOutNotify)
:CActive(aPriority),iNotify(aTimeOutNotify)
    {
    }
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CTimeChnageMonitor::~CTimeChnageMonitor()
    {
    Cancel();
    iChangeNotifier.Close();
    }
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CTimeChnageMonitor* CTimeChnageMonitor::NewL(const TInt aPriority, MTimeChnageNotify& aTimeOutNotify)
    {
    CTimeChnageMonitor *p = new (ELeave) CTimeChnageMonitor(aPriority,aTimeOutNotify);
    CleanupStack::PushL(p);
    p->ConstructL();
    CleanupStack::Pop();
    return p;
    }
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CTimeChnageMonitor::ConstructL()
{
    CActiveScheduler::Add(this);
    
    iChangeNotifier.Create();
    iChangeNotifier.Logon(iStatus);
    SetActive();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CTimeChnageMonitor::DoCancel()
{
    iChangeNotifier.LogonCancel();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CTimeChnageMonitor::RunL()
{
    if(iStatus.Int() != KErrCancel)
    {
        TInt changes = iStatus.Int();
        if ((changes & EChangesSystemTime)
        || (changes & EChangesLocale)
        || (changes & EChangesMidnightCrossover))
        {
            iNotify.ChangeEventNowL(5);
        }
        
        iChangeNotifier.Logon(iStatus);
        SetActive();
    }
}

