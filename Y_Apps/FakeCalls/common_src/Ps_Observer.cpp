
#include "Ps_Observer.h"

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
CmyPsObserver* CmyPsObserver::NewLC(MMyPsObsCallBack &aCallBack,const TUid& aCategory, TUint aKey, TBool aIntType)
	{
 	CmyPsObserver* self = new (ELeave) CmyPsObserver(aCallBack,aCategory,aKey,aIntType);
  	CleanupStack::PushL(self);
  	self->ConstructL();
  	return self;
	}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
CmyPsObserver* CmyPsObserver::NewL(MMyPsObsCallBack &aCallBack,const TUid& aCategory, TUint aKey, TBool aIntType)
	{
 	CmyPsObserver* self = CmyPsObserver::NewLC(aCallBack,aCategory,aKey,aIntType);
  	CleanupStack::Pop(self);
  	return self;
	}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
CmyPsObserver::CmyPsObserver(MMyPsObsCallBack &aCallBack,const TUid& aCategory, TUint aKey, TBool aIntType)
:CActive(EPriorityStandard),iCallBack(aCallBack),iCategory(aCategory),iKey(aKey),iIntType(aIntType)
	{
	
	}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
CmyPsObserver::~CmyPsObserver()
	{
 	Cancel();
	iProperty.Close();
	}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 

void CmyPsObserver::ConstructL()
{
 	CActiveScheduler::Add(this);
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
void CmyPsObserver::Start(TBool aDefine)
{
 	TInt r(KErrNone);
 	
 	if(aDefine)
 	{
	 	if(iIntType)
		 	r=RProperty::Define(iKey,RProperty::EInt,TSecurityPolicy(TSecurityPolicy::EAlwaysPass),TSecurityPolicy(TSecurityPolicy::EAlwaysPass)); 
		else
			r=RProperty::Define(iKey,RProperty::EByteArray,TSecurityPolicy(TSecurityPolicy::EAlwaysPass),TSecurityPolicy(TSecurityPolicy::EAlwaysPass),100);
 	}
 	
	if(r == KErrNone || r == KErrAlreadyExists)
	{	
		r = iProperty.Attach( iCategory, iKey );
	}
	
	if(r != KErrNone)
	{
	 	User::LeaveIfError(r);
	}
	else
	{
	 	iProperty.Subscribe( iStatus );
    	SetActive();
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
TInt CmyPsObserver::SetPropertyL(TInt aValue)
{
	TInt err(KErrNone);
	Cancel();
	
	err = iProperty.Set(aValue);//iCategory,iKey,
	
	iProperty.Subscribe( iStatus );
    SetActive();
    
	return err;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
TInt CmyPsObserver::GetPropertyL(TInt& aValue)
{
	TInt err(KErrNone);
	Cancel();
	
	err = iProperty.Get(aValue);
	
	iProperty.Subscribe(iStatus );
    SetActive();
    
	return err;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
TInt CmyPsObserver::SetPropertyL(const TDesC8 aValue)
{
	TInt err(KErrNone);
	Cancel();
	
	err = iProperty.Set(aValue);
	
	iProperty.Subscribe( iStatus );
    SetActive();
    
	return err;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
TInt CmyPsObserver::GetPropertyL(TDes8& aValue)
{
	TInt err(KErrNone);
	Cancel();
	
	err = iProperty.Get(aValue);
	
	iProperty.Subscribe(iStatus );
    SetActive();
    
	return err;
}
/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/ 
void CmyPsObserver::RunL()
{
  	iProperty.Subscribe( iStatus );
    SetActive();
    
  	if(iIntType)
    {
    	TInt valValue(0);
	    TInt err = iProperty.Get( valValue );
    	iCallBack.PsValueChangedL(valValue,err);
    }
    else
    {
    	TBuf8<255> bufValue;
	    TInt err = iProperty.Get(bufValue);
    	iCallBack.PsValueChangedL(bufValue,err);	
    }
}
/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/ 
void CmyPsObserver::DoCancel()
	{
	iProperty.Cancel();
	}

