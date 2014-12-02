/* 
	Copyright (c) 2006-07, 
	Jukka Silvennoinen
	All rights reserved 
*/

#include <f32file.h>
#include <hal.h>
#include <hal_data.h>

#include "TraceMonitor.h"

/*
-------------------------------------------------------------------------------
****************************  CTraceMonitor   *************************************
-------------------------------------------------------------------------------
*/  

CTraceMonitor* CTraceMonitor::NewL(MTraceCallBack& aCallBack)
    {
    CTraceMonitor* self = NewLC(aCallBack);
    CleanupStack::Pop(self);
    return self;
    }

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/    
CTraceMonitor* CTraceMonitor::NewLC(MTraceCallBack& aCallBack)
    {
    CTraceMonitor* self = new (ELeave) CTraceMonitor(aCallBack);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }


/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CTraceMonitor::CTraceMonitor(MTraceCallBack& aCallBack)
:iCallBack(aCallBack)
    {
    }
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CTraceMonitor::~CTraceMonitor()
{
	Stop();
	
	delete iIdleTimer;
    delete iUpdateTimer;
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CTraceMonitor::ConstructL(void)
{
	iUpdateTimer = CPeriodic::NewL(CActive::EPriorityHigh);
    iIdleTimer = CPeriodic::NewL(CActive::EPriorityIdle+1);
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CTraceMonitor::Stop()
{
    if (iIdleTimer)
        iIdleTimer->Cancel();

    if (iUpdateTimer)
        iUpdateTimer->Cancel();
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/	
void CTraceMonitor::StartL()
{
	Stop();
	
    iIdleCounter = iMaxIdleCounter = 0;
    
    iUpdateTimer->Start(KUpdateInterval, KUpdateInterval, TCallBack(DoUpdateL, this));
    iIdleTimer->Start(KIdleInterval, KIdleInterval, TCallBack(DoIdleL, this));
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/	
TInt CTraceMonitor::DoUpdateL(TAny* aPtr)
{
	CTraceMonitor* self = static_cast<CTraceMonitor*>(aPtr);

    if (self->iIdleCounter > self->iMaxIdleCounter)
    {
        self->iMaxIdleCounter = self->iIdleCounter;
    }
    
    TMemoryInfoV1Buf   memInfoBuf;
	UserHal::MemoryInfo(memInfoBuf);

	TTime NowTime;
	NowTime.HomeTime();
	
	self->iCallBack.HandleUpdate(self->iIdleCounter,(memInfoBuf().iFreeRamInBytes / 1000),NowTime);

    self->iIdleCounter = 0;

	return KErrNone;
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/	
TInt CTraceMonitor::DoIdleL(TAny* aPtr)
{
	CTraceMonitor* self = static_cast<CTraceMonitor*>(aPtr);

    ++self->iIdleCounter;
    
    return KErrNone;
}



