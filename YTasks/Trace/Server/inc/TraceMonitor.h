/* 
	Copyright (c) 2006-07, 
	Jukka Silvennoinen
	All rights reserved 
*/
#ifndef __CRASHMONITOR_H__
#define __CRASHMONITOR_H__

#include<e32const.h>
#include<e32cmn.h>
#include<e32base.h>

const TInt KUpdateInterval 	= 1000000;   // 1 sec
const TInt KIdleInterval 	= 10000;    // 0.01 sec

	class MTraceCallBack
	{
	public:
		virtual void HandleUpdate(const TInt& aCpuLoad,const TInt& aMemory,const TTime& aTime) = 0;
	};

	class CTraceMonitor : public CBase
	{
	public:
	    static CTraceMonitor* NewL(MTraceCallBack& aCallBack);
	    static CTraceMonitor* NewLC(MTraceCallBack& aCallBack);
	    ~CTraceMonitor();
	    void StartL(void);
	    void Stop(void);
		TUint32	GetMax(void){return iMaxIdleCounter;};
	private:
	    static TInt DoUpdateL(TAny* aPtr);
    	static TInt DoIdleL(TAny* aPtr);
	    CTraceMonitor(MTraceCallBack& aCallBack);
	    void ConstructL(void);
	private:
		MTraceCallBack& 	iCallBack;
		CPeriodic* 			iUpdateTimer;
    	CPeriodic* 			iIdleTimer;
    	TInt				iIdleCounter,iMaxIdleCounter;
	};



#endif // __CRASHMONITOR_H__
