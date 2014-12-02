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

#include "MainServer.h"
#include "Mainservercommon.h"
#include "Common.h"

#ifdef __SERIES60_3X__
	#include "MainServer_policy.h"
	#include "MainSession.h"
#else
	#include "MainSession_2nd.h"
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
	delete iImsiReader;
	iImsiReader = NULL;
	
	delete iCellIDCheck;
	iCellIDCheck = NULL;
	
	delete iImsi;
	iImsi = NULL;
	
	delete iCountryCode;
	iCountryCode = NULL;
	
	delete iNwId;
	iNwId = NULL;
	
	delete iExampleTimer;
	iExampleTimer = NULL;
	

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

/*
	RFile DebugFile;
	RFs FsSession;
	TBuf8<100> DbgHjelp;
	FsSession.Connect();
	TInt ErrNo(KErrNone);
	
	_LIT(KtxFileName			,"YuccaBrowser_2000713D.exe");
	_LIT(KRecFilename			,"C:\\LaunchDbg.txt");
	FsSession.Delete(KRecFilename);		
	DebugFile.Create(FsSession,KRecFilename,EFileWrite|EFileShareAny);	

	DebugFile.Write(_L8("Stating,\n"));
	
	CApaCommandLine* cmdLine = CApaCommandLine::NewL();
	
	cmdLine->SetExecutableNameL(KtxFileName);
	cmdLine->SetCommandL(EApaCommandRun);
	
	
	DbgHjelp.Copy(_L8("Construct, err= "));
	DbgHjelp.AppendNum(ErrNo);
	DbgHjelp.Append(_L8(", \r\n"));
	
	DebugFile.Write(DbgHjelp);
	
	RApaLsSession ls;
	ErrNo = ls.Connect();
	
	DbgHjelp.Copy(_L8("Ls connect, err= "));
	DbgHjelp.AppendNum(ErrNo);
	DbgHjelp.Append(_L8(", \r\n"));
	
	DebugFile.Write(DbgHjelp);
	
	if(KErrNone == ErrNo)
	{
		ErrNo = ls.StartApp(*cmdLine);	
		ls.Close();
	
		DbgHjelp.Copy(_L8("StartApp, err= "));
		DbgHjelp.AppendNum(ErrNo);
		DbgHjelp.Append(_L8(", \r\n"));
		
		DebugFile.Write(DbgHjelp);
	}
		
	delete cmdLine;
	cmdLine = NULL;
	
	DebugFile.Write(_L8("Done,\r\n"));
	DebugFile.Close();
	FsSession.Close();*/
	
	
	iExampleTimer = CTimeOutTimer::NewL(EPriorityNull, *this);

	iCellIDCheck = new(ELeave)CCellIDCheck(*this);
  	iCellIDCheck->ConstructL();
    
    iImsiReader = CImsiReader::NewL(this);

	StartL(KMainTaskServerName);// then start server
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
	
	if(iSessionCount <= 0)
	{	// last session has disconnected, thus we could prepare for shut-down.
		StartShutDownTimer();
	}
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
		
		if(ProsID.iId  == (TUint32)KMAINYTasksUid31.iUid
		|| ProsID.iId  == (TUint32)KMAINYTasksUid32.iUid
		|| ProsID.iId  == (TUint32)KMAINYTasksUid33.iUid
		|| ProsID.iId  == (TUint32)KMAINYTasksUid34.iUid)// is it same as with client
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
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_Server::GotIMSIL(const TDesC& aIMSI, TInt aError)
{
	delete iImsi;
	iImsi = NULL;
	iImsi = aIMSI.Alloc();
}
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_Server::CellIDL(const TDesC& aCountryCode,const TDesC& aNwId,TUint aAreaCode,TUint aCellId)
{
	delete iCountryCode;
	iCountryCode = NULL;
	iCountryCode = aCountryCode.Alloc();
	
	delete iNwId;
	iNwId = NULL;
	iNwId = aNwId.Alloc();
	
	iAreaCode = aAreaCode;
	iCellId = aCellId;
}

/*
-------------------------------------------------------------------------------
******************************** Public Functions ****************************
-------------------------------------------------------------------------------
*/
void CExPolicy_Server::GetExtraInfoL(TExtraInfoItem& aItem)
{
	aItem.iAreaCode = iAreaCode;
	aItem.iCellId = iCellId;
	if(iImsi)
	{
		if(iImsi->Des().Length() > 40)
		{
			aItem.iImsi.Copy(iImsi->Des().Left(40));
		}
		else
		{
			aItem.iImsi.Copy(*iImsi);
		}
	}
	
	if(iCountryCode)
	{
		if(iCountryCode->Des().Length() > 100)
		{
			aItem.iCountryCode.Copy(iCountryCode->Des().Left(100));
		}
		else
		{
			aItem.iCountryCode.Copy(*iCountryCode);
		}
	}
	
	if(iNwId)
	{
		if(iNwId->Des().Length() > 100)
		{
			aItem.iNwId.Copy(iNwId->Des().Left(100));
		}
		else
		{
			aItem.iNwId.Copy(*iNwId);
		}
	}	
}

/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TInt CExPolicy_Server::SetFileMapping(TFileMappingItem& aDataMapping)
{		
	TInt RetErr(KErrNone);
	RApaLsSession ls;
	RetErr= ls.Connect();
	if(RetErr == KErrNone)
	{
		if(aDataMapping.iUid.iUid == 0)
		{
			RetErr = ls.DeleteDataMapping(aDataMapping.iDataType);			
		}
		else
		{
			RetErr = ls.InsertDataMapping(aDataMapping.iDataType,aDataMapping.iPriority,aDataMapping.iUid);
		}
		
	}

	ls.Close();
	
	return RetErr;
}
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_Server::KillThread(TDes8& aName)
{
	TFileName res,Hjep;
	
	Hjep.Copy(aName);
	TFindThread find;
	while(find.Next(res) == KErrNone)
	{
		if(Hjep == res)
		{
			RThread ph;
	  		ph.Open(find);
			ph.Kill(0);
			break;	
		}
	}	
}
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_Server::KillProcess(TDes8& aName)
{
	TFileName res,Hjep;
	
	Hjep.Copy(aName);
	TFindProcess find;
	while(find.Next(res) == KErrNone)
	{
		if(Hjep == res)
		{
			RProcess ph;
	  		ph.Open(find);
			ph.Kill(0);
			break;	
		}
	}
}
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TInt CExPolicy_Server::KillTasks(TUid& aUid)
{		
	TInt RetErr(KErrNone);
	
	RWsSession wsSession;
	RetErr= wsSession.Connect();
	if(RetErr == KErrNone)
	{
		TApaTaskList taskList( wsSession );
		TApaTask task = taskList.FindApp(aUid);

		if( task.Exists() ) 
		{
			task.KillTask();
		}		
	}

	wsSession.Close();
	
	return RetErr;
}
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_Server::ShowGlobalDbgNoteL(const TDesC& aMessage)
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
***************** functions for running the server exe **********************
-----------------------------------------------------------------------------
*/
#ifdef __SERIES60_3X__
// Perform all server initialisation, in particular creation of the
// scheduler and server and then run the scheduler
// 
static void RunServerL()
{
	User::LeaveIfError(User::RenameThread(KMainTaskServerName)); 
	
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
    aMessage.Panic(KMainTaskServerName, aPanic);
    }
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_Server::PanicServer(TInt aPanic)//TTimeServPanic aPanic)
    {
    User::Panic(KMainTaskServerName, aPanic);
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
	User::LeaveIfError(semaphore.OpenGlobal(KExapmpleServerSemaphoreName));

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

