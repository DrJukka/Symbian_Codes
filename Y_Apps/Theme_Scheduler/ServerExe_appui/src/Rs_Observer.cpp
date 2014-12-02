
#include "Rs_Observer.h"

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
CmyRsObserver* CmyRsObserver::NewLC(MMyRsObsCallBack &aCallBack,const TUid& aCategory, TUint aKey, TBool aIntType)
	{
	CmyRsObserver* self = new (ELeave) CmyRsObserver(aCallBack,aCategory,aKey,aIntType);
  	CleanupStack::PushL(self);
  	self->ConstructL();
  	return self;
	}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
CmyRsObserver* CmyRsObserver::NewL(MMyRsObsCallBack &aCallBack,const TUid& aCategory, TUint aKey, TBool aIntType)
	{
	CmyRsObserver* self = CmyRsObserver::NewLC(aCallBack,aCategory,aKey,aIntType);
  	CleanupStack::Pop(self);
  	return self;
	}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
CmyRsObserver::CmyRsObserver(MMyRsObsCallBack &aCallBack,const TUid& aCategory, TUint aKey, TBool aIntType)
:CActive(EPriorityStandard),iCallBack(aCallBack),iCategory(aCategory),iKey(aKey),iIntType(aIntType)
	{
	
	}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
CmyRsObserver::~CmyRsObserver()
	{
 	Cancel();
	delete iRepository;
	}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 

void CmyRsObserver::ConstructL()
{
 	CActiveScheduler::Add(this);
 	iRepository = CRepository::NewL(iCategory);
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 

void CmyRsObserver::StartMonitoringL()
{
	Cancel();
 	iRepository->NotifyRequest(iKey, iStatus );
 	SetActive();
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
TInt CmyRsObserver::SetPropertyL(TInt aValue)
{
	TInt err(KErrNone);
	Cancel();
	
	err = iRepository->Set(iKey,aValue);
	
	iRepository->NotifyRequest(iKey, iStatus );
	SetActive();
    
	return err;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
TInt CmyRsObserver::GetPropertyL(TInt& aValue)
{
	TInt err(KErrNone);
	Cancel();
	
	err = iRepository->Get(iKey,aValue);
	
	iRepository->NotifyRequest(iKey, iStatus );
    SetActive();
    
	return err;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
TInt CmyRsObserver::SetPropertyL(const TDesC8 aValue)
{
	TInt err(KErrNone);
	Cancel();
	
	err = iRepository->Set(iKey,aValue);
	
	iRepository->NotifyRequest(iKey, iStatus );
    SetActive();
    
	return err;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
TInt CmyRsObserver::GetPropertyL(TDes8& aValue)
{
	TInt err(KErrNone);
	Cancel();
	
	err = iRepository->Get(iKey,aValue);
	
	iRepository->NotifyRequest(iKey, iStatus );
    SetActive();
    
	return err;
}
/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/ 
void CmyRsObserver::RunL()
{
	iRepository->NotifyRequest(iKey, iStatus );
    SetActive();
    
  	if(iIntType)
    {
    	TInt valValue(0);
	    TInt err = iRepository->Get(iKey,valValue);
    	iCallBack.RsValueChangedL(this,valValue,err);
    }
    else
    {
    	TBuf8<255> bufValue;
	    TInt err = iRepository->Get(iKey,bufValue);
    	iCallBack.RsValueChangedL(this,bufValue,err);	
    }
}
/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/ 
void CmyRsObserver::DoCancel()
	{
		iRepository->NotifyCancel(iKey);
	}

