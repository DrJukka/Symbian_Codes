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
CAutoScheduler::CAutoScheduler(MScheduleObserver& aUpdate):iUpdate(aUpdate)
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

	delete iNunmber;
    delete iMessage;
	
	delete iSMSSender;
	iSMSSender = NULL;
	if(iFile.SubSessionHandle())
	{
		iFile.Write(_L8("Bye bye..\n\n"));
	  	iFile.Close();
	}
	
	iFsSession.Close();
}
      
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CAutoScheduler* CAutoScheduler::NewL(MScheduleObserver& aUpdate)
{
    CAutoScheduler* MeUpdate = CAutoScheduler::NewLC(aUpdate);
    CleanupStack::Pop(MeUpdate) ;
    return MeUpdate;
}
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CAutoScheduler* CAutoScheduler::NewLC(MScheduleObserver& aUpdate)
{
	CAutoScheduler* MeUpdate = new (ELeave) CAutoScheduler(aUpdate);
    CleanupStack::PushL(MeUpdate) ;
    MeUpdate->ConstructL() ;
    return MeUpdate;
}


_LIT(KRecFilename					,"C:\\FMsgScheduler.txt");
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CAutoScheduler::ConstructL()
{
	User::LeaveIfError(iFsSession.Connect());
    
/*	iFsSession.Delete(KRecFilename);
	iFile.Create(iFsSession,KRecFilename,EFileWrite|EFileShareAny);
   */ 
    if(iFile.SubSessionHandle())
	{
		iFile.Write(_L8("Started \n"));
	}
		
	iTimeOutTimer = CTimeOutTimer::NewL(EPriorityHigh, *this);	
	
	iSMSSender = CSMSSender::CSMSSender::NewL(*this);
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/

void CAutoScheduler::HandleStatusChange(MMsvObserver::TStatus aStatus)
{
	if(iFile.SubSessionHandle())
	{
		switch(aStatus)
        {
        case ECreated:
        	iFile.Write(_L8("SMS status: ECreated \n"));
			break;
        case EMovedToOutBox:
        	iFile.Write(_L8("SMS status:  EMovedToOutBox\n"));
			break;
        case EScheduledForSend:
        	iFile.Write(_L8("SMS status:  EScheduledForSend\n"));
			break;
        case ESent:
        	iFile.Write(_L8("SMS status: ESent \n"));
			break;
        case EDeleted:
        	iFile.Write(_L8("SMS status: EDeleted \n"));
			break;
        case EScheduled:
        	iFile.Write(_L8("SMS status:  EScheduled\n"));
			break;
        case EGotReply:
        	iFile.Write(_L8("SMS status:  EGotReply\n"));
			break;
        };
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/

void CAutoScheduler::HandleError(const TInt& aError)
	{
	if(iFile.SubSessionHandle())
	{
		switch(aError)
        {
		case EScheduleFailed:
        	iFile.Write(_L8("SMS Error: EScheduleFailed\n"));
			break;
        case ESendFailed:
        	iFile.Write(_L8("SMS Error: ESendFailed\n"));
			break;
        case ENoServiceCentre:
        	iFile.Write(_L8("SMS Error: ENoServiceCentre\n"));
			break;
        case EFatalServerError:
        	iFile.Write(_L8("SMS Error: EFatalServerError\n"));
			break;
        default:
        	{
				TBuf8<100> hejlpper(_L8("General err: "));
				hejlpper.AppendNum(aError);
				iFile.Write(hejlpper);
        	}
        	break;
        };
	}
	}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CAutoScheduler::NewScheduleL(CMsgSched* aNextSchedule,const TTime aSchedlingTime)
{
	iFlashSMS = iUnicode = EFalse;
	delete iNunmber;
	iNunmber = NULL;
    delete iMessage;
    iMessage = NULL;
	iNextTime.HomeTime();
    iNextIndex = -1;
    
	LogCurrentTime();
	
	if(iTimeOutTimer && aNextSchedule)
	{
		if(aNextSchedule->iNunmber)
		{
			iNunmber = aNextSchedule->iNunmber->Des().AllocL();
		}
		
		iFlashSMS = aNextSchedule->iFlashSMS;
		iUnicode = aNextSchedule->iUnicode;
		
		if(aNextSchedule->iMessage)
		{
			iMessage = aNextSchedule->iMessage->Des().AllocL();
		}
	
		iNextTime 	= aSchedlingTime;
		iNextIndex	= aNextSchedule->iIndex;
		iRepeat		= aNextSchedule->iRepeat;
		
		LogScheduleTime();
		
		iTimeOutTimer->Cancel();
		delete iTimeOutTimer;
		iTimeOutTimer = NULL;
		
		if(iFile.SubSessionHandle())
		{
			iFile.Write(_L8("New schedule\n"));
		}
		
	    LogCurrentTime();
		LogScheduleTime();
    	
		iTimeOutTimer = CTimeOutTimer::NewL(EPriorityHigh, *this);
		iTimeOutTimer->At(iNextTime);
	}
	else
	{
		iUpdate.StuffDoneL(-1,iNextIndex );
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
	
	TTimeIntervalDays DaysInterval = iNextTime.DaysFrom(TimeNow);
	if(DaysInterval.Int() <= 0)
	{
		iNextTime.HoursFrom(TimeNow,HourInterval);
		if(HourInterval.Int() <= 0)
		{
			iNextTime.MinutesFrom(TimeNow,MinInterval);
			if(MinInterval.Int() <= 0)
			{
				iNextTime.SecondsFrom(TimeNow, SecInterval);
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
	
		iTimeOutTimer->At(iNextTime);	
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
void CAutoScheduler::DoDoTheStuffNowL(void)
{
	TInt ErrorNum(KErrNone);
	
	TBool OkToSend(EFalse);
	if(iRepeat != 0)
	{
		OkToSend = ETrue;
	}
	else if(iUpdate.DeleteSchedule(iNextIndex))
	{
		OkToSend = ETrue;
	}
	
	if(OkToSend)
	{
		TRAP(ErrorNum,
		if(iSMSSender)
		{
			iSMSSender->SendMsgL(iMessage,iNunmber,iUnicode,iFlashSMS);
		});
	}
	
	delete iNunmber;
	iNunmber = NULL;
    delete iMessage;
    iMessage = NULL;
    iNextIndex = -1;
    
	TRAP(ErrorNum,iUpdate.StuffDoneL(ErrorNum,iNextIndex));
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
		
		MsgBuffer.AppendNum(iNextTime.DateTime().Day() + 1);
					
		TInt MyMonth = (iNextTime.DateTime().Month() + 1);
		MsgBuffer.Append(_L("."));
		if(MyMonth < 10)
			MsgBuffer.AppendNum(0);
					
		MsgBuffer.AppendNum(MyMonth);
					
		MsgBuffer.Append(_L("."));						
		MsgBuffer.AppendNum(iNextTime.DateTime().Year());		
					
		MsgBuffer.Append(_L(" - "));
		
		MsgBuffer.AppendNum(iNextTime.DateTime().Hour());
		MsgBuffer.Append(_L(":"));
		if(iNextTime.DateTime().Minute() < 10)
			MsgBuffer.AppendNum(0);
		
		MsgBuffer.AppendNum(iNextTime.DateTime().Minute());
		
		MsgBuffer.Append(_L("."));
		if(iNextTime.DateTime().Second() < 10)
			MsgBuffer.AppendNum(0);
		
		MsgBuffer.AppendNum(iNextTime.DateTime().Second());
		MsgBuffer.Append(_L8("\n"));
				
		iFile.Write(MsgBuffer);
	}
}
