/* Copyright (c) 2001 - 2008, Dr. Jukka Silvennoinen. All rights reserved */


#ifndef __EXAMPLESERVERSESSION_HEADER__
#define __EXAMPLESERVERSESSION_HEADER__

#include <e32base.h>
#include <S32FILE.H>
#include <D32DBMS.H>
#include "CTimeOutTimer.h"
#include "MsgDataBase.h" 
#include "AutoScheduler.h"
#include "clientservercommon.h"

class CFakeSMSSender;

// ----------------------------------------------------------------------------------------
// Server
// ----------------------------------------------------------------------------------------

class CExPolicy_Server : public CPolicyServer,MTimeOutNotify,MScheduleObserver,MTimeChnageNotify
    {
public : // New methods
    static CExPolicy_Server* NewLC();	
    ~CExPolicy_Server(); 
public:// public function used by sessions to handle client requests
	void AskUpdateValuesL(void);
protected:
	void TimerExpired();
	void ChangeEventNowL(TInt aDelay);
	void StuffDoneL(TInt aError,TInt aIndex);
	TBool DeleteSchedule(TInt aIndex);
private:// internal functions to handle data
	void LogTime(const TTime& aTime, const TDesC8& aTitle);
	void ResolveNextSchduleTimeL(TInt& aIndex,RPointerArray<CMsgSched>& aItemArray);
	TBool AreWeStillOnL(TInt& aIndex);
	
	TBool IsTimeBeforeL(TTime& aTime,TTime& aCompare);
	// other internal functions
	void ShutMeDown();
	CExPolicy_Server();
	void ConstructL();	
	CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;
private:
	CTimeOutTimer*				iExitTimer;
	CTimeChnageMonitor*         iTimeChnageMonitor;
	TTime						iModTime;				
	RPointerArray<CMsgSched>	iItemArray;
	CAutoScheduler*				iAutoSchedule;
	TBool						iUpdateDelay;
	RFs							iFsSession;
	RFile						iFile;
	CFakeSMSSender*             iFakeSMSSender;
    };


	
#endif //__EXAMPLESERVERSESSION_HEADER__
