/* Copyright (c) 2001 - 2008, Dr. Jukka Silvennoinen. All rights reserved */

#ifndef __AUTO_SCHEDULER__
#define __AUTO_SCHEDULER__

#include <e32base.h>
#include <S32FILE.H>
#include <mprofileengine.h>

#include "Call_Observer.h"
#include "CTimeOutTimer.h"

	class MScheduleObserver
	{
	public:
		virtual void StuffDoneL(TInt aError) = 0;
		};

	
class CAutoScheduler : public CBase,MTimeOutNotify, MCallObsCallBack
    {
public : 
    static CAutoScheduler* NewL(MScheduleObserver& aUpdate, TTime aTime,TInt aProfile);
    static CAutoScheduler* NewLC(MScheduleObserver& aUpdate,TTime aTime,TInt aProfile);
    ~CAutoScheduler();
	void NewScheduleL(TTime aNewTime, TInt aProfile);
protected:
	void TimerExpired();
	void NotifyChangeInCallStatusL(CTelephony::TCallStatus& aStatus, TInt aError);
private: 
	void LogCurrentTime(void);
	void LogScheduleTime(void);
	void DoDoTheStuffNowL(void);
	CAutoScheduler(MScheduleObserver& aUpdate,TTime aTime,TInt aProfile);
    void ConstructL(void);
private:
	MScheduleObserver& 	iUpdate;
	CTimeOutTimer* 		iTimeOutTimer;
    TTime				iTime;
	TInt 				iProfile;
	MProfileEngine* 	iProfileEngine;
	RFs 				iFsSession;
	CMyCallObserver*	iMyCallObserver;
	TBool				iCallIsOn;
	RFile				iFile;
    };


#endif //__AUTO_SCHEDULER__

