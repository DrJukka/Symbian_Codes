/* Copyright (c) 2001 - 2007 , J.P. Silvennoinen. All rights reserved */

#ifndef YSTATUSGETTERS_H
#define YSTATUSGETTERS_H

#include <e32base.h>
#include <Etel3rdParty.h>


   
 /*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/  
class MBatteryObserver
	{
	public:
		virtual void BatteryLevelL(TUint aChangeStatus,CTelephony::TBatteryStatus aStatus) = 0;
	};
	
	
class CBatteryCheck : public CActive
  { 
public:
    CBatteryCheck(MBatteryObserver& aObserver);
    void ConstructL(void);
	~CBatteryCheck();
private:	
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

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
class MNwSignalObserver
	{
	public:
		virtual void SignalStatus(TInt32 aStrength,TInt8 aBars) = 0;
	};
	
	
	
class CNwSignalCheck : public CActive
  { 
public:
    CNwSignalCheck(MNwSignalObserver& aObserver);
    void ConstructL(void);
	~CNwSignalCheck();
private:
	void GetSignalInfo();
    void RunL();
    void DoCancel();
private:
	MNwSignalObserver& 					iObserver;
    CTelephony* 						iTelephony;
    CTelephony::TSignalStrengthV1 		iSigStrengthV1;
    CTelephony::TSignalStrengthV1Pckg 	iSigStrengthV1Pckg;
    TBool								iGettingSignal;
   };

#endif	// YSTATUSGETTERS_H

