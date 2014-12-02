/* 
	Copyright (c) 2006, 
	Jukka Silvennoinen
	All rights reserved 
*/
#include "CTimeOutTimer.h"

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CTimeOutTimer::CTimeOutTimer(const TInt aPriority,MTimeOutNotify& aTimeOutNotify)
:CTimer(aPriority),iNotify(aTimeOutNotify)
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
void CTimeOutTimer::ConstructL()
{
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CTimeOutTimer::RunL()
{
	iNotify.TimerExpired();
}
