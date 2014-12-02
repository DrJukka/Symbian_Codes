
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


#include "CTimeOutTimer.h"
#include "ThemeDataBase.h" 
#include "AutoScheduler.h"
#include "ForeGroundObserver.h"


class CMyAppUi : public CAknAppUi,MTimeOutNotify,MScheduleObserver,MFgrCallBack
{
public:
    ~CMyAppUi();
	void ConstructObserversL(void);
	void HandleWsEventL(const TWsEvent& aEvent,CCoeControl* aDestination);
	void HandleCommandL(TInt aCommand);
public:
	void ShowNoteL(const TDesC& aMessage);
protected:
	void ForegroundEventL(const TUid& aAppUid);
	void TimerExpired();
	void StuffDoneL(TInt aError);
private:
	void LogTime(const TTime& aTime, const TDesC8& aTitle);
	void ResolveNextSchduleTimeL(TTime& aNextSchedule, TInt& aProfile,RArray<TProfSched>& aItemArray);
	TBool AreWeStillOnL(TTime& aNextSchedule, TInt& aProfile);
private:
    CTimeOutTimer*				iExitTimer;
	TTime						iModTime;				
	RArray<TProfSched>			iItemArray;
	CAutoScheduler*				iAutoSchedule;
	CArrayFixFlat<TInt>* 		iRandomArray;
	CFgrObserver*				iFgrObserve;
	TBool						iLastWasTheOne;
	TInt						iDbgLastIndex;
	RFs							iFsSession;
	RFile						iFile;
};

#endif // 
