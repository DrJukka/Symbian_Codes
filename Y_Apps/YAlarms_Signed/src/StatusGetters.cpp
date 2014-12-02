
#include "StatusGetters.h"

/*
-----------------------------------------------------------------------------
******************************  CBatteryCheck  ******************************
-----------------------------------------------------------------------------
*/
CBatteryCheck::~CBatteryCheck()
{
	Cancel();
	delete iTelephony;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CBatteryCheck::ConstructL(void)
{
    iTelephony = CTelephony::NewL();
    GetBatteryInfo();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CBatteryCheck::CBatteryCheck(MBatteryObserver& aObserver)
: CActive(EPriorityStandard),iObserver(aObserver),iBatteryInfoV1Pckg(iBatteryInfoV1)
{
	CActiveScheduler::Add(this);
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
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
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CBatteryCheck::RunL()
{
    iObserver.BatteryLevelL(iBatteryInfoV1.iChargeLevel,iBatteryInfoV1.iStatus);
	
	if(iStatus != KErrCancel)
	{
		iTelephony->NotifyChange(iStatus,CTelephony::EBatteryInfoChange,iBatteryInfoV1Pckg);	
		SetActive();
	}

	iGettingBattery = EFalse;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CBatteryCheck::DoCancel()
{
	if(iGettingBattery)
    	iTelephony->CancelAsync(CTelephony::EGetBatteryInfoCancel);
	else
		iTelephony->CancelAsync(CTelephony::EBatteryInfoChangeCancel);
}

/*
-----------------------------------------------------------------------------
*****************************  CNwSignalCheck  ******************************
-----------------------------------------------------------------------------
*/

CNwSignalCheck::~CNwSignalCheck()
{
	Cancel();
	delete iTelephony;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CNwSignalCheck::ConstructL(void)
{
    iTelephony = CTelephony::NewL();
    GetSignalInfo();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CNwSignalCheck::CNwSignalCheck(MNwSignalObserver& aObserver)
: CActive(EPriorityStandard),iObserver(aObserver),iSigStrengthV1Pckg(iSigStrengthV1)
{
	CActiveScheduler::Add(this);
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CNwSignalCheck::GetSignalInfo()
{
    if(iTelephony && !IsActive())
    {
    	iGettingSignal = ETrue;
   	 	iTelephony->GetSignalStrength(iStatus, iSigStrengthV1Pckg);
   	 	SetActive();
    }
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CNwSignalCheck::RunL()
{
    iObserver.SignalStatus(iSigStrengthV1.iSignalStrength,iSigStrengthV1.iBar);

	if(iStatus != KErrCancel)
	{
		iTelephony->NotifyChange(iStatus,CTelephony::ESignalStrengthChange,iSigStrengthV1Pckg);	
		SetActive();
	}

	iGettingSignal = EFalse;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CNwSignalCheck::DoCancel()
{
	if(iGettingSignal)
    	iTelephony->CancelAsync(CTelephony::EGetSignalStrengthCancel);
	else
		iTelephony->CancelAsync(CTelephony::ESignalStrengthChangeCancel);
}


