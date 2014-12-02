/* 
	Copyright (c) 2006
	Jukka. Silvennoinen. 
	All rights reserved 
*/

#include "Mainservercommon.h"
#include "MainServerSession.h"
//#include <aknglobalconfirmationquery.h> 

#include <e32math.h>
//#include <AknQueryDialog.h>

/*
----------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------
*/	
void RMainServerClient::Close()
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
	const TUidType serverUid(KNullUid,KNullUid,KMAINServerUid3);

	// EPOC and EKA2 is easy, we just create a new server process. 
	// Simultaneous launching of two such processes should be detected 
	// when the second one attempts to create the server object, 
	// failing with KErrAlreadyExists.
	
	RProcess server;
	TInt r=server.Create(KMainTaskServerImg,KNullDesC);

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
TInt RMainServerClient::Connect()
{
	TInt retry=2;
	for (;;)
	{
		TInt r=CreateSession(KMainTaskServerName,
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
TInt RMainServerClient::Connect()
{		
    TInt error = ::StartLockServer();			
    if (KErrNone == error)
        {
        error = CreateSession(KMainTaskServerName,
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

    TFindServer findTimeServer(KMainTaskServerName);
    TFullName name;

	result = findTimeServer.Next(name);
	if (result == KErrNone)
        {
		// Server already running
		return KErrNone;
	    }

	RSemaphore semaphore;		
	result = semaphore.CreateGlobal(KMainTaskServerSemaphoreName, 0);
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

	const TUidType serverUid(KNullUid, KNullUid, KMAINServerUid3);

#ifdef __WINS__

	RLibrary lib;
	result = lib.Load(KMainTaskServerImg, serverUid);
    if (result != KErrNone)
        {
        return  result;
        }

    //  Get the WinsMain function
	TLibraryFunction functionWinsMain = lib.Lookup(1);

    //  Call it and cast the result to a thread function
	TThreadFunction serverThreadFunction = reinterpret_cast<TThreadFunction>(functionWinsMain());

	TName threadName(KMainTaskServerName);

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

	result = server.Create(KMainTaskServerImg, _L(""), serverUid);
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

TInt RMainServerClient::GetExtraInfoL(TExtraInfoItem& aItemBuffer)
{
	TInt Ret(KErrNone);
	
	TPckgBuf<TExtraInfoItem> ItemBuffer(aItemBuffer);
	
#ifdef __SERIES60_3X__
	
	TIpcArgs args(&ItemBuffer);
	Ret = SendReceive(EMainServGetEInfo, args);
	
//	TAny* messageParameters[KMaxMessageArguments];
//  messageParameters[0] = static_cast<TAny*>(&ItemBuffer);
//	Ret = SendReceive(EMainServGetEInfo, &messageParameters[0]);
#else	
	TIpcArgs args(&ItemBuffer);
	Ret = SendReceive(EMainServGetEInfo, args);	
#endif

	aItemBuffer.iCountryCode.Copy(ItemBuffer().iCountryCode);
	aItemBuffer.iNwId.Copy(ItemBuffer().iNwId);
	aItemBuffer.iImsi.Copy(ItemBuffer().iImsi);
	
    aItemBuffer.iAreaCode = ItemBuffer().iAreaCode;
    aItemBuffer.iCellId = ItemBuffer().iCellId;
    
	return Ret;	
}
/*
----------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------
*/
TInt RMainServerClient::SetFileMappingL(TFileMappingItem& aDataMapping)
{
	TInt Ret(KErrNone);
	
	TPckgBuf<TFileMappingItem> ItemBuffer(aDataMapping);
	
#ifdef __SERIES60_3X__
	TIpcArgs args(&ItemBuffer);
	Ret = SendReceive(EMainServSetFileMapping, args);
//	TAny* messageParameters[KMaxMessageArguments];
//    messageParameters[0] = static_cast<TAny*>(&ItemBuffer);
//	Ret = SendReceive(EMainServSetFileMapping, &messageParameters[0]);
#else	
	TIpcArgs args(&ItemBuffer);
	Ret = SendReceive(EMainServSetFileMapping, args);
#endif

    aDataMapping.iDataType = ItemBuffer().iDataType;
    aDataMapping.iUid = ItemBuffer().iUid;
    aDataMapping.iPriority= ItemBuffer().iPriority;
        	
	return Ret;	
}


/*
----------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------
*/
TInt RMainServerClient::KillTaskL(TUid& aUid)
{
	TInt Ret(KErrNone);
	
	TPckgBuf<TUid> ItemBuffer(aUid);
	
#ifdef __SERIES60_3X__
	TIpcArgs args(&ItemBuffer);
	Ret = SendReceive(EMainServKillTask, args);
//	TAny* messageParameters[KMaxMessageArguments];
//    messageParameters[0] = static_cast<TAny*>(&ItemBuffer);
//	Ret = SendReceive(EMainServKillTask, &messageParameters[0]);
#else	
	TIpcArgs args(&ItemBuffer);
	Ret = SendReceive(EMainServKillTask, args);	
#endif

   	aUid = ItemBuffer();
	
	return Ret;	
}
/*
----------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------
*/
TInt RMainServerClient::KillThreadL(TDes8& aName)
{
	TInt Ret(KErrNone);
	
#ifdef __SERIES60_3X__	
	TIpcArgs args(&aName);
	Ret = SendReceive(EMainServKillThread, args);
//	TAny* messageParameters[KMaxMessageArguments];
//    messageParameters[0] = static_cast<TAny*>(&aName);   
//	Ret = SendReceive(EMainServKillThread, &messageParameters[0]);
#else	
	TIpcArgs args(&aName);
	Ret = SendReceive(EMainServKillThread, args);
#endif

	return Ret;	
}
/*
----------------------------------------------------------------------------------------
	
----------------------------------------------------------------------------------------
*/
TInt RMainServerClient::KillProcessL(TDes8& aName)
{
	TInt Ret(KErrNone);
	
#ifdef __SERIES60_3X__	
	TIpcArgs args(&aName);
	Ret = SendReceive(EMainServKillProcess, args);
//	TAny* messageParameters[KMaxMessageArguments];
//    messageParameters[0] = static_cast<TAny*>(&aName);  
//	Ret = SendReceive(EMainServKillProcess, &messageParameters[0]);
#else	
	TIpcArgs args(&aName);
	Ret = SendReceive(EMainServKillProcess, args);
#endif

	return Ret;		
}


