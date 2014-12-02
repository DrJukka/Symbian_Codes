/* 
	Copyright (c) 2006, 
	Jukka Silvennoinen. 
	All rights reserved 
*/


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

#include "commonstrings.h"
#include "DLLRegistration.h" 

#ifdef __SERIES60_3X__
	#include "ExampleServer_policy.h"
	#include "ExampleSession.h"
#else
	#include "ExampleSession_2nd.h"
	#include "Etelmm.h"
	#include <HAL.H>
	#include <PLPVARIANT.H>
	#include <PLPVAR.H>
#endif

//delay which after we will shut down after last sessions is disconnected.
const TInt KShutDownDelay		 = 2000000;



// database column names
_LIT(NCol0					,"index");
_LIT(NCol1					,"name");
_LIT(NCol2					,"value");
// database table name
_LIT(KtxtImsilist			,"imsis");
_LIT(KtxtNumberlist			,"numbers");
_LIT(KtxtStatuslist			,"status");

_LIT(NStatCol1				,"protect");
_LIT(NStatCol2				,"screen");

_LIT(KtxtVallist			,"val");
_LIT(NValCol1				,"check");
_LIT(NValCol2				,"also");

/*
----------------------------------------------------------------------------------------
// CExPolicy_Server
----------------------------------------------------------------------------------------
*/
inline CExPolicy_Server::CExPolicy_Server():
#ifdef __SERIES60_3X__
	CPolicyServer(0,SClientServerPolicy,ESharableSessions)
#else
	CServer(EPriorityNormal)
#endif
{
}

/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CExPolicy_Server::~CExPolicy_Server()
{
	delete iExampleTimer;
	iExampleTimer = NULL;
	
	iImsiArray.Reset();
	iNumberArray.Reset();
	
	delete iNProtRegister;
	iNProtRegister = NULL;

#ifdef __SERIES60_3X__
	delete iRegReader;
	iRegReader = NULL;
#else
#endif

	if(iFile.SubSessionHandle())
	{
	  	iFile.Write(_L8("Bye,bye"));
	  	iFile.Close();
	}

	iItemsDatabase.Close();
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
	// Lowest priority, so should only run if nothing 
	// else is been done same time.
	// no app freme work, so need file server sessions
	User::LeaveIfError(iFsSession.Connect());
	
/*	_LIT(KRecFilename			,"C:\\NProtSrver.txt");
		
	iFsSession.Delete(KRecFilename);		
	User::LeaveIfError(iFile.Create(iFsSession,KRecFilename,EFileWrite|EFileShareAny));	
*/	
	if(iFile.SubSessionHandle())
	{
		iFile.Write(_L8("Mie Strttaaan ny\n,"));
	}
	
	iNProtRegister = new(ELeave)CNProtRegister(*this);

#ifdef __SERIES60_3X__
	iRegReader = CRegReader::NewL(this);
#else
#endif

	iExampleTimer = CTimeOutTimer::NewL(EPriorityNull, *this);
		
	StartShutDownTimer();// start shutdown first
	StartL(KExapmpleServerName);// then start server
	
	
	
	TFindFile PrivFolder(iFsSession);
	if(KErrNone == PrivFolder.FindByDir(KtxDatabaseName, KNullDesC))// finds the drive
	{		
		iNumberArray.Reset();// first reset the array
		iImsiArray.Reset();// first reset the array

		User::LeaveIfError(iItemsDatabase.Open(iFsSession,PrivFolder.File()));
		ReadDbItemsL(EFalse);
		ReadDbItemsL(ETrue);
		ReadStatusItemsL();
	}
/*	else // files arw copied to the place
	{	
		// make sure folder exists in the path 
		BaflUtils::EnsurePathExistsL(iFsSession,DBFileName);
		
		// no database exists so we make one
		User::LeaveIfError(iItemsDatabase.Create(iFsSession,DBFileName));     
		// and will create the onlt table needed for it
		CreateTableL(iItemsDatabase,EFalse);
		CreateTableL(iItemsDatabase,ETrue);
		
		iScreenStatus = iProtectStatus = EFalse;
		CreateStatusTableL(iItemsDatabase);
	}*/

#ifdef __SERIES60_3X__

#else	
	
	TPlpVariantMachineId id; 
	RMobilePhone::TMobilePhoneSubscriberId ImSiId;
	
	#ifdef __WINS__
		id.Copy(_L("357933001298208"));
		ImSiId.Copy(_L("520011298208"));
	#else	
	
	RMobilePhone MyPhone;
	RTelServer MyTelServer;
	RTelServer::TPhoneInfo phoneInfo;

	if(KErrNone == MyTelServer.Connect())
	{
		if(KErrNone == MyTelServer.GetPhoneInfo(0, phoneInfo))
		{
			if(KErrNone == MyPhone.Open(MyTelServer, phoneInfo.iName))
			{
				TRequestStatus MyStatus;
				MyPhone.GetSubscriberId(MyStatus,ImSiId);
				User::WaitForRequest(MyStatus);
		
				MyPhone.Close();
			}
		}
		
		MyTelServer.Close();
	}
	
	
  	PlpVariant::GetMachineIdL(id);
	
	#endif
	GotValuesL(id,ImSiId);	
#endif
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_Server::GotValuesL(const TDesC& aIMEI,const TDesC& aIMSI)
{
	iImsi.Copy(aIMSI);
	iImei.Copy(aIMEI);
	
	if(iFile.SubSessionHandle())
	{
		TBuf8<100> Bububu;
		iFile.Write(_L8("IMEI: "));
		Bububu.Copy(iImei);
		iFile.Write(Bububu);
		
		iFile.Write(_L8("\nIMSI: "));
		Bububu.Copy(iImsi);
		iFile.Write(Bububu);
		iFile.Write(_L8(",\n"));
	}
	
	if(iNProtRegister)
	{
		iNProtRegister->SetIMEI(iImei);	
	#ifdef __WINS__
		iIsThai = ETrue;
		iOperator.Copy(_L("Jukka"));
	#else	
		iIsThai = iNProtRegister->IsThaiNetWorkL(iOperator,iImsi);
	#endif
	}
	
	if(iFile.SubSessionHandle())
	{
		TBuf8<100> Bububu;
		Bububu.Copy(iOperator);
		iFile.Write(Bububu);
		iFile.Write(_L8(",\n"));
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_Server::CheckCurrentSIMOkL(void)
{
#ifdef __SERIES60_3X__

#else
	if(iFile.SubSessionHandle())
	{
		TBuf8<100> Bububu;
		iFile.Write(_L8("SIM Check: \nIMEI: "));
		Bububu.Copy(iImei);
		iFile.Write(Bububu);
		
		iFile.Write(_L8("\nIMSI: "));
		Bububu.Copy(iImsi);
		iFile.Write(Bububu);
		iFile.Write(_L8(",\n"));
	}
		
	RFs fsSession; //file server session
	if(KErrNone == fsSession.Connect())
	{
		TFindFile findFile( fsSession );
		if(KErrNone == findFile.FindByDir(KtxtCheckStuffExeName, KNullDesC))
		{
			TThreadId aId;
			RApaLsSession ls;
			if(KErrNone == ls.Connect())
			{
				ls.StartDocument(findFile.File(),aId);
				ls.Close();
			}
		}
		
		fsSession.Close();
	}
#endif
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TBool CExPolicy_Server::IsCodeOkL(const TDesC& aCode,const TDesC8& aString)
{
	TBool Ret(EFalse);
	
	if(aString.Length() && aCode.Length())
	{
		if(iIsThai)
		{
			Ret = iIsThai;
		}
		else
		{
			TFindFile PrivFolder(iFsSession);
			if(KErrNone == PrivFolder.FindByDir(KtxDatabaseName, KNullDesC))// finds the drive
			{
				TParsePtrC Hjelppp(PrivFolder.File());
				TFileName FilNammm;
				
				FilNammm.Copy(Hjelppp.Drive());
				FilNammm.Append(KtxValFileName);
			
				if(BaflUtils::FileExists(iFsSession,FilNammm))
				{
					RDbNamedDatabase ValDatabase;
					if(KErrNone == ValDatabase.Open(iFsSession,FilNammm))
					{
						if(OkToAddToValDb(ValDatabase,aCode,aString))
						{
							Ret = ETrue;
							AddToValDatabase(ValDatabase,aCode,aString);
						}
						
						ValDatabase.Close();
					}
				}
				else
				{
					// make sure folder exists in the path 
					BaflUtils::EnsurePathExistsL(iFsSession,FilNammm);
					
					RDbNamedDatabase ValDatabase;
					// no database exists so we make one
					User::LeaveIfError(ValDatabase.Create(iFsSession,FilNammm));     
					// and will create the onlt table needed for it
					CreateValTableL(ValDatabase);
					
					AddToValDatabase(ValDatabase,aCode,aString);
					
					ValDatabase.Close();
					
					Ret = ETrue;	
				}
			}
		}
	}

	return Ret;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TBool CExPolicy_Server::OkToAddToValDb(RDbDatabase& aDatabase,const TDesC& aCode,const TDesC8& aString)
{
	TBool Ret(ETrue);
	
	TFileName QueryBuffer;
	QueryBuffer.Copy(_L("SELECT * FROM "));
	QueryBuffer.Append(KtxtVallist);
	
	RDbView Myview;
	Myview.Prepare(aDatabase,TDbQuery(QueryBuffer));
	CleanupClosePushL(Myview);
	Myview.EvaluateAll();
	Myview.FirstL();
	
	QueryBuffer.Copy(aString);
		
	while(Myview.AtRow()) // Just delete one instance of the message           
	{	
		Myview.GetL();		
		
		if(QueryBuffer == Myview.ColDes(3)
		&& aCode == Myview.ColDes(2))
		{
			Ret = EFalse;
			break;	
		}
			
		Myview.NextL();
	} 
	
	CleanupStack::PopAndDestroy(1); // Myview
	
	return Ret;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_Server::AddToValDatabase(RDbDatabase& aDatabase,const TDesC& aCode,const TDesC8& aString)
{
	TFileName QueryBuffer;
	QueryBuffer.Copy(_L("SELECT * FROM "));
	QueryBuffer.Append(KtxtVallist);

	aDatabase.Begin();

	RDbView Myview;
	Myview.Prepare(aDatabase,TDbQuery(QueryBuffer));
	CleanupClosePushL(Myview);

	Myview.InsertL(); 
		
	Myview.SetColL(2,aCode);
	
	QueryBuffer.Copy(aString);
	
	Myview.SetColL(3,QueryBuffer);

	Myview.PutL();  
		
	CleanupStack::PopAndDestroy(1); // Myview
	aDatabase.Commit();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_Server::CreateValTableL(RDbDatabase& aDatabase) 
	{
	// Create a table definition
	CDbColSet* columns=CDbColSet::NewLC();
	
	// Add Columns
	TDbCol id(NCol0,EDbColInt32);
	id.iAttributes=id.EAutoIncrement;// automatic indexing for items,it is our key field.
	columns->AddL(id);				 
	
	columns->AddL(TDbCol(NValCol1, EDbColText,200));
	columns->AddL(TDbCol(NValCol2, EDbColText,200));
	
	// Create a table
	User::LeaveIfError(aDatabase.CreateTable(KtxtVallist,*columns));
				
	// cleanup the column set
	CleanupStack::PopAndDestroy();
}


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TBool CExPolicy_Server::LockScreenLockedUnLocked(TBool aSetOn)
{
	if(aSetOn)
	{
		TFindFile PrivFolder(iFsSession);
		if(KErrNone == PrivFolder.FindByDir(KtxDatabaseName, KNullDesC))// finds the drive
		{
			TParsePtrC Hjelppp(PrivFolder.File());
			TFileName FilNammm;
			
			FilNammm.Copy(Hjelppp.Drive());
			FilNammm.Append(KtxLockScreenInfName);
			
			RFile DonewFil;
			if(KErrNone == DonewFil.Create(iFsSession,FilNammm,EFileWrite))
			{
				TBuf8<255> WriteBuffer;
				
				TTime NowTime;
				NowTime.HomeTime();
				
				WriteBuffer.Copy(iImsi);
				WriteBuffer.Append(_L8(": "));
				WriteBuffer.AppendNum(NowTime.DateTime().Day());
				WriteBuffer.Append(_L8("/ "));
				WriteBuffer.AppendNum(NowTime.DateTime().Month());
				WriteBuffer.Append(_L8("/ "));
				WriteBuffer.AppendNum(NowTime.DateTime().Year());
				
				WriteBuffer.Append(_L8(" -- "));
				WriteBuffer.AppendNum(NowTime.DateTime().Hour());
				WriteBuffer.Append(_L8(": "));
				WriteBuffer.AppendNum(NowTime.DateTime().Minute());
				
				DonewFil.Write(WriteBuffer);
				DonewFil.Close();	
			}
		}
	}
	else
	{
		TFindFile PrivFolder(iFsSession);
		if(KErrNone == PrivFolder.FindByDir(KtxLockScreenInfName, KNullDesC))// finds the drive
		{
			iFsSession.Delete(PrivFolder.File());
		}
	}

	return HadNonClearedLockScreen();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TBool CExPolicy_Server::HadNonClearedLockScreen(void)
{
	TBool Ret(EFalse);
	
	TFindFile PrivFolder(iFsSession);
	if(KErrNone == PrivFolder.FindByDir(KtxLockScreenInfName, KNullDesC))// finds the drive
	{
		Ret = ETrue;
	}
	
	return Ret;	
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TBool CExPolicy_Server::IsReadyForAction(void)
{
	TBool Ret(EFalse);
	
	if(iImei.Length())
	{
		Ret = ETrue;
	}
	
	return Ret;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_Server::CreateStatusTableL(RDbDatabase& aDatabase) 
{
	// Create a table definition
	CDbColSet* columns=CDbColSet::NewLC();
	
	// Add Columns
	TDbCol id(NCol0,EDbColInt32);
	id.iAttributes=id.EAutoIncrement;// automatic indexing for items,it is our key field.
	columns->AddL(id);				 
	
	columns->AddL(TDbCol(NStatCol1, EDbColInt32));
	columns->AddL(TDbCol(NStatCol2, EDbColInt32));
	
	User::LeaveIfError(aDatabase.CreateTable(KtxtStatuslist,*columns));
				
	// cleanup the column set
	CleanupStack::PopAndDestroy();
}


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_Server::CreateTableL(RDbDatabase& aDatabase,TBool aImsi) 
	{
	// Create a table definition
	CDbColSet* columns=CDbColSet::NewLC();
	
	// Add Columns
	TDbCol id(NCol0,EDbColInt32);
	id.iAttributes=id.EAutoIncrement;// automatic indexing for items,it is our key field.
	columns->AddL(id);				 
	
	columns->AddL(TDbCol(NCol1, EDbColText,100));
	columns->AddL(TDbCol(NCol2, EDbColText,100));
	
	// Create a table
	if(aImsi)
		User::LeaveIfError(aDatabase.CreateTable(KtxtImsilist,*columns));
	else
		User::LeaveIfError(aDatabase.CreateTable(KtxtNumberlist,*columns));
				
	// cleanup the column set
	CleanupStack::PopAndDestroy();
}

/*
----------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------
*/
void CExPolicy_Server::AddSession()
{
	++iSessionCount;
	
	if(iFile.SubSessionHandle())
	{
		iFile.Write(_L8("Add session\n,"));
	}
	
	if(iExampleTimer)
	{	// we have new session connected 
		// so we need to cancel any pending exit timer requests.
		iExampleTimer->Cancel();
	}
}

/*
----------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------
*/
void CExPolicy_Server::DropSession()
{
	--iSessionCount;

	if(iFile.SubSessionHandle())
	{
		iFile.Write(_L8("Remove session\n,"));
	}
	
	if(iSessionCount <= 0)
	{	// last session has disconnected, thus we could prepare for shut-down.
		StartShutDownTimer();
	}
}
#ifdef __SERIES60_3X__
/*
----------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------
*/
CSession2* CExPolicy_Server::NewSessionL(const TVersion&,const RMessage2&) const
	{
	return new (ELeave) CExPolicy_ServerSession();
	}

/*
----------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------
*/
// this is custom security check implemented just for example purposes
// check that client process has secure UID of our UI client application
CPolicyServer::TCustomResult CExPolicy_Server::CustomSecurityCheckL(const RMessage2& aMsg, TInt& /*aAction*/, TSecurityInfo& /*aMissing*/)
{ 
	CPolicyServer::TCustomResult test;
	test=EFail;// by default we will fail the test
	
	RThread ClientThread;
	
	aMsg.Client(ClientThread);// get client thread 
	
	if(ClientThread.Handle())// has handle --> is valid
	{
		RProcess ClientProcess;
		ClientThread.Process(ClientProcess);// then get the process
		
		TSecureId ProsID = ClientProcess.SecureId();// and check secure UID
		
		if(ProsID.iId  == KUidPhoneManager.iUid
		|| ProsID.iId  == KUidLockScreen.iUid)// is it same as with client
		{	
			test=EPass;// pass check if it is, othervise it will fail
		}
	}
	
	return test;
} 

#else 
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CSharableSession* CExPolicy_Server::NewSessionL(const TVersion& aVersion) const
    {
	// check we're the right version
	if (!User::QueryVersionSupported(TVersion(SServMajorVersionNumber,
                                              SServMinorVersionNumber,
                                              SServBuildVersionNumber),
                                     aVersion))
        {
		User::Leave(KErrNotSupported);
        }

	// make new session
	RThread client = Message().Client();
	return CExPolicy_ServerSession::NewL(client, *const_cast<CExPolicy_Server*> (this));
    }
#endif
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/	
void CExPolicy_Server::StartShutDownTimer()
{
	if(iFile.SubSessionHandle())
	{
		iFile.Write(_L8("Start exit timer\n,"));
	}
	
	if(iExampleTimer)
	{
		iExampleTimer->Cancel();// cancel any previous request first
		iExampleTimer->After(KShutDownDelay);// and start new one.
	}
}

/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_Server::TimerExpired()
{
	if(iFile.SubSessionHandle())
	{
		iFile.Write(_L8("Timer done\n,"));
	}
	
	if(iSessionCount <= 0)
	{   // timer expired without cancellation and we dont have sessions
	    // thus we can exit now.
		ShutMeDown();
	}
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
-------------------------------------------------------------------------------
************ internal database handling functions ****************************
-------------------------------------------------------------------------------
*/
void CExPolicy_Server::ReadStatusItemsL(void)
{
	iScreenStatus = iProtectStatus = EFalse;
	
	TFileName QueryBuffer;
	QueryBuffer.Copy(_L("SELECT * FROM "));// just get all columns & rows
	QueryBuffer.Append(KtxtStatuslist);
	
	RDbView Myview;
	Myview.Prepare(iItemsDatabase,TDbQuery(QueryBuffer));
	CleanupClosePushL(Myview);
	Myview.EvaluateAll();
	Myview.FirstL();
	
	while(Myview.AtRow()) // Just delete one instance of the message           
	{	
		Myview.GetL();		
		
		TExampleItem NewItem;	
		NewItem.iIndex = Myview.ColInt(1);
		
		if(Myview.ColInt(2) > 50)
		{
			iProtectStatus = ETrue;
		}
		
 		if(Myview.ColInt(3) > 50)
		{
			iScreenStatus = ETrue;
		}
			
		Myview.NextL();
	} 
	
	CleanupStack::PopAndDestroy(1); // Myview
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/

void CExPolicy_Server::ReadDbItemsL(TBool aImsi)
{
	TFileName QueryBuffer;
	QueryBuffer.Copy(_L("SELECT * FROM "));// just get all columns & rows
	
	if(aImsi)
		QueryBuffer.Append(KtxtImsilist);
	else
		QueryBuffer.Append(KtxtNumberlist);
	
	
	RDbView Myview;
	Myview.Prepare(iItemsDatabase,TDbQuery(QueryBuffer));
	CleanupClosePushL(Myview);
	Myview.EvaluateAll();
	Myview.FirstL();
	
	while(Myview.AtRow()) // Just delete one instance of the message           
	{	
		Myview.GetL();		
		
		TExampleItem NewItem;	
		NewItem.iIndex = Myview.ColInt(1);
        NewItem.iName.Copy(Myview.ColDes(2));
        NewItem.iValue.Copy(Myview.ColDes(3));
		
		if(aImsi)
			iImsiArray.Append(NewItem);	
		else
			iNumberArray.Append(NewItem);	
		Myview.NextL();
	} 
	
	CleanupStack::PopAndDestroy(1); // Myview
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
TBool CExPolicy_Server::CodeMatches(const TDesC& aCode)
{
	TBool Ret(EFalse);
	
	if(iNProtRegister && aCode.Length())
	{
		TBuf<20> hjelpper;
		iNProtRegister->BackUpPasswordL(hjelpper);
		if(aCode == hjelpper)
		{
			Ret = ETrue;	
		}
	}

	return Ret;
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CExPolicy_Server::TryRegisterApp(TUint& aResult)
{
	aResult = KInvalidVers;
#ifdef __WINS__
	{
		{
#else
	if(iIsThai && iNProtRegister)
	{
		if(iOperator.Length())
		{
#endif	
			if(iFile.SubSessionHandle())
			{
			  	iFile.Write(_L8("Reg Thai\n"));
			}
			
			TTime StartTime;
			StartTime.HomeTime();
			
			TBuf<24> Buff;
			TBuf8<100> Hjelp;
				
			if(iFile.SubSessionHandle())
			{
			  	iFile.Write(_L8("Get Imei\n"));
			}
			iNProtRegister->GetImeiL(Buff);
			
			Hjelp.Copy(Buff);
			Buff.Zero();
			
			TInt TimYear = (StartTime.DateTime().Year() - 2000);
			
			if(TimYear >= 100)
			{
				TimYear = (TimYear % 100);
			}
				
			TInt TimMonth= (StartTime.DateTime().Month() + 1);
			TInt TimDate = (StartTime.DateTime().Day() + 1);
		
			if(TimYear < 10)
			{
				Hjelp.AppendNum(0);
				Hjelp.AppendNum(TimYear);
			}
			else
			{
				Hjelp.AppendNum(TimYear);
			}
			
			if(TimMonth < 10)
			{
				Hjelp.AppendNum(0);
			}
			
			Hjelp.AppendNum(TimMonth);
			
			if(TimDate < 10)
			{
				Hjelp.AppendNum(0);
			}
			
			Hjelp.AppendNum(TimDate);
			
			TimDate = TimDate + 30;
			if(TimDate < 10)
			{
				Hjelp.AppendNum(0);
			}
				
			Hjelp.AppendNum(TimDate);
				
			if(iFile.SubSessionHandle())
			{
			  	iFile.Write(_L8("Reg num\n"));
			}
			
			iNProtRegister->RegistrationNumberL(Buff,Hjelp);
			
			TBuf<10> TryMe;
			TryMe.Copy(Ktx30DayTrial);
			TryMe.Append(Buff);
			
			if(iFile.SubSessionHandle())
			{
			  	iFile.Write(_L8("Try reg me\n"));
			}
			
			iNProtRegister->TryRegisterApp(TryMe, aResult,StartTime,iIsThai);
		}
	}
	
	if(iFile.SubSessionHandle())
	{
	  	iFile.Write(_L8("Thai reg ok\n"));
	}
}
				
				
				

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CExPolicy_Server::TryRegisterApp(TDesC& aBuffer, TUint& aResult, TTime& aStartTime)
{
	aResult = KInvalidVers;

	if(iNProtRegister)
	{
		iNProtRegister->TryRegisterApp(aBuffer, aResult,aStartTime,iIsThai);
	}
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CExPolicy_Server::CheckRegStatusL(TBool& aFirstStart, TInt& aDaysLeft, TUint& aResult,TDes& aOperator)
{
	aDaysLeft = 0;
	aResult = KInvalidVers;
	aFirstStart = ETrue;
	
	if(iNProtRegister)
	{
		aOperator.Copy(iOperator);
		aFirstStart = iNProtRegister->CheckRegStatusL(aDaysLeft,aResult,iIsThai);
	}
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/

void CExPolicy_Server::DeleteFromDatabaseL(TInt& aIndex, TBool aImsi)
{	
	TFileName QueryBuffer;
	QueryBuffer.Copy(_L("SELECT * FROM "));
	
	if(aImsi)
		QueryBuffer.Append(KtxtImsilist);
	else
		QueryBuffer.Append(KtxtNumberlist);
	
	QueryBuffer.Append(_L(" WHERE "));
	QueryBuffer.Append(NCol0);
	QueryBuffer.Append(_L(" = "));
	QueryBuffer.AppendNum(aIndex);
		
	iItemsDatabase.Begin();
	
	RDbView Myview;
	// query buffr with index finds only the selected item row.
	Myview.Prepare(iItemsDatabase,TDbQuery(QueryBuffer));
	CleanupClosePushL(Myview);
	
	Myview.EvaluateAll();
	Myview.FirstL();
	// we have autoincrement in index so it should be unique
	// but just to make sure, we use 'while', instead of 'if'
	while(Myview.AtRow())            
	{	
		Myview.GetL();
		Myview.DeleteL();	
		Myview.NextL();
	}
			
	CleanupStack::PopAndDestroy(1); // Myview
	iItemsDatabase.Commit();
	// compacts the databse, by physicaly removig deleted data.
	iItemsDatabase.Compact();
}


/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CExPolicy_Server::DeleteStatusInfoL(void)
{	
	TFileName QueryBuffer;
	QueryBuffer.Copy(_L("SELECT * FROM "));
	QueryBuffer.Append(KtxtStatuslist);
		
	iItemsDatabase.Begin();
	
	RDbView Myview;
	// query buffr with index finds only the selected item row.
	Myview.Prepare(iItemsDatabase,TDbQuery(QueryBuffer));
	CleanupClosePushL(Myview);
	
	Myview.EvaluateAll();
	Myview.FirstL();
	// we have autoincrement in index so it should be unique
	// but just to make sure, we use 'while', instead of 'if'
	while(Myview.AtRow())            
	{	
		Myview.GetL();
		Myview.DeleteL();	
		Myview.NextL();
	}
			
	CleanupStack::PopAndDestroy(1); // Myview
	iItemsDatabase.Commit();
	// compacts the databse, by physicaly removig deleted data.
	iItemsDatabase.Compact();
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
TInt CExPolicy_Server::UpdateStatusL(TBool& aValue, TBool aProtect)
{	
	TInt ErrRet(KErrNone);
	
	TRAP(ErrRet,
	DeleteStatusInfoL();

	if(aProtect)
	{
		iProtectStatus = aValue;
	}
	else
	{
		iScreenStatus = aValue;
	}

	TFileName QueryBuffer;
	QueryBuffer.Copy(_L("SELECT * FROM "));
	QueryBuffer.Append(KtxtStatuslist);
	
	iItemsDatabase.Begin();
	
	RDbView Myview;
	Myview.Prepare(iItemsDatabase,TDbQuery(QueryBuffer));
	CleanupClosePushL(Myview);
	
	Myview.EvaluateAll();
	Myview.FirstL();
	
	Myview.InsertL(); 
		
	if(iProtectStatus)
		Myview.SetColL(2,0x100);
	else
		Myview.SetColL(2,0x000);
	
	if(iScreenStatus)
		Myview.SetColL(3,0x100);
	else
		Myview.SetColL(3,0x000);

	Myview.PutL(); 
			
	CleanupStack::PopAndDestroy(1); // Myview
	iItemsDatabase.Commit();
	);
	
	return ErrRet;
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/

void CExPolicy_Server::UpdateDatabaseL(const TDesC& aName,const TDesC& aValue,TInt& aIndex,TBool aImsi)
{	
	TFileName QueryBuffer;
	QueryBuffer.Copy(_L("SELECT * FROM "));
	
	if(aImsi)
		QueryBuffer.Append(KtxtImsilist);
	else
		QueryBuffer.Append(KtxtNumberlist);
	
	QueryBuffer.Append(_L(" WHERE "));
	QueryBuffer.Append(NCol0);
	QueryBuffer.Append(_L(" = "));
	QueryBuffer.AppendNum(aIndex);
	
	iItemsDatabase.Begin();
	
	RDbView Myview;
	Myview.Prepare(iItemsDatabase,TDbQuery(QueryBuffer));
	CleanupClosePushL(Myview);
	
	Myview.EvaluateAll();
	Myview.FirstL();
	
	if(Myview.AtRow())            
	{			
		Myview.UpdateL();
		Myview.SetColL(2,aName);		
		Myview.SetColL(3,aValue);	
		Myview.PutL();
	}
			
	CleanupStack::PopAndDestroy(1); // Myview
	iItemsDatabase.Commit();
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/


void CExPolicy_Server::SaveToDatabaseL(const TDesC& aName,const TDesC& aValue,TInt& aIndex,TBool aImsi)
{	
	TFileName QueryBuffer;
	QueryBuffer.Copy(_L("SELECT * FROM "));
	
	if(aImsi)
		QueryBuffer.Append(KtxtImsilist);
	else
		QueryBuffer.Append(KtxtNumberlist);

	iItemsDatabase.Begin();

	RDbView Myview;
	Myview.Prepare(iItemsDatabase,TDbQuery(QueryBuffer));
	CleanupClosePushL(Myview);

	Myview.InsertL(); 
		
	Myview.SetColL(2,aName);
	Myview.SetColL(3,aValue);

	Myview.PutL();  
	
	aIndex = Myview.ColInt(1);// autoincrement gives us unique index.
			
	CleanupStack::PopAndDestroy(1); // Myview
	iItemsDatabase.Commit();
}
/* 
-----------------------------------------------------------------------------
*********public functions called by sessions to handle data *****************
-----------------------------------------------------------------------------
*/
TInt CExPolicy_Server::SetStatusL(TBool& aValue,TBool aProtect)
{
	return UpdateStatusL(aValue,aProtect);
}
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TBool CExPolicy_Server::GetStatus(TBool aProtect)
{
	if(aProtect)
		return iProtectStatus;
	else
		return iScreenStatus;
}

/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
// function to delete data from the database.
TInt CExPolicy_Server::DeleteItemData(TExampleItem& aItem,TBool aImsi)
{
	TInt Ret(KErrNotFound);
	
	if(aImsi)
	{
		for(TInt i=0; i < iImsiArray.Count(); i++)
		{
			if(iImsiArray[i].iIndex == aItem.iIndex)
			{
				iImsiArray.Remove(i);
				DeleteFromDatabaseL(aItem.iIndex,aImsi);
				Ret = KErrNone;
				break;
			}
		}
	}
	else
	{
		for(TInt i=0; i < iNumberArray.Count(); i++)
		{
			if(iNumberArray[i].iIndex == aItem.iIndex)
			{
				iNumberArray.Remove(i);
				DeleteFromDatabaseL(aItem.iIndex,aImsi);
				Ret = KErrNone;
				break;
			}
		}
	}
	
	return Ret;
}
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
// function to update value fields of tems in the database
TInt CExPolicy_Server::SetItemValueData(TExampleItem& aItem,TBool aImsi)
{
	TInt Ret(KErrNotFound);
	
	if(aImsi)
	{
		for(TInt i=0; i < iImsiArray.Count(); i++)
		{
			if(iImsiArray[i].iIndex == aItem.iIndex)
			{
				iImsiArray[i].iValue.Copy(aItem.iValue);
				UpdateDatabaseL(aItem.iName,aItem.iValue,aItem.iIndex,aImsi);
				Ret = KErrNone;
				break;
			}
		}
	}
	else
	{
		for(TInt i=0; i < iNumberArray.Count(); i++)
		{
			if(iNumberArray[i].iIndex == aItem.iIndex)
			{
				iNumberArray[i].iValue.Copy(aItem.iValue);
				UpdateDatabaseL(aItem.iName,aItem.iValue,aItem.iIndex,aImsi);
				Ret = KErrNone;
				break;
			}
		}
	}
	
	return Ret;
}
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
// this function is used for both updating and adding items in the database
TInt CExPolicy_Server::SetItemData(TExampleItem& aItem,TBool aImsi)
{
	// by default we would say that specified item can not be found
	TInt Ret(KErrNotFound);
	
	if( aItem.iIndex < 0)// if index is -1 or smaller, it's new item
	{		
		SaveToDatabaseL(aItem.iName,aItem.iValue,aItem.iIndex,aImsi);
		
		if(aImsi)
		{
			iImsiArray.Append(aItem);
		}
		else
		{
			iNumberArray.Append(aItem);
		}
		
		Ret = KErrNone;
	}
	else // index tells us which item in the database needs to be updated
	{
		if(aImsi)
		{
			for(TInt i=0; i < iImsiArray.Count(); i++)
			{
				if(iImsiArray[i].iIndex == aItem.iIndex)
				{
					iImsiArray[i].iValue.Copy(aItem.iValue);
					iImsiArray[i].iName.Copy(aItem.iName);
					UpdateDatabaseL(aItem.iName,aItem.iValue,aItem.iIndex,aImsi);
					Ret = KErrNone;
					break;
				}
			}
		}
		else
		{
			for(TInt i=0; i < iNumberArray.Count(); i++)
			{
				if(iNumberArray[i].iIndex == aItem.iIndex)
				{
					iNumberArray[i].iValue.Copy(aItem.iValue);
					iNumberArray[i].iName.Copy(aItem.iName);
					UpdateDatabaseL(aItem.iName,aItem.iValue,aItem.iIndex,aImsi);
					Ret = KErrNone;
					break;
				}
			}
		}
	}
	
	return Ret;	
}
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_Server::SetPasswordL(const TDesC& aPassword)
{
	TFindFile PrivFolder(iFsSession);
	if(KErrNone == PrivFolder.FindByDir(KtxInifileName, KNullDesC))// finds the drive
	{
		iFsSession.Delete(PrivFolder.File());
		
		CDictionaryFileStore* MyDStore = CDictionaryFileStore::OpenLC(iFsSession,PrivFolder.File(), TUid::Uid(0x102013AD));
			
		TUid FileUid = {0x8876};

		RDictionaryWriteStream out2;
		out2.AssignLC(*MyDStore,FileUid);
		
		TBuf<100> MixBuffer,MixBuffer2;
		
		MixBuffer2.Copy(aPassword);
		if(MixBuffer2.Length() % 2)
		{
			MixBuffer2.Append(_L(" "));
		}
		
		MixOutPut(MixBuffer2,MixBuffer); 
		
		out2 << MixBuffer;
		out2.CommitL(); 	
		CleanupStack::PopAndDestroy(1);// out2
		
		MyDStore->CommitL();
		CleanupStack::PopAndDestroy(1);// Store
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_Server::MixOutPut(const TDesC& aString, TDes& aOutString) 
{
	aOutString.Zero();
	for(TInt i= aString.Length(); i > 1; i = i - 2)
	{
		aOutString.Append((aString[i - 2] ^ aString[i - 1]));
		aOutString.Append(aString[i - 1]);
	}
}
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_Server::GetPasswordL(TDes& aPassword)
{
	aPassword.Zero();
	
	TFindFile PrivFolder(iFsSession);
	if(KErrNone == PrivFolder.FindByDir(KtxInifileName, KNullDesC))// finds the drive
	{		
		if(BaflUtils::FileExists(iFsSession,PrivFolder.File()))
		{
			CDictionaryFileStore* MyDStore = CDictionaryFileStore::OpenLC(iFsSession,PrivFolder.File(), TUid::Uid(0x102013AD));
			
			TUid FileUid = {0x8876};

			if(MyDStore->IsPresentL(FileUid))
			{
				RDictionaryReadStream in;
				in.OpenLC(*MyDStore,FileUid);
				
				TBuf<100> MixBuffer,MixBuffer2;
				
				in >> MixBuffer;
				
				MixOutPut(MixBuffer,MixBuffer2);
				 
				MixBuffer2.TrimAll();
				
				aPassword.Copy(MixBuffer2);
				
				CleanupStack::PopAndDestroy(1);// in
			}
			CleanupStack::PopAndDestroy(1);// Store
		}
	}	
}
	
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
// function to get item according to the Item array index
// instead of database index.
// this is needed for reading all items according to the item count
// when application is started
TInt CExPolicy_Server::GetListItemData(TExampleItem& aItem,TBool aImsi)
{
	TInt Ret(KErrNotFound);
	
	TInt SelIndex = aItem.iIndex;
	
	if(aImsi)
	{
		if(iImsiArray.Count() > SelIndex && SelIndex >= 0)
		{
			aItem.iValue.Copy(iImsiArray[SelIndex].iValue);
			aItem.iName.Copy(iImsiArray[SelIndex].iName);
			aItem.iIndex = iImsiArray[SelIndex].iIndex;
			Ret = KErrNone;
		}
	}
	else
	{
		if(iNumberArray.Count() > SelIndex && SelIndex >= 0)
		{
			aItem.iValue.Copy(iNumberArray[SelIndex].iValue);
			aItem.iName.Copy(iNumberArray[SelIndex].iName);
			aItem.iIndex = iNumberArray[SelIndex].iIndex;
			Ret = KErrNone;
		}		
	}
	return Ret;	
}
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
// get item data according to the database index
// data already read to the item array so get it directly from there.
TInt CExPolicy_Server::GetItemData(TExampleItem& aItem,TBool aImsi)
{
	TInt Ret(KErrNotFound);
	
	if(aImsi)
	{
		for(TInt i=0; i < iImsiArray.Count(); i++)
		{
			if(iImsiArray[i].iIndex == aItem.iIndex)
			{
				aItem.iValue.Copy(iImsiArray[i].iValue);
				aItem.iName.Copy(iImsiArray[i].iName);
				Ret = KErrNone;
				break;
			}
		}
	}
	else
	{
		for(TInt i=0; i < iNumberArray.Count(); i++)
		{
			if(iNumberArray[i].iIndex == aItem.iIndex)
			{
				aItem.iValue.Copy(iNumberArray[i].iValue);
				aItem.iName.Copy(iNumberArray[i].iName);
				Ret = KErrNone;
				break;
			}
		}
	}
	return Ret;	
}
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
// count of items in the database (item array)
// used to get number of items to determine index range used with GetListItemData
TInt CExPolicy_Server::GetItemCount(TInt& aItemCount,TBool aImsi)
{
	TInt Ret(KErrNone);
	
	if(aImsi)
		aItemCount = iImsiArray.Count();
	else
		aItemCount = iNumberArray.Count();
	
	return Ret;	
}
/* 
-----------------------------------------------------------------------------
***************** functions for running the server exe **********************
-----------------------------------------------------------------------------
*/
#ifdef __SERIES60_3X__
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
#else
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_Server::PanicClient(const RMessage& aMessage, TInt aPanic)
    {
    aMessage.Panic(KExapmpleServerName, aPanic);
    }
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CExPolicy_Server::PanicServer(TInt aPanic)//TTimeServPanic aPanic)
    {
    User::Panic(KExapmpleServerName, aPanic);
    }
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/  
void CExPolicy_Server::ThreadFunctionL()
    {
    // Construct active scheduler
    CActiveScheduler* activeScheduler = new (ELeave) CActiveScheduler;
    CleanupStack::PushL(activeScheduler) ;

    // Install active scheduler
    // We don't need to check whether an active scheduler is already installed
    // as this is a new thread, so there won't be one
    CActiveScheduler::Install(activeScheduler);

    // Construct our server
    CExPolicy_Server::NewLC();    // anonymous

	RSemaphore semaphore;
	User::LeaveIfError(semaphore.OpenGlobal(KExapmpleServerSemaphoreName));

	// Semaphore opened ok
	semaphore.Signal();
	semaphore.Close();

	// Start handling requests
	CActiveScheduler::Start();

    CleanupStack::PopAndDestroy(2, activeScheduler);    //  anonymous CSALockServer
    }
    
TInt CExPolicy_Server::ThreadFunction(TAny* /*aNone*/)
    {
    CTrapCleanup* cleanupStack = CTrapCleanup::New();
	if (cleanupStack == NULL)
	    {
        PanicServer(-1);
	    }

    TRAPD(err, ThreadFunctionL());
    if (err != KErrNone)
        {
        PanicServer(err);//ESrvCreateServer);
        }

    delete cleanupStack;
    cleanupStack = NULL;

    return KErrNone;
    }
    
	#ifdef __WINS__

	IMPORT_C TInt WinsMain();
	EXPORT_C TInt WinsMain()
	    {
		return reinterpret_cast<TInt>(&CExPolicy_Server::ThreadFunction);
	    }


	GLDEF_C TInt E32Dll(TDllReason)
	    {
		return KErrNone;
	    }
	    
	    

	#else  // __ARMI__
	TInt E32Main()
    {
		return CExPolicy_Server::ThreadFunction(NULL);
    }
	#endif
	

#endif