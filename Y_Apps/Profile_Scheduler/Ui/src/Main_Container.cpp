/* Copyright (c) 2001 - 2008, Dr. Jukka Silvennoinen. All rights reserved */

#include <barsread.h>

#include <EIKSPANE.H>
#include <AknAppUi.h>
#include <EIKBTGPC.H>
#include <aknnavi.h> 
#include <aknnavide.h> 
#include <akntabgrp.h> 
#include <aknlists.h> 
#include <akniconarray.h> 
#include <eikmenub.h>
#include <aknquerydialog.h> 
#include <AknIconArray.h>
#include <bautils.h>
#include <stringloader.h> 
#include <AknGlobalNote.h>

#include "Main_Container.h"                     // own definitions

#include "YApp_E8876005.hrh"                   // own resource header
#include "YApp_E8876005.h"

#ifdef SONE_VERSION
	#include <YApp_2002884E.rsg>
#else
	#ifdef LAL_VERSION
		#include <YApp_20022080.rsg>
	#else
		#include <YApp_E8876005.rsg>
	#endif
#endif

#include "Profile_Settings.h"
#include "Help_Container.h"

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CMainContainer::CMainContainer(CEikButtonGroupContainer* aCba)
:iCba(aCba)
{		

}

	/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CMainContainer::~CMainContainer()
{     
	delete iMyHelpContainer;
	iMyHelpContainer = NULL;
	
    delete iProfileBox;
    
    iItemArray.Reset();
#ifdef LAL_VERSION    
    delete iApi;
#endif     
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::ConstructL(void)
{
	CreateWindowL();
	
#ifdef SONE_VERSION
	iAddIsValid = ETrue;
#else
	#ifdef LAL_VERSION
		iAddIsValid = EFalse;
		iApi=MAdtronicApiClient::NewL((const TUint8* const)"510AED7C");
	#else
		iAddIsValid = ETrue;
	#endif
#endif
		
	SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());

	MakeProfileBoxL();
	
	ActivateL();
	SetMenuL();
	
#ifdef LAL_VERSION
	iApi->StatusL(this);
//	installationValid();
#endif
	
	DrawNow();
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::SizeChanged()
{
	if(iProfileBox)
	{
		iProfileBox->SetRect(Rect());
	}
	
	if(iProfileSettings)
	{
		iProfileSettings->SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
	}	
	
	if(iMyHelpContainer)
	{
		iMyHelpContainer->SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect()); // Sets rectangle of lstbox.
	}	
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::HandleResourceChange(TInt aType)
{
	if ( aType==KEikDynamicLayoutVariantSwitch )
	{  	
		TRect rect;
		AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, rect);
	
		SetRect(rect);
	}
	
	CCoeControl::HandleResourceChange(aType);
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CMainContainer::installationValid()
{
	iAddIsValid = ETrue;
	MakeProfileBoxL();
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CMainContainer::installationInvalid()
{
	iAddIsValid = EFalse;
	STATIC_CAST(CMgAppUi*,CEikonEnv::Static()->AppUi())->ShowNoteL(_L("You need to install and activate adtronic"));
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CMainContainer::adtronicNotInstalled()
{
	iAddIsValid = EFalse;
	STATIC_CAST(CMgAppUi*,CEikonEnv::Static()->AppUi())->ShowNoteL(_L("You need to install and activate adtronic"));
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CMainContainer::MakeProfileBoxL()
{
	TInt MySetIndex(0);
	
	if(iProfileBox)
	{
		MySetIndex = iProfileBox->CurrentItemIndex();
	}
    
	delete iProfileBox;
	iProfileBox = NULL;
	
    iProfileBox   = new( ELeave ) CAknDoubleTimeStyleListBox();
	iProfileBox->ConstructL(this,EAknListBoxSelectionList);
	iProfileBox->Model()->SetItemTextArray(GetProfilesArrayL());
	
    iProfileBox->Model()->SetOwnershipType(ELbmOwnsItemArray);;
	iProfileBox->CreateScrollBarFrameL( ETrue );
    iProfileBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
	iProfileBox->SetRect(Rect());
	iProfileBox->ActivateL();

	TInt ItemsCount = iProfileBox->Model()->ItemTextArray()->MdcaCount();
	
	if(ItemsCount > MySetIndex && MySetIndex >= 0)
		iProfileBox->SetCurrentItemIndex(MySetIndex);
	else if(ItemsCount > 0)
		iProfileBox->SetCurrentItemIndex(0);
	
	UpdateScrollBar(iProfileBox);
	iProfileBox->DrawNow();
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CDesCArray* CMainContainer::GetProfilesArrayL(void)
{
	CDesCArrayFlat* MyArray = new(ELeave)CDesCArrayFlat(1);
	CleanupStack::PushL(MyArray);

	iItemArray.Reset();
    
	if(iAddIsValid)
	{
		RArray<TProfSched>	ItemTmpArray;
	
		CScheduleDB* ScheduleDB = new(ELeave)CScheduleDB();
		CleanupStack::PushL(ScheduleDB);
		ScheduleDB->ConstructL();
		ScheduleDB->ReadDbItemsL(ItemTmpArray);	
		CleanupStack::PopAndDestroy(ScheduleDB);
		
		TTime NextSchedule;
		TInt NextIndex(0);
		do
		{	
			NextSchedule.HomeTime();
			NextIndex = 0;
			ResolveNextSchduleTimeL(NextSchedule,NextIndex,ItemTmpArray);
			
			if(NextIndex < 0 && NextIndex >= ItemTmpArray.Count())
			{
				NextIndex = 0;
			}
			
			if(NextIndex >= 0 && NextIndex < ItemTmpArray.Count())
			{
				TProfSchedExt NewIttem;
				
				NewIttem.iIndex			= ItemTmpArray[NextIndex].iIndex;
				NewIttem.iProfileId		= ItemTmpArray[NextIndex].iProfileId;
				NewIttem.iRepeat		= ItemTmpArray[NextIndex].iRepeat;
				NewIttem.iTime			= ItemTmpArray[NextIndex].iTime;
				NewIttem.iEnabled		= ItemTmpArray[NextIndex].iEnabled;
				NewIttem.iNextSchedule	= NextSchedule;
				
				iItemArray.Append(NewIttem);
				
				ItemTmpArray.Remove(NextIndex);
			}
			
			
		}while(ItemTmpArray.Count());
		
		
		TBuf<250> Bufff;
		TBuf<100> HjelpBuf1,HjelpBuf2;
		
		for(TInt i=0; i < iItemArray.Count(); i++)
		{
			Bufff.Zero();
	
			STATIC_CAST(CMgAppUi*,CEikonEnv::Static()->EikAppUi())->GetRepeatTypeName(iItemArray[i].iRepeat,HjelpBuf1);
			STATIC_CAST(CMgAppUi*,CEikonEnv::Static()->EikAppUi())->GetProfileNameL(iItemArray[i].iProfileId,HjelpBuf2);//,Bufff);
			
	/*		if(Bufff.Length()
			&& BaflUtils::FileExists(CEikonEnv::Static()->FsSession(),Bufff))
			{
				iItemArray[i]->iRingtone = Bufff.AllocL();
			}
	*/		
			Bufff.Num(iItemArray[i].iTime.DateTime().Hour());
			Bufff.Append(_L(":"));
			if(iItemArray[i].iTime.DateTime().Minute() < 10)
			{
				Bufff.AppendNum(0);	
			}
			Bufff.AppendNum(iItemArray[i].iTime.DateTime().Minute());
			Bufff.Append(_L("\t"));
			
			if(iItemArray[i].iEnabled){
			
				if(iItemArray[i].iNextSchedule.DateTime().Day() < 9)
				{
					Bufff.AppendNum(0);	
				}
				Bufff.AppendNum(iItemArray[i].iNextSchedule.DateTime().Day() + 1);
				//Bufff.Append(DateChar);
				Bufff.Append(_L("/"));
				
				TMonthNameAbb ThisMonth(iItemArray[i].iNextSchedule.DateTime().Month());
				
				Bufff.Append(ThisMonth);
				
				Bufff.Append(_L("/"));
				
				TInt Yearrr = iItemArray[i].iNextSchedule.DateTime().Year();
				TInt Hudreds = Yearrr / 100;
				
				Yearrr = (Yearrr - (Hudreds * 100));
				if(Yearrr < 10)
				{
					Bufff.AppendNum(0);	
				}
				
				Bufff.AppendNum(Yearrr);
			}
			
			Bufff.Append(_L("\t"));
			Bufff.Append(HjelpBuf2);
			Bufff.Append(_L("\t"));
			
			
			
			if(iItemArray[i].iEnabled){
				Bufff.Append(HjelpBuf1);
			}else{
				StringLoader::Load(HjelpBuf1,R_SH_STR_DISABLED);
				Bufff.Append(HjelpBuf1);
			}
			
			MyArray->AppendL(Bufff);
		}
		
		ItemTmpArray.Reset();
	}
	
	CleanupStack::Pop(MyArray);
	return MyArray;
}

/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
void CMainContainer::ResolveNextSchduleTimeL(TTime& aNextSchedule, TInt& aIndex,RArray<TProfSched>& aItemArray)
{
	TTime NowTim;
	NowTim.HomeTime();
	
	aNextSchedule.HomeTime();
	aNextSchedule = aNextSchedule + TTimeIntervalYears(1);
	
	for(TInt i=0; i < aItemArray.Count(); i++)
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
    		aNextSchedule = NewAlarm;
    		aIndex = i;
    	}
	}
}
/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
TBool CMainContainer::IsTimeBeforeL(TTime& aTime,TTime& aCompare)
{
	TBool IsBefore(EFalse);
	TTimeIntervalMinutes MinInterval(0);
	TTimeIntervalHours 	 HourInterval(0);
	
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
				IsBefore = ETrue;
			}
		}
	}
	
	return IsBefore;
}
/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/	
TTimeIntervalDays CMainContainer::CheckDaysType(TDay aDay, TInt aType)
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
TTimeIntervalDays CMainContainer::AddDaysToWorkDays(TDay aDay)
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
TTimeIntervalDays CMainContainer::AddDaysToNonWorkDays(TDay aDay)
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
TTimeIntervalDays CMainContainer::AddNextScheduleDays(TDay aDay, TInt aType)
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
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::UpdateScrollBar(CEikListBox* aListBox)
    {
    if (aListBox)
        {   
        TInt pos(aListBox->View()->CurrentItemIndex());
        if (aListBox->ScrollBarFrame())
            {
            aListBox->ScrollBarFrame()->MoveVertThumbTo(pos);
            }
        }
    }

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::Draw(const TRect& /*aRect*/) const
{
	CWindowGc& gc = SystemGc();

	gc.Clear(Rect());
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
TKeyResponse CMainContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aEventCode)
{
	TKeyResponse Ret = EKeyWasNotConsumed;	

	if(iMyHelpContainer)
	{
		Ret = iMyHelpContainer->OfferKeyEventL(aKeyEvent,aEventCode);
	}
	else if(iProfileSettings)
	{
		return iProfileSettings->OfferKeyEventL(aKeyEvent,aEventCode);
	}
	else
	{
		switch (aKeyEvent.iCode)
	    {
	    case EKeyDevice3:
			CEikonEnv::Static()->AppUiFactory()->MenuBar()->TryDisplayMenuBarL();
			break;
		case EKeyRightArrow:
		case EKeyLeftArrow:
		default:
			if(iProfileBox)
			{
				Ret = iProfileBox->OfferKeyEventL(aKeyEvent,aEventCode);
			}
			break;
		}
	}
	
	return Ret;
}


/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CMainContainer::SetRandomProfsL(void)
{
	MProfileEngine* ProfileEngine = STATIC_CAST(CMgAppUi*,CEikonEnv::Static()->EikAppUi())->ProfileEngine();
	
	if(ProfileEngine)
	{
		CScheduleDB* ScheduleDB = new(ELeave)CScheduleDB();
		CleanupStack::PushL(ScheduleDB);
		ScheduleDB->ConstructL();
		
		MProfilesNamesArray* Arrr = ProfileEngine->ProfilesNamesArrayLC();
		
		CArrayFixFlat<TInt>* SelectedItems = new(ELeave)CArrayFixFlat<TInt>(10);
		CleanupStack::PushL(SelectedItems);
		
		CDesCArrayFlat* SelectArrray = new(ELeave)CDesCArrayFlat(20);
		CleanupStack::PushL(SelectArrray);
		
		CAknListQueryDialog* dialog = new(ELeave)CAknListQueryDialog(SelectedItems);
		dialog->PrepareLC(R_RANDOM_SELECTION_QUERY);
				
		TFileName Hjelpper;
		
		CArrayFixFlat<TInt>* HadItems = new(ELeave)CArrayFixFlat<TInt>(10);
		CleanupStack::PushL(HadItems);
  		
  		ScheduleDB->GetRandomProfilesL(*HadItems);
    	
		
		for (TInt i=0; i < Arrr->MdcaCount(); i++)
		{
			Hjelpper.Num(1);
			Hjelpper.Append(_L("\t"));
			
			if(Arrr->ProfileName (i))
			{
				Hjelpper.Append(Arrr->ProfileName (i)->ShortName());
			}
			else
			{
				StringLoader::Load(Hjelpper,R_SH_STR_NONAME);
			}
			
			for(TInt p=0; p < HadItems->Count(); p++)
			{
				if(HadItems->At(p) == Arrr->ProfileName(i)->Id())
				{
					dialog->ListBox()->View()->ToggleItemL(i);
				}
			}
					
			SelectArrray->AppendL(Hjelpper);
		}			
		
		CleanupStack::PopAndDestroy(HadItems);
			
		dialog->SetItemTextArray(SelectArrray); 
		dialog->SetOwnershipType(ELbmDoesNotOwnItemArray); 

		CArrayPtr<CGulIcon>* icons =new( ELeave ) CAknIconArray(1);
		CleanupStack::PushL(icons);
		
		_LIT(KtxAvkonMBM	,"Z:\\resource\\apps\\avkon2.mif");
		
		CFbsBitmap* FrameMsk2(NULL);
		CFbsBitmap* FrameImg2(NULL);
		AknIconUtils::CreateIconL(FrameImg2,FrameMsk2,KtxAvkonMBM,EMbmAvkonQgn_indi_checkbox_on,EMbmAvkonQgn_indi_checkbox_on_mask);

		icons->AppendL(CGulIcon::NewL(FrameImg2,FrameMsk2));
		
		CFbsBitmap* FrameMsk(NULL);
		CFbsBitmap* FrameImg(NULL);
		AknIconUtils::CreateIconL(FrameImg,FrameMsk,KtxAvkonMBM,EMbmAvkonQgn_indi_checkbox_off,EMbmAvkonQgn_indi_checkbox_off_mask);

		icons->AppendL(CGulIcon::NewL(FrameImg,FrameMsk));
		
		CleanupStack::Pop(icons);
		dialog->SetIconArrayL(icons); 

		if (dialog->RunLD ())
		{
			CArrayFixFlat<TInt>* AddItems = new(ELeave)CArrayFixFlat<TInt>(10);
			CleanupStack::PushL(AddItems);
			// clear all, and set... 
			for(TInt ii=0; ii < SelectedItems->Count(); ii++)
			{
				TInt SelType = SelectedItems->At(ii);
				if(SelType >= 0 && SelType < Arrr->MdcaCount())
				{
					AddItems->AppendL(Arrr->ProfileName(SelType)->Id());
				}
			}
			
			
			ScheduleDB->SetRandomProfilesL(*AddItems);
		
			CleanupStack::PopAndDestroy(AddItems);
		}

		CleanupStack::PopAndDestroy(2);  //SelectedItems,SelectArrray
		CleanupStack::PopAndDestroy(1);//Arrr
		CleanupStack::PopAndDestroy(ScheduleDB);
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::ShowNoteL(const TDesC& aMessage)
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
void CMainContainer::HandleViewCommandL(TInt aCommand)
{
	switch(aCommand)
	{
	case EAppHelpBack:
		{
			delete iMyHelpContainer;
			iMyHelpContainer = NULL;
		}
		SetMenuL();
		DrawNow();
		break;
	case EAppHelp:
		{
			delete iMyHelpContainer;
			iMyHelpContainer = NULL;    		
			iMyHelpContainer = CMyHelpContainer::NewL();
		}
		SetMenuL();
		DrawNow();    		
		break;	
	case EProfileModify:
		{
			TVwsViewId id = TVwsViewId( TUid::Uid(0x100058F8),TUid::Uid(0x01));
			TBuf8<2> Dummy;
			STATIC_CAST(CAknAppUi*,CEikonEnv::Static()->EikAppUi())->ActivateViewL( id, TUid::Uid(0x00),Dummy);
		}
		break;
	case EProfDisable:
	case EProfEnable:
		if(iProfileBox)
		{
			TInt Curr = iProfileBox->CurrentItemIndex();
			if(Curr >= 0 && Curr < iItemArray.Count())
			{
				if(aCommand == EProfEnable)
					iItemArray[Curr].iEnabled = ETrue;	
				else // EProfDisable
					iItemArray[Curr].iEnabled = EFalse;	
				
				CScheduleDB* ScheduleDB = new(ELeave)CScheduleDB();
				CleanupStack::PushL(ScheduleDB);
				ScheduleDB->ConstructL();								
				ScheduleDB->UpdateDatabaseL(iItemArray[Curr].iProfileId,iItemArray[Curr].iRepeat,iItemArray[Curr].iTime,iItemArray[Curr].iIndex,iItemArray[Curr].iEnabled);

				CleanupStack::PopAndDestroy(ScheduleDB);
				
				iProfileBox->Model()->SetItemTextArray(GetProfilesArrayL());
			}
		}
		DrawNow();
		break;		
	case EProfPlay:
		{
			
		}
		break;
	case EProfRandom:
		{
			SetRandomProfsL();
		}
		break;
	case EProfModify:
		if(iProfileBox)
		{
			TInt Curr = iProfileBox->CurrentItemIndex();
			if(Curr >= 0 && Curr < iItemArray.Count())
			{
				// get current and add data to settings..
				delete iProfileSettings;
				iProfileSettings = NULL;
				iProfileSettings = new(ELeave)CProfileSettings();
				iProfileSettings->ConstructL();
				iProfileSettings->SetDataL(iItemArray[Curr].iProfileId,iItemArray[Curr].iRepeat,iItemArray[Curr].iTime,iItemArray[Curr].iIndex,iItemArray[Curr].iEnabled);
			}
		}
		SetMenuL();
		DrawNow();
		break;
	case EProfRemove:
		if(iProfileBox)
		{
			TInt Curr = iProfileBox->CurrentItemIndex();
			if(Curr >= 0 && Curr < iItemArray.Count())
			{
				TBuf<60> Hjelppersmal;
				StringLoader::Load(Hjelppersmal,R_SH_STR_REMOVESCHED);
			
				CAknQueryDialog* dlg = CAknQueryDialog::NewL();
				if(dlg->ExecuteLD(R_QUERY,Hjelppersmal))
				{
					CScheduleDB* ScheduleDB = new(ELeave)CScheduleDB();
					CleanupStack::PushL(ScheduleDB);
					ScheduleDB->ConstructL();
				  	ScheduleDB->DeleteFromDatabaseL(iItemArray[Curr].iIndex);
				    CleanupStack::PopAndDestroy(ScheduleDB);
				    
					MakeProfileBoxL();
				}
			}
		}
		SetMenuL();
		DrawNow();
		break;
	case EProfNew:
		{		
			delete iProfileSettings;
			iProfileSettings = NULL;
			iProfileSettings = new(ELeave)CProfileSettings();
			iProfileSettings->ConstructL();
		}
		SetMenuL();
		DrawNow();
		break;
	case ESettOk:
		if(iProfileSettings)
		{
			TInt 		MyIndex(-1);
        	TInt		MyProfileId(-1);
        	TInt		MyRepeat(0);
        	TTime		MyTime;
        	TBool		myEnabled(EFalse);
        	
        	MyTime.HomeTime();	
        	
        	iProfileSettings->GetValuesL(MyProfileId,MyRepeat,MyTime,MyIndex,myEnabled);
        	
			CScheduleDB* ScheduleDB = new(ELeave)CScheduleDB();
			CleanupStack::PushL(ScheduleDB);
			ScheduleDB->ConstructL();
			
			if(MyIndex >= 0)
				ScheduleDB->UpdateDatabaseL(MyProfileId,MyRepeat,MyTime,MyIndex,myEnabled);
			else
				ScheduleDB->SaveToDatabaseL(MyProfileId,MyRepeat,MyTime,MyIndex,myEnabled);
			
			CleanupStack::PopAndDestroy(ScheduleDB);
				    
			MakeProfileBoxL();
		}
	case ESettCancel:
		{
			delete iProfileSettings;
			iProfileSettings = NULL;
		}
		SetMenuL();
		DrawNow();
		break;
	default:

		break;
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CCoeControl* CMainContainer::ComponentControl( TInt /*aIndex*/) const
{
	CCoeControl* RetCont(NULL);

	if(iMyHelpContainer)
	{
		RetCont = iMyHelpContainer;		
	}
	else if(iProfileSettings)
	{
		RetCont = iProfileSettings;
	}
	else 
	{
		RetCont = iProfileBox;
	}
	
	return RetCont;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TInt CMainContainer::CountComponentControls() const
{
	TInt RetCount(0);
	
	if(iMyHelpContainer)
	{
		RetCount = 1;
	}
	else if(iProfileSettings)
	{
		RetCount = 1;
	}
	else 
	{
		RetCount = 1;
	}
 
	return RetCount;
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::SetMenuL(void)
{
	CEikonEnv::Static()->AppUiFactory()->MenuBar()->StopDisplayingMenuBar();

	TInt MenuRes(R_MAIN_MENUBAR);
	TInt ButtomRes(R_MAIN_CBA);
	
	if(iMyHelpContainer)
	{
		ButtomRes = R_APPHELP_CBA;
	}
	else if(iProfileSettings)
	{
		ButtomRes = R_SETT_CBA;
	}
	
	CEikonEnv::Static()->AppUiFactory()->MenuBar()->SetMenuTitleResourceId(MenuRes);
	
	if(iCba)
	{
		iCba->SetCommandSetL(ButtomRes);
		iCba->DrawDeferred();
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::InitMenuPanelL(TInt aResourceId, CEikMenuPane* aMenuPane)
{
	if(aResourceId == R_MAIN_MENU)
	{
		TBool Dim(ETrue),DimPlay(ETrue),DimEnable(ETrue);
	
		if(iProfileBox)
		{
			TInt CurrInd = iProfileBox->CurrentItemIndex();
			if(CurrInd >= 0)
			{
				if(CurrInd < iItemArray.Count())
				{
					/*if(iItemArray[CurrInd])
					{
						if(iItemArray[CurrInd]->iRingtone)
						{
							DimPlay = EFalse;
						}
					}*/
				}
				
				Dim = EFalse;
			}
			
			if(CurrInd >= 0 && CurrInd < iItemArray.Count())
			{
				DimEnable = iItemArray[CurrInd].iEnabled;
			}
		}
		
#ifdef SONE_VERSION
    if(!KAppIsTrial){
        aMenuPane->SetItemDimmed(EVisitDrJukka,ETrue);
    } 
#else

#endif  
		
		if(DimPlay)
		{
			aMenuPane->SetItemDimmed(EProfPlay,ETrue);
		}
		
		if(!iAddIsValid)
		{
			aMenuPane->SetItemDimmed(EProfNew,ETrue);
			aMenuPane->SetItemDimmed(EProfDisable,ETrue);
			aMenuPane->SetItemDimmed(EProfEnable,ETrue);
			
			aMenuPane->SetItemDimmed(EProfRemove,ETrue);
			aMenuPane->SetItemDimmed(EProfModify,ETrue);
			aMenuPane->SetItemDimmed(EProfPlay,ETrue);
			aMenuPane->SetItemDimmed(EProfRandom,ETrue);
			aMenuPane->SetItemDimmed(EProfileModify,ETrue);
		}
		
		if(Dim){
			aMenuPane->SetItemDimmed(EProfRemove,ETrue);
			aMenuPane->SetItemDimmed(EProfModify,ETrue);
			aMenuPane->SetItemDimmed(EProfDisable,ETrue);
			aMenuPane->SetItemDimmed(EProfEnable,ETrue);
		}else {
		
			if(DimEnable)
				aMenuPane->SetItemDimmed(EProfEnable,ETrue);
			else
				aMenuPane->SetItemDimmed(EProfDisable,ETrue);
		}
	}
}

		

