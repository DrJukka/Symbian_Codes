/* Copyright (c) 2001 - 2005 ,Jukka Silvennoinen Solution One Telecom LTd. All rights reserved */

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
