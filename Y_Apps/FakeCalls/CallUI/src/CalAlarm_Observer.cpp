
#include "CalAlarm_Observer.h"

#include <ASShdDefs.h>
#include <ASShdAlarm.h>

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
CCalAlarmObserver* CCalAlarmObserver::NewLC(MMyLogCallBack &aCallBack)
	{
 	CCalAlarmObserver* self = new (ELeave) CCalAlarmObserver(aCallBack);
  	CleanupStack::PushL(self);
  	self->ConstructL();
  	return self;
	}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
CCalAlarmObserver* CCalAlarmObserver::NewL(MMyLogCallBack &aCallBack)
	{
 	CCalAlarmObserver* self = CCalAlarmObserver::NewLC(aCallBack);
  	CleanupStack::Pop(self);
  	return self;
	}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
CCalAlarmObserver::CCalAlarmObserver(MMyLogCallBack &aCallBack)
:CActive(EPriorityStandard),iCallBack(aCallBack)
	{
	
	}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
CCalAlarmObserver::~CCalAlarmObserver()
	{
 	Cancel();
	iASCliSession.Close();
	
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
void CCalAlarmObserver::ConstructL()
{
 	CActiveScheduler::Add(this);
 	iASCliSession.Connect();
 	
 /*	_LIT(KRecFilename					,"C:\\AlarmSched.txt");
	User::LeaveIfError(iFsSession.Connect());
    
	iFsSession.Delete(KRecFilename);
	iFile.Create(iFsSession,KRecFilename,EFileWrite|EFileShareAny);
  */ 
    if(iFile.SubSessionHandle())
	{
		iFile.Write(_L8("Started \n"));
	} 	
 	
 	iAlarmIsUp = iAlarmIsSnoozed = EFalse;
 	StartMonitoring();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
void CCalAlarmObserver::StartMonitoring()
{
	if(iFile.SubSessionHandle())
	{
		iFile.Write(_L8("Start monitoring\n"));
	}

	iASCliSession.NotifyChange(iStatus, iAlarmId);
	SetActive();
}
/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/ 
void CCalAlarmObserver::RunL()
{	 
	TInt ErrNo(KErrNone);
	
	TRAP(ErrNo,
    switch(iStatus.Int())
    {
    case EAlarmChangeEventState:// An alarm has changed state.
	case EAlarmChangeEventStatus:// An alarm has changed status.
	case EAlarmChangeEventCharacteristics:// An alarm has changed characteristics.
	case EAlarmChangeEventAlarmDeletion:// An alarm has been deleted from the queue of alarms. 
	case EAlarmChangeEventAlarmAddition:// An alarm has been added to the queue of alarms. 
	case EAlarmChangeEventSoundPlaying:// The sound for an alarm has just started playing. 
	case EAlarmChangeEventSoundStopped:// The sound for an alarm has just stopped playing. 
	case EAlarmChangeEventPlayIntervalsChanged:// The sound intervals associated with sound timing have changed. 
	case EAlarmChangeEventGlobalSoundStateChanged:// The global sound state (on/off) has changed. 
	case EAlarmChangeEventHeadQueueItemChanged:// The next alarm at the head of the alarm queue has changed. 
	case EAlarmChangeEventSystemDateTimeChanged:// The system date or time has changed, or the days defined as workdays have changed. 
	case EAlarmChangeEventSoundSilence:// Alarm sounds have been temporarily silenced, the current alarm has been paused or re-enabled. 
	case EAlarmChangeEventAlarmData:// The data associated with an alarm has changed. 
	case EAlarmChangeEventRestoreStarted:// A restore from backup of the alarm server has started. Alarms cannot be added/deleted until this has finished. 
	case EAlarmChangeEventRestoreFailed:// A restore from backup of the alarm server has failed. Alarms can be added/deleted again. 
	case EAlarmChangeEventRestoreCompleted:// A restore from backup of the alarm server has completed. The alarm queue has changed. 
	case EAlarmChangeEventLast:// Last change event (anchor). This is always at the end of the list. 
	case EAlarmChangeEventUndefined:// An undefined alarm event has occurred. 
 		break;
	case EAlarmChangeEventTimerExpired:// An alarm has expired. 
		{
			TASShdAlarm allarm;
			if(iASCliSession.GetAlarmDetails(iAlarmId,allarm) == KErrNone)
			{
				iMessage.Copy(allarm.Message());
			}
		}
		break;
	case EAlarmChangeEventAlarmUIVisible:// The alarm alert server has been instructed to show the 'alarm expired' display. 
  		{
			TASShdAlarm allarm;
  		    if(iASCliSession.GetAlarmDetails(iAlarmId,allarm) == KErrNone)
  		    {
				iMessage.Copy(allarm.Message());
  		    }
  		    
  		  	iCallBack.DoShowAlarmNoteL(iMessage);
 		}
 		break;
	case EAlarmChangeEventAlarmUIInvisible:// The alarm alert server has been instructed to hide the 'alarm expired' display. 
  		{
			iMessage.Zero();
  			iCallBack.DoHideAlarmNoteL();
 		}
 		break;
    default:
    	break;
    });
    
    TASShdAlarm allarm;
    TInt alErr = iASCliSession.GetAlarmDetails(iAlarmId,allarm);		
	if(iFile.SubSessionHandle())
	{
		TBuf8<200> hjewlppp(_L8("message: "));
		hjewlppp.Append(allarm.Message());
		hjewlppp.Append(_L8(", err= "));
		hjewlppp.AppendNum(alErr);
		hjewlppp.Append(_L8(", Stat= "));
		hjewlppp.AppendNum(iStatus.Int());
		hjewlppp.Append(_L8(", ErrNo= "));
		hjewlppp.AppendNum(ErrNo);
			
		iFile.Write(hjewlppp);
		iFile.Write(_L8("\n"));
	}
	StartMonitoring();
}
/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/ 
void CCalAlarmObserver::LogTime(const TTime& aTime, TDes8& aBuffer)
{
	aBuffer.Append(_L8("Time: "));
	
	aBuffer.AppendNum(aTime.DateTime().Day()+ 1);
				
	TInt MyMonth = (aTime.DateTime().Month() + 1);
	aBuffer.Append(_L8("."));
	if(MyMonth < 10)
		aBuffer.AppendNum(0);
				
	aBuffer.AppendNum(MyMonth);
				
	aBuffer.Append(_L8("."));						
	aBuffer.AppendNum(aTime.DateTime().Year());		
				
	aBuffer.Append(_L8(" - "));
	
	aBuffer.AppendNum(aTime.DateTime().Hour());
	aBuffer.Append(_L8(":"));
	if(aTime.DateTime().Minute() < 10)
		aBuffer.AppendNum(0);
	
	aBuffer.AppendNum(aTime.DateTime().Minute());
	
	aBuffer.Append(_L8("."));
	if(aTime.DateTime().Second() < 10)
		aBuffer.AppendNum(0);
	
	aBuffer.AppendNum(aTime.DateTime().Second());
}
/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/ 
void CCalAlarmObserver::DoCancel()
	{
		iASCliSession.NotifyChangeCancel();
	}

