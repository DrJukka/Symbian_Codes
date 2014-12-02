/* 
	Copyright (c) 2006, 
	Jukka Silvennoinen. 
	All rights reserved 
*/

#ifndef __MAINSERVERSESSION_HEADER__
#define __MAINSERVERSESSION_HEADER__

#include <e32base.h>
#include <S32FILE.H>
#include <D32DBMS.H>
#include <aknserverapp.h>
#include "CTimeOutTimer.h"
#include "Mainservercommon.h"
#include "IMSI_IMEI_Getters.h"
	
// ----------------------------------------------------------------------------------------
// Server
// ----------------------------------------------------------------------------------------

class CExPolicy_Server : 
#ifdef __SERIES60_3X__
	public CPolicyServer
#else 
	public CServer
#endif
,MTimeOutNotify,public MAknServerAppExitObserver,MImsiObserver,MCellIdObserver
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
	TInt SetFileMapping(TFileMappingItem& aDataMapping);
	TInt KillTasks(TUid& aUid);
	void KillThread(TDes8& aName);
	void KillProcess(TDes8& aName);
	void GetExtraInfoL(TExtraInfoItem& aItem);
protected:
	void GotIMSIL(const TDesC& aIMSI, TInt aError);
	void CellIDL(const TDesC& aCountryCode,const TDesC& aNwId,TUint aAreaCode,TUint aCellId);

	void TimerExpired();
	void HandleServerAppExit(TInt aReason){};
private:
	void ShowGlobalDbgNoteL(const TDesC& aMessage);
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
private:
	CTimeOutTimer*			iExampleTimer;
	TInt					iSessionCount;
	CImsiReader*			iImsiReader;
	CCellIDCheck*			iCellIDCheck;
	TUint 					iAreaCode;
	TUint 					iCellId;
	HBufC*					iImsi;
	HBufC*					iCountryCode;
	HBufC*					iNwId;
	RFs						iFsSession;
	RFile					iFile;
    };
	
#endif //__MAINSERVERSESSION_HEADER__
