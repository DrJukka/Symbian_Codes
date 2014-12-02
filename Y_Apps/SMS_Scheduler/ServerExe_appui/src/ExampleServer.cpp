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
		
	iItemArray.ResetAndDestroy();
    
    delete iAutoSchedule;
    iAutoSchedule = NULL;
    
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
*/	
	if(iFile.SubSessionHandle())
	{	
		iFile.Write(_L8("start, "));
	}
	
	StartL(KExapmpleServerName);// then start server
	
	
	if(iFile.SubSessionHandle())
	{	
		iFile.Write(_L8("started, "));
	}
}

/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
TBool CExPolicy_Server::AreWeStillOnL(TInt& aIndex,TTime& aSchedulingTime)
{
	TBool Ret(EFalse);
	
	TBuf8<255> DbgByff(_L8("AWSO: "));
	
	TFindFile AufFolder(iFsSession);
	if(KErrNone == AufFolder.FindByDir(KtxDatabaseName, KNullDesC))
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
		
	//	if(ModInterval.Int() != 0
	//	|| iItemArray.Count() == 0)
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
		Ret = ResolveNextSchduleTimeL(aSchedulingTime,aIndex,iItemArray);
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
TBool CExPolicy_Server::ResolveNextSchduleTimeL(TTime& aNextSchedule, TInt& aIndex,RPointerArray<CMsgSched>& aItemArray)
{
	TBool ret(EFalse);

	TTime NowTim;
	NowTim.HomeTime();
	
	aNextSchedule.HomeTime();
	aNextSchedule = aNextSchedule + TTimeIntervalYears(1);
	
	for(TInt i=0; i < aItemArray.Count(); i++)
	{
		if(aItemArray[i] && aItemArray[i]->iEnabled)
		{
			NowTim.HomeTime();
			
			if(aItemArray[i]->iRepeat == 0)
			{
				NowTim = aItemArray[i]->iTime;
			}
			else
			{
				NowTim = NowTim + CheckDaysType(NowTim.DayNoInWeek(),(aItemArray[i]->iRepeat - 1));
			}
			
			TTime NewAlarm(TDateTime(NowTim.DateTime().Year(),NowTim.DateTime().Month(),NowTim.DateTime().Day(),aItemArray[i]->iTime.DateTime().Hour(),aItemArray[i]->iTime.DateTime().Minute(), aItemArray[i]->iTime.DateTime().Second() ,0));
			
			NowTim.HomeTime();
			
			if(IsTimeBeforeL(NewAlarm,NowTim))
			{
				if(aItemArray[i]->iRepeat > 0)
				{
					NewAlarm = NewAlarm + AddNextScheduleDays(NowTim.DayNoInWeek(),(aItemArray[i]->iRepeat - 1));
				}
			}
			
			if(IsTimeBeforeL(NewAlarm,aNextSchedule))
			{
				ret = ETrue;
				aNextSchedule = NewAlarm;
				aIndex = i;
			}
		}
	}
	
	return ret;
}
/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
TTimeIntervalDays CExPolicy_Server::AddNextScheduleDays(TDay aDay, TInt aType)
{
	TTimeIntervalDays Ret(0);

	switch(aType)
	{
	case 1:// workdays
		if(aDay == EMonday)
		{
			aDay = ETuesday;
		}
		else if(aDay == ETuesday)
		{
			aDay = EWednesday;
		}
		else if(aDay == EWednesday)
		{
			aDay = EThursday;
		}
		else if(aDay == EThursday)
		{
			aDay = EFriday;
		}
		else if(aDay == EFriday)
		{
			aDay = ESaturday;
		}
		else if(aDay == ESaturday)
		{
			aDay = ESunday;
		}
		else if(aDay == ESunday)
		{
			aDay = EMonday;
		}
		
		Ret = 1 + AddDaysToWorkDays(aDay).Int();
		break;
	case 2://KtxNonwork
		if(aDay == EMonday)
		{
			aDay = ETuesday;
		}
		else if(aDay == ETuesday)
		{
			aDay = EWednesday;
		}
		else if(aDay == EWednesday)
		{
			aDay = EThursday;
		}
		else if(aDay == EThursday)
		{
			aDay = EFriday;
		}
		else if(aDay == EFriday)
		{
			aDay = ESaturday;
		}
		else if(aDay == ESaturday)
		{
			aDay = ESunday;
		}
		else if(aDay == ESunday)
		{
			aDay = EMonday;
		}
		
		Ret = 1 + AddDaysToNonWorkDays(aDay).Int();
		break;
	case 3://KtxMondays
		if(aDay == EMonday)
		{
			Ret = 7;
		}
		else if(aDay == ETuesday)
		{
			Ret = 6;
		}
		else if(aDay == EWednesday)
		{
			Ret = 5;
		}
		else if(aDay == EThursday)
		{
			Ret = 4;
		}
		else if(aDay == EFriday)
		{
			Ret = 3;
		}
		else if(aDay == ESaturday)
		{
			Ret = 2;
		}
		else if(aDay == ESunday)
		{
			Ret = 1;
		}
		break;
	case 4://KtxTuesdays
		if(aDay == EMonday)
		{
			Ret = 1;
		}
		else if(aDay == ETuesday)
		{
			Ret = 7;
		}
		else if(aDay == EWednesday)
		{
			Ret = 6;
		}
		else if(aDay == EThursday)
		{
			Ret = 5;
		}
		else if(aDay == EFriday)
		{
			Ret = 4;
		}
		else if(aDay == ESaturday)
		{
			Ret = 3;
		}
		else if(aDay == ESunday)
		{
			Ret = 2;
		}
		break;
	case 5://KtxWednesdays
		if(aDay == EMonday)
		{
			Ret = 2;
		}
		else if(aDay == ETuesday)
		{
			Ret = 1;
		}
		else if(aDay == EWednesday)
		{
			Ret = 7;
		}
		else if(aDay == EThursday)
		{
			Ret = 6;
		}
		else if(aDay == EFriday)
		{
			Ret = 5;
		}
		else if(aDay == ESaturday)
		{
			Ret = 4;
		}
		else if(aDay == ESunday)
		{
			Ret = 3;
		}
		break;
	case 6://KtxThursdays
		if(aDay == EMonday)
		{
			Ret = 3;
		}
		else if(aDay == ETuesday)
		{
			Ret = 2;
		}
		else if(aDay == EWednesday)
		{
			Ret = 1;
		}
		else if(aDay == EThursday)
		{
			Ret = 7;
		}
		else if(aDay == EFriday)
		{
			Ret = 6;
		}
		else if(aDay == ESaturday)
		{
			Ret = 5;
		}
		else if(aDay == ESunday)
		{
			Ret = 4;
		}
		break;
	case 7://KtxFridays
		if(aDay == EMonday)
		{
			Ret = 4;
		}
		else if(aDay == ETuesday)
		{
			Ret = 3;
		}
		else if(aDay == EWednesday)
		{
			Ret = 2;
		}
		else if(aDay == EThursday)
		{
			Ret = 1;
		}
		else if(aDay == EFriday)
		{
			Ret = 7;
		}
		else if(aDay == ESaturday)
		{
			Ret = 6;
		}
		else if(aDay == ESunday)
		{
			Ret = 5;
		}
		break;
	case 8://KtxSaturdays
		if(aDay == EMonday)
		{
			Ret = 5;
		}
		else if(aDay == ETuesday)
		{
			Ret = 4;
		}
		else if(aDay == EWednesday)
		{
			Ret = 3;
		}
		else if(aDay == EThursday)
		{
			Ret = 2;
		}
		else if(aDay == EFriday)
		{
			Ret = 1;
		}
		else if(aDay == ESaturday)
		{
			Ret = 7;
		}
		else if(aDay == ESunday)
		{
			Ret = 6;
		}
		break;
	case 9://KtxSundays
		if(aDay == EMonday)
		{
			Ret = 6;
		}
		else if(aDay == ETuesday)
		{
			Ret = 5;
		}
		else if(aDay == EWednesday)
		{
			Ret = 4;
		}
		else if(aDay == EThursday)
		{
			Ret = 3;
		}
		else if(aDay == EFriday)
		{
			Ret = 2;
		}
		else if(aDay == ESaturday)
		{
			Ret = 1;
		}
		else if(aDay == ESunday)
		{
			Ret = 7;
		}
		break;
	default://KtxEveryday
		Ret = 1;
		break;
	}

	return Ret;	
}
/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/	
TTimeIntervalDays CExPolicy_Server::CheckDaysType(TDay aDay, TInt aType)
{
	TTimeIntervalDays Ret(0);

	switch(aType)
	{
	case 1:// workdays
		Ret = AddDaysToWorkDays(aDay);
		break;
	case 2://KtxNonwork
		Ret = AddDaysToNonWorkDays(aDay);
		break;
	case 3://KtxMondays
		if(aDay == ETuesday)
		{
			Ret = 6;
		}
		else if(aDay == EWednesday)
		{
			Ret = 5;
		}
		else if(aDay == EThursday)
		{
			Ret = 4;
		}
		else if(aDay == EFriday)
		{
			Ret = 3;
		}
		else if(aDay == ESaturday)
		{
			Ret = 2;
		}
		else if(aDay == ESunday)
		{
			Ret = 1;
		}
		break;
	case 4://KtxTuesdays
		if(aDay == EMonday)
		{
			Ret = 1;
		}
		else if(aDay == EWednesday)
		{
			Ret = 6;
		}
		else if(aDay == EThursday)
		{
			Ret = 5;
		}
		else if(aDay == EFriday)
		{
			Ret = 4;
		}
		else if(aDay == ESaturday)
		{
			Ret = 3;
		}
		else if(aDay == ESunday)
		{
			Ret = 2;
		}
		break;
	case 5://KtxWednesdays
		if(aDay == EMonday)
		{
			Ret = 2;
		}
		else if(aDay == ETuesday)
		{
			Ret = 1;
		}
		else if(aDay == EThursday)
		{
			Ret = 6;
		}
		else if(aDay == EFriday)
		{
			Ret = 5;
		}
		else if(aDay == ESaturday)
		{
			Ret = 4;
		}
		else if(aDay == ESunday)
		{
			Ret = 3;
		}
		break;
	case 6://KtxThursdays
		if(aDay == EMonday)
		{
			Ret = 3;
		}
		else if(aDay == ETuesday)
		{
			Ret = 2;
		}
		else if(aDay == EWednesday)
		{
			Ret = 1;
		}
		else if(aDay == EFriday)
		{
			Ret = 6;
		}
		else if(aDay == ESaturday)
		{
			Ret = 5;
		}
		else if(aDay == ESunday)
		{
			Ret = 4;
		}
		break;
	case 7://KtxFridays
		if(aDay == EMonday)
		{
			Ret = 4;
		}
		else if(aDay == ETuesday)
		{
			Ret = 3;
		}
		else if(aDay == EWednesday)
		{
			Ret = 2;
		}
		else if(aDay == EThursday)
		{
			Ret = 1;
		}
		else if(aDay == ESaturday)
		{
			Ret = 6;
		}
		else if(aDay == ESunday)
		{
			Ret = 5;
		}
		break;
	case 8://KtxSaturdays
		if(aDay == EMonday)
		{
			Ret = 5;
		}
		else if(aDay == ETuesday)
		{
			Ret = 4;
		}
		else if(aDay == EWednesday)
		{
			Ret = 3;
		}
		else if(aDay == EThursday)
		{
			Ret = 2;
		}
		else if(aDay == EFriday)
		{
			Ret = 1;
		}
		else if(aDay == ESunday)
		{
			Ret = 6;
		}
		break;
	case 9://KtxSundays
		if(aDay == EMonday)
		{
			Ret = 6;
		}
		else if(aDay == ETuesday)
		{
			Ret = 5;
		}
		else if(aDay == EWednesday)
		{
			Ret = 4;
		}
		else if(aDay == EThursday)
		{
			Ret = 3;
		}
		else if(aDay == EFriday)
		{
			Ret = 2;
		}
		else if(aDay == ESaturday)
		{
			Ret = 1;
		}
		break;
	default://KtxEveryday
		// no need to add
		break;
	}

	return Ret;
}
/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
TTimeIntervalDays CExPolicy_Server::AddDaysToWorkDays(TDay aDay)
{
	TTimeIntervalDays Ret(0);

	TLocale locc;
    TUint currWorkDays = locc.WorkDays();
	
	if(aDay == EMonday)
	{
		if(currWorkDays & 0x01) //mon
			Ret = 0;
		else if(currWorkDays & 0x02)//Tue
			Ret = 1;
		else if(currWorkDays & 0x04)//Wed
			Ret = 2;
		else if(currWorkDays & 0x08)//Thur
			Ret = 3;
		else if(currWorkDays & 0x10)//Fri
			Ret = 4;
		else if(currWorkDays & 0x20)//Sat
			Ret = 5;
		else if(currWorkDays & 0x40)//Sun
			Ret = 6;
	}
	else if(aDay == ETuesday)
	{
		if(currWorkDays & 0x02)//Tue
			Ret = 0;
		else if(currWorkDays & 0x04)//Wed
			Ret = 1;
		else if(currWorkDays & 0x08)//Thur
			Ret = 2;
		else if(currWorkDays & 0x10)//Fri
			Ret = 3;
		else if(currWorkDays & 0x20)//Sat
			Ret = 4;
		else if(currWorkDays & 0x40)//Sun
			Ret = 5;
		else if(currWorkDays & 0x01) //mon
			Ret = 6;
	}
	else if(aDay == EWednesday)
	{
		if(currWorkDays & 0x04)//Wed
			Ret = 0;
		else if(currWorkDays & 0x08)//Thur
			Ret = 1;
		else if(currWorkDays & 0x10)//Fri
			Ret = 2;
		else if(currWorkDays & 0x20)//Sat
			Ret = 3;
		else if(currWorkDays & 0x40)//Sun
			Ret = 4;
		else if(currWorkDays & 0x01) //mon
			Ret = 5;
		else if(currWorkDays & 0x02)//Tue
			Ret = 6;
	}
	else if(aDay == EThursday)
	{
		if(currWorkDays & 0x08)//Thur
			Ret = 0;
		else if(currWorkDays & 0x10)//Fri
			Ret = 1;
		else if(currWorkDays & 0x20)//Sat
			Ret = 2;
		else if(currWorkDays & 0x40)//Sun
			Ret = 3;
		else if(currWorkDays & 0x01) //mon
			Ret = 4;
		else if(currWorkDays & 0x02)//Tue
			Ret = 5;
		else if(currWorkDays & 0x04)//Wed
			Ret = 6;
	}
	else if(aDay == EFriday)
	{
		if(currWorkDays & 0x10)//Fri
			Ret = 0;
		else if(currWorkDays & 0x20)//Sat
			Ret = 1;
		else if(currWorkDays & 0x40)//Sun
			Ret = 2;
		else if(currWorkDays & 0x01) //mon
			Ret = 3;
		else if(currWorkDays & 0x02)//Tue
			Ret = 4;
		else if(currWorkDays & 0x04)//Wed
			Ret = 5;
		else if(currWorkDays & 0x08)//Thur
			Ret = 6;
	}
	else if(aDay == ESaturday)
	{
		if(currWorkDays & 0x20)//Sat
			Ret = 0;
		else if(currWorkDays & 0x40)//Sun
			Ret = 1;
		else if(currWorkDays & 0x01) //mon
			Ret = 2;
		else if(currWorkDays & 0x02)//Tue
			Ret = 3;
		else if(currWorkDays & 0x04)//Wed
			Ret = 4;
		else if(currWorkDays & 0x08)//Thur
			Ret = 5;
		else if(currWorkDays & 0x10)//Fri
			Ret = 6;
	}
	else if(aDay == ESunday)
	{
		if(currWorkDays & 0x40)//Sun
			Ret = 0;
		else if(currWorkDays & 0x01) //mon
			Ret = 1;
		else if(currWorkDays & 0x02)//Tue
			Ret = 2;
		else if(currWorkDays & 0x04)//Wed
			Ret = 3;
		else if(currWorkDays & 0x08)//Thur
			Ret = 4;
		else if(currWorkDays & 0x10)//Fri
			Ret = 5;
		else if(currWorkDays & 0x20)//Sat
			Ret = 6;
	}
		
	return Ret;	
}
/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
TTimeIntervalDays CExPolicy_Server::AddDaysToNonWorkDays(TDay aDay)
{
	TTimeIntervalDays Ret(0);

	TLocale locc;
    TUint currWorkDays = locc.WorkDays();
	
	if(aDay == EMonday)
	{
		if(!(currWorkDays & 0x01)) //mon
			Ret = 0;
		else if(!(currWorkDays & 0x02))//Tue
			Ret = 1;
		else if(!(currWorkDays & 0x04))//Wed
			Ret = 2;
		else if(!(currWorkDays & 0x08))//Thur
			Ret = 3;
		else if(!currWorkDays & 0x10)//Fri
			Ret = 4;
		else if(!(currWorkDays & 0x20))//Sat
			Ret = 5;
		else if(!(currWorkDays & 0x40))//Sun
			Ret = 6;
	}
	else if(aDay == ETuesday)
	{
		if(!(currWorkDays & 0x02))//Tue
			Ret = 0;
		else if(!(currWorkDays & 0x04))//Wed
			Ret = 1;
		else if(!(currWorkDays & 0x08))//Thur
			Ret = 2;
		else if(!(currWorkDays & 0x10))//Fri
			Ret = 3;
		else if(!(currWorkDays & 0x20))//Sat
			Ret = 4;
		else if(!(currWorkDays & 0x40))//Sun
			Ret = 5;
		else if(!(currWorkDays & 0x01)) //mon
			Ret = 6;
	}
	else if(aDay == EWednesday)
	{
		if(!(currWorkDays & 0x04))//Wed
			Ret = 0;
		else if(!(currWorkDays & 0x08))//Thur
			Ret = 1;
		else if(!(currWorkDays & 0x10))//Fri
			Ret = 2;
		else if(!(currWorkDays & 0x20))//Sat
			Ret = 3;
		else if(!(currWorkDays & 0x40))//Sun
			Ret = 4;
		else if(!(currWorkDays & 0x01)) //mon
			Ret = 5;
		else if(!(currWorkDays & 0x02))//Tue
			Ret = 6;
	}
	else if(aDay == EThursday)
	{
		if(!(currWorkDays & 0x08))//Thur
			Ret = 0;
		else if(!(currWorkDays & 0x10))//Fri
			Ret = 1;
		else if(!(currWorkDays & 0x20))//Sat
			Ret = 2;
		else if(!(currWorkDays & 0x40))//Sun
			Ret = 3;
		else if(!(currWorkDays & 0x01)) //mon
			Ret = 4;
		else if(!(currWorkDays & 0x02))//Tue
			Ret = 5;
		else if(!(currWorkDays & 0x04))//Wed
			Ret = 6;
	}
	else if(aDay == EFriday)
	{
		if(!(currWorkDays & 0x10))//Fri
			Ret = 0;
		else if(!(currWorkDays & 0x20))//Sat
			Ret = 1;
		else if(!(currWorkDays & 0x40))//Sun
			Ret = 2;
		else if(!(currWorkDays & 0x01)) //mon
			Ret = 3;
		else if(!(currWorkDays & 0x02))//Tue
			Ret = 4;
		else if(!(currWorkDays & 0x04))//Wed
			Ret = 5;
		else if(!(currWorkDays & 0x08))//Thur
			Ret = 6;
	}
	else if(aDay == ESaturday)
	{
		if(!(currWorkDays & 0x20))//Sat
			Ret = 0;
		else if(!(currWorkDays & 0x40))//Sun
			Ret = 1;
		else if(!(currWorkDays & 0x01)) //mon
			Ret = 2;
		else if(!(currWorkDays & 0x02))//Tue
			Ret = 3;
		else if(!(currWorkDays & 0x04))//Wed
			Ret = 4;
		else if(!(currWorkDays & 0x08))//Thur
			Ret = 5;
		else if(!(currWorkDays & 0x10))//Fri
			Ret = 6;
	}
	else if(aDay == ESunday)
	{
		if(!(currWorkDays & 0x40))//Sun
			Ret = 0;
		else if(!(currWorkDays & 0x01)) //mon
			Ret = 1;
		else if(!(currWorkDays & 0x02))//Tue
			Ret = 2;
		else if(!(currWorkDays & 0x04))//Wed
			Ret = 3;
		else if(!(currWorkDays & 0x08))//Thur
			Ret = 4;
		else if(!(currWorkDays & 0x10))//Fri
			Ret = 5;
		else if(!(currWorkDays & 0x20))//Sat
			Ret = 6;
	}
		
	return Ret;	
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
	TTime SchedulingTime;
	SchedulingTime.HomeTime();

	if(AreWeStillOnL(Inddex,SchedulingTime))
	{
		if(Inddex >= 0&& iItemArray.Count() > Inddex)
		{
			if(!iAutoSchedule)
			{
				iAutoSchedule = CAutoScheduler::NewL(*this);
			}
			
			iAutoSchedule->NewScheduleL(iItemArray[Inddex],SchedulingTime);
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
