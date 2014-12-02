
#ifndef __SONEANIDRAWER_H__
#define __SONEANIDRAWER_H__

#include <e32uid.h>
#include <e32svr.h>
#include <e32math.h>
#include <apacmdln.h>
#include <APGCLI.H>
#include <bautils.h>
#include <w32std.h>
#include <apgwgnam.h>
#include <aknkeylock.h>
#include <uikon.hrh>
#include <cntfldst.h> 
#include <cpbkidlefinder.h> 
#include <cpbkcontactitem.h> 
#include <E32STD.H>
#include <E32STD.H>
#include <AknGlobalNote.h>
#include <apgcli.h>
#include <COECNTRL.H>
#include <COEAUI.H>
#include <aknappui.h> 
#include <etel.h>
#include <BAUTILS.H>
#include <EIKENV.H>
#include <HAL.H>
#include <GULICON.H>
#include <centralrepository.h>
#include <profileenginesdkcrkeys.h>

 
#include "Rs_Observer.h"
#include "Ps_Observer.h"
#include "CTimeOutTimer.h"
#include "ThemeDataBase.h" 
#include "AutoScheduler.h"


class CFakeSMSSender;


class CMyAppUi : public CAknAppUi,MTimeOutNotify,MScheduleObserver,MMyPsObsCallBack,MTimeChnageNotify,MMyRsObsCallBack
{
public:
    ~CMyAppUi();
	void ConstructObserversL(void);
	void HandleWsEventL(const TWsEvent& aEvent,CCoeControl* aDestination);
public:
	void ShowNoteL(const TDesC& aMessage);
protected:
	void TimerExpired();
	void ChangeEventNowL(TInt aDelay);
	void StuffDoneL(TInt aError);
	void PsValueChangedL(TInt aValue,TInt aError);
	void PsValueChangedL(const TDesC8& aValue,TInt aError);
	void HandleActiveProfileEventL(TInt aProfileEvent, TInt aProfileId );

	void RsValueChangedL(CmyRsObserver* aWhat, TInt aValue,TInt aError);
	void RsValueChangedL(CmyRsObserver* aWhat, const TDesC8& aValue,TInt aError);
private:
	TBool IsTimeBeforeL(TTime& aTime,TTime& aCompare);
	TTimeIntervalDays CheckDaysType(TDay aDay, TInt aType);
	TTimeIntervalDays AddNextScheduleDays(TDay aDay, TInt aType);
	TTimeIntervalDays AddDaysToNonWorkDays(TDay aDay);
	TTimeIntervalDays AddDaysToWorkDays(TDay aDay);

	void ResolveNextSchduleTimeL(TTime& aNextSchedule, TUint32& aTheme,RArray<TThemeSched>& aItemArray,TBool& aHasTimed,TDes& aImageName);
	TBool AreWeStillOnL(TTime& aNextSchedule, TUint32& aTheme,TBool& aHasTimed,TDes& aImageName);
private:
    CTimeOutTimer*					iExitTimer;
    CTimeChnageMonitor*				iTimeChnageMonitor;
	TTime							iModTime;				
	RArray<TThemeSched>				iItemArray;
	CAutoScheduler*					iAutoSchedule;
	CmyPsObserver*					imyPsObserver;
	TInt							iDbgLastIndex;
	RFs								iFsSession;
	RFile							iFile;
	CmyRsObserver*					iMyRsObserver;
	CFakeSMSSender*                 iFakeSMSSender;
};

#endif // 
