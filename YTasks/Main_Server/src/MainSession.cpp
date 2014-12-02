/* 
	Copyright (c) 2006, 
	Jukka Silvennoinen. 
	All rights reserved 
*/

#include "MainSession.h"

#include "Mainservercommon.h"
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
      	case EMainServSetFileMapping:
            SetFileMappingL(aMessage);
            break;
		case EMainServKillTask:
            KillTaskL(aMessage);
            break;
        case EMainServKillProcess:
            KillProcessL(aMessage);
            break;
        case EMainServKillThread:
            KillThreadL(aMessage);
            break;
        case EMainServGetEInfo:
            GetExtraInfoL(aMessage);
            break;
        default:
        	break;	
        }
    }

/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_ServerSession::GetExtraInfoL(const RMessage2& aMessage)
{        
	TInt Err(KErrNone);
	
	TExtraInfoItem Dattta;
	Server().GetExtraInfoL(Dattta);
	
	TPckgBuf<TExtraInfoItem> ItemData(Dattta);
	aMessage.WriteL(0,ItemData);
    aMessage.Complete(Err);		
}
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_ServerSession::KillThreadL(const RMessage2& aMessage)
{        
	TInt Err(KErrNone);

	TBuf8<255> Hjelpper;
	
//	TRAP(Err,
	aMessage.ReadL(0,Hjelpper,0);
	
	Server().KillThread(Hjelpper);
	
    aMessage.Complete(Err);		
}
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_ServerSession::KillProcessL(const RMessage2& aMessage)
{        
	TInt Err(KErrNone);

	TBuf8<255> Hjelpper;
	
//	TRAP(Err,
	aMessage.ReadL(0,Hjelpper,0);
	
	Server().KillProcess(Hjelpper);
	
    aMessage.Complete(Err);		
}
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_ServerSession::KillTaskL(const RMessage2& aMessage)
{        
	TInt Err(KErrNone);

	TPckgBuf<TUid> ItemData;
	
//	TRAP(Err,
	aMessage.ReadL(0,ItemData,0);
	
	Err = Server().KillTasks(ItemData());
	
	aMessage.WriteL(0,ItemData);
    aMessage.Complete(Err);		
}
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_ServerSession::SetFileMappingL(const RMessage2& aMessage)
{        
	TInt Err(KErrNone);

	TPckgBuf<TFileMappingItem> ItemData;
	
//	TRAP(Err,
	aMessage.ReadL(0,ItemData,0);
	
	Err = Server().SetFileMapping(ItemData());

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

