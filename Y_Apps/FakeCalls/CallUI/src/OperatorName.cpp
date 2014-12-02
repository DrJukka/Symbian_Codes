
#include "OperatorName.h"


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
CGetOperatorName* CGetOperatorName::NewLC(MOperatorNameCallback& aCallBack)
	{
  	CGetOperatorName* self = new (ELeave) CGetOperatorName(aCallBack);
  	CleanupStack::PushL(self);
  	self->ConstructL();
  	return self;
	}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
CGetOperatorName* CGetOperatorName::NewL(MOperatorNameCallback& aCallBack)
	{
  	CGetOperatorName* self = CGetOperatorName::NewLC(aCallBack);
  	CleanupStack::Pop();
  	return self;
	}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
CGetOperatorName::~CGetOperatorName()
	{
	// always cancel any pending request before deleting the objects
	Cancel();
	delete iTelephony;
	}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
void CGetOperatorName::ConstructL(void)
	{
	// Active objects needs to be added to active scheduler
	CActiveScheduler::Add(this);
    iTelephony = CTelephony::NewL();// construct CTelephony
    // ask CTelephony to give us the current operator name
	// RunL will be called when the name is read 
    
    iGettingOperator = EFalse;
    iTelephony->GetCurrentNetworkName(iStatus, iIdV2Pkg);
    SetActive();// after starting the request AO needs to be set active
    
	}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
CGetOperatorName::CGetOperatorName(MOperatorNameCallback& aCallBack)
: CActive(EPriorityNormal),iCallBack(aCallBack)
,iIdV1Pkg(iIdV1),iIdV2Pkg(iIdV2)
	{
	}
/*
-----------------------------------------------------------------------------
RunL is called by Active schduler when the requeststatus variable iStatus 
is set to something else than pending, function Int() can be then used
to determine if the request failed or succeeded
-----------------------------------------------------------------------------
*/ 
void CGetOperatorName::RunL()
	{
    if(iStatus.Int() == KErrNone){
    
        if(iGettingOperator){
            // use callback function to tell owner that we have finished
            iCallBack.OperatorNameL(iIdV1.iOperatorName,iStatus.Int());
        }else{
            iCallBack.OperatorNameL(iIdV2.iNetworkName,iStatus.Int());
        }
    }else{
    
        if(!iGettingOperator){
            iGettingOperator = ETrue;
            iTelephony->GetOperatorName(iStatus, iIdV1Pkg);
            SetActive();// after starting the request AO needs to be set active
        }else{
            iCallBack.OperatorNameL(iIdV1.iOperatorName,iStatus.Int());
        }
    }
	}
/*
-----------------------------------------------------------------------------
newer call DoCancel in your code, just call Cancel() and let the
active scheduler handle calling this functions, if the AO is active
-----------------------------------------------------------------------------
*/ 
void CGetOperatorName::DoCancel()
	{
	if(iTelephony)
		{
		// since CTelephony implements many different functions
		// You need to specifying what you want to cancel
        if(iGettingOperator){
            iTelephony->CancelAsync(CTelephony::EGetOperatorNameCancel);
        }else{
            iTelephony->CancelAsync(CTelephony::EGetCurrentNetworkNameCancel);
        }
		}
	}


// End of File  

