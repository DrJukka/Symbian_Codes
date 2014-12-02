
#include "SoneAniDrawer.h"
#include "Common.h"
#include "Fake_SMS.h" 
#include "TrialHanler.h"

#include <eikbtgpc.h> 
#include <aknutils.h> 
#include <centralrepository.h>
#include <LogsInternalCRKeys.h>
#include <coreapplicationuisdomainpskeys.h>

const TInt KExitTimeOut 		= 2000000;
const TInt KCallRelockDelay 	= 1000000;
const TInt KLockScreenTimeOut	= 2000000;
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
	iExitTimer = NULL;
	
    delete iFakeSMSSender;
    iFakeSMSSender = NULL;
    
	delete imyPsObserver;
	imyPsObserver = NULL;
	delete iForegroundObserver;
	iForegroundObserver = NULL;
	delete iClockDrawer;
	iClockDrawer = NULL;
	delete iMyCallObserver;
	iMyCallObserver = NULL;
	delete iInActivityTimer;
	iInActivityTimer = NULL;
	
	delete iMissCallObserver;
	iMissCallObserver = NULL;

	delete iBatteryCheck;
	iBatteryCheck = NULL;
	
	delete iEmailStatusObserver;
	iEmailStatusObserver = NULL;

	delete iSMSMonitor;
	iSMSMonitor = NULL;
	
	delete iCalAlarmObserver;
	iCalAlarmObserver = NULL;
	
	iKeyLock.Close();
	
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
	// start exit timer first
	iExitNow = ETrue;
	iLockNow = EFalse;
	iExitTimer =  CTimeOutTimer::NewL(0, *this);
	iExitTimer->After(KExitTimeOut);
		
	User::LeaveIfError(iKeyLock.Connect());
	User::LeaveIfError(iFsSession.Connect());

/*	_LIT(KRecFilename			,"C:\\KeyLockClock.txt");
	iFsSession.Delete(KRecFilename);
	
	iFile.Create(iFsSession,KRecFilename,EFileWrite|EFileShareAny);	
	
	if(iFile.SubSessionHandle())
	{	
		iFile.Write(_L8("start, "));
	}
	*/
	
	iOn = EFalse;
	iLockEnabled = EFalse;
	iSettings.iBgColorRgb = iSettings.iFontColorRgb = TRgb(0x5ffffff);
	iSettings.iLightOn = 1;
	
	iModTime.HomeTime();
	iModTime = iModTime + TTimeIntervalDays(2);
	
	if(DoCheckSettingsNowL())
	{
		GetS60PlatformVersionL(iSettings.iMajor,iSettings.iMinor);	
	
		iAlarmIsOn = iCallIsOn = EFalse;
		iMyCallObserver = CMyCallObserver::NewL(*this);
		iForegroundObserver = CForegroundObserver::NewL(this);
	
		TBuf8<255> dbgBuff;
		
		TInt errNo = KErrNone;	
		
		TRAP(errNo,imyPsObserver = CmyPsObserver::NewL(*this,KUidServer, KAppChnages,ETrue));
		imyPsObserver->Start(ETrue);
		
		dbgBuff.Append(_L8("Pserr, "));
		dbgBuff.AppendNum(errNo);
	
		dbgBuff.Append(_L8("\n\r"));
		
		if(iFile.SubSessionHandle())
		{	
			iFile.Write(dbgBuff);
		}
		
		delete iExitTimer;
		iExitTimer = NULL;
		
		iBatteryCheck = CBatteryCheck::NewL(*this);
		
		iMissCallObserver = CmyRsObserver::NewL(*this,KCRUidLogs,KLogsNewMissedCalls, ETrue);
		iMissCallObserver->GetPropertyL(iMonitorVals.iMissedCalls);
		iMissCallObserver->StartMonitoringL();
		
		iEmailStatusObserver = CmyPsObserver::NewL(*this,KPSUidCoreApplicationUIs,KCoreAppUIsNewEmailStatus,ETrue);
		iEmailStatusObserver->Start(EFalse);
		iEmailStatusObserver->GetPropertyL(iMonitorVals.iEmailStatus);
		
		iSMSMonitor = CSMSMonitor::NewL(this);
		iCalAlarmObserver = CCalAlarmObserver::NewL(*this);
	}
	else
	{
		// exit timer is already on, so we'll exit soon..
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
_LIT(KS60ProductIDFile, "Series60v*.sis");
_LIT(KROMInstallDir, "z:\\system\\install\\");
void CMyAppUi::GetS60PlatformVersionL(TUint& aMajor, TUint& aMinor )
{
	aMajor = aMinor = 0;
		
    TFindFile ff(iFsSession);

    CDir* result;

    if(ff.FindWildByDir( KS60ProductIDFile, KROMInstallDir, result ) == KErrNone)
    {
    	if(result)
    	{
	    	CleanupStack::PushL( result );
			
			if(result->Count() > 0)
			{
	    		result->Sort( ESortByName|EDescending );
				
				if((*result)[0].iName.Length() > 11)
				{
	    			aMajor = (*result)[0].iName[9] - '0';
    				aMinor = (*result)[0].iName[11] - '0';
				}
			}
			
	    	CleanupStack::PopAndDestroy(); // result
    	}
    }
    
    
 /*   TInt mUid = 0;
    HAL::Get(HALData::EMachineUid, mUid);
    
    if(mUid == 0x2000DA56
    || mUid == 0x2000DA60){
		iIs5800Device = ETrue;
    }else{
		iIs5800Device = EFalse;
    }
*/
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMyAppUi::InActivityExpiredL(TInt anError)
{
	if(iFile.SubSessionHandle())
	{
		iFile.Write(_L8("InActivityExpiredL\r\n"));
	}

	if(iLockEnabled && iLockTime > 0)
	{
		TInt userInAc = User::InactivityTime().Int();

		if(userInAc >= iLockTime && !iCallIsOn && !iAlarmIsOn)
		{
			if(!iKeyLock.IsKeyLockEnabled())
			{// lock only if no lock is currently active
				iKeyLock.EnableKeyLock();
				iKeyLock.OfferKeyLock();	
			}
		}
		else
		{
			iInActivityTimer->Inactivity(iLockTime);
		}
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMyAppUi::InboxChangedL(TInt aSMS,TInt aMMS)
{
	if(iFile.SubSessionHandle())
	{	
		iFile.Write(_L8("InboxChangedL\n\r"));
	}

	iMonitorVals.iSMSCount = aSMS;
	iMonitorVals.iMMSCount = aMMS;
	
	if(iClockDrawer)
	{
		iClockDrawer->SetMonitorValues(iMonitorVals);
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMyAppUi::BatteryLevelL(TUint aChangeStatus,CTelephony::TBatteryStatus aStatus)
{
	if(iFile.SubSessionHandle())
	{
		iFile.Write(_L8("BatteryLevelL\r\n"));
	}

	iMonitorVals.iBatStatus = aStatus;
	iMonitorVals.iBatteryCharge = aChangeStatus;

	if(iClockDrawer)
	{
		iClockDrawer->SetMonitorValues(iMonitorVals);
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMyAppUi::RsValueChangedL(CmyRsObserver* aWhat, TInt aValue,TInt aError)
{
	if(iFile.SubSessionHandle())
	{	
		iFile.Write(_L8("RsValueChangedL\n\r"));
	}

	if(aError == KErrNone)
	{
		if(aWhat == iMissCallObserver)
		{
			iMonitorVals.iMissedCalls = aValue;
			
			if(iClockDrawer)
			{
				iClockDrawer->SetMonitorValues(iMonitorVals);
			}
		}
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMyAppUi::RsValueChangedL(CmyRsObserver* aWhat, const TDesC8& /*aValue*/,TInt aError)
{
	if(iFile.SubSessionHandle())
	{	
		iFile.Write(_L8("RsValueChangedL2\n\r"));
	}

	if(aError == KErrNone)
	{
		if(aWhat == iMissCallObserver)
		{
			//never called
		}
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMyAppUi::DoShowAlarmNoteL(const TDes& /*aMessage*/)
{
	iAlarmIsOn = ETrue;
	delete iClockDrawer;
	iClockDrawer = NULL;	
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMyAppUi::DoHideAlarmNoteL(void)
{
	iAlarmIsOn = EFalse;
	ForegroundChanged(KUidNullUID);// if we start monitoring UID, then we would need to change this one..
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMyAppUi::NotifyChangeInCallStatusL(CTelephony::TCallStatus& aStatus, TInt /*aError*/)
{
	if(iFile.SubSessionHandle())
	{	
		iFile.Write(_L8("NotifyChangeInCallStatusL\n\r"));
	}

	if(aStatus == CTelephony::EStatusIdle 
	|| aStatus == CTelephony::EStatusUnknown)
	{
		iCallIsOn = EFalse;
		ForegroundChanged(KUidNullUID);// if we start monitoring UID, then we would need to change this one..
		
		if(!iClockDrawer){// N97-mini problem re-showing UI when call is ended
			iLockNow = ETrue; //make sure it locks in first round
			iExitNow = EFalse;
			delete iExitTimer;
			iExitTimer = NULL;
			iExitTimer =  CTimeOutTimer::NewL(0, *this);
			iExitTimer->After(KCallRelockDelay); // 1.0 sec delay to re-lock
		}
	}
	else
	{
		iCallIsOn = ETrue;
		delete iClockDrawer;
		iClockDrawer = NULL;
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMyAppUi::PsValueChangedL(const TUid& /*aCategory*/, const TUint /*aKey*/,const TDesC8& /*aValue*/,TInt /*aError*/)
{
	if(iFile.SubSessionHandle())
	{	
		iFile.Write(_L8("PsValueChangedL 2, "));
	}
	
	DoCheckSettingsNowL();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMyAppUi::PsValueChangedL(const TUid& aCategory, const TUint aKey,TInt aValue,TInt aError)
{
	if(iFile.SubSessionHandle())
	{	
		iFile.Write(_L8("PsValueChangedL, "));
	}

	if(aError == KErrNone && aCategory == KPSUidCoreApplicationUIs)
	{
		iMonitorVals.iEmailStatus = aValue;
					
		if(iClockDrawer)
		{
			iClockDrawer->SetMonitorValues(iMonitorVals);
		}
	}

	DoCheckSettingsNowL();
}
/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/

TBool CMyAppUi::ForegroundChanged(TUid /*AppUid*/)
{
	if(iFile.SubSessionHandle())
	{	
		iFile.Write(_L8("ForegroundChanged, "));
	}

	if(iKeyLock.IsKeyLockEnabled() && !iCallIsOn && !iAlarmIsOn)
	{	
		if(iLockNow){
			iLockNow = EFalse;

			TBool setOn = EFalse;
			TRAPD(ErNo,
			if(DoCheckSettingsNowL()){
				if(iOn)
				{
					TPixelsTwipsAndRotation SizeAndRotation;
					CEikonEnv::Static()->ScreenDevice()->GetScreenModeSizeAndRotation(CEikonEnv::Static()->ScreenDevice()->CurrentScreenMode(),SizeAndRotation);
									 	
					if(SizeAndRotation.iPixelSize.iWidth > SizeAndRotation.iPixelSize.iHeight){
						SetOrientationL(CAknAppUiBase::EAppUiOrientationLandscape);
					}else{
						SetOrientationL(CAknAppUiBase::EAppUiOrientationPortrait);
					}
										
					setOn = ETrue;
				}
			});
					
			if(setOn){
				if(!iClockDrawer){
					iClockDrawer =  CClockDrawer::NewL(*this,iFont,iSettings);
					iClockDrawer->SetMonitorValues(iMonitorVals);
				}else{
					iClockDrawer->ReFreshhhL();
				}
			}else{
				if(iClockDrawer){
					iClockDrawer->ForegroundChanged(EFalse);
				}
				delete iClockDrawer;
				iClockDrawer = NULL;
			}
			
		}else{
			if(iExitTimer){
				iExitTimer->Cancel();
			}
	
			delete iExitTimer;
			iExitTimer = NULL;
			iExitNow = EFalse;
			iLockNow = ETrue;
			iExitTimer =  CTimeOutTimer::NewL(0, *this);
			iExitTimer->After(KLockScreenTimeOut);
		}
		
	}
	else if(iClockDrawer)
	{
		iLockNow = EFalse;
		iClockDrawer->ForegroundChanged(EFalse);	
		delete iClockDrawer;
		iClockDrawer = NULL;
	}	
	
	if(iFile.SubSessionHandle())
	{
		TBuf8<100> MsgBuffer;
		MsgBuffer.Copy(_L8("ForegroundChanged-done, "));
		
		if(iClockDrawer)
			MsgBuffer.Append(_L("On\n\r"));
		else
			MsgBuffer.Append(_L("Off\n\r"));
		
		iFile.Write(MsgBuffer);
	}
	
	return EFalse;
}

/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
void CMyAppUi::DisableScreen(TInt aDelay)
{
	if(iFile.SubSessionHandle())
	{
		iFile.Write(_L8("DisableScreen\r\n"));
	}
	
	if(iClockDrawer) // || iVideoDrawer
	{
		delete iClockDrawer;
		iClockDrawer = NULL;
	}
	
	if(iFile.SubSessionHandle())
	{
		iFile.Write(_L8("DisableScreen-Done\r\n"));
	}
}
/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
TBool CMyAppUi::IsKeyBoardLocked()
	{
		if(iKeyLock.IsKeyLockEnabled())
			return ETrue;
		else
			return EFalse;
	}
	
/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
TBool CMyAppUi::DoCheckSettingsNowL(void)
{
	TBool retVal(EFalse);
	
	TBuf8<255> DbgByff(_L8("AWSO: "));
	
	GetValuesFromStoreL();
	
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
            
            //turn off.
            iOn = iLockEnabled = EFalse;
        }
    }	
	
	if(iFile.SubSessionHandle())
	{	
		if(iOn)
			DbgByff.Append(_L8("iOn,"));
		
		if(iLockEnabled)
			DbgByff.Append(_L8("iLockEnabled"));
		
		DbgByff.Append(_L8("\n"));
		iFile.Write(DbgByff);
	}
	
	if(iOn)
	{
		retVal = ETrue;
	}
	
	if(iLockEnabled && iLockTime > 0)
	{
		retVal = ETrue;
		
		if(!iInActivityTimer)
		{
			iInActivityTimer = CInActivityTimer::NewL(0,*this);
		}
		
		iInActivityTimer->Inactivity(iLockTime);
	}
	
	if(retVal)
	{
		if(iClockDrawer)
		{
			iClockDrawer->SetValuesL(iSettings);
		}
	}
	else
	{
		if(iExitTimer)
		{
			iExitTimer->Cancel();
		}

		delete iExitTimer;
		iExitTimer = NULL;
		iExitNow = ETrue;
		iLockNow = EFalse;
		iExitTimer =  CTimeOutTimer::NewL(0, *this);
		iExitTimer->After(KExitTimeOut);		
	}
	
	return retVal;
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
void CMyAppUi::TimerExpired()
{
	if(iExitNow)
	{
		CActiveScheduler::Stop();
	}
	else
	{
		ForegroundChanged(KUidNullUID);// if we start monitoring UID, then we would need to change this one..			
		
		iExitNow = ETrue;
	}
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
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMyAppUi::GetValuesFromStoreL(void)
{	
	TFindFile AufFolder(CCoeEnv::Static()->FsSession());
	if(KErrNone == AufFolder.FindByDir(KtxMonFile, KNullDesC))
	{
		TTimeIntervalSeconds ModInterval(0);
		
		TEntry anEntry;
		if(KErrNone == iFsSession.Entry(AufFolder.File(),anEntry))
		{	
			anEntry.iModified.SecondsFrom(iModTime,ModInterval);
		}
	
		if(ModInterval.Int() != 0)
		{
			iModTime = anEntry.iModified;
		
			iOn = EFalse;
			iLockEnabled = EFalse;

			iSettings.iScreenMode = -1; //automatic
			
			iSettings.iBgColorRgb = iSettings.iFontColorRgb = TRgb(0x5ffffff);
			iSettings.iLightOn = 1;
			
			CDictionaryFileStore* MyDStore = CDictionaryFileStore::OpenLC(CCoeEnv::Static()->FsSession(),AufFolder.File(), TUid::Uid(0x102013AD));
	
			TUint32 TmpValue(0);
			GetValuesL(MyDStore,0x5555,TmpValue);
			
			if(TmpValue > 50)
				iOn = ETrue;
			else// if(TmpValue > 10)
				iOn = EFalse;
			
			TmpValue = 0;
			GetValuesL(MyDStore,0x7777,TmpValue);
			
			if(TmpValue > 50)
				iLockEnabled = ETrue;
			else 
				iLockEnabled  = EFalse;
			
			TmpValue = 0;
			GetValuesL(MyDStore,0x9999,TmpValue);
			
			if(TmpValue < 10)
				iLockTime = 10;
			else if(TmpValue > 3599)
				iLockTime = 3599;
			else
				iLockTime = TmpValue;
			
			TmpValue = 0;
			GetValuesL(MyDStore,0x5678,TmpValue);
			
			if(TmpValue < 1)
				iSettings.iLightTime = 1;
			else if(TmpValue > 3599)
				iSettings.iLightTime = 3599;
			else
				iSettings.iLightTime = TmpValue;
			
			TmpValue = 0;
			GetValuesL(MyDStore,0x8888,TmpValue);
			
			if(TmpValue > 2)
				iSettings.iLightOn = 0;
			else 
				iSettings.iLightOn  = TmpValue;
	
			TmpValue = 0;
			GetValuesL(MyDStore,0x6666,TmpValue);
			
			if(TmpValue < 0)
				iSettings.iLightVal = 0;
			else if(TmpValue > 100)
				iSettings.iLightVal = 100;
			else
				iSettings.iLightVal = TmpValue;
			
			GetValuesL(MyDStore,0x1234,iFont);
				
			TmpValue = 0;
			GetValuesL(MyDStore,0x1001,TmpValue);
			iSettings.iBgColorRgb = TRgb(TmpValue);
			
			TmpValue = 0;
			GetValuesL(MyDStore,0x2002,TmpValue);
			iSettings.iFontColorRgb = TRgb(TmpValue);
			
			TmpValue = 0;
			GetValuesL(MyDStore,0x7676,TmpValue);
					
			if(TmpValue > 2)
				iSettings.iShowDate = 0;
			else 
				iSettings.iShowDate  = TmpValue;
			
			TmpValue = 0;
			GetValuesL(MyDStore,0x6667,TmpValue);
					
			if(TmpValue < 0)
				iSettings.iLightVal2 = 0;
			else if(TmpValue > 100)
				iSettings.iLightVal2 = 100;
			else
				iSettings.iLightVal2 = TmpValue;	
			
			
			TmpValue = 0;
			GetValuesL(MyDStore,0x2772,TmpValue);
						
			if(TmpValue > 2 || TmpValue == 0){
				iSettings.iScreenMode = -1;
			}else{
				iSettings.iScreenMode = (TmpValue - 1);
			}
			
					
			CleanupStack::PopAndDestroy(1);// Store
		}
	}
}
		
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMyAppUi::GetValuesL(CDictionaryFileStore* aDStore,TInt aUID,TDes& aValue)
{
	aValue.Zero();
	
	if(aDStore == NULL)
		return;
		
	TUid FileUid = {0x10};
	FileUid.iUid = aUID;

	if(aDStore->IsPresentL(FileUid))
	{
		RDictionaryReadStream in;
		in.OpenLC(*aDStore,FileUid);
		in >> aValue;
		CleanupStack::PopAndDestroy(1);// in
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMyAppUi::GetValuesL(CDictionaryFileStore* aDStore,TInt aUID,TUint32& aValue)
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
		aValue = 0;
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
	
	appUi->BaseConstructL(CAknAppUi::EAknEnableSkin | CAknAppUi::EAppOrientationAutomatic |CEikAppUi::ENoAppResourceFile);//|CEikAppUi::ENoScreenFurniture);
	appUi->ConstructObserversL();
										  
	appUi->ConstructObserversL();
	

//	coeEnv->AddResourceChangeObserverL(*appUi);
	
	coeEnv->SetAppUi(appUi); // takes ownership

	AknsUtils::InitSkinSupportL();	
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
