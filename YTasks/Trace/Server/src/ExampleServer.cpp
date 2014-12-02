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
#include "ExampleServerSession.h"

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
	delete iScreenDrawer;
	iScreenDrawer = NULL;
	
	delete iTraceMonitor;
	iTraceMonitor = NULL;
	
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
	
	TMemoryInfoV1Buf   memInfoBuf;
	UserHal::MemoryInfo(memInfoBuf);
	iTotalRamInBytes = (memInfoBuf().iTotalRamInBytes / 1000);
		
	iTraceMonitor = CTraceMonitor::NewL(*this);
	iExampleTimer = CTimeOutTimer::NewL(EPriorityNull, *this);

	StartL(KTraceServerName);// then start server
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
void CExPolicy_Server::ReReadSettingsL()
{
if(iFile.SubSessionHandle())
{
	iFile.Write(_L8("ReReadSettingsL\n,"));
}

	delete iScreenDrawer;
	iScreenDrawer = NULL;
	
	GetValuesFromStoreL();
	
	if(iSettings.iEnabledOn)
	{
		iScreenDrawer = CScreenDrawer::NewL(iSettings);
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_Server::GetValuesFromStoreL(void)
{	
	TFindFile AppFile(iFsSession);
	if(KErrNone == AppFile.FindByDir(KTraceSetFileName, KNullDesC))
	{
		CDictionaryFileStore* MyDStore = CDictionaryFileStore::OpenLC(iFsSession,AppFile.File(), TUid::Uid(0x102013AD));

		TInt32 TmpValue(-1);

		GetValuesL(MyDStore,0x7777,TmpValue);
		
		if(TmpValue > 50)
			iSettings.iEnabledOn = ETrue; // not set, use On
		else 
			iSettings.iEnabledOn  = EFalse;

		TmpValue = -1;
		GetValuesL(MyDStore,0x5555,TmpValue);
				
		if(TmpValue < 0 || TmpValue > 2)
		{
			TmpValue = 0;
		}
		
		iSettings.iDrawStyle = TmpValue;
		
		TmpValue = -1;
		GetValuesL(MyDStore,0x8877,TmpValue);
		
		iSettings.iHorSlider = TmpValue;
		
		if(iSettings.iHorSlider < 0)
			iSettings.iHorSlider = 0;
		else if(iSettings.iHorSlider > 100)
			iSettings.iHorSlider = 100;

		TmpValue = -1;
		GetValuesL(MyDStore,0x7788,TmpValue);
		
		iSettings.iVerSlider = TmpValue;
		
		if(iSettings.iVerSlider < 0)
			iSettings.iVerSlider = 0;
		else if(iSettings.iVerSlider > 100)
			iSettings.iVerSlider = 100;
		
		TmpValue = -1;
		GetValuesL(MyDStore,0x6666,TmpValue);
		
		iSettings.iFontSize = TmpValue;
		
		if(iSettings.iFontSize < 10)
			iSettings.iFontSize = 10;
		else if(iSettings.iFontSize > 150)
			iSettings.iFontSize = 150;				
		
		CleanupStack::PopAndDestroy(1);// Store
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_Server::GetValuesL(CDictionaryFileStore* aDStore,TInt aUID,TInt32& aValue)
{
	if(aDStore == NULL)
		return;
		
	TUid FileUid = {0x10};
	FileUid.iUid = aUID;

	if(aDStore->IsPresentL(FileUid))
	{
		RDictionaryReadStream in;
		in.OpenLC(*aDStore,FileUid);
		aValue = in.ReadInt32L();
		CleanupStack::PopAndDestroy(1);// in
	}
	else
		aValue = -1;
}
	
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_Server::HandleUpdate(const TInt& aCpuLoad,const TInt& aMemory,const TTime& aTime)
{
	CExampleItem* NewIttem = new(ELeave)CExampleItem();
	
    NewIttem->iCpuLoad = aCpuLoad;
    NewIttem->iMemory  = aMemory;
	NewIttem->iTime    = aTime;
	
	iArray.Append(NewIttem);
	
	if(iScreenDrawer)
	{
//		TInt CputmpVal = (100 - ((aCpuLoad * 100) / GetMax()));
//		TInt MemtmpVal = ((aMemory * 100) / iTotalRamInBytes);
	
		iScreenDrawer->SetValues(aMemory,iTotalRamInBytes,aCpuLoad,GetMax());
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_Server::ShowNoteL(const TDesC& aMessage)
{
	CAknGlobalConfirmationQuery* pQ = CAknGlobalConfirmationQuery::NewL();
	CleanupStack::PushL(pQ);

	TRequestStatus theStat = KRequestPending;
	pQ->ShowConfirmationQueryL(theStat,aMessage, R_AVKON_SOFTKEYS_OK_EMPTY);
	User::WaitForRequest(theStat);
   
	CleanupStack::PopAndDestroy(pQ);
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
void CExPolicy_Server::GetItemValueL(TInt& aIndex,TInt& aCpuLoad,TInt& aMemory,TTime& aTime)
{
	if(aIndex >= 0 && aIndex < iArray.Count())
	{
		if(iArray[aIndex])
		{
			aCpuLoad = iArray[aIndex]->iCpuLoad;
	        aMemory  = iArray[aIndex]->iMemory;
	        aTime    = iArray[aIndex]->iTime;
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
TUint32	CExPolicy_Server::GetMax(void)
{
	TUint32 Ret(100);
	
	if(iTraceMonitor)
	{
		Ret = iTraceMonitor->GetMax();
	}
	
	return Ret;
}
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/	
void CExPolicy_Server::StartTraceL()
{
	if(iTraceMonitor)
	{
		iTraceMonitor->StartL();
	}
	
	ReReadSettingsL();
}

/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/	
void CExPolicy_Server::StopTraceL()
{
	if(iTraceMonitor)
	{
		iTraceMonitor->Stop();
	}
	
	delete iScreenDrawer;
	iScreenDrawer = NULL;
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
	User::LeaveIfError(User::RenameThread(KTraceServerName)); 
	
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
    aMessage.Panic(KTraceServerName, aPanic);
    }
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_Server::PanicServer(TInt aPanic)//TTimeServPanic aPanic)
    {
    User::Panic(KTraceServerName, aPanic);
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
	User::LeaveIfError(semaphore.OpenGlobal(KTraceServerSemaphoreName));

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

