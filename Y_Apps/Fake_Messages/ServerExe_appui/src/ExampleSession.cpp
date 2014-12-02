/* Copyright (c) 2001 - 2008, Dr. Jukka Silvennoinen. All rights reserved */


#include "ExampleSession.h"

#include "clientservercommon.h"
#include <e32svr.h>


/*
----------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------
*/
void CExPolicy_ServerSession::CreateL()
	{
	
	}
/*
----------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------
*/
CExPolicy_ServerSession::~CExPolicy_ServerSession()
{
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
        case EExampleReReadValues:
            AskUpdateValuesL(aMessage);
            break;
        default:
        	break;	
        }
    }


/* 
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/

void CExPolicy_ServerSession::AskUpdateValuesL(const RMessage2& aMessage)
{        
	Server().AskUpdateValuesL();
	
	aMessage.Complete(KErrNone);
}  
 
/*
----------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------
*/
void CExPolicy_ServerSession::ServiceError(const RMessage2& aMessage,TInt aError)
	{
	CSession2::ServiceError(aMessage,aError);
	}

