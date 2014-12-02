/* Copyright (c) 2001 - 2004 , Jukka Silvennoinen Solution One Telecom LTd. All rights reserved */

#include "ExampleSession_2nd.h"
#include "clientservercommon.h"

#include <e32svr.h>

/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CExPolicy_ServerSession::CExPolicy_ServerSession(RThread& aClient, CExPolicy_Server& aServer)
:   CSession(aClient), iServer(aServer)
    {
    Server().AddSession();// tell server we have new session
    }
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CExPolicy_ServerSession::~CExPolicy_ServerSession()
{
    Server().DropSession();	// tll server we are disconnecting
}
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_ServerSession::ServiceL(const RMessage& aMessage)
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
        case EServerCheckSIM:
        	CheckSIMCard();
        	break;
        default:
        	break;	
        }
    }
/* 
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
void CExPolicy_ServerSession::CheckSIMCard(void)
{     
  	TRAPD(ERR,Server().CheckCurrentSIMOkL());
	Message().Complete(EServerCheckSIMComplete);  
}

/* 
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
void CExPolicy_ServerSession::LockScreenOnOffL(const RMessage& /*aMessage*/,TBool aSetOn)
{        
	TBool EStatus = Server().LockScreenLockedUnLocked(aSetOn);
	
	const TAny* clientsDescriptor = Message().Ptr0();
	TPckgBuf<TBool> ItemStatBuf(EStatus);
	
	Message().WriteL(clientsDescriptor, ItemStatBuf);
	
	if(aSetOn)
    	Message().Complete(EServerLockScreenOnComplete);
	else
		Message().Complete(EServerLockScreenOffComplete);
}
 
/* 
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
void CExPolicy_ServerSession::LockScreenStatusL(const RMessage& /*aMessage*/)
{        
	TBool EStatus = Server().HadNonClearedLockScreen();
	
	const TAny* clientsDescriptor = Message().Ptr0();
	TPckgBuf<TBool> ItemStatBuf(EStatus);
	
	Message().WriteL(clientsDescriptor, ItemStatBuf);;
    Message().Complete(EServerLockScreenStatusComplete);	
}
 
/* 
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
void CExPolicy_ServerSession::DoRegNowL(const RMessage& /*aMessage*/,TBool aThai)
{        
	TInt Err(KErrNone);
	
	const TAny* clientsDescriptor = Message().Ptr0();
	TPckgBuf<TNProtRegItem> ItemBuffer;
	
	TRAP(Err,
	Message().ReadL(Message().Ptr0(),ItemBuffer);
	
	if(aThai)
		Server().TryRegisterApp(ItemBuffer().iStatus);
	else
		Server().TryRegisterApp(ItemBuffer().iCode,ItemBuffer().iStatus,ItemBuffer().iStartTime);
	);
	
	Message().WriteL(clientsDescriptor, ItemBuffer);
	
	if(aThai)
	    Message().Complete(EServerRegisterNowThaiComplete);	
	else
		Message().Complete(EServerRegisterNowComplete);	
}


/* 
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
void CExPolicy_ServerSession::IsCodeOk(const RMessage& /*aMessage*/)
{        
	TBool EStatus(EFalse);
	
	TInt Err(KErrNone);
	
	const TAny* clientsDescriptor = Message().Ptr0();
	TPckgBuf<TNProtRegItem> ItemBuffer;
	
	TRAP(Err,
	Message().ReadL(Message().Ptr0(),ItemBuffer);
	
	EStatus = Server().CodeMatches(ItemBuffer().iCode);
	);	
	
	ItemBuffer().iFirstRun = EStatus;

	Message().WriteL(clientsDescriptor, ItemBuffer);;
    
    Message().Complete(EServerCheckCodeOkComplete);	
}
/* 
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
void CExPolicy_ServerSession::CheckRegStatusL(const RMessage& /*aMessage*/)
{         	
	const TAny* clientsDescriptor = Message().Ptr0();
    
    TPckgBuf<TNProtRegItem> ItemBuffer;
    	
    Server().CheckRegStatusL(ItemBuffer().iFirstRun,ItemBuffer().iDays,ItemBuffer().iStatus,ItemBuffer().iCode);
	
    Message().WriteL(clientsDescriptor, ItemBuffer);
    Message().Complete(EServerRegStatusComplete);   
}

/* 
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
void CExPolicy_ServerSession::SetGetPasswordL(const RMessage& /*aMessage*/,TBool aSetIt)
{        
	TInt Err(KErrNone);
	
	const TAny* clientsDescriptor = Message().Ptr0();
	
	TPckgBuf<TExampleItem> ItemBuffer;
	
	TRAP(Err,
	Message().ReadL(Message().Ptr0(),ItemBuffer);
	
	if(aSetIt)
		Server().SetPasswordL(ItemBuffer().iValue);
	else
		Server().GetPasswordL(ItemBuffer().iValue);
	);
	
	Message().WriteL(clientsDescriptor, ItemBuffer);
	
	if(aSetIt)
    	Message().Complete(ESetProtectPasswordComplete);	
	else
		Message().Complete(EGetProtectPasswordComplete);	
}



/* 
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
void CExPolicy_ServerSession::IsServerInitializedL(const RMessage& /*aMessage*/)
{        
	TBool EStatus = Server().IsReadyForAction();
	
	TPckgBuf<TBool> ItemStatBuf(EStatus);	

	const TAny* clientsDescriptor = Message().Ptr0();
    
    Message().WriteL(clientsDescriptor, ItemStatBuf);
    Message().Complete(EServerInitializedStatusComplete);
}
/* 
-----------------------------------------------------------------------------
;
-----------------------------------------------------------------------------
*/
void CExPolicy_ServerSession::GetEnabledStatusL(const RMessage& /*aMessage*/,TBool aProtect)
{     
	const TAny* clientsDescriptor = Message().Ptr0();   
	TBool EStatus = Server().GetStatus(aProtect);
	
	TPckgBuf<TBool> ItemStatBuf(EStatus);
	
	Message().WriteL(clientsDescriptor, ItemStatBuf);
	
	if(aProtect)
    	Message().Complete(EGetEnabledProtectComplete);
	else
		Message().Complete(EGetEnabledScreenComplete);
}

/* 
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
void CExPolicy_ServerSession::SetEnabledStatusL(const RMessage& /*aMessage*/,TBool aProtect)
{      
	const TAny* clientsDescriptor = Message().Ptr0();  
	TPckgBuf<TBool> StatusBuffer;
	
	Message().ReadL(Message().Ptr0(),StatusBuffer);
	
	TInt RetError = Server().SetStatusL(StatusBuffer(),aProtect);
	
	Message().WriteL(clientsDescriptor, StatusBuffer);
    
    if(aProtect)
    	Message().Complete(ESetEnabledProtectComplete);	
    else
    	Message().Complete(ESetEnabledScreenComplete);	
}

/* 
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
// used by client to get count of items in the database
void CExPolicy_ServerSession::GetItemCountL(const RMessage& /*aMessage*/,TBool aImsi)
{      
	const TAny* clientsDescriptor = Message().Ptr0();  
	TInt ItemCount(0);
	
	TInt RetError = Server().GetItemCount(ItemCount,aImsi);
	
	TPckgBuf<TInt> ItemCountBuf(ItemCount);
	
	Message().WriteL(clientsDescriptor, ItemCountBuf);
	
	if(aImsi)
    	Message().Complete(EExamplePServGetCountComplete);
	else
		Message().Complete(EExamplePServGetCount2Complete);
}
/* 
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
// used by client to delete items in the database
void CExPolicy_ServerSession::DeleteValueL(const RMessage& /*aMessage*/,TBool aImsi)
{      
	const TAny* clientsDescriptor = Message().Ptr0();  
	TPckgBuf<TExampleItem> ItemBuffer;
	
	Message().ReadL(Message().Ptr0(),ItemBuffer);
	
	TInt RetError = Server().DeleteItemData(ItemBuffer(),aImsi);
	
	if(aImsi)
    	Message().Complete(EExamplePServDeleteItemComplete);
	else
		Message().Complete(EExamplePServDeleteItem2Complete);
} 


/* 
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
// used by client to set name and value pairs of items in the database
void CExPolicy_ServerSession::SetItemValueAndNameL(const RMessage& /*aMessage*/,TBool aImsi)
{      
	const TAny* clientsDescriptor = Message().Ptr0();  
	TPckgBuf<TExampleItem> ItemBuffer;
	
	 Message().ReadL(Message().Ptr0(),ItemBuffer);
	
	TInt RetError = Server().SetItemData(ItemBuffer(),aImsi);
	
	Message().WriteL(clientsDescriptor, ItemBuffer);
	
	if(aImsi)
    	Message().Complete(EExamplePServSetValueAndNameComplete);
	else
		Message().Complete(EExamplePServSetValueAndName2Complete);
}  
/* 
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
// used by client to set name fields of items in the database
void CExPolicy_ServerSession::SetItemValueL(const RMessage& /*aMessage*/,TBool aImsi)
{     
	const TAny* clientsDescriptor = Message().Ptr0();   
	TPckgBuf<TExampleItem> ItemBuffer;
	
	Message().ReadL(Message().Ptr0(),ItemBuffer);
	
	TInt RetError = Server().SetItemValueData(ItemBuffer(),aImsi);
	
	Message().WriteL(clientsDescriptor, ItemBuffer);
	
	if(aImsi)
    	Message().Complete(EExamplePServSetValueComplete);
	else
		Message().Complete(EExamplePServSetValue2Complete);
} 
/* 
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
// used by client to set name and value pairs with list index, instead of database index
void CExPolicy_ServerSession::GetListItemValueAndNameL(const RMessage& /*aMessage*/,TBool aImsi)
{        
	const TAny* clientsDescriptor = Message().Ptr0();
	TPckgBuf<TExampleItem> ItemBuffer;
	
	Message().ReadL(Message().Ptr0(),ItemBuffer);
	
	TInt RetError = Server().GetListItemData(ItemBuffer(),aImsi);
	
	Message().WriteL(clientsDescriptor, ItemBuffer);
	
	if(aImsi)
	    Message().Complete(EGetItemListItemComplete);
	else
		Message().Complete(EGetItemListItem2Complete);
}  
/* 
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
// used by client to set name and value pairs with database index
void CExPolicy_ServerSession::GetItemValueL(const RMessage& /*aMessage*/,TBool aImsi)
{     
	const TAny* clientsDescriptor = Message().Ptr0();   
	TPckgBuf<TExampleItem> ItemBuffer;
	
	Message().ReadL(Message().Ptr0(),ItemBuffer);
	
	TInt RetError = Server().GetItemData(ItemBuffer(),aImsi);
	
	Message().WriteL(clientsDescriptor, ItemBuffer);
	
	if(aImsi)
    	Message().Complete(EExamplePServReadValueAndNameComplete);
	else
		Message().Complete(EExamplePServReadValueAndName2Complete);
}  
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_ServerSession::PanicClient(TInt aPanic) const
	{
	Panic(KExapmpleServerName, aPanic) ; // Note: this panics the client thread, not server
	}
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CExPolicy_ServerSession* CExPolicy_ServerSession::NewL(RThread& aClient, CExPolicy_Server& aServer)
    {
    CExPolicy_ServerSession* self = CExPolicy_ServerSession::NewLC(aClient, aServer) ;
    CleanupStack::Pop(self) ;
    return self ;
    }
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CExPolicy_ServerSession* CExPolicy_ServerSession::NewLC(RThread& aClient, CExPolicy_Server& aServer)
    {
    CExPolicy_ServerSession* self = new (ELeave) CExPolicy_ServerSession(aClient, aServer);
    CleanupStack::PushL(self) ;
    self->ConstructL() ;
    return self ;
    }
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_ServerSession::ConstructL()
    {
    
    }

	