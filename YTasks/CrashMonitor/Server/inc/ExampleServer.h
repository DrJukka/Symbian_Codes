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
#include "CrashMonitor.h"

class CExampleItem: public CBase
    {
    public:
        CExampleItem(): iIndex(-1){};
        ~CExampleItem(){delete iProcess;delete iName; delete iExitCatogory;};
    public:
    	TInt 		iIndex;
    	HBufC* 		iProcess;
        HBufC*  	iName;
        HBufC*  	iExitCatogory;
        TExitType	iExitType;
        TInt		iExitReason;
        TTime		iTime;
    };
	
// ----------------------------------------------------------------------------------------
// Server
// ----------------------------------------------------------------------------------------

class CExPolicy_Server : 
#ifdef __SERIES60_3X__
	public CPolicyServer
#else 
	public CServer
#endif
,MTimeOutNotify,MExitMonitorCallBack
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
	void ClearListItemsL();
	void StartShutDownTimer();
	TInt GetItemsCount(void);
	void GetItemValueL(TInt& aIndex,TDes& aName,TDes& aExitCatogory,TExitType& aExitType, TInt& aExitReason,TDes& aProcess,TTime& aTime);
protected:
	void TimerExpired();
	void HandleExit(const TDesC& aName,const TExitType&  aExitType,const TInt& aExitReason,const TDesC& aExitCategory,const TDesC& aProcess,const TTime& aTime);
private:
	void ShowExitNoteL(const TDesC& aName,const TExitType&  aExitType,const TInt& aExitReason,const TDesC& aExitCategory);
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
private:
	CTimeOutTimer*				iExampleTimer;
	TInt						iSessionCount;
	RPointerArray<CExampleItem>	iArray;
	CExitMonitor*				iExitMonitor;
	RFs							iFsSession;
	RFile						iFile;
    };
	
#endif //__EXAMPLESERVERSESSION_HEADER__
