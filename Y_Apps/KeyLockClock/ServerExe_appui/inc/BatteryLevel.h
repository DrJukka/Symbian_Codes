
#ifndef __BATTEREYY_MON_H__
#define __BATTEREYY_MON_H__


#include <e32base.h>
#include <Etel3rdParty.h>


class MBatteryObserver
	{
	public:
		virtual void BatteryLevelL(TUint aChangeStatus,CTelephony::TBatteryStatus aStatus) = 0;
	};
	
class CBatteryCheck : public CActive
  { 
public:
	static CBatteryCheck* NewL(MBatteryObserver& aObserver);
	static CBatteryCheck* NewLC(MBatteryObserver& aObserver);
    
	~CBatteryCheck();
private:	
	CBatteryCheck(MBatteryObserver& aObserver);
	void ConstructL(void);
	void GetBatteryInfo();
    void RunL();
    void DoCancel();
private:
	MBatteryObserver& 				iObserver;
    CTelephony* 					iTelephony;
    CTelephony::TBatteryInfoV1 		iBatteryInfoV1;
    CTelephony::TBatteryInfoV1Pckg 	iBatteryInfoV1Pckg;
    TBool							iGettingBattery;
   };

#endif //__BATTEREYY_MON_H__
