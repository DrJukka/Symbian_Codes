
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
#include <AknKeyLock.h>

#include "InActivityTimer.h"
#include "Drawer_Observers.h"
#include "CTimeOutTimer.h"
#include "Ps_Observer.h"
#include "Ind_Drawer.h"
#include "Call_Observer.h"
#include "Rs_Observer.h"
#include "BatteryLevel.h"
#include "SMS_Monitor.h"
#include "CalAlarm_Observer.h"

class CFakeSMSSender;
	
const TUid KUidNullUID		= { 0x00000000 }; 

class CMyAppUi : public CAknAppUi,MTimeOutNotify,MMyPsObsCallBack,MForegroundCallBack
,MLayoutChnageCallBack,MCallObsCallBack,MExampleTimerNotify,MMyRsObsCallBack,MBatteryObserver
,MInboxCallBack,MMyLogCallBack
{
public:
    ~CMyAppUi();
	void ConstructObserversL(void);
	void HandleWsEventL(const TWsEvent& aEvent,CCoeControl* aDestination);
public:
	void ShowNoteL(const TDesC& aMessage);
protected:
	void DoShowAlarmNoteL(const TDes& aMessage);
	void DoHideAlarmNoteL(void);
		
	void TimerExpired();
	void PsValueChangedL(const TUid& aCategory, const TUint aKey,TInt aValue,TInt aError);
	void PsValueChangedL(const TUid& aCategory, const TUint aKey,const TDesC8& aValue,TInt aError);	
	TBool ForegroundChanged(TUid AppUid);
	void DisableScreen(TInt aDelay);
	TBool IsKeyBoardLocked();
	void NotifyChangeInCallStatusL(CTelephony::TCallStatus& aStatus, TInt aError);
	void InActivityExpiredL(TInt anError);
	void InboxChangedL(TInt aSMS,TInt aMMS);	
	void BatteryLevelL(TUint aChangeStatus,CTelephony::TBatteryStatus aStatus);
	void RsValueChangedL(CmyRsObserver* aWhat, TInt aValue,TInt aError);
	void RsValueChangedL(CmyRsObserver* aWhat, const TDesC8& aValue,TInt aError);	
private:
	void GetS60PlatformVersionL(TUint& aMajor, TUint& aMinor );
	void GetValuesL(CDictionaryFileStore* aDStore,TInt aUID,TUint32& aValue);
	void GetValuesL(CDictionaryFileStore* aDStore,TInt aUID,TDes& aValue);
	void GetValuesFromStoreL(void);
	TBool DoCheckSettingsNowL(void);
private:
    CTimeOutTimer*			iExitTimer;
    CmyPsObserver*			imyPsObserver;
    TBool					iExitNow,iLockNow,iOn,iLockEnabled,iCallIsOn,iAlarmIsOn;
    TInt					iLockTime;
    CForegroundObserver*	iForegroundObserver;
    CClockDrawer*			iClockDrawer;
	RFs						iFsSession;
	RFile					iFile;
	RAknKeyLock 			iKeyLock;
	TFileName				iFont;
	CMyCallObserver*		iMyCallObserver;
	CInActivityTimer*		iInActivityTimer;
	TScreenVals				iSettings;
	TMonitorVals			iMonitorVals;
	CmyRsObserver*			iMissCallObserver;
	CBatteryCheck*			iBatteryCheck;
	CmyPsObserver*			iEmailStatusObserver;
	CSMSMonitor*			iSMSMonitor;
	TTime					iModTime;	
	CCalAlarmObserver*		iCalAlarmObserver;
	CFakeSMSSender*         iFakeSMSSender;
};

#endif // 
