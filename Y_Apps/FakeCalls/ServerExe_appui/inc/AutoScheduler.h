/* Copyright (c) 2001 - 2008, Dr. Jukka Silvennoinen. All rights reserved */

#ifndef __AUTO_SCHEDULER__
#define __AUTO_SCHEDULER__

#include <e32base.h>
#include <S32FILE.H>
#include <mprofileengine.h>

#include "CTimeOutTimer.h"
#include "MsgDataBase.h" 

	class MScheduleObserver
	{
	public:
		virtual void StuffDoneL(TInt aError,TInt aIndex) = 0;
		virtual TBool DeleteSchedule(TInt aIndex) = 0;
		};

class CAutoScheduler : public CBase,MTimeOutNotify
    {
public : 
    static CAutoScheduler* NewL(MScheduleObserver& aUpdate);
    static CAutoScheduler* NewLC(MScheduleObserver& aUpdate);
    ~CAutoScheduler();
	void NewScheduleL(CMsgSched* aNextSchedule,const TTime aSchedlingTime);
	void NewScheduleL(CKeyCapcap* aNextSchedule,const TTime aSchedlingTime);
	void DoLaunchCallUIL(const TDesC& aNumber);
protected:
	void TimerExpired();
private: 
	void LogCurrentTime(void);
	void LogScheduleTime(void);
	void DoDoTheStuffNowL(void);
	CAutoScheduler(MScheduleObserver& aUpdate);
    void ConstructL(void);
private:
	MScheduleObserver& 	iUpdate;
	CTimeOutTimer* 		iTimeOutTimer;
    HBufC* 				iNunmber;
    HBufC* 				iName;
    TTime				iNextTime;
    TInt				iNextIndex,iRepeat;

	RFs 				iFsSession;
	RFile				iFile;
    };


#endif //__AUTO_SCHEDULER__

