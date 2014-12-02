/* Copyright (c) 2001 - 2008 , J.P. Silvennoinen. All rights reserved */

#include 	"Caller_Active.h" 

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CTelephonyMonitor::CTelephonyMonitor(MTelMonCallback& aCallBack)
:CActive(EPriorityStandard),iCallBack(aCallBack),iCurrentStatusPckg(iCurrentStatus)
{
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CTelephonyMonitor::~CTelephonyMonitor()
{
	Cancel();
	delete iTelephony;
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CTelephonyMonitor::ConstructL(void)
{
	CActiveScheduler::Add(this);

	iTelephony = CTelephony::NewL();
	iTelephony->NotifyChange(iStatus,CTelephony::EVoiceLineStatusChange,iCurrentStatusPckg);
	SetActive();
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CTelephonyMonitor::DoCancel()
{
	iTelephony->CancelAsync(CTelephony::EVoiceLineStatusChangeCancel);
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CTelephonyMonitor::RunL()
{	
	TRAPD(Errrr,iCallBack.NotifyChangeInStatus(iCurrentStatus.iStatus,iStatus.Int()));	
	iTelephony->NotifyChange(iStatus,CTelephony::EVoiceLineStatusChange,iCurrentStatusPckg);
	SetActive();
}

