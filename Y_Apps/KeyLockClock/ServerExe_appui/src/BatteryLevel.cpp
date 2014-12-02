
#include "BatteryLevel.h"

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CBatteryCheck* CBatteryCheck::NewL(MBatteryObserver& aObserver)
	{
	CBatteryCheck* self = CBatteryCheck::NewLC(aObserver);
	CleanupStack::Pop(self);
	return self;
	}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CBatteryCheck* CBatteryCheck::NewLC(MBatteryObserver& aObserver)
	{
	CBatteryCheck* self = new (ELeave) CBatteryCheck(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CBatteryCheck::~CBatteryCheck()
{
	Cancel();
	delete iTelephony;
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CBatteryCheck::ConstructL(void)
{
    iTelephony = CTelephony::NewL();
    GetBatteryInfo();
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CBatteryCheck::CBatteryCheck(MBatteryObserver& aObserver)
: CActive(EPriorityStandard),iObserver(aObserver),iBatteryInfoV1Pckg(iBatteryInfoV1)
{
	CActiveScheduler::Add(this);
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CBatteryCheck::GetBatteryInfo()
{
    if(iTelephony && !IsActive())
    {
    	iGettingBattery = ETrue;
   	 	iTelephony->GetBatteryInfo(iStatus, iBatteryInfoV1Pckg);
    	SetActive();
    }
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CBatteryCheck::RunL()
{
    iObserver.BatteryLevelL(iBatteryInfoV1.iChargeLevel,iBatteryInfoV1.iStatus);
	
	if(iStatus != KErrCancel && iTelephony)
	{
		iTelephony->NotifyChange(iStatus,CTelephony::EBatteryInfoChange,iBatteryInfoV1Pckg);	
		SetActive();
	}

	iGettingBattery = EFalse;
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CBatteryCheck::DoCancel()
{
	if(iTelephony)
	{
		if(iGettingBattery)
			iTelephony->CancelAsync(CTelephony::EGetBatteryInfoCancel);
		else
			iTelephony->CancelAsync(CTelephony::EBatteryInfoChangeCancel);
	}
}

