/* 
	Copyright (c) 2006, 
	Jukka Silvennoinen
	All rights reserved 
*/
#ifndef __CTIMEOUTTIMER_H__
#define __CTIMEOUTTIMER_H__

#include <E32BASE.H>

	class MTimeOutNotify
	{
	public:
		virtual void TimerExpired() = 0;
		};

	class CTimeOutTimer: public CTimer
		{
	public:
		CTimeOutTimer(const TInt aPriority, MTimeOutNotify& aTimeOutNotify);
		void ConstructL();
		~CTimeOutTimer();
	protected:
		virtual void RunL();
	private:
		MTimeOutNotify& 	iNotify;
		};

#endif // __CTIMEOUTTIMER_H__
