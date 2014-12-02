/* Copyright (c) 2001 - 2008 , J.P. Silvennoinen. All rights reserved */

#ifndef __TELLEPHONEMONITORT_H__
#define __TELLEPHONEMONITORT_H__

#include <coecobs.h>
#include <etel.h>
#include <Etel3rdParty.h>


class MTelMonCallback
{
public:
	virtual void NotifyChangeInStatus(CTelephony::TCallStatus& aStatus, TInt aError)=0;
};

class CTelephonyMonitor : public CActive
		{
	public:
		~CTelephonyMonitor();
		CTelephonyMonitor(MTelMonCallback& aCallBack);
		void ConstructL();	
	protected:
		void DoCancel();
		void RunL();
	private:
		
	private:
		MTelMonCallback& 				iCallBack;
		CTelephony*						iTelephony;
		CTelephony::TCallStatusV1 		iCurrentStatus;
		CTelephony::TCallStatusV1Pckg	iCurrentStatusPckg;	
		};


#endif // __TELLEPHONEMONITORT_H__
