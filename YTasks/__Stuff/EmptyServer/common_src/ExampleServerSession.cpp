/* 
	Copyright (c) 2006
	Jukka. Silvennoinen. 
	All rights reserved 
*/

#include "clientservercommon.h"
#include "ExampleServerSession.h"


#include <e32math.h>
#include <AknQueryDialog.h>

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
	const TUidType serverUid(KNullUid,KNullUid,KExapmpleServerUid3);

	// EPOC and EKA2 is easy, we just create a new server process. 
	// Simultaneous launching of two such processes should be detected 
	// when the second one attempts to create the server object, 
	// failing with KErrAlreadyExists.
	
	RProcess server;
	TInt r=server.Create(KExapmpleServerImg,KNullDesC);

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
		TInt r=CreateSession(KExapmpleServerName,
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
        error = CreateSession(KExapmpleServerName,
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

    TFindServer findTimeServer(KExapmpleServerName);
    TFullName name;

	result = findTimeServer.Next(name);
	if (result == KErrNone)
        {
		// Server already running
		return KErrNone;
	    }

	RSemaphore semaphore;		
	result = semaphore.CreateGlobal(KExapmpleServerSemaphoreName, 0);
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

	const TUidType serverUid(KNullUid, KNullUid, KExapmpleServerUid3);

#ifdef __WINS__

	RLibrary lib;
	result = lib.Load(KExapmpleServerImg, serverUid);
    if (result != KErrNone)
        {
        return  result;
        }

    //  Get the WinsMain function
	TLibraryFunction functionWinsMain = lib.Lookup(1);

    //  Call it and cast the result to a thread function
	TThreadFunction serverThreadFunction = reinterpret_cast<TThreadFunction>(functionWinsMain());

	TName threadName(KExapmpleServerName);

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

	result = server.Create(KExapmpleServerImg, _L(""), serverUid);
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
TInt RExampleServerClient::DoSIMCheckNow(void)
{
	return SendReceive(EServerCheckSIM);
}

/*
----------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------
*/
TBool RExampleServerClient::SetLockScreenStatus(TBool aStatus)
{
	TPckgBuf<TBool> CountBuffer;

#ifdef __SERIES60_3X__
	TIpcArgs args(&CountBuffer);

	if(aStatus)
  		SendReceive(EServerLockScreenOn,args);
	else
		SendReceive(EServerLockScreenOff,args);
#else
	TAny* messageParameters[KMaxMessageArguments];

	messageParameters[0] = static_cast<TAny*>(&CountBuffer);

	if(aStatus)
  		SendReceive(EServerLockScreenOn,&messageParameters[0]);
	else
		SendReceive(EServerLockScreenOff,&messageParameters[0]);
	
#endif

  	TBool Pillu = CountBuffer();
  	
  	Pillu = Pillu;
  	
  	return Pillu;
}

/*
----------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------
*/
TBool RExampleServerClient::LastLockScreenStatus(void)
{
	TPckgBuf<TBool> CountBuffer;

#ifdef __SERIES60_3X__
	TIpcArgs args(&CountBuffer);

  	SendReceive(EServerLockScreenStatus,args);
#else
	TAny* messageParameters[KMaxMessageArguments];

	messageParameters[0] = static_cast<TAny*>(&CountBuffer);
    	
	SendReceive(EServerLockScreenStatus, &messageParameters[0]);
#endif
  	
  	TBool Pillu = CountBuffer();
  	
  	Pillu = Pillu;
  	
  	return Pillu;
}

/*
----------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------
*/
TBool RExampleServerClient::IsCodeOk(const TDesC& aCode)
{
	TBool Ret(EFalse);
	
	if(aCode.Length())
	{
		TPckgBuf<TNProtRegItem> ItemBuffer;
	
		ItemBuffer().iCode.Copy(aCode);
	#ifdef __SERIES60_3X__
	
		TIpcArgs args(&ItemBuffer);

	  	SendReceive(EServerCheckCodeOk,args);
	
	#else
		TAny* messageParameters[KMaxMessageArguments];

		messageParameters[0] = static_cast<TAny*>(&ItemBuffer);
    	
		SendReceive(EServerCheckCodeOk, &messageParameters[0]);
	#endif
	
		Ret= ItemBuffer().iFirstRun;
	}
	
	return Ret;
}
/*
----------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------
*/
TBool RExampleServerClient::ServerInitialized(void)
{
	TPckgBuf<TBool> CountBuffer;

#ifdef __SERIES60_3X__
	TIpcArgs args(&CountBuffer);
	
	SendReceive(EServerInitializedStatus, args);
#else
	TAny* messageParameters[KMaxMessageArguments];

	messageParameters[0] = static_cast<TAny*>(&CountBuffer);
    
	SendReceive(EServerInitializedStatus, &messageParameters[0]);
#endif
  
  	return CountBuffer();
}
/*
----------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------
*/
TInt RExampleServerClient::CheckRegStatusL(TBool& aFirstStart, TInt& aDaysLeft, TUint& aResult,TDes& aOperator)
{
	TInt Ret(KErrNone);
	
	TPckgBuf<TNProtRegItem> ItemBuffer;

#ifdef __SERIES60_3X__
	
	TIpcArgs args(&ItemBuffer);
	Ret = SendReceive(EServerRegStatus, args);
#else	
	TAny* messageParameters[KMaxMessageArguments];

    messageParameters[0] = static_cast<TAny*>(&ItemBuffer);
    
	SendReceive(EServerRegStatus, &messageParameters[0]);
#endif

	aResult 	= ItemBuffer().iStatus;
	aFirstStart	= ItemBuffer().iFirstRun;
	aDaysLeft	= ItemBuffer().iDays;
	aOperator.Copy(ItemBuffer().iCode);
	
	return Ret;	
}
/*
----------------------------------------------------------------------------------------
	TAny* messageParameters[KMaxMessageArguments];

	messageParameters[0] = static_cast<TAny*>(&CountBuffer);
    
	SendReceive(EServerInitializedStatus, &messageParameters[0]);
----------------------------------------------------------------------------------------
*/
TInt RExampleServerClient::TryRegisterApp(TDesC& aBuffer, TUint& aResult, TTime& aStartTime)
{
	TInt Ret(KErrNone);
		
	TPckgBuf<TNProtRegItem> ItemBuffer;

	ItemBuffer().iCode.Copy(aBuffer);
	ItemBuffer().iStartTime = aStartTime;
	
#ifdef __SERIES60_3X__
	
	TIpcArgs args(&ItemBuffer);
	Ret = SendReceive(EServerRegisterNow, args);
#else

	TAny* messageParameters[KMaxMessageArguments];

	messageParameters[0] = static_cast<TAny*>(&ItemBuffer);
    
	SendReceive(EServerRegisterNow, &messageParameters[0]);
#endif		
	aResult = ItemBuffer().iStatus;
	
	return Ret;	
}
/*
----------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------
*/	
TInt RExampleServerClient::TryRegisterApp(TUint& aResult)
{
	TInt Ret(KErrNone);
	
	TPckgBuf<TNProtRegItem> ItemBuffer;

#ifdef __SERIES60_3X__
	
	TIpcArgs args(&ItemBuffer);
	Ret = SendReceive(EServerRegisterNowThai,args);
#else	
	TAny* messageParameters[KMaxMessageArguments];

	messageParameters[0] = static_cast<TAny*>(&ItemBuffer);
    
	SendReceive(EServerRegisterNowThai, &messageParameters[0]);
#endif
	aResult = ItemBuffer().iStatus;
	
	return Ret;
}

/*
----------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------
*/
// function to retrieve count of the items in database
TInt RExampleServerClient::GetItemCount(TInt& aItemCount,TBool aImsi)
{ 
	TPckgBuf<TInt> CountBuffer;
  	
  	TInt Retp(KErrNone);
 
 #ifdef __SERIES60_3X__
	
	TIpcArgs args(&CountBuffer);
	if(aImsi)
  		Retp = SendReceive(EExamplePServGetCount,args);
  	else
  		Retp = SendReceive(EExamplePServGetCount2,args);
#else	
  	TAny* messageParameters[KMaxMessageArguments];

	messageParameters[0] = static_cast<TAny*>(&CountBuffer);
	
  	if(aImsi)
  		SendReceive(EExamplePServGetCount,&messageParameters[0]);
  	else
  		SendReceive(EExamplePServGetCount2,&messageParameters[0]);
#endif 
  	aItemCount =CountBuffer();
  	
  	return Retp;
}
/*
----------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------
*/
TBool RExampleServerClient::GetStatus(TBool aProtect)
{
	TPckgBuf<TBool> CountBuffer;

#ifdef __SERIES60_3X__
	
	TIpcArgs args(&CountBuffer);

	if(aProtect)
  		SendReceive(EGetEnabledProtect,args);
  	else
  		SendReceive(EGetEnabledScreen,args);
#else	
	TAny* messageParameters[KMaxMessageArguments];

	messageParameters[0] = static_cast<TAny*>(&CountBuffer);

  	if(aProtect)
  		SendReceive(EGetEnabledProtect,&messageParameters[0]);
  	else
  		SendReceive(EGetEnabledScreen,&messageParameters[0]);
#endif
 
  	return CountBuffer();
}
/*
----------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------
*/
TInt RExampleServerClient::SetStatus(TBool aProtect,TBool& On)
{
	TInt Ret(KErrNone);
	TPckgBuf<TBool> CountBuffer;
	
	CountBuffer() = On;

#ifdef __SERIES60_3X__
	
	TIpcArgs args(&CountBuffer);
	
	if(aProtect)
  		Ret =SendReceive(ESetEnabledProtect,args);
  	else
  		Ret =SendReceive(ESetEnabledScreen,args);
#else	
	TAny* messageParameters[KMaxMessageArguments];

	messageParameters[0] = static_cast<TAny*>(&CountBuffer);
  	
  	if(aProtect)
  		SendReceive(ESetEnabledProtect,&messageParameters[0]);
  	else
  		SendReceive(ESetEnabledScreen,&messageParameters[0]);
#endif

  	return Ret;
}
/*
----------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------
*/
// function to retrieve selected (indicated by listbox/item array index, i.e. from 0 to (Count- 1)) name value pair
TInt RExampleServerClient::GetListItemValueAndName(TDes& aName,TDes& aValue,TInt& aListIndex,TBool aImsi)
{
	TInt Ret(KErrNone);
	TPckgBuf<TExampleItem> ItemBuffer;
	ItemBuffer().iIndex = aListIndex;

#ifdef __SERIES60_3X__
	
	TIpcArgs args(&ItemBuffer);
	
	if(aImsi)
  		Ret = SendReceive(EGetItemListItem,args);
  	else
  		Ret = SendReceive(EGetItemListItem2,args);
  	
#else
	
	TAny* messageParameters[KMaxMessageArguments];

	messageParameters[0] = static_cast<TAny*>(&ItemBuffer);
  	
  	if(aImsi)
  		SendReceive(EGetItemListItem,&messageParameters[0]);
  	else
  		SendReceive(EGetItemListItem2,&messageParameters[0]);
#endif
	
	aName.Copy(ItemBuffer().iName);
	aValue.Copy(ItemBuffer().iValue);
	aListIndex = ItemBuffer().iIndex;
	
	return Ret;
}
/*
----------------------------------------------------------------------------------------
;
----------------------------------------------------------------------------------------
*/
// function to retrieve selected (indicated by database index) name value pair
TInt RExampleServerClient::GetItemValueAndName(TDes& aName,TDes& aValue,TInt& aIndex,TBool aImsi)
{
	TInt Ret(KErrNone);
	TPckgBuf<TExampleItem> ItemBuffer;
	ItemBuffer().iIndex = aIndex;

#ifdef __SERIES60_3X__
	
	TIpcArgs args(&ItemBuffer);

  	if(aImsi)
  		Ret = SendReceive(EExamplePServReadValueAndName,args);
	else
		Ret = SendReceive(EExamplePServReadValueAndName2,args);
	
#else	
	TAny* messageParameters[KMaxMessageArguments];

	messageParameters[0] = static_cast<TAny*>(&ItemBuffer);
  	
  	if(aImsi)
  		SendReceive(EExamplePServReadValueAndName,&messageParameters[0]);
	else
		SendReceive(EExamplePServReadValueAndName2,&messageParameters[0]);
#endif

	aName.Copy(ItemBuffer().iName);
	aValue.Copy(ItemBuffer().iValue);
	aIndex = ItemBuffer().iIndex;
	
	return Ret;
}
/*
----------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------
*/
// function to set selected (indicated by database index) name value pair
TInt RExampleServerClient::SetItemValueAndName(const TDesC& aName,const TDesC& aValue,TInt& aIndex,TBool aImsi)
{
	TInt Ret(KErrNone);
	TPckgBuf<TExampleItem> ItemBuffer;

	ItemBuffer().iName.Copy(aName);
	ItemBuffer().iValue.Copy(aValue);
	ItemBuffer().iIndex = aIndex;

#ifdef __SERIES60_3X__
	
	TIpcArgs args(&ItemBuffer);

 	if(aImsi)
  		Ret = SendReceive(EExamplePServSetValueAndName,args);
  	else
  		Ret = SendReceive(EExamplePServSetValueAndName2,args);
	
#else	
	TAny* messageParameters[KMaxMessageArguments];

	messageParameters[0] = static_cast<TAny*>(&ItemBuffer);
  	
  	if(aImsi)
  		SendReceive(EExamplePServSetValueAndName,&messageParameters[0]);
  	else
  		SendReceive(EExamplePServSetValueAndName2,&messageParameters[0]);
#endif  	

	aIndex = ItemBuffer().iIndex;

	return Ret;
}
/*
----------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------
*/
// function to set selected (indicated by database index) item value only, while keeping the item name
TInt RExampleServerClient::SetItemValue(const TDesC& aValue,TInt& aIndex,TBool aImsi)
{
	TInt Ret(KErrNone);
	TPckgBuf<TExampleItem> ItemBuffer;

	ItemBuffer().iValue.Copy(aValue);
	ItemBuffer().iIndex = aIndex;

#ifdef __SERIES60_3X__
	
	TIpcArgs args(&ItemBuffer);
	
	if(aImsi)
  		Ret = SendReceive(EExamplePServSetValue,args);
  	else
  		Ret = SendReceive(EExamplePServSetValue2,args);
  	
#else	
	TAny* messageParameters[KMaxMessageArguments];

	messageParameters[0] = static_cast<TAny*>(&ItemBuffer);
  	
  	if(aImsi)
  		SendReceive(EExamplePServSetValue,&messageParameters[0]);
  	else
  		SendReceive(EExamplePServSetValue2,&messageParameters[0]);
#endif

	aIndex = ItemBuffer().iIndex;

	return Ret;
}

/*
----------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------
*/
// function to delete selected (indicated by database index) item
TInt RExampleServerClient::DeleteItem(const TDesC& aName,const TDesC& aValue,TInt& aIndex,TBool aImsi)
{
	TPckgBuf<TExampleItem> ItemBuffer;
	
	ItemBuffer().iName.Copy(aName);
	ItemBuffer().iValue.Copy(aValue);
	ItemBuffer().iIndex = aIndex;
	
	TInt Ret(KErrNone);
	
#ifdef __SERIES60_3X__
	
	TIpcArgs args(&ItemBuffer);
	
	if(aImsi)
  		Ret = SendReceive(EExamplePServDeleteItem,args);
  	else
  		Ret = SendReceive(EExamplePServDeleteItem2,args);
  	
#else
	TAny* messageParameters[KMaxMessageArguments];

	messageParameters[0] = static_cast<TAny*>(&ItemBuffer);
  	
  	if(aImsi)
  		SendReceive(EExamplePServDeleteItem,&messageParameters[0]);
  	else
  		SendReceive(EExamplePServDeleteItem2,&messageParameters[0]);
#endif

  	return  Ret;
}
	
/*
----------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------
*/
TInt RExampleServerClient::SetPassword(const TDesC& aPassword)
{
	TInt Ret(KErrNone);
	TPckgBuf<TExampleItem> ItemBuffer;
	
	ItemBuffer().iName.Copy(aPassword);
	ItemBuffer().iValue.Copy(aPassword);
	ItemBuffer().iIndex = -1;

#ifdef __SERIES60_3X__
	
	TIpcArgs args(&ItemBuffer);
	Ret = SendReceive(ESetProtectPassword,args);
#else	
	TAny* messageParameters[KMaxMessageArguments];

	messageParameters[0] = static_cast<TAny*>(&ItemBuffer);
  	
  	SendReceive(ESetProtectPassword,&messageParameters[0]);
#endif
  
  	return  Ret;
}
/*
----------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------
*/
TInt RExampleServerClient::GetPassword(TDes& aPassword)
{
	TInt Ret(KErrNone);
	TPckgBuf<TExampleItem> ItemBuffer;
	
	ItemBuffer().iName.Copy(aPassword);
	ItemBuffer().iValue.Copy(aPassword);
	ItemBuffer().iIndex = -1;

#ifdef __SERIES60_3X__
	
	TIpcArgs args(&ItemBuffer);
	Ret = SendReceive(EGetProtectPassword,args);
#else	
	TAny* messageParameters[KMaxMessageArguments];

	messageParameters[0] = static_cast<TAny*>(&ItemBuffer);
  	
  	SendReceive(EGetProtectPassword,&messageParameters[0]);
#endif

  	aPassword.Copy(ItemBuffer().iValue);
  	
	return Ret;  		
}

