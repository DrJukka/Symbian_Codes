/* 
	Copyright (c) 2006, 
	Jukka Silvennoinen. 
	All rights reserved 
*/

#include "ExampleSession.h"

#include "clientservercommon.h"
#include <e32svr.h>


/*
----------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------
*/
void CExPolicy_ServerSession::CreateL()
	{
	Server().AddSession();// tell server we have new session
	}
/*
----------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------
*/
CExPolicy_ServerSession::~CExPolicy_ServerSession()
{
	Server().DropSession();	// tll server we are disconnecting
}
/*
----------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------
*/
// standard client request handling..
void CExPolicy_ServerSession::ServiceL(const RMessage2& aMessage)
	{

    switch (aMessage.Function())
        {
        case EExamplePServReadValueAndName:
            GetItemValueL(aMessage,ETrue);
            break;
        case EExamplePServSetValue:
        	SetItemValueL(aMessage,ETrue);
        	break;
        case EExamplePServDeleteItem:
        	DeleteValueL(aMessage,ETrue);
        	break;
        case EExamplePServGetCount:
        	GetItemCountL(aMessage,ETrue);
        	break;
        case EExamplePServSetValueAndName:
        	SetItemValueAndNameL(aMessage,ETrue);
        	break;
		case EGetItemListItem:
			GetListItemValueAndNameL(aMessage,ETrue);
        	break;
        	
        case EExamplePServReadValueAndName2:
            GetItemValueL(aMessage,EFalse);
            break;
        case EExamplePServSetValue2:
        	SetItemValueL(aMessage,EFalse);
        	break;
        case EExamplePServDeleteItem2:
        	DeleteValueL(aMessage,EFalse);
        	break;
        case EExamplePServGetCount2:
        	GetItemCountL(aMessage,EFalse);
        	break;
        case EExamplePServSetValueAndName2:
        	SetItemValueAndNameL(aMessage,EFalse);
        	break;
		case EGetItemListItem2:
			GetListItemValueAndNameL(aMessage,EFalse);
        	break;
        	
        case EGetEnabledProtect:
			GetEnabledStatusL(aMessage,ETrue);
        	break;
        case EGetEnabledScreen:
			GetEnabledStatusL(aMessage,EFalse);
        	break;
        case ESetEnabledProtect:
			SetEnabledStatusL(aMessage,ETrue);
        	break;
        case ESetEnabledScreen:
			SetEnabledStatusL(aMessage,EFalse);
        	break;
        	
        case ESetProtectPassword:
        	SetGetPasswordL(aMessage,ETrue);
        	break;
		case EGetProtectPassword:
			SetGetPasswordL(aMessage,EFalse);
        	break;
        	
        case EServerInitializedStatus:
			IsServerInitializedL(aMessage);
        	break;
        case EServerRegStatus:
			CheckRegStatusL(aMessage);
        	break;
        	
        case EServerRegisterNow:
			DoRegNowL(aMessage,EFalse);
        	break;
        	
        case EServerLockScreenOn:
			LockScreenOnOffL(aMessage,ETrue);
			break;
		case EServerLockScreenOff:
			LockScreenOnOffL(aMessage,EFalse);
			break;
		case EServerLockScreenStatus:
			LockScreenStatusL(aMessage);
        	break;
        	
        case EServerRegisterNowThai:
			DoRegNowL(aMessage,ETrue);
        	break;
        case EServerCheckCodeOk:
			IsCodeOk(aMessage);
        	break;
        default:
        	break;	
        }
    }

/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_ServerSession::LockScreenOnOffL(const RMessage2& aMessage,TBool aSetOn)
{        
	TBool EStatus = Server().LockScreenLockedUnLocked(aSetOn);
	
	TPckgBuf<TBool> ItemStatBuf(EStatus);
	
	aMessage.WriteL(0,ItemStatBuf);
    aMessage.Complete(KErrNone);	
}
 
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_ServerSession::LockScreenStatusL(const RMessage2& aMessage)
{        
	TBool EStatus = Server().HadNonClearedLockScreen();
	
	TPckgBuf<TBool> ItemStatBuf(EStatus);
	
	aMessage.WriteL(0,ItemStatBuf);
    aMessage.Complete(KErrNone);	
}
 
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_ServerSession::DoRegNowL(const RMessage2& aMessage,TBool aThai)
{        
	TInt Err(KErrNone);
	
	TPckgBuf<TNProtRegItem> ItemBuffer;
	
	TRAP(Err,
	aMessage.ReadL(0,ItemBuffer,0);
	
	if(aThai)
		Server().TryRegisterApp(ItemBuffer().iStatus);
	else
		Server().TryRegisterApp(ItemBuffer().iCode,ItemBuffer().iStatus,ItemBuffer().iStartTime);
	);
	
	aMessage.WriteL(0,ItemBuffer);
    aMessage.Complete(Err);	
}

/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_ServerSession::IsCodeOk(const RMessage2& aMessage)
{        
	TBool EStatus(EFalse);
	
	TInt Err(KErrNone);
	
	TPckgBuf<TNProtRegItem> ItemBuffer;
	
	TRAP(Err,
	aMessage.ReadL(0,ItemBuffer,0);
	
	EStatus = Server().CodeMatches(ItemBuffer().iCode);
	);	
	
	ItemBuffer().iFirstRun = EStatus;

	aMessage.WriteL(0,ItemBuffer);
    aMessage.Complete(Err);	
}
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_ServerSession::CheckRegStatusL(const RMessage2& aMessage)
{        
	TInt Err(KErrNone);
	
	TPckgBuf<TNProtRegItem> ItemBuffer;
	
	TRAP(Err,
	aMessage.ReadL(0,ItemBuffer,0);
	
	Server().CheckRegStatusL(ItemBuffer().iFirstRun,ItemBuffer().iDays,ItemBuffer().iStatus,ItemBuffer().iCode);
	);
	
	aMessage.WriteL(0,ItemBuffer);
    aMessage.Complete(Err);	
}

/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_ServerSession::SetGetPasswordL(const RMessage2& aMessage,TBool aSetIt)
{        
	TInt Err(KErrNone);
	
	TPckgBuf<TExampleItem> ItemBuffer;
	
	TRAP(Err,
	aMessage.ReadL(0,ItemBuffer,0);
	
	if(aSetIt)
		Server().SetPasswordL(ItemBuffer().iValue);
	else
		Server().GetPasswordL(ItemBuffer().iValue);
	);
	
	aMessage.WriteL(0,ItemBuffer);
    aMessage.Complete(Err);	
}



/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_ServerSession::IsServerInitializedL(const RMessage2& aMessage)
{        
	TBool EStatus = Server().IsReadyForAction();
	
	TPckgBuf<TBool> ItemStatBuf(EStatus);
	
	aMessage.WriteL(0,ItemStatBuf);
    aMessage.Complete(KErrNone);	
}
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_ServerSession::GetEnabledStatusL(const RMessage2& aMessage,TBool aProtect)
{        
	TBool EStatus = Server().GetStatus(aProtect);
	
	TPckgBuf<TBool> ItemStatBuf(EStatus);
	
	aMessage.WriteL(0,ItemStatBuf);
    aMessage.Complete(KErrNone);	
}

/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_ServerSession::SetEnabledStatusL(const RMessage2& aMessage,TBool aProtect)
{        
	TPckgBuf<TBool> StatusBuffer;
	
	aMessage.ReadL(0,StatusBuffer,0);
	
	TInt RetError = Server().SetStatusL(StatusBuffer(),aProtect);
	
	aMessage.WriteL(0,StatusBuffer);
    aMessage.Complete(RetError);	
}

/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
// used by client to get count of items in the database
void CExPolicy_ServerSession::GetItemCountL(const RMessage2& aMessage,TBool aImsi)
{        
	TInt ItemCount(0);
	
	TInt RetError = Server().GetItemCount(ItemCount,aImsi);
	
	TPckgBuf<TInt> ItemCountBuf(ItemCount);
	
	aMessage.WriteL(0,ItemCountBuf);
    aMessage.Complete(RetError);	
}
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
// used by client to delete items in the database
void CExPolicy_ServerSession::DeleteValueL(const RMessage2& aMessage,TBool aImsi)
{        
	TPckgBuf<TExampleItem> ItemBuffer;
	
	aMessage.ReadL(0,ItemBuffer,0);
	
	TInt RetError = Server().DeleteItemData(ItemBuffer(),aImsi);
	
    aMessage.Complete(RetError);	
} 


/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
// used by client to set name and value pairs of items in the database
void CExPolicy_ServerSession::SetItemValueAndNameL(const RMessage2& aMessage,TBool aImsi)
{        
	TPckgBuf<TExampleItem> ItemBuffer;
	
	aMessage.ReadL(0,ItemBuffer,0);
	
	TInt RetError = Server().SetItemData(ItemBuffer(),aImsi);
	
	aMessage.WriteL(0,ItemBuffer);
    aMessage.Complete(RetError);	
}  
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
// used by client to set name fields of items in the database
void CExPolicy_ServerSession::SetItemValueL(const RMessage2& aMessage,TBool aImsi)
{        
	TPckgBuf<TExampleItem> ItemBuffer;
	
	aMessage.ReadL(0,ItemBuffer,0);
	
	TInt RetError = Server().SetItemValueData(ItemBuffer(),aImsi);
	
	aMessage.WriteL(0,ItemBuffer);
    aMessage.Complete(RetError);	
} 
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
// used by client to set name and value pairs with list index, instead of database index
void CExPolicy_ServerSession::GetListItemValueAndNameL(const RMessage2& aMessage,TBool aImsi)
{        
	TPckgBuf<TExampleItem> ItemBuffer;
	
	aMessage.ReadL(0,ItemBuffer,0);
	
	TInt RetError = Server().GetListItemData(ItemBuffer(),aImsi);
	
	aMessage.WriteL(0,ItemBuffer);
    aMessage.Complete(RetError);	
}  
/* 
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
// used by client to set name and value pairs with database index
void CExPolicy_ServerSession::GetItemValueL(const RMessage2& aMessage,TBool aImsi)
{        
	TPckgBuf<TExampleItem> ItemBuffer;
	
	aMessage.ReadL(0,ItemBuffer,0);
	
	TInt RetError = Server().GetItemData(ItemBuffer(),aImsi);
	
	aMessage.WriteL(0,ItemBuffer);
    aMessage.Complete(RetError);	
}  
 
/*
----------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------
*/
void CExPolicy_ServerSession::ServiceError(const RMessage2& aMessage,TInt aError)
	{
	CSession2::ServiceError(aMessage,aError);
	}

