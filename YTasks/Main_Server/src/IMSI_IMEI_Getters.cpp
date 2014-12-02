/* 
	Copyright (c) 2006, 
	Jukka Silvennoinen
	All rights reserved 
*/
#include <akninputlanguageinfo.h> 
#include <IMCVCODC.H>
#include <HAL.H>
#include <HAL_data.H>
#include <sysutil.h> 

#include "IMSI_IMEI_Getters.h"


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CCellIDCheck::~CCellIDCheck()
{
	Cancel();
	delete iTelephony;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CCellIDCheck::ConstructL(void)
{
    iTelephony = CTelephony::NewL();
    iTelephony->GetCurrentNetworkInfo(iStatus, iIdV1Pkg);
	SetActive();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CCellIDCheck::CCellIDCheck(MCellIdObserver& aObserver)
: CActive(EPriorityNormal),iObserver(aObserver),iIdV1Pkg(iIdV1)
{
	CActiveScheduler::Add(this);
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CCellIDCheck::RunL()
{
    iObserver.CellIDL(iIdV1.iCountryCode,iIdV1.iNetworkId,iIdV1.iLocationAreaCode,iIdV1.iCellId);

}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CCellIDCheck::DoCancel()
{
	iTelephony->CancelAsync(CTelephony::EGetCurrentNetworkInfoCancel);
}

/*
-----------------------------------------------------------------------------
******************************* CImsiReader *********************************
-----------------------------------------------------------------------------
*/
CImsiReader* CImsiReader::NewL(MImsiObserver* aObserver)
    {
    CImsiReader* self = NewLC(aObserver);
    CleanupStack::Pop(self);
    return self;
    }

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CImsiReader* CImsiReader::NewLC(MImsiObserver* aObserver)
    {
    CImsiReader* self = new (ELeave) CImsiReader(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }
    
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
CImsiReader::CImsiReader(MImsiObserver* aObserver)
:CActive(EPriorityNormal),iObserver(aObserver),iImsiV1Pkg(iImsiV1)
{
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CImsiReader::~CImsiReader()
{
	Cancel();
	delete iTelephony;
	
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CImsiReader::ConstructL(void)
{	
	CActiveScheduler::Add(this);

	iTelephony = CTelephony::NewL();
	iTelephony->GetSubscriberId(iStatus,iImsiV1Pkg);
	SetActive();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CImsiReader::DoCancel()
{
	iTelephony->CancelAsync(CTelephony::EGetSubscriberIdCancel);
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CImsiReader::RunL()
{
	iObserver->GotIMSIL(iImsiV1.iSubscriberId,iStatus.Int());
}





