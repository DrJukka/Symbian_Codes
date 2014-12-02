/* 	
	Copyright (c) 2001 - 2004, 
	Jukka Silvennoinen. 
	All rights reserved 
*/


#ifndef __EXAMPLE_TIMER_H__
#define __EXAMPLE_TIMER_H__

#include <E32BASE.H>

/*
---------------------------------------------------------------------------
*/ // call back function definitions for the timer.
	class MExampleTimerNotify
	{
	public:
		virtual void TimerExpired(TAny* aTimer,TInt aError) = 0;
		};
/*
---------------------------------------------------------------------------
*/
	class CExampleTimer: public CActive
		{
	public:
		static CExampleTimer* NewL(const TInt aPriority,MExampleTimerNotify& aNotify);
		~CExampleTimer();
	public:
		void At(const TTime& aTime);
		void After(TTimeIntervalMicroSeconds32 aInterval);
		void Inactivity(TTimeIntervalSeconds aSeconds);
	protected:
		void RunL();
		void DoCancel();
	private:
		CExampleTimer(const TInt aPriority,MExampleTimerNotify& aNotify);
		void ConstructL(void);
	private:
		RTimer					iTimer;
		MExampleTimerNotify&	iNotify;
		};

#endif // __EXAMPLE_TIMER_H__
