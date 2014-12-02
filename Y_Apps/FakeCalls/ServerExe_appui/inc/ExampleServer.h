/* Copyright (c) 2001 - 2008, Dr. Jukka Silvennoinen. All rights reserved */


#ifndef __EXAMPLESERVERSESSION_HEADER__
#define __EXAMPLESERVERSESSION_HEADER__

#include <e32base.h>
#include <S32FILE.H>
#include <D32DBMS.H>
#include <AknKeyLock.h>
#include <HWRMVibra.h>
#include <MdaAudioTonePlayer.h>

#include "CTimeOutTimer.h"
#include "MsgDataBase.h" 
#include "AutoScheduler.h"
#include "clientservercommon.h"
#include "Drawer_Observers.h"
#include "Call_Observer.h"
#include "CalAlarm_Observer.h"

const TUid KUidNullUID		= { 0x00000000 }; 


class CFakeSMSSender;
// ----------------------------------------------------------------------------------------
// Server
// ----------------------------------------------------------------------------------------


class CExPolicy_Server : public CPolicyServer,MTimeOutNotify,MScheduleObserver
,MKeyCallBack,MForegroundCallBack,MCallObsCallBack,MMyLogCallBack,MTimeChnageNotify
, public MMdaAudioToneObserver
    {
public : // New methods
    static CExPolicy_Server* NewLC();	
    ~CExPolicy_Server(); 
public:// public function used by sessions to handle client requests
	void AskUpdateValuesL(void);
protected:
    void MatoPrepareComplete(TInt /*aError*/){};
    void MatoPlayComplete(TInt /*aError*/){};
	void DoShowAlarmNoteL(const TDes& aMessage);
	void DoHideAlarmNoteL(void);	
	void TimerExpired();
	void ChangeEventNowL(TInt aDelay);
	void StuffDoneL(TInt aError,TInt aIndex);
	TBool DeleteSchedule(TInt aIndex);
	TBool KeyCaptured(TWsEvent aEvent);
	TBool ForegroundChanged(TUid AppUid);
	void NotifyChangeInCallStatusL(CTelephony::TCallStatus& aStatus, TInt aError);
private:// internal functions to handle data
	void GetValuesL(CDictionaryFileStore* aDStore,TInt aUID,TUint32& aValue);
	void GetValuesL(CDictionaryFileStore* aDStore,TInt aUID,TDes& aValue);
	TBool GetValuesFromStoreL(void);
	
	void LogTime(const TTime& aTime, const TDesC8& aTitle);
	TBool ResolveNextSchduleTimeL(TTime& aNextSchedule, TInt& aIndex,RPointerArray<CMsgSched>& aItemArray);
	TBool AreWeStillOnL(TInt& aIndex,TTime& aSchedulingTime);
	TTimeIntervalDays AddDaysToNonWorkDays(TDay aDay);
	TTimeIntervalDays AddDaysToWorkDays(TDay aDay);
	TTimeIntervalDays CheckDaysType(TDay aDay, TInt aType);
	TTimeIntervalDays AddNextScheduleDays(TDay aDay, TInt aType);
	
	TBool IsTimeBeforeL(TTime& aTime,TTime& aCompare);
	// other internal functions
	void ShutMeDown();
	CExPolicy_Server();
	void ConstructL();	
	CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;
private:
	CTimeOutTimer*				iExitTimer;
	TTime						iModTime;				
	RPointerArray<CMsgSched>	iItemArray;
	CAutoScheduler*				iAutoSchedule;
	TBool						iUpdateDelay,iCallIsOn,iAlarmIsOn;
	TBool						iNoOn,iYesOn,iOffOn,iCameraOn;
	CKeyObserver*				iKeyObserver;
	CForegroundObserver*		iForegroundObserver;
	RAknKeyLock 				iKeyLock;
	CMyCallObserver*			iMyCallObserver;
	RPointerArray<CKeyCapcap>	iKeyArray;
	RFs							iFsSession;
	RFile						iFile;
	CCalAlarmObserver*			iCalAlarmObserver;
	CFakeSMSSender*             iFakeSMSSender;
	CTimeChnageMonitor*         iTimeChnageMonitor;
	CHWRMVibra*                 iHWRMVibra; 
	CMdaAudioToneUtility* 		iToneUtility;
    };


	
#endif //__EXAMPLESERVERSESSION_HEADER__
