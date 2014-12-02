
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
CKeyObserver* CKeyObserver::NewL(MKeyCallBack* aObserver)
	{
	CKeyObserver* self = CKeyObserver::NewLC(aObserver);
	CleanupStack::Pop(self);
	return self;
	}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CKeyObserver* CKeyObserver::NewLC(MKeyCallBack* aObserver)
	{
	CKeyObserver* self = new (ELeave) CKeyObserver(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CKeyObserver::CKeyObserver(MKeyCallBack* aObserver)
:CActive(EPriorityHigh),iObserver(aObserver)
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
	iHandle1 = iHandle2 = iHandle3 = iHandle4 = iHandle5 = -1;
	
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
		
	if (iStatus == KErrNone && iObserver) 
	{
		if(iObserver->KeyCaptured(e))
		{
			TInt wgId = iWsSession.GetFocusWindowGroup();
			iWsSession.SendEventToWindowGroup(wgId, e);
		}
		else
		{
			Listen();
		}
	}
	
	if (iStatus != KErrCancel) 
	{
		Listen();
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
	
	iHandle1 = iWg.CaptureKey(EKeyUpArrow, 0,0,3333333);
	iHandle2 = iWg.CaptureKey(EKeyDownArrow, 0,0,3333333);
	iHandle3 = iWg.CaptureKey(EKeyLeftArrow, 0,0,3333333);
	iHandle4 = iWg.CaptureKey(EKeyRightArrow, 0,0,3333333);
	
	iHandle5 = iWg.CaptureKey(EKeyApplication0 , 0,0,3333333);

	
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
	
	if(iHandle5 > -1)
		iWg.CancelCaptureKey(iHandle5);
	
	
	iHandle1 = iHandle2 = iHandle3 = iHandle4 = iHandle5 = -1;
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

