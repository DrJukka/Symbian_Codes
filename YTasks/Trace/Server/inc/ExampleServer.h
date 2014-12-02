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
#include "TraceMonitor.h"
#include "Screen_Drawer.h"


	
class CExampleItem: public CBase
    {
    public:
        CExampleItem(){};
        ~CExampleItem(){};
    public:
    	TInt 		iCpuLoad;
    	TInt		iMemory;
        TTime		iTime;
    };

class CDictionaryFileStore;

// ----------------------------------------------------------------------------------------
// Server
// ----------------------------------------------------------------------------------------

class CExPolicy_Server : 
#ifdef __SERIES60_3X__
	public CPolicyServer
#else 
	public CServer
#endif
,MTimeOutNotify,MTraceCallBack
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
	void GetItemValueL(TInt& aIndex,TInt& aCpuLoad,TInt& aMemory,TTime& aTime);
	TUint32	GetMax(void);
	void StopTraceL();
	void StartTraceL();
	void ReReadSettingsL();
	void GetValuesFromStoreL();
	void GetValuesL(CDictionaryFileStore* aDStore,TInt aUID,TInt32& aValue);
protected:
	void TimerExpired();
	void HandleUpdate(const TInt& aCpuLoad,const TInt& aMemory,const TTime& aTime);
private:
	void ShowNoteL(const TDesC& aMessage);
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
	CTraceMonitor*				iTraceMonitor;
	RFs							iFsSession;
	RFile						iFile;
	TSettingsItem				iSettings;
	CScreenDrawer*				iScreenDrawer;
	TInt						iTotalRamInBytes;
    };


	
#endif //__EXAMPLESERVERSESSION_HEADER__
