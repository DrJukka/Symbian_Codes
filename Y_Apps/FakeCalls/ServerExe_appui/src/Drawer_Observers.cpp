
#include "Drawer_Observers.h"

#include <BAUTILS.H>
#include <EIKENV.H>
#include <HAL.H>


/*
-----------------------------------------------------------------------------
********************************** CForegroundObserver **************************
-----------------------------------------------------------------------------
*/
CForegroundObserver* CForegroundObserver::NewL(MForegroundCallBack* aObserver)
	{
	CForegroundObserver* self = CForegroundObserver::NewLC(aObserver);
	CleanupStack::Pop(self);
	return self;
	}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CForegroundObserver* CForegroundObserver::NewLC(MForegroundCallBack* aObserver)
	{
	CForegroundObserver* self = new (ELeave) CForegroundObserver(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CForegroundObserver::CForegroundObserver(MForegroundCallBack* aObserver)
:CActive(EPriorityHigh),iObserver(aObserver)
{
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CForegroundObserver::~CForegroundObserver()
{
	Cancel();

	iWg.Close();
	iWsSession.Close();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CForegroundObserver::ConstructL()
{
	CActiveScheduler::Add(this);

	User::LeaveIfError(iWsSession.Connect());
	iWg = RWindowGroup(iWsSession);
	
	User::LeaveIfError(iWg.Construct((TUint32)&iWg, EFalse));
	iWg.SetOrdinalPosition(-1);
	iWg.EnableReceiptOfFocus(EFalse);

//	iWg.SetPointerCapture(RWindowBase::TCaptureDragDrop);

	CApaWindowGroupName* wn(NULL);
	wn =CApaWindowGroupName::NewL(iWsSession);
	if(wn)
	{
		CleanupStack::PushL(wn);
		
		wn->SetHidden(ETrue);
		wn->SetWindowGroupName(iWg);
		
		CleanupStack::PopAndDestroy();
	}
	
	iWg.EnableFocusChangeEvents();
	
	Listen();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CForegroundObserver::RunL()
{
	if (iStatus == KErrNone) 
	{
		TWsEvent e;
		iWsSession.GetEvent(e);
	}
	
	if(iObserver && (iStatus != KErrCancel))
	{
		TInt wgid = iWsSession.GetFocusWindowGroup();
		
		CApaWindowGroupName* gn(NULL);
		gn = CApaWindowGroupName::NewL(iWsSession, wgid);
		if(gn)
		{
			CleanupStack::PushL(gn); // gn
			
			TUid TmpUID(gn->AppUid());
		
			CleanupStack::PopAndDestroy(); // gn
		
			if(!iObserver->ForegroundChanged(TmpUID))
			{	
				Listen();
			}
		}
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TInt CForegroundObserver::RunError(TInt /*aError*/)
{
	return KErrNone;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CForegroundObserver::DoCancel()
{
	iWsSession.EventReadyCancel();
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CForegroundObserver::Listen()
{
	Cancel();
	iStatus=KRequestPending;
	iWsSession.EventReady(&iStatus);
	SetActive();
}


/*
-----------------------------------------------------------------------------
********************************** CKeyObserver **************************
-----------------------------------------------------------------------------
*/
CKeyObserver* CKeyObserver::NewL(MKeyCallBack* aObserver,const TBool& aNoOn,const TBool& aYesOn,const TBool& aOffOn,const TBool& aCameraOn)
	{
	CKeyObserver* self = CKeyObserver::NewLC(aObserver,aNoOn,aYesOn,aOffOn,aCameraOn);
	CleanupStack::Pop(self);
	return self;
	}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CKeyObserver* CKeyObserver::NewLC(MKeyCallBack* aObserver,const TBool& aNoOn,const TBool& aYesOn,const TBool& aOffOn,const TBool& aCameraOn)
	{
	CKeyObserver* self = new (ELeave) CKeyObserver(aObserver,aNoOn,aYesOn,aOffOn,aCameraOn);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CKeyObserver::CKeyObserver(MKeyCallBack* aObserver,const TBool& aNoOn,const TBool& aYesOn,const TBool& aOffOn,const TBool& aCameraOn)
:CActive(EPriorityHigh),iObserver(aObserver),iNoOn(aNoOn),iYesOn(aYesOn),iOffOn(aOffOn),iCameraOn(aCameraOn)
{
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CKeyObserver::~CKeyObserver()
{
	Cancel();

	CancelCaptureKeys();
	
	iWg.Close();
	iWsSession.Close();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CKeyObserver::ConstructL()
{
	CActiveScheduler::Add(this);
	iHandle1 = iHandle2 = iHandle3 = iHandle4 = -1;
	
	User::LeaveIfError(iWsSession.Connect());
	iWg = RWindowGroup(iWsSession);
		
	User::LeaveIfError(iWg.Construct((TUint32)&iWg, EFalse));
	iWg.SetOrdinalPosition(-1);
	iWg.EnableReceiptOfFocus(EFalse);

	CApaWindowGroupName* wn(NULL);
	wn=CApaWindowGroupName::NewL(iWsSession);
	if(wn)
	{
		CleanupStack::PushL(wn);
		
		wn->SetHidden(ETrue);
		wn->SetWindowGroupName(iWg);
		
		CleanupStack::PopAndDestroy();
	}
	
	iWg.EnableOnEvents();

	CaptureKeys();
	Listen();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CKeyObserver::RunL()
{
	TWsEvent e;
	iWsSession.GetEvent(e);
		
	if (iObserver)
	{
		if(iObserver->KeyCaptured(e)){
			Listen();
		}else{
			// we are propably being deleted and do nothing..
		}
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TInt CKeyObserver::RunError(TInt /*aError*/)
{
	return KErrNone;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CKeyObserver::DoCancel()
{
	iWsSession.EventReadyCancel();
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CKeyObserver::CaptureKeys(void)
{
	CancelCaptureKeys();

	if(iYesOn){
		iHandle1 = iWg.CaptureKey(EKeyYes, 0,0,1111111);
	}
	
	if(iNoOn){
		iHandle2 = iWg.CaptureKey(EKeyNo, 0,0,1111111);
	}
	
	if(iCameraOn){
		iHandle3 = iWg.CaptureKey(EKeyDevice7, 0,0,1111111);// Camera
	}
	
	if(iOffOn){
		iHandle4 = iWg.CaptureKey(EKeyDevice2, 0,0,1111111); // off	
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CKeyObserver::CancelCaptureKeys(void)
{
	if(iHandle1 > -1)
		iWg.CancelCaptureKey(iHandle1);
	
	if(iHandle2 > -1)
		iWg.CancelCaptureKey(iHandle2);

	if(iHandle3 > -1)
		iWg.CancelCaptureKey(iHandle3);
	
	if(iHandle4 > -1)
		iWg.CancelCaptureKey(iHandle4);
		
	
	iHandle1 = iHandle2 = iHandle3 = iHandle4 = -1;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CKeyObserver::Listen()
{
	Cancel();
	iStatus=KRequestPending;
	iWsSession.EventReady(&iStatus);
	SetActive();
}

