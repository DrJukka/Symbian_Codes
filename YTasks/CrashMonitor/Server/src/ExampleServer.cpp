/* 
	Copyright (c) 2006, 
	Jukka Silvennoinen. 
	All rights reserved 
*/


#include <e32uid.h>
#include <e32svr.h>
#include <e32math.h>
#include <apacmdln.h>
#include <APGCLI.H>
#include <bautils.h>
#include <w32std.h>
#include <apgwgnam.h>
#include <aknkeylock.h>
#include <uikon.hrh>
#include <cntfldst.h> 
#include <cpbkidlefinder.h> 
#include <cpbkcontactitem.h> 
#include <E32STD.H>
#include <AknGlobalConfirmationQuery.h>

#include "ExampleServer.h"
#include "clientservercommon.h"
#include "Common.h"

#ifdef __SERIES60_3X__
	#include "ExampleServer_policy.h"
	#include "ExampleSession.h"
#else
	#include "ExampleSession_2nd.h"
#endif

//delay which after we will shut down after last sessions is disconnected.
const TInt KShutDownDelay		 = 500000;



/*
----------------------------------------------------------------------------------------
// CExPolicy_Server
----------------------------------------------------------------------------------------
*/
inline CExPolicy_Server::CExPolicy_Server():
#ifdef __SERIES60_3X__
	CPolicyServer(0,SClientServerPolicy,ESharableSessions)
#else
	CServer(EPriorityNormal)
#endif
{
}

/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CExPolicy_Server::~CExPolicy_Server()
{
	if(iExitMonitor)
	{
		iExitMonitor->Cancel();
	}
	
	delete iExitMonitor;
	iExitMonitor = NULL;
	
	delete iExampleTimer;
	iExampleTimer = NULL;
	
	iArray.ResetAndDestroy();
	
	if(iFile.SubSessionHandle())
	{
	  	iFile.Write(_L8("Bye,bye"));
	  	iFile.Close();
	}

	iFsSession.Close();
}
    
/*
----------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------
*/
CExPolicy_Server* CExPolicy_Server::NewLC()
	{
	CExPolicy_Server* self=new(ELeave) CExPolicy_Server;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_Server::ConstructL()
{
	User::LeaveIfError(iFsSession.Connect());
	
/*	_LIT(KRecFilename			,"C:\\CrashTest.txt");
		
	iFsSession.Delete(KRecFilename);		
	User::LeaveIfError(iFile.Create(iFsSession,KRecFilename,EFileWrite|EFileShareAny));	
*/	
	if(iFile.SubSessionHandle())
	{
		iFile.Write(_L8("Mie Strttaaan ny\n,"));
	}
	
	iExitMonitor = CExitMonitor::NewL(this);
	iExampleTimer = CTimeOutTimer::NewL(EPriorityNull, *this);

	StartL(KCrashServerName);// then start server
}


/*
----------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------
*/
void CExPolicy_Server::AddSession()
{
	++iSessionCount;
	
	if(iFile.SubSessionHandle())
	{
		iFile.Write(_L8("Add session\n,"));
	}
	
	if(iExampleTimer)
	{	// we have new session connected 
		// so we need to cancel any pending exit timer requests.
		iExampleTimer->Cancel();
	}
}

/*
----------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------
*/
void CExPolicy_Server::DropSession()
{
	--iSessionCount;

	if(iFile.SubSessionHandle())
	{
		iFile.Write(_L8("Remove session\n,"));
	}
	
/*	if(iSessionCount <= 0)
	{	// last session has disconnected, thus we could prepare for shut-down.
		StartShutDownTimer();
	}*/
}
#ifdef __SERIES60_3X__
/*
----------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------
*/
CSession2* CExPolicy_Server::NewSessionL(const TVersion&,const RMessage2&) const
	{
	return new (ELeave) CExPolicy_ServerSession();
	}

/*
----------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------
*/
// this is custom security check implemented just for example purposes
// check that client process has secure UID of our UI client application
CPolicyServer::TCustomResult CExPolicy_Server::CustomSecurityCheckL(const RMessage2& aMsg, TInt& /*aAction*/, TSecurityInfo& /*aMissing*/)
{ 
	if(iFile.SubSessionHandle())
	{
		iFile.Write(_L8("Security test\n,"));
	}
	
	CPolicyServer::TCustomResult test;
	test=EFail;// by default we will fail the test
	
	RThread ClientThread;
	
	aMsg.Client(ClientThread);// get client thread 
	
	if(ClientThread.Handle())// has handle --> is valid
	{
		RProcess ClientProcess;
		ClientThread.Process(ClientProcess);// then get the process
		
		TSecureId ProsID = ClientProcess.SecureId();// and check secure UID
		
		if(ProsID.iId  == (TUint32)KMAINServerUid3.iUid
		|| ProsID.iId  == (TUint32)KMAINYTasksUid32.iUid
		|| ProsID.iId  == (TUint32)KMAINYTasksUid31.iUid)// is it same as with client
		{	
			if(iFile.SubSessionHandle())
			{
				iFile.Write(_L8("Passed\n,"));
			}
			test=EPass;// pass check if it is, othervise it will fail
		}
	}
	
	return test;
} 



#else 
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CSharableSession* CExPolicy_Server::NewSessionL(const TVersion& aVersion) const
    {
	// check we're the right version
	if (!User::QueryVersionSupported(TVersion(SServMajorVersionNumber,
                                              SServMinorVersionNumber,
                                              SServBuildVersionNumber),
                                     aVersion))
        {
		User::Leave(KErrNotSupported);
        }

	// make new session
	RThread client = Message().Client();
	return CExPolicy_ServerSession::NewL(client, *const_cast<CExPolicy_Server*> (this));
    }
#endif
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_Server::HandleExit(const TDesC& aName,const TExitType&  aExitType,const TInt& aExitReason,const TDesC& aExitCategory,const TDesC& aProcess,const TTime& aTime)
{
	CExampleItem* NewIttem = new(ELeave)CExampleItem();
	
	if(aProcess.Length() > 50)
		NewIttem->iProcess = aProcess.Left(50).Alloc();
	else
		NewIttem->iProcess = aProcess.Alloc();
	
	if(aName.Length() > 50)
		NewIttem->iName = aName.Left(50).Alloc();
	else
		NewIttem->iName = aName.Alloc();
	
	if(aExitCategory.Length() > 20)
    	NewIttem->iExitCatogory = aExitCategory.Left(20).Alloc();
	else
		NewIttem->iExitCatogory = aExitCategory.Alloc();
	
    NewIttem->iExitType = aExitType;
    NewIttem->iExitReason = aExitReason;
	NewIttem->iTime = aTime;
	
	iArray.Append(NewIttem);
	
	TRAPD(Errrr,ShowExitNoteL(aName,aExitType,aExitReason,aExitCategory));
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_Server::ShowExitNoteL(const TDesC& aName,const TExitType&  aExitType,const TInt& aExitReason,const TDesC& aExitCategory)
{
	if(aExitType == EExitKill && aExitReason == 0)
	{
		// do nothing, normal exit for an app
	}
	else
	{
	    TBuf<250> ShowBuffer;
		ShowBuffer.Copy(aName);
		ShowBuffer.Append(_L(" "));

		ShowBuffer.Append(aExitCategory);
		ShowBuffer.Append(_L(" : "));
		ShowBuffer.AppendNum(aExitReason);

		CAknGlobalConfirmationQuery* pQ = CAknGlobalConfirmationQuery::NewL();
		CleanupStack::PushL(pQ);

		TRequestStatus theStat = KRequestPending;
		pQ->ShowConfirmationQueryL(theStat,ShowBuffer, R_AVKON_SOFTKEYS_OK_EMPTY);
		User::WaitForRequest(theStat);
	   
		CleanupStack::PopAndDestroy(pQ);
	}
}
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_Server::TimerExpired()
{
	if(iFile.SubSessionHandle())
	{
		iFile.Write(_L8("Timer done\n,"));
	}
	
	if(iSessionCount <= 0)
	{   // timer expired without cancellation and we dont have sessions
	    // thus we can exit now.
		ShutMeDown();
	}
}
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_Server::ShutMeDown()
{	
	if(iFile.SubSessionHandle())
	{
		iFile.Write(_L8("Out we go\n,"));
	}
	CActiveScheduler::Stop(); //wecan exit by stopping the active scheduler.
}

/*
-------------------------------------------------------------------------------
******************************** Public Functions ****************************
-------------------------------------------------------------------------------
*/
void CExPolicy_Server::GetItemValueL(TInt& aIndex,TDes& aName,TDes& aExitCatogory,TExitType& aExitType, TInt& aExitReason,TDes& aProcess,TTime& aTime)
{
	if(aIndex >= 0 && aIndex < iArray.Count())
	{
		if(iArray[aIndex])
		{
			if(iArray[aIndex]->iName)
			{
				aName.Copy(*iArray[aIndex]->iName);
			}
			
			if(iArray[aIndex]->iExitCatogory)
			{
	        	aExitCatogory.Copy(*iArray[aIndex]->iExitCatogory);
			}
			
			if(iArray[aIndex]->iProcess)
			{
	        	aProcess.Copy(*iArray[aIndex]->iProcess);
			}

			aTime = iArray[aIndex]->iTime;
	        aExitType = iArray[aIndex]->iExitType;
	        aExitReason = iArray[aIndex]->iExitReason;
		}
	}
	else if(iFile.SubSessionHandle())
	{
		iFile.Write(_L8("Bad index\n"));
	}
}
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/	
void CExPolicy_Server::ClearListItemsL()
{
	if(iFile.SubSessionHandle())
	{
		iFile.Write(_L8("Clear items\n"));
	}
	iArray.ResetAndDestroy();
}
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/	
void CExPolicy_Server::StartShutDownTimer()
{
	if(iFile.SubSessionHandle())
	{
		iFile.Write(_L8("Start exit timer\n,"));
	}
	
	if(iExampleTimer)
	{
		iExampleTimer->Cancel();// cancel any previous request first
		iExampleTimer->After(KShutDownDelay);// and start new one.
	}
}

/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/	
TInt CExPolicy_Server::GetItemsCount(void)
{ 
	TInt Ret = iArray.Count();
	if(iFile.SubSessionHandle())
	{
		TBuf8<255> ShowBuffer(_L8("GetItemsCount: "));
		ShowBuffer.AppendNum(Ret);
		ShowBuffer.Append(_L8("\n"));
		iFile.Write(ShowBuffer);
	}
	return Ret;
}
/* 
-----------------------------------------------------------------------------
***************** functions for running the server exe **********************
-----------------------------------------------------------------------------
*/
#ifdef __SERIES60_3X__
// Perform all server initialisation, in particular creation of the
// scheduler and server and then run the scheduler
// 
static void RunServerL()
{
	User::LeaveIfError(User::RenameThread(KCrashServerName)); 
	
	// create and install the active scheduler we need
	CActiveScheduler* s=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(s);
	CActiveScheduler::Install(s);
	// create the server (leave it on the cleanup stack)
	CExPolicy_Server::NewLC();

	RProcess::Rendezvous(KErrNone);

	// Ready to run
	CActiveScheduler::Start();
	// Cleanup the server and scheduler
	CleanupStack::PopAndDestroy(2);
	}

/*
----------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------
*/
TInt E32Main()
	{
	__UHEAP_MARK;

	CTrapCleanup* cleanup=CTrapCleanup::New();
	TInt r=KErrNoMemory;
	if (cleanup)
		{
		TRAP(r,RunServerL());
		delete cleanup;
		}
	__UHEAP_MARKEND;
	return r;
	}
#else
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_Server::PanicClient(const RMessage& aMessage, TInt aPanic)
    {
    aMessage.Panic(KCrashServerName, aPanic);
    }
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_Server::PanicServer(TInt aPanic)//TTimeServPanic aPanic)
    {
    User::Panic(KCrashServerName, aPanic);
    }
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/  
void CExPolicy_Server::ThreadFunctionL()
    {
    // Construct active scheduler
    CActiveScheduler* activeScheduler = new (ELeave) CActiveScheduler;
    CleanupStack::PushL(activeScheduler) ;

    // Install active scheduler
    // We don't need to check whether an active scheduler is already installed
    // as this is a new thread, so there won't be one
    CActiveScheduler::Install(activeScheduler);

    // Construct our server
    CExPolicy_Server::NewLC();    // anonymous

	RSemaphore semaphore;
	User::LeaveIfError(semaphore.OpenGlobal(KCrashServerSemaphoreName));

	// Semaphore opened ok
	semaphore.Signal();
	semaphore.Close();

	// Start handling requests
	CActiveScheduler::Start();

    CleanupStack::PopAndDestroy(2, activeScheduler);    //  anonymous CSALockServer
    }
    
TInt CExPolicy_Server::ThreadFunction(TAny* /*aNone*/)
    {
    CTrapCleanup* cleanupStack = CTrapCleanup::New();
	if (cleanupStack == NULL)
	    {
        PanicServer(-1);
	    }

    TRAPD(err, ThreadFunctionL());
    if (err != KErrNone)
        {
        PanicServer(err);//ESrvCreateServer);
        }

    delete cleanupStack;
    cleanupStack = NULL;

    return KErrNone;
    }
    
	#ifdef __WINS__

	IMPORT_C TInt WinsMain();
	EXPORT_C TInt WinsMain()
	    {
		return reinterpret_cast<TInt>(&CExPolicy_Server::ThreadFunction);
	    }


	GLDEF_C TInt E32Dll(TDllReason)
	    {
		return KErrNone;
	    }
	    
	    

	#else  // __ARMI__
	TInt E32Main()
    {
		return CExPolicy_Server::ThreadFunction(NULL);
    }
	#endif
	

#endif

