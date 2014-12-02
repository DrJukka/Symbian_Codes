/* Copyright (c) 2001 - 2008 , Dr Jukka Silvennoinen. All rights reserved */

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
		static CTimeOutTimer* NewL(const TInt aPriority, MTimeOutNotify& aTimeOutNotify);
		~CTimeOutTimer();
	protected:
		CTimeOutTimer(const TInt aPriority);
		void ConstructL(MTimeOutNotify& aTimeOutNotify);
		virtual void RunL();
	private:
		MTimeOutNotify* iNotify;
		};

	class MTimeChnageNotify
	{
	public:
		virtual void ChangeEventNowL(TInt aDelay) = 0;
		};

	class CTimeChnageMonitor: public CActive
		{
	public:
		static CTimeChnageMonitor* NewL(const TInt aPriority, MTimeChnageNotify& aTimeOutNotify);
		~CTimeChnageMonitor();
	protected:
		CTimeChnageMonitor(const TInt aPriority,MTimeChnageNotify &aTimeOutNotify);
		void ConstructL();
		void RunL();
		void DoCancel();
	private:
		MTimeChnageNotify& 	iNotify;
		RChangeNotifier		iChangeNotifier;
		};
		

#endif // __CTIMEOUTTIMER_H__
