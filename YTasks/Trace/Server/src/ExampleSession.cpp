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
        case EExamplePServGetCount:
            GetItemsCountL(aMessage);
            break;
        case EExamplePServGetItem:
        	GetItemValueL(aMessage);
        	break;
        case EExamplePServShutDown:
        	ShutMeDownNowL(aMessage);
        	break;
        case EExamplePServClearList:
        	ClearListItemsL(aMessage);
        	break;
        case EExamplePServStop:
        	StopTraceL(aMessage);
        	break;
		case EExamplePServStart:
			StartTraceL(aMessage);
        	break;
        case EExamplePServGetCPUMax:
			GetCPUMaxL(aMessage);
        	break;
        case EExampleReReadSett:
			ReReadSettingsL(aMessage);
        	break;
        default:
        	break;	
        }
    }
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_ServerSession::ReReadSettingsL(const RMessage2& aMessage)
{        
	Server().ReReadSettingsL();
	
    aMessage.Complete(KErrNone);	
}
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_ServerSession::GetCPUMaxL(const RMessage2& aMessage)
{        
	TUint32	ItCunt = Server().GetMax();
	
	TPckgBuf<TUint32> ItemStatBuf(ItCunt);
	
	aMessage.WriteL(0,ItemStatBuf);
    aMessage.Complete(KErrNone);	
}
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_ServerSession::StopTraceL(const RMessage2& aMessage)
{        
	Server().StopTraceL();	
    aMessage.Complete(KErrNone);	
}
 
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_ServerSession::StartTraceL(const RMessage2& aMessage)
{        
	Server().StartTraceL();	
    aMessage.Complete(KErrNone);	
}
 
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_ServerSession::ClearListItemsL(const RMessage2& aMessage)
{        
	Server().ClearListItemsL();	
    aMessage.Complete(KErrNone);	
}
 
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_ServerSession::ShutMeDownNowL(const RMessage2& aMessage)
{        
	Server().StartShutDownTimer();	
    aMessage.Complete(KErrNone);	
}
 
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_ServerSession::GetItemsCountL(const RMessage2& aMessage)
{        
	TInt ItCunt = Server().GetItemsCount();
	
	TPckgBuf<TInt> ItemStatBuf(ItCunt);
	
	aMessage.WriteL(0,ItemStatBuf);
    aMessage.Complete(KErrNone);	
}
 
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_ServerSession::GetItemValueL(const RMessage2& aMessage)
{        
	TInt Err(KErrNone);

	TPckgBuf<TExampleItem> ItemData;
	
//	TRAP(Err,
	aMessage.ReadL(0,ItemData,0);
	
	Server().GetItemValueL(ItemData().iIndex,ItemData().iCpuLoad,ItemData().iMemory,ItemData().iTime);

	aMessage.WriteL(0,ItemData);
    aMessage.Complete(Err);	
}	
 
/*
----------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------
*/
void CExPolicy_ServerSession::ServiceError(const RMessage2& aMessage,TInt aError)
	{
	CSession2::ServiceError(aMessage,aError);
	}

