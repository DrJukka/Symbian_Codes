/* 
	Copyright (c) 2001 - 2007, 
	Jukka Silvennoinen 
	S-One Telecom LTd. All rights reserved 
*/
#include <e32svr.h>
#include <e32math.h>
#include <apacmdln.h>
#include <APGCLI.H>
#include <bautils.h>
#include <w32std.h>
#include <COEMAIN.H>
#include <EIKFUTIL.H>
#include <aknglobalnote.h> 
#include <BAUTILS.H>
#include <aknswallpaperutils.h>
 
#include "ThemeDataBase.h" 
#include "AutoScheduler.h"
	
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CAutoScheduler::CAutoScheduler(MScheduleObserver& aUpdate)
:iUpdate(aUpdate)
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

	iAknsSrvSession.Close();
	delete iRandomArray;
	
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


_LIT(KRecFilename					,"C:\\ThemeScheduler.txt");
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
		iFile.Write(_L8("Starting \n"));
	}
	*/
	iAknsSrvSession.Connect(this);
		
	if(iHasTimed)
	{
	    LogCurrentTime();
    	LogScheduleTime();
	}
	
	iTimeOutTimer = CTimeOutTimer::NewL(EPriorityHigh, *this);
	
	
	if(iFile.SubSessionHandle())
	{
		iFile.Write(_L8("Started \n"));
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CAutoScheduler::ReFreshRandomL(void)
{
	delete iRandomArray;
	iRandomArray = NULL;
	iRandomArray = new(ELeave)CArrayFixFlat<TUint32>(10);
	
	CScheduleDB* ScheduleDB = new(ELeave)CScheduleDB();
	CleanupStack::PushL(ScheduleDB);
	ScheduleDB->ConstructL();
			    
	ScheduleDB->GetRandomProfilesL(*iRandomArray);	
	CleanupStack::PopAndDestroy(ScheduleDB);
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CAutoScheduler::NewScheduleL(TTime aNewTime, TUint32 aTheme,const TDesC& aImageName)
{
	iTime = aNewTime;
	iTheme = aTheme;
	iImageFile.Copy(aImageName);

	delete iRandomArray;
	iRandomArray = NULL;
	iRandomArray = new(ELeave)CArrayFixFlat<TUint32>(10);
	
	CScheduleDB* ScheduleDB = new(ELeave)CScheduleDB();
	CleanupStack::PushL(ScheduleDB);
	ScheduleDB->ConstructL();
			    
	ScheduleDB->GetRandomProfilesL(*iRandomArray);	
	CleanupStack::PopAndDestroy(ScheduleDB);

	
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
void CAutoScheduler::ShowNoteL(const TDesC& aMessage)
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
void CAutoScheduler::TimerExpired()
{
	if(iFile.SubSessionHandle())
	{
		iFile.Write(_L8("Timer ext,\n"));
	}
	
	LogCurrentTime();
	LogScheduleTime();
	
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
	else
	{
		if(iFile.SubSessionHandle())
		{
			iFile.Write(_L8("Done now..,\n"));
		}
		
		DoDoTheStuffNowL(iTheme,iImageFile);
		iUpdate.StuffDoneL(KErrNone);
	}		
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CAutoScheduler::DoDoTheStuffNowL(const TUint32& aThemeId,const TDesC& aImageName)
{
	TUint32 themeId(aThemeId);
	TInt ErrorNum(KErrNone),ErrorNum2(-6988);
	
	if(themeId == 1) //image changing
	{
		if(BaflUtils::FileExists(iFsSession,aImageName))
		{
			AknsWallpaperUtils::SetIdleWallpaper(aImageName,NULL);
		}
	}
	else
	{
		CArrayPtr< CAknsSrvSkinInformationPkg >* skinInfoArray = iAknsSrvSession.EnumerateSkinPackagesL(EAknsSrvAll);
		CleanupStack::PushL( skinInfoArray );
		  
		if(themeId == 0)
		{
			themeId = GetRandomThemeL();
		}
		
		for(TInt i=0; i < skinInfoArray->Count(); i++)
		{
			if(((TUint32)(skinInfoArray->At(i)->PID().Uid().iUid))  == themeId)
			{
				TAknsPkgID  currPkgId;
				GetCurrentSkinID(currPkgId);
				
				TAknsPkgID pkgId = skinInfoArray->At(i)->PID();
				
				if((TUint32)currPkgId.Uid().iUid != (TUint32)pkgId.Uid().iUid)
				{
					ErrorNum2 = iAknsSrvSession.SetAllDefinitionSets( pkgId );	
				
					SetNewSkinIdL( pkgId );
				}
			}
		}
	
		skinInfoArray->ResetAndDestroy();        
		CleanupStack::PopAndDestroy(1); //skinInfoArray
		
		if(ErrorNum2 != KErrNone)
		{
			ErrorNum = ErrorNum2;
		}
	}
	
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TUint32 CAutoScheduler::GetRandomThemeL(void)
{
	TUint32 ret(0);
	TTime CurrTime;
	
	if(iRandomArray)
	{		
		if(iRandomArray->Count() == 1)
		{
			ret = iRandomArray->At(0);
		}
		else
		{
			CurrTime.HomeTime();
			TInt64 Hjelp(CurrTime.Int64());

			//randdom = (Math::Random() & KMaxTInt); //Math::Rand(num64);
			
			TInt selInddex = (iRandomArray->Count() * Math::FRand(Hjelp));
			
			if(selInddex >= 0 && selInddex < iRandomArray->Count())
			{
				ret = iRandomArray->At(selInddex);
			}
		}	
	}

	if(ret == 0)
	{
		RAknsSrvSession aknsSrvSession;
		aknsSrvSession.Connect();
		
		CArrayPtr< CAknsSrvSkinInformationPkg >* skinInfoArray = aknsSrvSession.EnumerateSkinPackagesL(EAknsSrvAll);
		CleanupStack::PushL( skinInfoArray );

		if(skinInfoArray->Count() == 1)
		{
			ret = (TUint32)(skinInfoArray->At(0)->PID().Uid ().iUid);
		}
		else
		{
			CurrTime.HomeTime();
			TInt64 Hjelp(CurrTime.Int64());
			
			//randdom = (Math::Random() & KMaxTInt); //Math::Rand(num64);
			
			TInt selInddex  = (skinInfoArray->Count() * Math::FRand(Hjelp));
		
			if(selInddex >= 0 && selInddex < skinInfoArray->Count())
			{
				ret = (TUint32)(skinInfoArray->At(selInddex)->PID().Uid ().iUid);
			}

			skinInfoArray->ResetAndDestroy();        
			CleanupStack::PopAndDestroy(1); //skinInfoArray
			aknsSrvSession.Close();
		}
	}

	iLastRandom = ret;
	return ret;
} 
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CAutoScheduler::GetCurrentSkinID(TAknsPkgID& aPkgId)
{ 
	TAknsPkgIDBuf pidBuf; 
	
	TRAPD(errroror,
	CRepository* repository = CRepository::NewLC( KCRUidPersonalisation ); 
	repository->Get( KPslnActiveSkinUid, pidBuf ); 
	CleanupStack::PopAndDestroy(repository)); 
	
	aPkgId.SetFromDesL(pidBuf);
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CAutoScheduler::SetNewSkinIdL( TAknsPkgID aPkgId )
{
    TAknsPkgIDBuf pidBuf;            
    aPkgId.CopyToDes( pidBuf );
    CRepository* repository = CRepository::NewL( KCRUidPersonalisation );                    

    TInt retVal = repository->Set( KPslnActiveSkinUid, pidBuf );
    retVal = repository->Set( KPslnActiveSkinLocation, pidBuf );
    // KPslnActiveSkinLocation has to be changed also
    // if new skin resides on mmc whereas old one resided in phone mem
    // ...and vice versa
    
    delete repository;
    repository = NULL;
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
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CAutoScheduler::SkinContentChanged ()
{

}

void CAutoScheduler::SkinConfigurationChanged (const TAknsSkinStatusConfigurationChangeReason aReason)
{
/*	GetCurrentSkinIdL(iCurrentSkinPid);
	
	TBuf<250> ShowMe;
	switch(aReason)
	{
	case EAknsSkinStatusConfigurationMerged:// 	Enumeration value that indicates completion of skin content merge, i.e. the new configuration is available for reading.
		ShowMe.Copy(_L("ConfigurationMerged: "));
		break;
	case EAknsSkinStatusConfigurationDeployed:// 	Enumeration value that indicates completion of event propagation to clients, i.e. the new configuration has been deployed to the registered clients.
		ShowMe.Copy(_L("ConfigurationDeployed: "));
		ChnageBgL();
		break;
	case EAknsSkinStatusConfigurationFailedOOM:// 	Enumeration value that indicates unsuccessful completion of skin activation because of an out-of-memory condition.
		ShowMe.Copy(_L("ConfigurationFailedOOM: "));
		break;
	case EAknsSkinStatusConfigurationFailedCorrupt:// 	Enumeration value that indicates unsuccessful completion of skin activation because of a corrupted skin package.
		ShowMe.Copy(_L("ConfigurationFailedCorrupt: "));
		break;
	case EAknsSkinStatusMorphingStateChange:// 	Enumeration value that indicates a change in the state of the morphing skin items
		ShowMe.Copy(_L("MorphingStateChange: "));
		break;
	case EAknsSkinStatusWallpaperChanged: //
		ShowMe.Copy(_L("WallpaperChanged: "));
		break;
	};
	
	ShowMe.AppendNum(iCurrentSkinPid.Uid().iUid,EHex);
	ShowNoteL(ShowMe);	*/
}
void CAutoScheduler::SkinPackageChanged (const TAknsSkinStatusPackageChangeReason aReason)
{
/*	GetCurrentSkinIdL(iCurrentSkinPid);
	
	TBuf<250> ShowMe;
	switch(aReason)
	{
	case EAknsSkinStatusPackageListUpdate :
		ShowMe.Copy(_L("Chnage: Package List Update: "));
		break;
	default:
		ShowMe.Copy(_L("Change: unkown: "));
		break;
	};
	
	ShowMe.AppendNum(iCurrentSkinPid.Uid().iUid,EHex);
	
	ShowNoteL(ShowMe);*/
}
