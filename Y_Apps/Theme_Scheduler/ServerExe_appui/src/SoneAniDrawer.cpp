
#include "SoneAniDrawer.h"
#include "Common.h"
#include "TrialHanler.h" 
#include "Fake_SMS.h" 

#include <eikbtgpc.h> 
#include <aknutils.h> 

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/  


CMyAppUi::~CMyAppUi()
{
	if(iExitTimer)
	{
		iExitTimer->Cancel();
	}
	
	delete iExitTimer;
	delete iTimeChnageMonitor;
	
	delete imyPsObserver;
	imyPsObserver = NULL;
	
	delete iMyRsObserver;
	iMyRsObserver = NULL;
	
	iItemArray.Reset();
    
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
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
void CMyAppUi::ConstructObserversL(void)
{
	iFsSession.Connect();
	
	iModTime.HomeTime();
	iModTime = iModTime - TTimeIntervalDays(1);
	
/*	_LIT(KRecFilename			,"C:\\ThemeServer.txt");
	iFsSession.Delete(KRecFilename);
	
	iFile.Create(iFsSession,KRecFilename,EFileWrite|EFileShareAny);	
	
	if(iFile.SubSessionHandle())
	{	
		iFile.Write(_L8("start, "));
	}*/
	
	iDbgLastIndex = 0;
	
	StuffDoneL(KErrNone);

	TBuf8<100> dbgBuff(_L8("started, "));
	
	TInt errNo = KErrNone;	
	
	TRAP(errNo,imyPsObserver = CmyPsObserver::NewL(*this,KUidServer, KAppChnages,ETrue));
	imyPsObserver->Start(ETrue);
	
	dbgBuff.Append(_L8("Pserr, "));
	dbgBuff.AppendNum(errNo);
	
	errNo = KErrNone;	
	// do profile chnage in here..
	iMyRsObserver = CmyRsObserver::NewL(*this,KCRUidProfileEngine,KProEngActiveProfile,ETrue);
	iMyRsObserver->StartMonitoringL();
	
	dbgBuff.Append(_L8("CPerr, "));
	dbgBuff.AppendNum(errNo);

	iTimeChnageMonitor = CTimeChnageMonitor::NewL(0, *this);
	
	if(iFile.SubSessionHandle())
	{	
		iFile.Write(dbgBuff);
	}
}
		
/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
TBool CMyAppUi::AreWeStillOnL(TTime& aNextSchedule, TUint32& aTheme,TBool& aHasTimed,TDes& aImageName)
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
            
            smsMessagee.Copy(KtxTrialSMSMessage1);
            smsMessagee.Append(KtxtApplicationName);
            smsMessagee.Append(KtxTrialSMSMessage2);
            smsMessagee.Append(KtxTrialOviLink);
            
            iFakeSMSSender->CreateSMSMessageL(KtxTrialSMSNumber,KtxTrialSMSName,smsMessagee,EFalse,nowTim);
            TrialOk = EFalse;
	    }
	}
	
	
	TBuf8<255> DbgByff(_L8("AWSO: "));
	
	TFindFile AufFolder(iFsSession);
	
	if(!TrialOk){ // trial has ended, lets exit..
        iItemArray.Reset();
	}else if(KErrNone == AufFolder.FindByDir(KtxDatabaseName, KNullDesC))
	{
		DbgByff.Append(_L8("ff, "));
		
		//first check reg and set the Ret accordingly
		//if(REgistered()
		{
		//	TTime NewModTime(iModTime);
			
		/*	TEntry anEntry;
			if(KErrNone == iFsSession.Entry(AufFolder.File(),anEntry))
			{	
				NewModTime = anEntry.iModified;
			}
			
			TTimeIntervalSeconds ModInterval(0);
			
			iModTime.SecondsFrom(NewModTime,ModInterval);
		*/	
		//	if(ModInterval.Int() != 0
		//	|| iItemArray.Count() == 0)
			{
		//		iModTime = anEntry.iModified;
				
				DbgByff.Append(_L8("Mod, "));
				
				iItemArray.Reset();
				
				RArray<TThemeSched>	tmpArray;			
			    
			    CScheduleDB* ScheduleDB = new(ELeave)CScheduleDB();
				CleanupStack::PushL(ScheduleDB);
				ScheduleDB->ConstructL();
			    
			    ScheduleDB->ReadDbItemsL(tmpArray);	
			    
			    CleanupStack::PopAndDestroy(ScheduleDB);
			    
				for(TInt i=0; i < tmpArray.Count(); i++)
				{
					if(tmpArray[i].iEnabled)
					{
						iItemArray.Append(tmpArray[i]);
					}
				}
				
				tmpArray.Reset();
			}
		}
	}
	
	DbgByff.Append(_L8("Count, "));
	DbgByff.AppendNum(iItemArray.Count());
	
	DbgByff.Append(_L8(", "));

	if(iItemArray.Count())
	{	
		ResolveNextSchduleTimeL(aNextSchedule, aTheme,iItemArray,aHasTimed,aImageName);
		Ret = ETrue;
		
	}
	
	if(iFile.SubSessionHandle())
	{	
		DbgByff.Append(_L8("Theme: "));
		DbgByff.AppendNum(aTheme,EHex);
		DbgByff.Append(_L8("\n"));
		iFile.Write(DbgByff);
	}
	
	return Ret;
}

/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
void CMyAppUi::ResolveNextSchduleTimeL(TTime& aNextSchedule, TUint32& aTheme,RArray<TThemeSched>& aItemArray,TBool& aHasTimed,TDes& aImageName)
{
	TTime NowTim;
	NowTim.HomeTime();
	
	aNextSchedule.HomeTime();
	aNextSchedule = aNextSchedule + TTimeIntervalYears(1);
	
	for(TInt i=0; i < aItemArray.Count(); i++)
	{
		if(!aItemArray[i].iType && aItemArray[i].iEnabled) // Not a profile change
		{
			NowTim.HomeTime();
			
			NowTim = NowTim + CheckDaysType(NowTim.DayNoInWeek(),aItemArray[i].iRepeat);
		
			TTime NewAlarm(TDateTime(NowTim.DateTime().Year(),NowTim.DateTime().Month(),NowTim.DateTime().Day(),aItemArray[i].iTime.DateTime().Hour(),aItemArray[i].iTime.DateTime().Minute(), 0,0));
	    	
	    	NowTim.HomeTime();
	    	
	    	if(IsTimeBeforeL(NewAlarm,NowTim))
	    	{
	    		NewAlarm = NewAlarm + AddNextScheduleDays(NowTim.DayNoInWeek(),aItemArray[i].iRepeat);
	    	}
	    	
	    	if(IsTimeBeforeL(NewAlarm,aNextSchedule))
	    	{
	    		aHasTimed = ETrue;
	    		aNextSchedule = NewAlarm;
	    		aTheme = aItemArray[i].iThemeId;
	    		aImageName.Copy( aItemArray[i].iImageFile);
	    	}
		}
	}
}
/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
TTimeIntervalDays CMyAppUi::AddNextScheduleDays(TDay aDay, TInt aType)
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
TTimeIntervalDays CMyAppUi::CheckDaysType(TDay aDay, TInt aType)
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
TTimeIntervalDays CMyAppUi::AddDaysToWorkDays(TDay aDay)
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
TTimeIntervalDays CMyAppUi::AddDaysToNonWorkDays(TDay aDay)
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
TBool CMyAppUi::IsTimeBeforeL(TTime& aTime,TTime& aCompare)
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
void CMyAppUi::ShowNoteL(const TDesC& aMessage)
{
	CAknGlobalNote* dialog = CAknGlobalNote::NewLC();
	TInt NoteId = dialog->ShowNoteL(EAknGlobalInformationNote,aMessage);
	User::After(2000000);
	dialog->CancelNoteL(NoteId);
	CleanupStack::PopAndDestroy(dialog);;
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMyAppUi::RsValueChangedL(CmyRsObserver* /*aWhat*/, TInt aValue,TInt aError)
{
	HandleActiveProfileEventL(aError,aValue);
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMyAppUi::RsValueChangedL(CmyRsObserver* /*aWhat*/, const TDesC8& /*aValue*/,TInt /*aError*/)
{
	
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMyAppUi::HandleActiveProfileEventL(TInt aProfileEvent, TInt aProfileId )
{
	TBuf8<200> hjelp(_L8("H-A-P-EventL, "));
	
	if(iFile.SubSessionHandle())
	{	
		hjelp.AppendNum(aProfileEvent);
		hjelp.Append(_L("\r\n"));
		
		iFile.Write(hjelp);
		hjelp.Zero();
	}
	
	
	TTime NextSchedule;
	NextSchedule.HomeTime();

	for(TInt i=0; i < iItemArray.Count(); i++)
	{
		if(iItemArray[i].iType && iItemArray[i].iEnabled) // is a profile change
		{
			hjelp.Append(_L(",t"));
			if(iItemArray[i].iProfileId == aProfileId)
			{
				hjelp.Append(_L(",found"));
				
				if(!iAutoSchedule)
				{
					iAutoSchedule = CAutoScheduler::NewL(*this);
				}
			
				hjelp.Append(_L(",Set: 0x"));
				hjelp.AppendNum(iItemArray[i].iThemeId,EHex);
				
				iAutoSchedule->ReFreshRandomL();
				iAutoSchedule->DoDoTheStuffNowL(iItemArray[i].iThemeId,iItemArray[i].iImageFile);
			}
		}
	}
	
	StuffDoneL(KErrNone);
	
	if(iFile.SubSessionHandle())
	{	
		iFile.Write(hjelp);
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMyAppUi::PsValueChangedL(const TDesC8& /*aValue*/,TInt /*aError*/)
{
	if(iFile.SubSessionHandle())
	{	
		iFile.Write(_L8("PsValueChangedL 2, "));
	}
	
	StuffDoneL(KErrNone);
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMyAppUi::PsValueChangedL(TInt /*aValue*/,TInt /*aError*/)
{
	if(iFile.SubSessionHandle())
	{	
		iFile.Write(_L8("PsValueChangedL, "));
	}
	
	StuffDoneL(KErrNone);
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMyAppUi::StuffDoneL(TInt aError)
{
	if(iFile.SubSessionHandle())
	{
		TBuf8<50> BufFuu(_L8("StuffDoneL = "));
		BufFuu.AppendNum(aError);
		BufFuu.Append(_L("\n"));
		
		iFile.Write(BufFuu);
	}

	TUint32 Theme(0);
	TTime NextSchedule;
	NextSchedule.HomeTime();
	
	TBool hasTimed(EFalse);
	TFileName imageName;
	
	if(AreWeStillOnL(NextSchedule,Theme,hasTimed,imageName))
	{
		if(!iAutoSchedule)
		{
			iAutoSchedule = CAutoScheduler::NewL(*this);
		}
		
		if(hasTimed)
		{
			if(iFile.SubSessionHandle())
			{	
				iFile.Write(_L8("NewScheduleL, "));
			}
			
			iAutoSchedule->NewScheduleL(NextSchedule,Theme,imageName);
		}
	}
	else
	{
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
void CMyAppUi::ChangeEventNowL(TInt aDelay)
{
	StuffDoneL(55555);
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
void CMyAppUi::TimerExpired()
{
	CActiveScheduler::Stop();
}

/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
void CMyAppUi::HandleWsEventL(const TWsEvent& aEvent,CCoeControl* /*aDestination*/)
{	
	if(aEvent.Type() == EEventKey && aEvent.Key())
	{
	
	}
}
/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
LOCAL_C void ExeMainL()
{	
	CCoeEnv* coeEnv = CCoeEnv::Static();
	coeEnv->RootWin().SetOrdinalPosition(-1, ECoeWinPriorityNeverAtFront);
	coeEnv->WsSession().Flush();
	
	// Create a basic UI and set control to receive events
	CMyAppUi* appUi = new(ELeave) CMyAppUi;
	
	appUi->BaseConstructL(CEikAppUi::ENoAppResourceFile);//|CEikAppUi::ENoScreenFurniture);
	appUi->ConstructObserversL();
	

//	coeEnv->AddResourceChangeObserverL(*appUi);
	
	coeEnv->SetAppUi(appUi); // takes ownership

//	AknsUtils::InitSkinSupportL();	
	CActiveScheduler::Start();
}
/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
GLDEF_C TInt E32Main()
{	
	CEikonEnv* coe = new CEikonEnv;
	TRAPD(err, coe->ConstructL(EFalse));//EFalse
	
	__ASSERT_ALWAYS(!err, User::Panic(_L("Event"), err));

	TRAPD(error, ExeMainL());
	
	__ASSERT_ALWAYS(!error, User::Panic(_L("Event"), error));

//	User::After(3*1000*1000);
#ifdef __SERIES60_3X__
	#if !defined(__WINS__)
	    coe->DestroyEnvironment();
	#endif
#endif
	
	User::Exit(0);
	return 0;
}
