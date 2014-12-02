/* 	
	Copyright (c) 2001 - 2004, 
	Jukka Silvennoinen. 
	All rights reserved 
*/


#ifndef __INACTIVITY_TIMER_H__
#define __INACTIVITY_TIMER_H__

#include <E32BASE.H>


	class MExampleTimerNotify
	{
	public:
		virtual void InActivityExpiredL(TInt anError) = 0;
		};

	class CInActivityTimer: public CActive
		{
	public:
		static CInActivityTimer* NewL(const TInt aPriority,MExampleTimerNotify& aNotify);
		~CInActivityTimer();
	public:
		void Inactivity(TTimeIntervalSeconds aSeconds);
	protected:
		void RunL();
		void DoCancel();
	private:
		CInActivityTimer(const TInt aPriority,MExampleTimerNotify& aNotify);
		void ConstructL(void);
	private:
		RTimer					iTimer;
		MExampleTimerNotify&	iNotify;
		};

#endif // __INACTIVITY_TIMER_H__
