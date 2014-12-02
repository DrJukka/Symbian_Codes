/* 
	Copyright (c) 2006, 
	Jukka Silvennoinen. 
	All rights reserved 
*/

#ifndef __EXAMPLESERVERSESSION_HEADER__
#define __EXAMPLESERVERSESSION_HEADER__

#include <e32base.h>
#include <S32FILE.H>
#include <D32DBMS.H>
#include "CTimeOutTimer.h"

#include "clientservercommon.h"
#include "DLLRegistration.h" 

#ifdef __SERIES60_3X__
	#include "IMSI_IMEI_Getter.h"
#else
	
#endif

class CNProtRegister;

	
// ----------------------------------------------------------------------------------------
// Server
// ----------------------------------------------------------------------------------------

class CExPolicy_Server : 
#ifdef __SERIES60_3X__
	public CPolicyServer,MImsiImeiObserver
#else 
	public CServer
#endif
,MTimeOutNotify,MRegCodeValidator
    {
public : // New methods
    static CExPolicy_Server* NewLC();
	void AddSession();
	void DropSession();
    ~CExPolicy_Server();
#ifdef __SERIES60_3X__	
 
#else    
    static TInt ThreadFunction(TAny* sStarted);
#endif 
public:// public function used by sessions to handle client requests
	TInt DeleteItemData(TExampleItem& aItem,TBool aImsi);
	TInt SetItemData(TExampleItem& aItem,TBool aImsi);
	TInt SetItemValueData(TExampleItem& aItem,TBool aImsi);
	TInt GetItemData(TExampleItem& aItem,TBool aImsi);
	TInt GetItemCount(TInt& aItemCount,TBool aImsi);
	TInt GetListItemData(TExampleItem& aItem,TBool aImsi);
	
	TInt SetStatusL(TBool& aValue,TBool aProtect);
	TBool GetStatus(TBool aProtect);
	
	void GetPasswordL(TDes& aPassword);
	void SetPasswordL(const TDesC& aPassword);
	
	TBool IsReadyForAction(void);
	void CheckRegStatusL(TBool& aFirstStart, TInt& aDaysLeft, TUint& aResult,TDes& aOperator);
	void TryRegisterApp(TDesC& aBuffer, TUint& aResult, TTime& aStartTime);
	
	void TryRegisterApp(TUint& aResult);
	TBool CodeMatches(const TDesC& aCode);

	TBool HadNonClearedLockScreen(void);
	TBool LockScreenLockedUnLocked(TBool aSetOn);

	void CheckCurrentSIMOkL(void);
protected:
	void GotValuesL(const TDesC& aIMEI,const TDesC& aIMSI);
	TBool IsCodeOkL(const TDesC& aCode,const TDesC8& aString);
private:// internal functions to handle data
	void MixOutPut(const TDesC& aString, TDes& aOutString);
	void AddToValDatabase(RDbDatabase& aDatabase,const TDesC& aCode,const TDesC8& aString);
	TBool OkToAddToValDb(RDbDatabase& aDatabase,const TDesC& aCode,const TDesC8& aString);

	void DeleteFromDatabaseL(TInt& aIndex, TBool aImsi);
	void DeleteStatusInfoL(void);
	TInt UpdateStatusL(TBool& aValue, TBool aProtect);
	void UpdateDatabaseL(const TDesC& aName,const TDesC& aValue,TInt& aIndex,TBool aImsi);
	void SaveToDatabaseL(const TDesC& aName,const TDesC& aValue,TInt& aIndex,TBool aImsi);
	void ReadDbItemsL(TBool aImsi);
	void ReadStatusItemsL(void);
	void CreateValTableL(RDbDatabase& aDatabase);
	void CreateTableL(RDbDatabase& aDatabase,TBool aImsi);
	void CreateStatusTableL(RDbDatabase& aDatabase);
	// other internal functions
	void StartShutDownTimer();
	void ShutMeDown();
	CExPolicy_Server();
	void ConstructL();
#ifdef __SERIES60_3X__	
	CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;
	//From CPolicyServer
	CPolicyServer::TCustomResult CustomSecurityCheckL(const RMessage2& aMsg, TInt& aAction, TSecurityInfo& aMissing);
#else 
	// From CServer
	static void PanicClient(const RMessage& aMessage, TInt aReason);
    static void PanicServer(TInt aPanic);//TTimeServPanic aPanic);
    static void ThreadFunctionL();
    CSharableSession* NewSessionL(const TVersion &aVersion) const;
#endif 
protected:
	void TimerExpired();
private:
	CTimeOutTimer*			iExampleTimer;
	TInt					iSessionCount;
	RArray<TExampleItem>	iImsiArray;
	RArray<TExampleItem>	iNumberArray;
	RDbNamedDatabase 		iItemsDatabase;
	RFs						iFsSession;
	TBool					iScreenStatus,iProtectStatus,iIsThai;
	CNProtRegister*			iNProtRegister;
#ifdef __SERIES60_3X__
	CRegReader*				iRegReader;
#else

#endif
	TBuf<30>				iImsi,iImei,iOperator;
	
	RFile						iFile;
    };


	
#endif //__EXAMPLESERVERSESSION_HEADER__
