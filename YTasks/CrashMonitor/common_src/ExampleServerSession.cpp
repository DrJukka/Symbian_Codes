/* 
	Copyright (c) 2006
	Jukka. Silvennoinen. 
	All rights reserved 
*/

#include "clientservercommon.h"
#include "ExampleServerSession.h"
#include <aknglobalconfirmationquery.h> 

#include <e32math.h>
#include <AknQueryDialog.h>
#include "Common.h"
/*
----------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------
*/	
void RExampleServerClient::Close()
	{
	RSessionBase::Close();  //basecall
	}
	
	

#ifdef __SERIES60_3X__
/*
----------------------------------------------------------------------------------------
// Standard server startup code
----------------------------------------------------------------------------------------
*/

static TInt StartServer()
{
	const TUidType serverUid(KNullUid,KNullUid,KUidCrashServerUID);

	// EPOC and EKA2 is easy, we just create a new server process. 
	// Simultaneous launching of two such processes should be detected 
	// when the second one attempts to create the server object, 
	// failing with KErrAlreadyExists.
	
	RProcess server;
	TInt r=server.Create(KCrashServerImg,KNullDesC);

	if (r!=KErrNone)
	{
		return r;
	}
	
	TRequestStatus stat;
	server.Rendezvous(stat);
	if (stat!=KRequestPending)
	{
		server.Kill(0);		// abort startup
	}
	else
	{	
		server.Resume();	// logon OK - start the server
	}
		
	User::WaitForRequest(stat);		// wait for start or death
	// we can't use the 'exit reason' if the server panicked as this
	// is the panic 'reason' and may be '0' which cannot be distinguished
	// from KErrNone
	r=(server.ExitType()==EExitPanic) ? KErrGeneral : stat.Int();
	server.Close();
	return r;
}

/*
----------------------------------------------------------------------------------------
// This is the standard retry pattern for server connection
----------------------------------------------------------------------------------------
*/
TInt RExampleServerClient::Connect()
{
	TInt retry=2;
	for (;;)
	{
		TInt r=CreateSession(KCrashServerName,
		TVersion(SServMajorVersionNumber,
                 SServMinorVersionNumber,
                 SServBuildVersionNumber)
		,1);
		if (r!=KErrNotFound && r!=KErrServerTerminated)
		{
			return r;
		}
		if (--retry==0)
		{
			return r;
		}
		r=StartServer();
		
		if (r!=KErrNone && r!=KErrAlreadyExists)
		{
			return r;
		}
	}
}

#else 
static const TUint KDefaultMessageSlots = 2;


#ifdef __WINS__
static const TUint KServerMinHeapSize =  0x1000;  //  4K
static const TUint KServerMaxHeapSize = 0x10000;  // 64K
#endif
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
static TInt StartLockServer();
static TInt CreateLockServerProcess();


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TInt RExampleServerClient::Connect()
{		
    TInt error = ::StartLockServer();			
    if (KErrNone == error)
        {
        error = CreateSession(KCrashServerName,
                              TVersion(SServMajorVersionNumber,
                 					   SServMinorVersionNumber,
                    					SServBuildVersionNumber),
                              KDefaultMessageSlots);
        }
    return error;
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
static TInt StartLockServer()
    {
    TInt result;

    TFindServer findTimeServer(KCrashServerName);
    TFullName name;

	result = findTimeServer.Next(name);
	if (result == KErrNone)
        {
		// Server already running
		return KErrNone;
	    }

	RSemaphore semaphore;		
	result = semaphore.CreateGlobal(KCrashServerSemaphoreName, 0);
    if (result != KErrNone)
        {
        return  result;
        }

    result = CreateLockServerProcess();
    if (result != KErrNone)
        {
        return  result;
        }

	semaphore.Wait();
	semaphore.Close();       

    return  KErrNone;
    }
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
static TInt CreateLockServerProcess()
    {
    TInt result;

	const TUidType serverUid(KNullUid, KNullUid, KCrashServerUid3);

#ifdef __WINS__

	RLibrary lib;
	result = lib.Load(KCrashServerImg, serverUid);
    if (result != KErrNone)
        {
        return  result;
        }

    //  Get the WinsMain function
	TLibraryFunction functionWinsMain = lib.Lookup(1);

    //  Call it and cast the result to a thread function
	TThreadFunction serverThreadFunction = reinterpret_cast<TThreadFunction>(functionWinsMain());

	TName threadName(KCrashServerName);

	// Append a random number to make it unique
	threadName.AppendNum(Math::Random(), EHex);

	RThread server;

	result = server.Create(threadName,   // create new server thread
							 serverThreadFunction, // thread's main function
							 KDefaultStackSize,
							 NULL,
							 &lib,
							 NULL,
							 KServerMinHeapSize,
							 KServerMaxHeapSize,
							 EOwnerProcess);

	lib.Close();	// if successful, server thread has handle to library now

    if (result != KErrNone)
        {
        return  result;
        }

	server.SetPriority(EPriorityMore);


#else

	RProcess server;

	result = server.Create(KCrashServerImg, _L(""), serverUid);
    if (result != KErrNone)
        {
        return  result;
        }

#endif

	server.Resume();
    server.Close();

    return  KErrNone;
    }

#endif


/*
----------------------------------------------------------------------------------------
****************** Client Server functions follow **************************************
----------------------------------------------------------------------------------------
*/	
TInt RExampleServerClient::ShutServerDownNow(void)
{
	return SendReceive(EExamplePServShutDown);
}
/*
----------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------
*/
TInt RExampleServerClient::ClearItemList(void)
{
	return SendReceive(EExamplePServClearList);
}

/*
----------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------
*/
TInt RExampleServerClient::GetItemL(TExampleItem& aItemBuffer)
{
	TInt Ret(KErrNone);
	
	TPckgBuf<TExampleItem> ItemBuffer(aItemBuffer);
	
#ifdef __SERIES60_3X__
	
	TIpcArgs args(&ItemBuffer);
	Ret = SendReceive(EExamplePServGetItem, args);
#else	
	TAny* messageParameters[KMaxMessageArguments];

    messageParameters[0] = static_cast<TAny*>(&ItemBuffer);
    
	Ret = SendReceive(EExamplePServGetItem, &messageParameters[0]);
#endif

	aItemBuffer.iName.Copy(ItemBuffer().iName);
    aItemBuffer.iExitCatogory.Copy(ItemBuffer().iExitCatogory);
    aItemBuffer.iProcess.Copy(ItemBuffer().iProcess);
    aItemBuffer.iExitType = ItemBuffer().iExitType;
    aItemBuffer.iExitReason = ItemBuffer().iExitReason;
    aItemBuffer.iTime = ItemBuffer().iTime;
    
	return Ret;	
}

/*
----------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------
*/
// function to retrieve count of the items in database
TInt RExampleServerClient::GetItemCount(TInt& aItemCount)
{ 
	TPckgBuf<TInt> CountBuffer;
  	
  	TInt Retp(KErrNone);
 
 #ifdef __SERIES60_3X__
	
	TIpcArgs args(&CountBuffer);
  	Retp = SendReceive(EExamplePServGetCount,args);
#else	
  	TAny* messageParameters[KMaxMessageArguments];

	messageParameters[0] = static_cast<TAny*>(&CountBuffer);
	
  	SendReceive(EExamplePServGetCount,&messageParameters[0]);

#endif 
  	aItemCount =CountBuffer();
  	
  	return Retp;
}
