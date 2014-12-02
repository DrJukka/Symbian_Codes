/* Copyright (c) 2001 - 2008, Dr. Jukka Silvennoinen. All rights reserved */

#include <e32svr.h>
#include <e32math.h>
#include <apacmdln.h>
#include <APGCLI.H>
#include <bautils.h>
#include <w32std.h>
#include <COEMAIN.H>
#include <EIKFUTIL.H>

#include "AutoScheduler.h"
	
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CAutoScheduler::CAutoScheduler(MScheduleObserver& aUpdate,TTime aTime,TInt aProfile)
:iUpdate(aUpdate),iTime(aTime),iProfile(aProfile)
    {
    // Implementation not required
    }

/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CAutoScheduler::~CAutoScheduler()
{	
	if(iTimeOutTimer)
		iTimeOutTimer->Cancel();
		
	delete iTimeOutTimer;
	iTimeOutTimer = NULL;

	if(iProfileEngine)
	{
		iProfileEngine->Release();
	}
	
	delete iMyCallObserver;
	iMyCallObserver = NULL;
	
	if(iFile.SubSessionHandle())
	{
		iFile.Write(_L8("Bye bye.."));
	  	iFile.Close();
	}
	
	iFsSession.Close();
}
      
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CAutoScheduler* CAutoScheduler::NewL(MScheduleObserver& aUpdate,TTime aTime,TInt aProfile)
{
    CAutoScheduler* MeUpdate = CAutoScheduler::NewLC(aUpdate,aTime,aProfile);
    CleanupStack::Pop(MeUpdate) ;
    return MeUpdate;
}
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CAutoScheduler* CAutoScheduler::NewLC(MScheduleObserver& aUpdate,TTime aTime,TInt aProfile)
{
	CAutoScheduler* MeUpdate = new (ELeave) CAutoScheduler(aUpdate,aTime,aProfile);
    CleanupStack::PushL(MeUpdate) ;
    MeUpdate->ConstructL() ;
    return MeUpdate;
}


_LIT(KRecFilename					,"C:\\ProfScheduler.txt");
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CAutoScheduler::ConstructL()
{
	User::LeaveIfError(iFsSession.Connect());
    
/*	iFsSession.Delete(KRecFilename);
	iFile.Create(iFsSession,KRecFilename,EFileWrite|EFileShareAny);
    
    if(iFile.SubSessionHandle())
	{
		iFile.Write(_L8("Started \n"));
	}*/
	
	iProfileEngine = CreateProfileEngineL();
	
	iMyCallObserver = CMyCallObserver::NewL(*this);
	
    LogCurrentTime();
    LogScheduleTime();
    	
	iTimeOutTimer = CTimeOutTimer::NewL(EPriorityHigh, *this);
	iTimeOutTimer->At(iTime);		
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CAutoScheduler::NewScheduleL(TTime aNewTime, TInt aProfile)
{
	iTime = aNewTime;
	iProfile = aProfile;

	if(iTimeOutTimer)
	{
		iTimeOutTimer->Cancel();
		
		if(iFile.SubSessionHandle())
		{
			iFile.Write(_L8("New schedule\n"));
		}
		
	    LogCurrentTime();
		LogScheduleTime();
    	
		iTimeOutTimer = CTimeOutTimer::NewL(EPriorityHigh, *this);
		iTimeOutTimer->At(iTime);
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CAutoScheduler::TimerExpired()
{

	if(iFile.SubSessionHandle())
	{
		iFile.Write(_L8("Timer ext,\n"));
	}
	LogCurrentTime();
	
	TBool SleepMore(ETrue);
	TTimeIntervalMinutes MinInterval(0);
	TTimeIntervalHours 	 HourInterval(0);
	TTimeIntervalSeconds SecInterval(0);
	
	TTime TimeNow;
	TimeNow.HomeTime();
	
	TTimeIntervalDays DaysInterval = iTime.DaysFrom(TimeNow);
	if(DaysInterval.Int() <= 0)
	{
		iTime.HoursFrom(TimeNow,HourInterval);
		if(HourInterval.Int() <= 0)
		{
			iTime.MinutesFrom(TimeNow,MinInterval);
			if(MinInterval.Int() <= 0)
			{
				iTime.SecondsFrom(TimeNow, SecInterval);
				if(SecInterval.Int() <= 0)
				{
					SleepMore = EFalse;
				}
			}
		}
	}
	
	if(SleepMore)
	{
		if(iFile.SubSessionHandle())
		{
			iFile.Write(_L8("Sleep more,\n"));
		}
	
		iTimeOutTimer->At(iTime);	
	}
	else if(iCallIsOn)
	{
		iTime = iTime + TTimeIntervalMinutes(2);
	
		if(iFile.SubSessionHandle())
		{
			iFile.Write(_L8("Sleep more call is on!!,\n"));
		}
			
		iTimeOutTimer->At(iTime);	
	}
	else
	{
		if(iFile.SubSessionHandle())
		{
			iFile.Write(_L8("Done now..,\n"));
		}
		
		DoDoTheStuffNowL();
	}		
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CAutoScheduler::NotifyChangeInCallStatusL(CTelephony::TCallStatus& aStatus, TInt /*aError*/)
{
	if(aStatus == CTelephony::EStatusIdle 
	|| aStatus == CTelephony::EStatusUnknown)
	{
		iCallIsOn = EFalse;
	}
	else
	{
		iCallIsOn = ETrue;
	}	
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CAutoScheduler::DoDoTheStuffNowL(void)
{
	TInt ErrorNum(KErrNone);
	
	TRAP(ErrorNum,
	
	TInt currProf = iProfileEngine->ActiveProfileId();
	if(currProf != iProfile)
	{	
		iProfileEngine->SetActiveProfileL(iProfile);
	}
	);

	iUpdate.StuffDoneL(ErrorNum);
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CAutoScheduler::LogCurrentTime(void)
{
	if(iFile.SubSessionHandle())
	{		
		TTime CurrTime;
		CurrTime.HomeTime();
	
		TBuf8<200> MsgBuffer(_L8("\nNow Time: "));
		
		MsgBuffer.AppendNum(CurrTime.DateTime().Day()+ 1);
					
		TInt MyMonth = (CurrTime.DateTime().Month() + 1);
		MsgBuffer.Append(_L("."));
		if(MyMonth < 10)
			MsgBuffer.AppendNum(0);
					
		MsgBuffer.AppendNum(MyMonth);
					
		MsgBuffer.Append(_L("."));						
		MsgBuffer.AppendNum(CurrTime.DateTime().Year());		
					
		MsgBuffer.Append(_L(" - "));
		
		MsgBuffer.AppendNum(CurrTime.DateTime().Hour());
		MsgBuffer.Append(_L(":"));
		if(CurrTime.DateTime().Minute() < 10)
			MsgBuffer.AppendNum(0);
		
		MsgBuffer.AppendNum(CurrTime.DateTime().Minute());
		
		MsgBuffer.Append(_L("."));
		if(CurrTime.DateTime().Second() < 10)
			MsgBuffer.AppendNum(0);
		
		MsgBuffer.AppendNum(CurrTime.DateTime().Second());
		MsgBuffer.Append(_L8("\n"));
				
		iFile.Write(MsgBuffer);
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CAutoScheduler::LogScheduleTime(void)
{
	if(iFile.SubSessionHandle())
	{		
		TBuf8<200> MsgBuffer(_L8("Schedule time: "));
		
		MsgBuffer.AppendNum(iTime.DateTime().Day() + 1);
					
		TInt MyMonth = (iTime.DateTime().Month() + 1);
		MsgBuffer.Append(_L("."));
		if(MyMonth < 10)
			MsgBuffer.AppendNum(0);
					
		MsgBuffer.AppendNum(MyMonth);
					
		MsgBuffer.Append(_L("."));						
		MsgBuffer.AppendNum(iTime.DateTime().Year());		
					
		MsgBuffer.Append(_L(" - "));
		
		MsgBuffer.AppendNum(iTime.DateTime().Hour());
		MsgBuffer.Append(_L(":"));
		if(iTime.DateTime().Minute() < 10)
			MsgBuffer.AppendNum(0);
		
		MsgBuffer.AppendNum(iTime.DateTime().Minute());
		
		MsgBuffer.Append(_L("."));
		if(iTime.DateTime().Second() < 10)
			MsgBuffer.AppendNum(0);
		
		MsgBuffer.AppendNum(iTime.DateTime().Second());
		MsgBuffer.Append(_L8("\n"));
				
		iFile.Write(MsgBuffer);
	}
}
