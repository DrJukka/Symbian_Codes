
#include "SoneAniDrawer.h"
#include "Common.h"

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
	
	delete iFgrObserve;
	iFgrObserve = NULL;
	
	iItemArray.Reset();
    
    delete iRandomArray;
    iRandomArray = NULL;
    
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
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
void CMyAppUi::ConstructObserversL(void)
{
	iFsSession.Connect();
	
	_LIT(KRecFilename			,"C:\\ProfServer.txt");
	iFsSession.Delete(KRecFilename);
	
	iFile.Create(iFsSession,KRecFilename,EFileWrite|EFileShareAny);	
	
	if(iFile.SubSessionHandle())
	{	
		iFile.Write(_L8("start, "));
	}
	
	iDbgLastIndex = 0;
	
	StuffDoneL(KErrNone);

	iFgrObserve = CFgrObserver::NewL(*this);

	if(iFile.SubSessionHandle())
	{	
		iFile.Write(_L8("started, "));
	}
}
		
/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
TBool CMyAppUi::AreWeStillOnL(TTime& aNextSchedule, TInt& aProfile)
{
	TBool Ret(EFalse);
	
	TBuf8<255> DbgByff(_L8("AWSO: "));
	
	TFindFile AufFolder(iFsSession);
	if(KErrNone == AufFolder.FindByDir(KtxDatabaseName, KNullDesC))
	{
		DbgByff.Append(_L8("ff, "));
		
		//first check reg and set the Ret accordingly
		//if(REgistered()
		{
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
				
				iItemArray.Reset();
			    
			    delete iRandomArray;
			    iRandomArray = NULL;
			    iRandomArray = new(ELeave)CArrayFixFlat<TInt>(10);
			    
			    CScheduleDB* ScheduleDB = new(ELeave)CScheduleDB();
				CleanupStack::PushL(ScheduleDB);
				ScheduleDB->ConstructL();
			    
			    ScheduleDB->ReadDbItemsL(iItemArray);	
			    ScheduleDB->GetRandomProfilesL(*iRandomArray);
			    
			    CleanupStack::PopAndDestroy(ScheduleDB);
			}
		}
	}
	
	DbgByff.Append(_L8("Count, "));
	DbgByff.AppendNum(iItemArray.Count());
	
	if(iRandomArray)
	{
		DbgByff.Append(_L8(", Rndom, "));
		DbgByff.AppendNum(iRandomArray->Count());
	}
	
	DbgByff.Append(_L8(", "));

	if(iItemArray.Count())
	{	
		ResolveNextSchduleTimeL(aNextSchedule, aProfile,iItemArray);
		Ret = ETrue;
	}
	
	if(iFile.SubSessionHandle())
	{	
		DbgByff.Append(_L8("Prof: "));
		DbgByff.AppendNum(aProfile);
		DbgByff.Append(_L8("\n"));
		iFile.Write(DbgByff);
	}
	
	return Ret;
}

/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
void CMyAppUi::ResolveNextSchduleTimeL(TTime& aNextSchedule, TInt& aProfile,RArray<TProfSched>& aItemArray)
{
	if(iRandomArray)
	{
		iDbgLastIndex++;
		
		if(iDbgLastIndex < 0 || iDbgLastIndex >= iRandomArray->Count())
		{
			iDbgLastIndex = 0;
		}
		
		aProfile = iRandomArray->At(iDbgLastIndex);
	}
	
	aNextSchedule = aNextSchedule + TTimeIntervalMinutes(2);
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
void CMyAppUi::ForegroundEventL(const TUid& aAppUid)
{
	TBuf8<250> BufFuu(_L8("FgE: "));
	
	BufFuu.AppendNum(aAppUid.iUid,EHex);
	BufFuu.Append(_L8(" -- "));
	BufFuu.AppendNum(KUidTOneViewer.iUid,EHex);
	
	if(iFile.SubSessionHandle())
	{
		BufFuu.Append(_L8("\n"));	
		iFile.Write(BufFuu);
	}
	
	if(aAppUid.iUid == KUidTOneViewer.iUid)
	{
		if(iFile.SubSessionHandle())
		{
			iFile.Write(_L8("Wem have UI on top now\n"));
		}
	
		iLastWasTheOne = ETrue;
	}
	else if(iLastWasTheOne)
	{
		iLastWasTheOne = EFalse;
		StuffDoneL(0x8876);
	}	
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

	TInt Proff(0);
	TTime NextSchedule;
	NextSchedule.HomeTime();

	if(AreWeStillOnL(NextSchedule,Proff))
	{
		if(!iAutoSchedule)
		{
			iAutoSchedule = CAutoScheduler::NewL(*this,NextSchedule,Proff);
		}
		else
		{
			iAutoSchedule->NewScheduleL(NextSchedule,Proff);
		}
	}
	else
	{
		if(iFile.SubSessionHandle())
		{
			iFile.Write(_L8("Start Exit timer now\n"));
		}
	
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
void CMyAppUi::TimerExpired()
{
	CActiveScheduler::Stop();
}
/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
void CMyAppUi::HandleCommandL(TInt aCommand)
{
	if(iFile.SubSessionHandle())
	{
		iFile.Write(_L8("HandleCommandL\n"));
	}
}
/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
void CMyAppUi::HandleWsEventL(const TWsEvent& aEvent,CCoeControl* /*aDestination*/)
{	
	if(iFile.SubSessionHandle())
	{
		iFile.Write(_L8("HandleEvent\n"));
	}
	
	if(aEvent.Type() == EEventKey && aEvent.Key())
	{
	
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMyAppUi::LogTime(const TTime& aTime, const TDesC8& aTitle)
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
