
#include "Call_Observer.h"

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
CMyCallObserver* CMyCallObserver::NewLC(MCallObsCallBack &aCallBack)
	{
 	CMyCallObserver* self = new (ELeave) CMyCallObserver(aCallBack);
  	CleanupStack::PushL(self);
  	self->ConstructL();
  	return self;
	}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
CMyCallObserver* CMyCallObserver::NewL(MCallObsCallBack &aCallBack)
	{
 	CMyCallObserver* self = CMyCallObserver::NewLC(aCallBack);
  	CleanupStack::Pop(self);
  	return self;
	}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
CMyCallObserver::CMyCallObserver(MCallObsCallBack &aCallBack)
:CActive(EPriorityStandard),iCallBack(aCallBack),iCallStatusPckg(iCallStatus)
	{
	
	}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
CMyCallObserver::~CMyCallObserver()
	{
  	Cancel();
  	delete iTelephony;
	}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
void CMyCallObserver::ConstructL()
	{
 	CActiveScheduler::Add(this);
  	iTelephony = CTelephony::NewL();
  	StartListeningForEvents(); 
	}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
void CMyCallObserver::StartListeningForEvents()
	{
	if(iTelephony)
		{
	  	iTelephony->NotifyChange(iStatus,CTelephony::EVoiceLineStatusChange,iCallStatusPckg);
	  	SetActive();
		}
	}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
void CMyCallObserver::RunL()
	{
  	CTelephony::TCallStatus status = iCallStatus.iStatus;
  	TInt errVal = iStatus.Int();
  	
  	StartListeningForEvents();
  	
  	iCallBack.NotifyChangeInCallStatusL(status,errVal);
	}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
void CMyCallObserver::DoCancel()
	{
	if(iTelephony)
		{
  		iTelephony->CancelAsync(CTelephony::EVoiceLineStatusChangeCancel);
		}
	}

