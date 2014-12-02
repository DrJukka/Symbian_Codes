/* Copyright (c) 2001 - 2008, Dr. Jukka Silvennoinen. All rights reserved */



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

#include "ExampleServer.h"
#include "clientservercommon.h"
#include "common.h"
#include "ExampleServer_policy.h"
#include "ExampleSession.h"
#include "TrialHanler.h" 
#include "Fake_SMS.h" 

//delay which after we will shut down after last sessions is disconnected.
const TInt KShutDownDelay		 = 2000000;



/*
----------------------------------------------------------------------------------------
// CExPolicy_Server
----------------------------------------------------------------------------------------
*/
inline CExPolicy_Server::CExPolicy_Server():CPolicyServer(0,SClientServerPolicy,ESharableSessions)
{
}

/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CExPolicy_Server::~CExPolicy_Server()
{
	if(iExitTimer)
	{
		iExitTimer->Cancel();
	}
	
	delete iExitTimer;
	delete iTimeChnageMonitor;
	
	iItemArray.ResetAndDestroy();
    
    delete iAutoSchedule;
    iAutoSchedule = NULL;
    
    delete iFakeSMSSender;
    iFakeSMSSender = NULL;
    
	if(iFile.SubSessionHandle())
	{
		iFile.Write(_L8("Bye, bye, "));	
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
	iFsSession.Connect();
	
/*	_LIT(KRecFilename			,"C:\\FMsgServer.txt");
	iFsSession.Delete(KRecFilename);
	
	iFile.Create(iFsSession,KRecFilename,EFileWrite|EFileShareAny);	
	
	if(iFile.SubSessionHandle())
	{	
		iFile.Write(_L8("start, "));
	}
*/	
	StartL(KExapmpleServerName);// then start server
	
	iTimeChnageMonitor = CTimeChnageMonitor::NewL(0, *this);
	
	if(iFile.SubSessionHandle())
	{	
		iFile.Write(_L8("started, "));
	}
}

/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
TBool CExPolicy_Server::AreWeStillOnL(TInt& aIndex)
{
	TBool Ret(EFalse);

    TBool TrialOk(ETrue);

    if(KAppIsTrial){
        TBool isFirstTime(EFalse);
        TInt timelefftt(0);
        if(!CTrialHandler::IsNowOkL(isFirstTime,timelefftt)){
        
            delete iFakeSMSSender;
            iFakeSMSSender = NULL;
            iFakeSMSSender = CFakeSMSSender::NewL();
            
            TTime nowTim;
            nowTim.HomeTime();
            
            TBuf<255> smsMessagee;
            
            HBufC* msgg = HBufC::NewLC(255);
            
            msgg->Des().Copy(KtxTrialSMSMessage1);
            msgg->Des().Append(KtxtApplicationName);
            msgg->Des().Append(KtxTrialSMSMessage2);
            msgg->Des().Append(KtxTrialOviLink);
            
            HBufC* msggNumb = KtxTrialSMSNumber().AllocLC();
            HBufC* msggName = KtxTrialSMSName().AllocLC();
            
            iFakeSMSSender->CreateSMSMessageL(msggNumb,msggName,msgg,EFalse,nowTim);
            TrialOk = EFalse;
            
            CleanupStack::PopAndDestroy(3);//msgg, msggNumb, msggName
        }else{
            CTrialHandler::SetDateNowL();
        }
    }	
	
	TBuf8<255> DbgByff(_L8("AWSO: "));
	
	TFindFile AufFolder(iFsSession);
	
    if(!TrialOk){ // trial has ended, lets exit..
        iItemArray.Reset();
    }else if(KErrNone == AufFolder.FindByDir(KtxDatabaseName, KNullDesC))
	{
		DbgByff.Append(_L8("ff, "));
		
		
		TTimeIntervalSeconds ModInterval(0);
		
		LogTime(iModTime,_L8("LastTime"));
		
		TEntry anEntry;
		if(KErrNone == iFsSession.Entry(AufFolder.File(),anEntry))
		{	
			LogTime(anEntry.iModified,_L8("ModTime"));
			anEntry.iModified.SecondsFrom(iModTime,ModInterval);
		}
		
		DbgByff.Append(_L8("Sec == "));
		DbgByff.AppendNum(ModInterval.Int());
		
		if(ModInterval.Int() != 0
		|| iItemArray.Count() == 0)
		{
			iModTime = anEntry.iModified;
			
			DbgByff.Append(_L8("Mod, "));
			
			iItemArray.ResetAndDestroy();
			
			CScheduleDB* ScheduleDB = new(ELeave)CScheduleDB();
			CleanupStack::PushL(ScheduleDB);
			ScheduleDB->ConstructL();
			   
			ScheduleDB->ReadDbItemsL(iItemArray);	
			
			CleanupStack::PopAndDestroy(ScheduleDB);
		}
	}
	
	DbgByff.Append(_L8("Count, "));
	DbgByff.AppendNum(iItemArray.Count());
	
	DbgByff.Append(_L8(", "));

	if(iItemArray.Count())
	{	
		ResolveNextSchduleTimeL(aIndex,iItemArray);
		Ret = ETrue;
	}
	
	if(iFile.SubSessionHandle())
	{	
		DbgByff.Append(_L8("ind: "));
		DbgByff.AppendNum(aIndex);
		DbgByff.Append(_L8("\n"));
		iFile.Write(DbgByff);
	}
	
	if(aIndex < 0)
	{
		Ret = EFalse;
	}
	
	return Ret;
}
/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
void CExPolicy_Server::ResolveNextSchduleTimeL(TInt& aIndex,RPointerArray<CMsgSched>& aItemArray)
{
	TTime NextSchedule;
	NextSchedule.HomeTime();
	NextSchedule = NextSchedule + TTimeIntervalYears(1);
	
	for(TInt i=0; i < aItemArray.Count(); i++)
	{    	
		if(aItemArray[i])
		{
	    	if(IsTimeBeforeL(aItemArray[i]->iTime,NextSchedule))
    		{
    			aIndex = i;
    			NextSchedule = aItemArray[i]->iTime;
    		}
		}
	}
}


/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
TBool CExPolicy_Server::IsTimeBeforeL(TTime& aTime,TTime& aCompare)
{
	TBool IsBefore(EFalse);
	TTimeIntervalMinutes MinInterval(0);
	TTimeIntervalHours 	 HourInterval(0);
	TTimeIntervalSeconds SecInterval(0);
	
	TTime TimeNow;
	TimeNow.HomeTime();
	
	TTimeIntervalDays DaysInterval = aTime.DaysFrom(aCompare);
	if(DaysInterval.Int() <= 0)
	{
		aTime.HoursFrom(aCompare,HourInterval);
		if(HourInterval.Int() <= 0)
		{
			aTime.MinutesFrom(aCompare,MinInterval);
			if(MinInterval.Int() <= 0)
			{
				aTime.SecondsFrom(aCompare, SecInterval);
				if(SecInterval.Int() <= 0)
				{
					IsBefore = ETrue;
				}
			}
		}
	}
	
	return IsBefore;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TBool CExPolicy_Server::DeleteSchedule(TInt aIndex)
{
	TBool ret(ETrue);
	
	TInt eerr(KErrNone);
	
	iItemArray.ResetAndDestroy();
	
	TRAP(eerr,
	CScheduleDB* ScheduleDB = new(ELeave)CScheduleDB();
	CleanupStack::PushL(ScheduleDB);
	ScheduleDB->ConstructL();
	ScheduleDB->DeleteFromDatabaseL(aIndex,EFalse);
	
	ScheduleDB->ReadDbItemsL(iItemArray);
	
	CleanupStack::PopAndDestroy(ScheduleDB));
	
	
	if(iFile.SubSessionHandle())
	{
		TBuf8<50> BufFuu(_L8("DeleteSchedule, ind= "));
		
		BufFuu.AppendNum(aIndex);
		BufFuu.Append(_L(",Err = "));
		BufFuu.AppendNum(eerr);
		BufFuu.Append(_L("\n"));
		
		iFile.Write(BufFuu);
	}
	

	if(eerr != KErrNone)
	{
		ret = EFalse;
	}
	
	return ret;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_Server::StuffDoneL(TInt aError,TInt aIndex)
{
	if(iFile.SubSessionHandle())
	{
		TBuf8<50> BufFuu(_L8("StuffDoneL, ind= "));
		
		BufFuu.AppendNum(aIndex);
		BufFuu.Append(_L(",Err = "));
		BufFuu.AppendNum(aError);
		BufFuu.Append(_L("\n"));
		
		iFile.Write(BufFuu);
	}	

	TInt Inddex(-1);

	if(AreWeStillOnL(Inddex))
	{
		if(Inddex >= 0&& iItemArray.Count() > Inddex)
		{
			if(!iAutoSchedule)
			{
				iAutoSchedule = CAutoScheduler::NewL(*this);
			}
			
			iAutoSchedule->NewScheduleL(iItemArray[Inddex]);
		}
	}
	else
	{
		if(iFile.SubSessionHandle())
		{
			iFile.Write(_L8("Start Exit timer now\n"));
		}
		
		iUpdateDelay = EFalse;
		
		delete iExitTimer;
		iExitTimer = NULL;
		iExitTimer = CTimeOutTimer::NewL(EPriorityHigh, *this);
		iExitTimer->After(2000000);// 2 sec timeout..
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_Server::ChangeEventNowL(TInt aDelay)
{
    StuffDoneL(55555,-1);
    if(KAppIsTrial){
        TBool isFirstTime(EFalse);
        TInt timelefftt(0);
        if(CTrialHandler::IsNowOkL(isFirstTime,timelefftt)){
            CTrialHandler::SetDateNowL();
        }
    }
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_Server::TimerExpired()
{
	if(iUpdateDelay)
	{
		StuffDoneL(696969,-1);
	}
	else
	{
		ShutMeDown();
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_Server::LogTime(const TTime& aTime, const TDesC8& aTitle)
{
	if(iFile.SubSessionHandle())
	{		
		TBuf8<200> MsgBuffer(aTitle);
		
		MsgBuffer.AppendNum(aTime.DateTime().Day() + 1);
					
		TInt MyMonth = (aTime.DateTime().Month() + 1);
		MsgBuffer.Append(_L("."));
		if(MyMonth < 10)
			MsgBuffer.AppendNum(0);
					
		MsgBuffer.AppendNum(MyMonth);
					
		MsgBuffer.Append(_L("."));						
		MsgBuffer.AppendNum(aTime.DateTime().Year());		
					
		MsgBuffer.Append(_L(" - "));
		
		MsgBuffer.AppendNum(aTime.DateTime().Hour());
		MsgBuffer.Append(_L(":"));
		if(aTime.DateTime().Minute() < 10)
			MsgBuffer.AppendNum(0);
		
		MsgBuffer.AppendNum(aTime.DateTime().Minute());
		
		MsgBuffer.Append(_L("."));
		if(aTime.DateTime().Second() < 10)
			MsgBuffer.AppendNum(0);
		
		MsgBuffer.AppendNum(aTime.DateTime().Second());
		MsgBuffer.Append(_L8("\n"));
				
		iFile.Write(MsgBuffer);
	}
}

/*
----------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------
*/
CSession2* CExPolicy_Server::NewSessionL(const TVersion&,const RMessage2&) const
	{
	return new (ELeave) CExPolicy_ServerSession();
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
*********public functions called by sessions to handle data *****************
-----------------------------------------------------------------------------
*/
void CExPolicy_Server::AskUpdateValuesL(void)
{
	if(iFile.SubSessionHandle())
	{
		iFile.Write(_L8("asked to update now...\n"));
	}
	
	iUpdateDelay = ETrue;
	delete iExitTimer;
	iExitTimer = NULL;
	iExitTimer = CTimeOutTimer::NewL(EPriorityHigh, *this);
	iExitTimer->After(1000000);// 1 sec timeout..
}

/* 
-----------------------------------------------------------------------------
***************** functions for running the server exe **********************
-----------------------------------------------------------------------------
*/
// Perform all server initialisation, in particular creation of the
// scheduler and server and then run the scheduler
// 
static void RunServerL()
{
	User::LeaveIfError(User::RenameThread(KExapmpleServerName)); 
	
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
