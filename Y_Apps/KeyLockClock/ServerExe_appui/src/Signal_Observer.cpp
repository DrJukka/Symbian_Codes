
#include "Signal_Observer.h"


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
CMySignalObserver* CMySignalObserver::NewLC(MSignalObsCallBack& aCallBack)
	{
  	CMySignalObserver* self = new (ELeave) CMySignalObserver(aCallBack);
  	CleanupStack::PushL(self);
  	self->ConstructL();
  	return self;
	}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
CMySignalObserver* CMySignalObserver::NewL(MSignalObsCallBack& aCallBack)
	{
  	CMySignalObserver* self = CMySignalObserver::NewLC(aCallBack);
  	CleanupStack::Pop();
  	return self;
	}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
CMySignalObserver::~CMySignalObserver()
	{
	// always cancel any pending request before deleting the objects
	Cancel();
	delete iTelephony;
	}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
void CMySignalObserver::ConstructL(void)
	{
	// Active objects needs to be added to active scheduler
	CActiveScheduler::Add(this);
    iTelephony = CTelephony::NewL();// construct CTelephony
    
    iMonitoring = EFalse;
   	// ask CTelephony to give us the current signal strenght
	// RunL will be called when the value is ready
   	iTelephony->GetSignalStrength(iStatus, iSigStrengthV1Pckg);
   	SetActive();// after starting the request AO needs to be set active
	}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
CMySignalObserver::CMySignalObserver(MSignalObsCallBack& aCallBack)
: CActive(EPriorityStandard),iCallBack(aCallBack)
,iSigStrengthV1Pckg(iSigStrengthV1)
	{
	}
/*
-----------------------------------------------------------------------------
RunL is called by Active schduler when the requeststatus variable iStatus 
is set to something else than pending, function Int() can be then used
to determine if the request failed or succeeded
-----------------------------------------------------------------------------
*/ 
void CMySignalObserver::RunL()
	{
	// use callback function to tell owner that we have finished
    iCallBack.SignalStatus(iSigStrengthV1.iSignalStrength,iSigStrengthV1.iBar);

	if(iStatus == KErrNone)
		{
		// got the initial value, thus we start monitoring changes now
		iMonitoring = ETrue;
		// ask CTelephony to notify when signal strenght changes
		// RunL will be called when this happens
		iTelephony->NotifyChange(iStatus,CTelephony::ESignalStrengthChange,iSigStrengthV1Pckg);	
		SetActive();// after starting the request AO needs to be set active
		}
	}
/*
-----------------------------------------------------------------------------
newer call DoCancel in your code, just call Cancel() and let the
active scheduler handle calling this functions, if the AO is active
-----------------------------------------------------------------------------
*/ 
void CMySignalObserver::DoCancel()
	{
	// since CTelephony implements many different functions
	// You need to specifying what you want to cancel
	// we also need to check first which call is currently active
	
	if(iMonitoring)
		{
		iTelephony->CancelAsync(CTelephony::ESignalStrengthChangeCancel);
		}
	else
		{
    	iTelephony->CancelAsync(CTelephony::EGetSignalStrengthCancel);
		}
	}


// End of File  

